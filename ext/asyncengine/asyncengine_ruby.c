#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_utils.h"
#include "ae_ip_utils.h"
#include "ae_call_from_other_thread.h"
#include "ae_next_tick.h"
#include "ae_timer.h"
#include "ae_udp.h"
#include "ae_tcp.h"


static VALUE AE_thread;

static ID att_handles;
static ID att_blocks;
static ID att_next_ticks;
static ID att_call_from_other_thread_procs;

static ID const_UV_ERRNOS;

static ID method_destroy;
static ID method_clear;
static ID method_next_tick;
static ID method_call_from_other_thread;

enum status {
  STOPPED = 1,
  RUNNING = 2,
  RELEASING = 3
};

static enum status AE_status;


/** Pre-declaration of static functions. TODO: Add more and so... */

static void ae_release_loop(void);
static int ae_is_running_thread(void);


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

  // TODO: Exit if RELEASING, creo.

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

  // TODO: test...
  if (AE_status == RELEASING)
    return;

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


VALUE AsyncEngine_run_loop(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();

  VALUE _rb_block;

  AE_RB_CHECK_NUM_ARGS(0,1);
  AE_RB_GET_BLOCK_OR_PROC(1, _rb_block);

  if (NIL_P(_rb_block))
    rb_raise(rb_eArgError, "no block given");

  // TODO: Check process (and set  @_pid = nil when terminating the loop).
  // raise AsyncEngine::Error, "cannot run AsyncEngine from a forked process"  unless Process.pid == @_pid

  // TODO: To find a crash in bugs.rb. But in fact the problem is that we MUST NOT
  // allow being here if AE_status != STOPPED. In that case let's raise and that's all or wait (mutex?).
  int i;
  if (AE_status == RELEASING) {
    // If current thread calls AE.run while AE was in RELEASING state, schedule another thread
    // and wait a bit.
    for(i=0; i<10; i++) {
      printf("--- waiting while AE_status == RELEASING (%d)---\n", i);
      rb_thread_schedule();
      if (AE_status != RELEASING)
        break;
    }
    if (AE_status == RELEASING)
      rb_raise(eAsyncEngineError, "AsyncEngine still in RELEASING status...");
  }
  AE_ASSERT(AE_status != RELEASING);

  // If already running pass the block to the reactor.
  if (AE_status == RUNNING) {
    if (ae_is_running_thread())
      rb_funcall2(mAsyncEngine, method_next_tick, 1, &_rb_block);
    else
      rb_funcall2(mAsyncEngine, method_call_from_other_thread, 1, &_rb_block);
    return Qtrue;
  }

    /* Set the UV loop. */
  AE_uv_loop = uv_default_loop();

  /* Initial status and flags. */
  AE_status = RUNNING;

  // Mark current thread as AE thread.
  AE_thread = rb_thread_current();  // TODO: Needed?

  // Get the VALUEs for @_handles and @_blocks (faster).
  // NOTE: AE_next_ticks cannot be loaded here since its value is changed in runtime.
  AE_handles = rb_ivar_get(mAsyncEngine, att_handles);
  AE_blocks = rb_ivar_get(mAsyncEngine, att_blocks);

  AE_UV_ERRNOS = rb_const_get(mAsyncEngine, const_UV_ERRNOS);

  /* Load the UV idle (AE.next_tick) and UV async (AE.call_from_other_thread) now. */
  load_ae_next_tick_uv_idle();
  load_ae_call_from_other_thread();

  /* Pass the given block to the reactor via next_tick. */
  rb_funcall2(mAsyncEngine, method_next_tick, 1, &_rb_block);

  // TODO: for testing.
  AE_ASSERT(ae_uv_num_active_reqs() == 0);

  AE_DEBUG("UV loop starts...");
  // uv_run() will block here until ae_release_loop() is called.
  rb_thread_call_without_gvl(run_uv_without_gvl, NULL, ae_ubf, NULL);
  AE_DEBUG("UV loop terminates");

  // TODO: Idem que abajo, bugs.rb llega aquí en otro estado.
  if (AE_status != RELEASING)  printf("------- after uv_run AE_status should be RELEASING but it's %d\n", AE_status);
  AE_ASSERT(AE_status == RELEASING);

  // TODO: for testing.
  // TODO: Al cambiar el diseño por el tema del async que mantiene el loop, bugs.rb hace
  // explotar este assert:
  AE_ASSERT(ae_uv_num_active_handlers() == 0);
  AE_ASSERT(ae_uv_num_active_reqs() == 0);

  // Unset the AE_uv_loop and AE_thread.
  AE_uv_loop = NULL;
  AE_thread = Qnil;

  // Now yes, set the status to CLOSED.
  AE_status = STOPPED;

  return Qtrue;
}


static
VALUE destroy_handle_with_rb_protect(VALUE handle)
{
  AE_TRACE();

  rb_funcall2(handle, method_destroy, 0, NULL);
  return Qnil;
}


static
int destroy_handle(VALUE key, VALUE handle, VALUE in)
{
  AE_TRACE();

  int error_tag;

  rb_protect(destroy_handle_with_rb_protect, handle, &error_tag);
  if (error_tag) {
    rb_set_errinfo(Qnil);
    AE_WARN("error rescued with rb_protect() while destroying the handle");  // TODO: for testing
  }

  return 0;
}


