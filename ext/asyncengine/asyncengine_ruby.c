#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_utils.h"
#include "ae_ip_utils.h"
#include "ae_async.h"
#include "ae_next_tick.h"
#include "ae_timer.h"
#include "ae_udp.h"
#include "ae_tcp.h"



static ID att_handles;
static ID att_blocks;
static ID att_next_ticks;
static ID const_UV_ERRNOS;

static ID method_destroy;
static ID method_clear;

enum status {
  STOPPED = 1,
  STARTING,
  STARTED,
  RELEASING
};

static enum status AE_status;


// TODO: Temporal function for debugging purposes, since loop->active_handles
// will be removed soon from UV (or it's not public API).
static
int ae_uv_num_active_handlers(void)
{
  AE_TRACE();

  if (AE_status == STOPPED)
    return 0;

  return AE_uv_loop->active_handles;
}


// TODO: Temporal function for debugging purposes.
static
int ae_uv_num_active_reqs(void)
{
  AE_TRACE();

  if (AE_status == STOPPED)
    return 0;

  ngx_queue_t *q;
  int count = 0;
  ngx_queue_foreach(q, &AE_uv_loop->active_reqs) {
    count++;
  }

  return count;
}


// TMP
VALUE AsyncEngine_num_uv_active_handles(VALUE self)
{
  AE_TRACE();

  return INT2FIX(ae_uv_num_active_handlers());
}


// TMP
VALUE AsyncEngine_num_uv_active_reqs(VALUE self)
{
  AE_TRACE();

  return INT2FIX(ae_uv_num_active_reqs());
}


VALUE AsyncEngine_init(VALUE self)
{
  AE_TRACE();

  AE_handles = rb_ivar_get(mAsyncEngine, att_handles);
  AE_blocks = rb_ivar_get(mAsyncEngine, att_blocks);
  // NOTE: AE_next_ticks cannot be loaded here since its value is changed in runtime.

  AE_UV_ERRNOS = rb_const_get(mAsyncEngine, const_UV_ERRNOS);

  /* Set the UV loop. */
  AE_uv_loop = uv_default_loop();

  /* Initial status and flags. */
  AE_status = STARTING;

  /* Load the UV idle (next tick) now. */
  load_ae_next_tick_uv_idle();

  return Qtrue;
}


static
void ae_ubf_uv_async_callback(uv_async_t* handle, int status)
{
  AE_TRACE2();

  //printf("*** ae_ubf_uv_async_callback(): AE_status: %d, AE_uv_loop: %ld\n", AE_status, AE_uv_loop);

  // TODO: testing, can be error?
  AE_ASSERT(! status);

  uv_close((uv_handle_t *)handle, ae_uv_handle_close_callback);

  AE_DEBUG("ae_execute_in_ruby_land(rb_thread_check_ints)");
  ae_execute_in_ruby_land(rb_thread_check_ints);
}


static
void ae_ubf(void)
{
  AE_TRACE2();

  //printf("*** ae_ubf(): AE_status: %d, AE_uv_loop: %ld\n", AE_status, AE_uv_loop);
  /*
   * When a signal is received by a Ruby process running a blocking code (without GVL)
   * Ruby calls the ubf() function. But this ubf() function could be called also from
   * other thread (i.e. ae_tread.kill) so we don't know if the ubf() is being executing
   * in AE thread, nor if it has been called due a received signal or a Thread#kill.
   * Therefore, do nothing but check interrupts in Ruby land via a thread safe uv_async.
   */

  // TODO: Make it static so no ALLOC required every time.

  uv_async_t* ae_ubf_uv_async = ALLOC(uv_async_t);

  AE_ASSERT(! uv_async_init(AE_uv_loop, ae_ubf_uv_async, ae_ubf_uv_async_callback));
  AE_ASSERT(! uv_async_send(ae_ubf_uv_async));
}


static
VALUE run_uv_without_gvl(void)
{
  AE_TRACE();

  // TODO: for testing.
  AE_ASSERT(AE_status == STARTING);

  // TODO: for testing.
  AE_ASSERT(ae_uv_num_active_reqs() == 0);

  AE_DEBUG("UV loop starts...");

  //while((! (AE_flags & DO_STOP)) && uv_run_once(AE_uv_loop)) {
  //printf("***** ae_uv_num_active_handlers = %d,  ae_uv_num_active_reqs = %d\n", ae_uv_num_active_handlers(), ae_uv_num_active_reqs());
  //};
  uv_run(AE_uv_loop);

  AE_DEBUG("UV loop terminates");

  /* Close the UV idle (next tick) now. */
  unload_ae_next_tick_uv_idle();

  AE_status = RELEASING;

  // TODO: for testing.
  AE_ASSERT(ae_uv_num_active_reqs() == 0);
  AE_ASSERT(ae_uv_num_active_handlers() == 0);

  return Qtrue;
}


VALUE AsyncEngine_run_uv(VALUE self)
{
  AE_TRACE();

  rb_thread_call_without_gvl(run_uv_without_gvl, NULL, ae_ubf, NULL);

  AE_DEBUG("function terminates");

  return Qtrue;
}


