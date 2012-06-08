#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_utils.h"
#include "ae_ip_utils.h"
#include "ae_async.h"
#include "ae_next_tick.h"
#include "ae_timer.h"
#include "ae_udp.h"
#include "ae_tcp.h"


static ID att_thread;
static ID att_handles;
static ID att_blocks;
static ID att_next_ticks;

static ID const_UV_ERRNOS;

static ID method_destroy;
static ID method_clear;
static ID method_next_tick;

enum status {
  STOPPED = 1,
  STARTING = 2,
  STARTED = 3,
  RELEASING = 4
};

static enum status AE_status;


/** Pre-declaration of static functions. */

static void ae_release_loop(void);


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

  uv_run(AE_uv_loop);
}


VALUE AsyncEngine_run_loop(VALUE self, VALUE _rb_block)
{
  AE_TRACE2();

  // TODO: To find a crash in bugs.rb. But in fact the problem is that we MUST NOT
  // allow being here if AE_status != STOPPED. In that case let's raise and that's all.
  switch(AE_status) {
    case STOPPED:
      break;
    case STARTING:
      AE_WARN("AE_status is STARTING, abort !!!");
      break;
    case STARTED:
      AE_WARN("AE_status is STARTED, abort !!!");
      break;
    case RELEASING:
      AE_WARN("AE_status is RELEASING, abort !!!");
      break;
  }
  AE_ASSERT(AE_status == STOPPED);

  // Set the @_thread attribute and store it.
  rb_ivar_set(mAsyncEngine, att_thread, rb_thread_current());
  AE_thread = rb_thread_current();  // TODO: Needed?

  // Get the VALUEs for @_handles and @_blocks (faster).
  // NOTE: AE_next_ticks cannot be loaded here since its value is changed in runtime.
  AE_handles = rb_ivar_get(mAsyncEngine, att_handles);
  AE_blocks = rb_ivar_get(mAsyncEngine, att_blocks);

  AE_UV_ERRNOS = rb_const_get(mAsyncEngine, const_UV_ERRNOS);

  /* Set the UV loop. */
  AE_uv_loop = uv_default_loop();

  /* Initial status and flags. */
  AE_status = STARTING;

  /* Load the UV idle (next tick) now. */
  load_ae_next_tick_uv_idle();

  /* Pass the given block to the reactor via next_tick. */
  rb_funcall2(mAsyncEngine, method_next_tick, 1, &_rb_block);

  // TODO: for testing.
  AE_ASSERT(ae_uv_num_active_reqs() == 0);

  AE_DEBUG("UV loop starts...");
  rb_thread_call_without_gvl(run_uv_without_gvl, NULL, ae_ubf, NULL);
  AE_DEBUG("UV loop terminates");

    // TODO: for testing.
  AE_ASSERT(ae_uv_num_active_handlers() == 0);
  AE_ASSERT(ae_uv_num_active_reqs() == 0);

  /* Close the UV idle (next tick) now. */
  unload_ae_next_tick_uv_idle();

  /*
   * uv_run() has exited due to one of the following reasons:
   * 
   * - The app called AE.stop which, in the end, calls to ae_release_loop(), so
   *   right now all the handles have been destroyed and freed.
   * - Some non ignored/trapped exception or signal ocurred so the exception manager
   *   stored it and called ae_release_loop(), so same case.
   * - There are no more active UV handles. In this case there could be, still, inactive
   *   UV handles (and therefore AE handles) so we need to call ae_release_loop() to clean
   *   them.
   *
   * To simplify, let's always call to ae_release_loop() so we are done (there is no problem
   * in calling it twice).
   */
  ae_release_loop();

  /*
   * After calling to ae_release_loop() all the handles has been closed/destroyed, but we
   * need to run uv_run() to free them.
   */
  AE_DEBUG("UV loop for free-ing remaining handles starts...");
  rb_thread_call_without_gvl(run_uv_without_gvl, NULL, NULL, NULL);
  AE_DEBUG("UV loop for free-ing remaining handles terminates");

  // Now yes, set the status to CLOSED.
  AE_status = STOPPED;

  // Unset the AE_uv_loop.
  AE_uv_loop = NULL;

  // Unset the @_thread attribute.
  rb_ivar_set(mAsyncEngine, att_thread, Qnil);
  AE_thread = Qnil;  // TODO: Needed?

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


static
void ae_release_loop(void)
{
  AE_TRACE2();

  // No more handles can be created now.
  AE_status = RELEASING;

  // Clear next_ticks.
  rb_ary_clear(rb_ivar_get(mAsyncEngine, att_next_ticks));

  /*
   * Run #destroy() for each existing AE handle. Note that #destroy() method
   * is called safely in every AE handle by ignoring any exception/error.
   * */
  rb_hash_foreach(AE_handles, destroy_handle, Qnil);

  // Clear blocks.
  // TODO: Probably this would cause some "error" when some callback try to get
  // its block... it should not happen anyhow.
  rb_funcall2(AE_blocks, method_clear, 0, NULL);
}


VALUE AsyncEngine_release_loop(VALUE self)
{
  AE_TRACE2();

  ae_release_loop();

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



void Init_asyncengine_ext()
{
  AE_TRACE();

  mAsyncEngine = rb_define_module("AsyncEngine");
  cAsyncEngineHandle = rb_define_class_under(mAsyncEngine, "Handle", rb_cObject);
  eAsyncEngineError = rb_define_class_under(mAsyncEngine, "Error", rb_eStandardError);

  rb_define_module_function(mAsyncEngine, "run_loop", AsyncEngine_run_loop, 1);
  rb_define_module_function(mAsyncEngine, "release_loop", AsyncEngine_release_loop, 0);
  rb_define_module_function(mAsyncEngine, "running?", AsyncEngine_is_running, 0);
  rb_define_module_function(mAsyncEngine, "check_running", AsyncEngine_check_running, 0);
  rb_define_module_function(mAsyncEngine, "num_uv_active_handles", AsyncEngine_num_uv_active_handles, 0);
  rb_define_module_function(mAsyncEngine, "num_uv_active_reqs", AsyncEngine_num_uv_active_reqs, 0);

  att_thread = rb_intern("@_thread");
  att_handles = rb_intern("@_handles");
  att_blocks = rb_intern("@_blocks");
  att_next_ticks = rb_intern("@_next_ticks");
  const_UV_ERRNOS = rb_intern("UV_ERRNOS");

  method_destroy = rb_intern("destroy");
  method_clear = rb_intern("clear");
  method_next_tick = rb_intern("next_tick");

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