static
void ae_release_loop(void)
{
  AE_TRACE2();

  // TODO: En bugs.rb entramos aquí con AE_status == STOPPED. Mal rollo, veo en los logs:
//   AE_TRACE2: asyncengine_ruby.c:294:AsyncEngine_release_loop
//   AE_TRACE2: asyncengine_ruby.c:254:ae_release_loop
//   --- AE_status = 2
//   AE_TRACE2: ae_timer.c:263:AsyncEngineTimer_destroy
//   AE_TRACE2: asyncengine_ruby.c:294:AsyncEngine_release_loop
//   AE_TRACE2: asyncengine_ruby.c:254:ae_release_loop
//   --- AE_status = 1
//   AE_ASSERT: asyncengine_ruby.c:257:ae_release_loop:: assertion `AE_status == RUNNING' failed
  //
  // Ok, creo que es porque al llamar a AE.stop no verifico si ya está en RELEASING y tal, tengo que hacerlo,
  // al igual que en todos los creadores de handles mirar el status y devolver false silenciosamente si AE
  // está RELEASING, y exception si está STOPPED.

  // TODO: Workaround cerdo temporal: return if RELEASING or STOPPED.
  if (AE_status == STOPPED || AE_status == RELEASING)  printf("--- AE_status = %d\n", AE_status);

  AE_ASSERT(AE_status == RUNNING);

  // No more handles can be created now.
  AE_status = RELEASING;

  // Clear @_next_ticks.
  rb_ary_clear(rb_ivar_get(mAsyncEngine, att_next_ticks));

  // Clear @_call_from_other_thread_procs.
  rb_ary_clear(rb_ivar_get(mAsyncEngine, att_call_from_other_thread_procs));

  /*
   * Run #destroy() for each existing AE handle. Note that #destroy() method
   * is called safely in every AE handle by ignoring any exception/error.
   * */
  rb_hash_foreach(AE_handles, destroy_handle, Qnil);

  // Clear @_blocks.
  // TODO: Probably this would cause some "error" when some callback try to get
  // its block... it should not happen anyhow.
  rb_funcall2(AE_blocks, method_clear, 0, NULL);

  /* Close AE internal handles. If the async handle is not closed uv_run() won't exit. */
  unload_ae_next_tick_uv_idle();
  unload_ae_call_from_other_thread();
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

  return (AE_status == RUNNING);
}


VALUE AsyncEngine_is_running(VALUE self)
{
  AE_TRACE();

  return (ae_is_running() ? Qtrue : Qfalse);
}


static
int ae_is_running_thread(void)
{
  AE_TRACE();

  return (rb_thread_current() == AE_thread ? 1 : 0);
}


VALUE AsyncEngine_is_running_thread(VALUE self)
{
  AE_TRACE();

  return (ae_is_running_thread() ? Qtrue : Qfalse);
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


int ae_is_releasing(void)
{
  AE_TRACE();

  return (AE_status == RELEASING);
}


void Init_asyncengine_ext()
{
  AE_TRACE();

  mAsyncEngine = rb_define_module("AsyncEngine");
  cAsyncEngineHandle = rb_define_class_under(mAsyncEngine, "Handle", rb_cObject);
  eAsyncEngineError = rb_define_class_under(mAsyncEngine, "Error", rb_eStandardError);

  rb_define_module_function(mAsyncEngine, "run_loop", AsyncEngine_run_loop, -1);
  rb_define_module_function(mAsyncEngine, "release_loop", AsyncEngine_release_loop, 0);
  rb_define_module_function(mAsyncEngine, "running?", AsyncEngine_is_running, 0);
  rb_define_module_function(mAsyncEngine, "running_thread?", AsyncEngine_is_running_thread, 0);
  rb_define_module_function(mAsyncEngine, "check_running", AsyncEngine_check_running, 0);
  rb_define_module_function(mAsyncEngine, "num_uv_active_handles", AsyncEngine_num_uv_active_handles, 0);
  rb_define_module_function(mAsyncEngine, "num_uv_active_reqs", AsyncEngine_num_uv_active_reqs, 0);

  att_handles = rb_intern("@_handles");
  att_blocks = rb_intern("@_blocks");
  att_next_ticks = rb_intern("@_next_ticks");
  att_call_from_other_thread_procs = rb_intern("@_call_from_other_thread_procs");
  const_UV_ERRNOS = rb_intern("UV_ERRNOS");

  method_destroy = rb_intern("destroy");
  method_clear = rb_intern("clear");
  method_next_tick = rb_intern("next_tick");
  method_call_from_other_thread = rb_intern("call_from_other_thread");

  init_ae_handle_common();
  init_ae_utils();
  init_ae_ip_utils();
  init_rb_utilities();
  init_ae_call_from_other_thread();
  init_ae_next_tick();
  init_ae_timer();
  init_ae_udp();
  init_ae_tcp();

  AE_status = STOPPED;
  AE_uv_loop = NULL;
}