static
VALUE run_uv_release_without_gvl(void)
{
  AE_TRACE();

  // TODO: Ñapa temporal para que no se ejecute esto dos veces, mierdas del .rb.
  // TODO: No funciona !!! sigue cascando por poner abajo AE_uv_loop = NULL.
  if (AE_status == STOPPED) {
    AE_ABORT("AE_status == STOPPED !!!!!!");
    return Qnil;
  }

  /* There MUST NOT be UV active handles at this time, we enter here just to
   * iterate once for freeing closed UV handles not freed yet (i.e. stopped timers
   * which let uv_run() to exit).
   */
  // TODO: Pues sí que hay en los test units !!!
  //AE_ASSERT(ae_uv_num_active_handlers() == 0);

  // TODO: for testing.
  int num_active_reqs = ae_uv_num_active_reqs();
  if (num_active_reqs)
    printf("WARN: run_uv_release_without_gvl():  ae_uv_num_active_reqs = %d\n", ae_uv_num_active_reqs());

  /* Run UV loop (it blocks if there were handles in the given block). */
  AE_DEBUG("uv_run() starts...");
  uv_run(AE_uv_loop);
  AE_DEBUG("uv_run() terminates");

  // TODO: for testing.
  AE_ASSERT(ae_uv_num_active_handlers() == 0);
  AE_ASSERT(ae_uv_num_active_reqs() == 0);

  AE_status = STOPPED;
  // TODO: Si pongo esto a NULL peta pero bien.
  // Ocurre porque se llama dos veces a AE.release() y la segunda encuentra esto a NULL !!!
  // Solucionado mirando arriba que AE_status no sea RELEASING, aunque es ñapa...
  // TODO: No, sigue cascando pero bien...
  AE_uv_loop = NULL;

  return Qtrue;
}


/*
 * This method is called in the _ensure_ block of AsyncEngine.run() method
 * after AsyncEngine.destroy_ae_handles() to cause a single UV iteration in order
 * to invoke all the uv_close callbacks and free() the uv_handles.
 */
VALUE AsyncEngine_run_uv_release(VALUE self)
{
  AE_TRACE();

  rb_thread_call_without_gvl(run_uv_release_without_gvl, NULL, NULL, NULL);

  return Qtrue;
}


static
int ae_is_running(void)
{
  AE_TRACE();

  return (AE_status == STARTED || AE_status == STARTING);
}


VALUE AsyncEngine_is_running(VALUE self)
{
  AE_TRACE();

  if (ae_is_running())
    return Qtrue;
  else
    return Qfalse;
}


void ae_check_running(void)
{
  AE_TRACE();

  if (! ae_is_running())
    rb_raise(eAsyncEngineError, "AsyncEngine is not ready yet");
}


VALUE AsyncEngine_check_running(VALUE self)
{
  AE_TRACE();

  ae_check_running();
  return Qtrue;
}


static
VALUE destroy_handle_with_rb_protect(VALUE handle)
{
  AE_TRACE2();

  rb_funcall2(handle, method_destroy, 0, NULL);
  return Qnil;
}


static
int destroy_handle(VALUE key, VALUE handle, VALUE in)
{
  AE_TRACE2();

  int error_tag;

  rb_protect(destroy_handle_with_rb_protect, handle, &error_tag);
  if (error_tag) {
    rb_set_errinfo(Qnil);  // TODO: sure?
    AE_WARN("************* error rescued (rb_protect() while destroying the handle");
  }

  return 0;
}


VALUE AsyncEngine_release(VALUE self)
{
  AE_TRACE2();

  VALUE size;

  // Clear next_ticks.
  rb_ary_clear(rb_ivar_get(mAsyncEngine, att_next_ticks));

  // Run #destroy() for each existing AE handle.
  rb_hash_foreach(AE_handles, destroy_handle, Qnil);

  return Qtrue;
}



void Init_asyncengine_ext()
{
  AE_TRACE();

  mAsyncEngine = rb_define_module("AsyncEngine");
  cAsyncEngineHandle = rb_define_class_under(mAsyncEngine, "Handle", rb_cObject);
  eAsyncEngineError = rb_define_class_under(mAsyncEngine, "Error", rb_eStandardError);

  rb_define_module_function(mAsyncEngine, "init", AsyncEngine_init, 0);
  rb_define_module_function(mAsyncEngine, "run_uv", AsyncEngine_run_uv, 0);
  rb_define_module_function(mAsyncEngine, "release", AsyncEngine_release, 0);
  rb_define_module_function(mAsyncEngine, "run_uv_release", AsyncEngine_run_uv_release, 0);
  rb_define_module_function(mAsyncEngine, "running?", AsyncEngine_is_running, 0);
  rb_define_module_function(mAsyncEngine, "check_running", AsyncEngine_check_running, 0);
  rb_define_module_function(mAsyncEngine, "num_uv_active_handles", AsyncEngine_num_uv_active_handles, 0);
  rb_define_module_function(mAsyncEngine, "num_uv_active_reqs", AsyncEngine_num_uv_active_reqs, 0);

  att_handles = rb_intern("@_handles");
  att_blocks = rb_intern("@_blocks");
  att_next_ticks = rb_intern("@_next_ticks");
  const_UV_ERRNOS = rb_intern("UV_ERRNOS");

  method_destroy = rb_intern("destroy");
  method_clear = rb_intern("clear");

  init_ae_handle_common();
  init_ae_utils();
  init_ae_ip_utils();
  init_rb_utilities();
  init_ae_async();
  init_ae_next_tick();
  init_ae_timer();
  init_ae_udp();
  init_ae_tcp();

  AE_status = STOPPED;
  AE_uv_loop = NULL;
}
