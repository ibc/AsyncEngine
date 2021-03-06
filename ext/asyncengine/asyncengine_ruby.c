#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_utils.h"
#include "ae_ip_utils.h"
#include "ae_call_from_other_thread.h"
#include "ae_next_tick.h"
#include "ae_timer.h"
#include "ae_udp.h"
#include "ae_tcp.h"
#include "ae_dns.h"


static VALUE mProcess;
static VALUE mKernel;

static VALUE AE_thread;
static int AE_pid;
static VALUE AE_mutex;

static ID att_handles;
static ID att_procs;
static ID att_next_tick_procs;
static ID att_call_from_other_thread_procs;
static ID att_user_error_handler;
static ID att_exit_error;
static ID att_on_exit_procs;

static ID method_destroy;
static ID method_clear;
static ID method_next_tick;
static ID method_call_from_other_thread;
static ID method_raise;
static ID method_call;

static uv_async_t* ae_ubf_uv_async;


/** Pre-declaration of static functions. */

static VALUE uv_run_without_gvl(void);
static void ae_ubf(void);
static void ae_ubf_uv_async_callback(uv_async_t* handle, int status);
static void ae_release_loop(void);
static int destroy_handle(VALUE key, VALUE handle, VALUE in);
static VALUE call_handle_destroy(VALUE handle);
static int ae_is_running_thread(void);
static VALUE ae_handle_error_with_rb_protect(VALUE error);


/** TODO: Temporal functions for debugging: libuv active handlers and requests counters. */

static
int ae_uv_num_active_handlers(void)
{
  AE_TRACE();

  if (AE_status == AE_STOPPED)
    return 0;

  return AE_uv_loop->active_handles;
}


static
VALUE AsyncEngine_num_uv_active_handles(VALUE self)
{
  AE_TRACE();

  return INT2FIX(ae_uv_num_active_handlers());
}


static
int ae_uv_num_active_reqs(void)
{
  AE_TRACE();

  if (AE_status == AE_STOPPED)
    return 0;

  ngx_queue_t *q;
  int count = 0;
  ngx_queue_foreach(q, &AE_uv_loop->active_reqs)
    count++;
  return count;
}


static
VALUE AsyncEngine_num_uv_active_reqs(VALUE self)
{
  AE_TRACE();

  return INT2FIX(ae_uv_num_active_reqs());
}


/** AE.run method. */

static
VALUE AsyncEngine_run(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();
  VALUE proc, captured_error, on_exit_procs;
  int r, i;

  AE_RB_CHECK_NUM_ARGS(0,1);
  AE_RB_ENSURE_BLOCK_OR_PROC(1, proc);

  if (AE_status == AE_RUNNING && getpid() != AE_pid)
    rb_raise(eAsyncEngineError, "cannot run AsyncEngine from a forked process while already running");

  // If already running pass the Proc to the reactor and return true.
  if (AE_status == AE_RUNNING) {
    if (ae_is_running_thread())
      rb_funcall2(mAsyncEngine, method_next_tick, 1, &proc);
    else
      rb_funcall2(mAsyncEngine, method_call_from_other_thread, 1, &proc);
    return Qtrue;
  }

  // Acquire the mutex.
  AE_DEBUG("pre rb_mutex_lock(AE_mutex)");
  rb_mutex_lock(AE_mutex);
  AE_DEBUG("post rb_mutex_lock(AE_mutex)");

  // Ensure AE_status is AE_STOPPED (for multi-thread exotic cases).
  if (AE_status == AE_RELEASING) {
    AE_DEBUG2("AE_mutex locked while in RELEASING status, returning");
    rb_mutex_unlock(AE_mutex);
    return Qnil;
  }
  else if (AE_status == AE_RUNNING) {
    AE_DEBUG2("AE_mutex locked while in RUNNING status, returning");
    rb_mutex_unlock(AE_mutex);
    return Qnil;
  }

  /* Set the UV loop. */
  AE_uv_loop = uv_default_loop();

  // Mark current thread and PID.
  AE_thread = rb_thread_current();
  AE_pid = getpid();

  // Get the VALUEs for @_handles and @_procs (faster).
  AE_handles = rb_ivar_get(mAsyncEngine, att_handles);
  AE_procs = rb_ivar_get(mAsyncEngine, att_procs);

  AE_ASSERT(AE_status == AE_STOPPED);

  /* Load the UV idle (AE.next_tick) and UV async (AE.call_from_other_thread) now. */
  load_ae_next_tick_uv_idle();
  load_ae_call_from_other_thread();

  /* Load the UV async for the ae_ubf() function. */
  AE_ASSERT(ae_ubf_uv_async == NULL); // TODO: testing.
  ae_ubf_uv_async = ALLOC(uv_async_t);
  r = uv_async_init(AE_uv_loop, ae_ubf_uv_async, ae_ubf_uv_async_callback);
  AE_ASSERT(r == 0);

  /* Initial status. */
  AE_status = AE_RUNNING;

  /* Pass the given Proc to the reactor via next_tick. */
  rb_funcall2(mAsyncEngine, method_next_tick, 1, &proc);

  // TODO: for testing.
  AE_ASSERT(ae_uv_num_active_reqs() == 0);

  AE_DEBUG("UV loop starts...");
  // uv_run() will block here until ae_release_loop() is called.
  rb_thread_call_without_gvl(uv_run_without_gvl, NULL, ae_ubf, NULL);
  AE_DEBUG("UV loop terminates");

  // TODO: for testing.
  AE_ASSERT(ae_uv_num_active_handlers() == 0);
  AE_ASSERT(ae_uv_num_active_reqs() == 0);

  // Unset the AE_uv_loop, AE_thread and AE_pid.
  AE_uv_loop = NULL;
  AE_thread = Qnil;
  AE_pid = -1;

  // Get the captured error in @_exit_error (if any) and clean @_exit_error.
  captured_error = rb_ivar_get(mAsyncEngine, att_exit_error);
  rb_ivar_set(mAsyncEngine, att_exit_error, Qnil);

  // Now yes, set the status to AE_STOPPED.
  AE_status = AE_STOPPED;

  // Release the mutex.
  AE_DEBUG("pre rb_mutex_unlock(AE_mutex)");
  rb_mutex_unlock(AE_mutex);
  AE_DEBUG("post rb_mutex_unlock(AE_mutex)");

  /*
   * Run the procs within @_on_exit_procs in order by passing the captured error
   * (is any) as argument.
   */
  on_exit_procs = rb_ivar_get(mAsyncEngine, att_on_exit_procs);
  rb_ivar_set(mAsyncEngine, att_on_exit_procs, rb_ary_new());
  for(i=0 ; i<RARRAY_LEN(on_exit_procs) ; i++) {
    ae_proc_call_1(rb_ary_entry(on_exit_procs, i), captured_error);
  }

  /*
   * If an exception/error has been captured by the exception manager, raise it now
   * but just in case it's an Exception object. Thread#kill does not set the error
   * to an Exception so Ruby will do the rest.
   */
  if (! NIL_P(captured_error)) {
    if (rb_obj_is_kind_of(captured_error, rb_eException) == Qtrue) {
      AE_DEBUG2("raising captured error (class: %s)", rb_obj_classname(captured_error));
      rb_funcall2(mKernel, method_raise, 1, &captured_error);
    }
    else
      AE_DEBUG2("AsyncEngine thread killed by Thread#kill");
  }

  return Qtrue;
}


static
VALUE uv_run_without_gvl(void)
{
  AE_TRACE();

  uv_run(AE_uv_loop);
}


static
void ae_ubf(void)
{
  AE_TRACE();
  int r;

  if (AE_status != AE_RUNNING)
    return;

  /*
   * When a signal is received by a Ruby process running a blocking code (without GVL)
   * Ruby calls the ubf() function. But this ubf() function could be called also from
   * other thread (i.e. ae_tread.kill) so we don't know if the ubf() is being executing
   * in AE thread, nor if it has been called due a received signal or a Thread#kill.
   * Therefore, do nothing but check interrupts in Ruby land via a thread safe uv_async.
   */

  r = uv_async_send(ae_ubf_uv_async);
  AE_ASSERT(r == 0);
}


static
void ae_ubf_uv_async_callback(uv_async_t* handle, int status)
{
  AE_TRACE();

  if (AE_status != AE_RUNNING)
    return;

  AE_DEBUG("ae_take_gvl_and_run_with_error_handler(rb_thread_check_ints)");
  ae_take_gvl_and_run_with_error_handler(rb_thread_check_ints);
}


/** AE.release_loop private method. */

static
VALUE AsyncEngine_release_loop(VALUE self)
{
  AE_TRACE();

  ae_release_loop();
  return Qnil;
}


static
void ae_release_loop(void)
{
  AE_TRACE();

  /*
   * Avoid this function to be called twice. When AE.stop is called it calls this method within
   * a next_tick() or call_from_other_thread(), so when arriving here AE could already be
   * releasing (by a previous AE.stop, exception or whatever).
   */
  switch(AE_status) {
    case AE_RUNNING:
      break;
    case AE_STOPPED:
      AE_DEBUG("AE_status == AE_STOPPED, returning");
      return;
      break;
    case AE_RELEASING:
      AE_DEBUG("AE_status == AE_RELEASING, returning");
      return;
      break;
  }

  // No more handles can be created from now.
  AE_status = AE_RELEASING;

  // Clear @_next_tick_procs.
  rb_ary_clear(rb_ivar_get(mAsyncEngine, att_next_tick_procs));

  // Clear @_call_from_other_thread_procs.
  rb_ary_clear(rb_ivar_get(mAsyncEngine, att_call_from_other_thread_procs));

  /*
   * Run #destroy() for each existing AE handle. Note that #destroy() method
   * is called safely in every AE handle by ignoring any exception/error.
   * */
  rb_hash_foreach(AE_handles, destroy_handle, Qnil);

  /* Close AE internal handles. If the async handles are not closed uv_run() won't exit. */
  unload_ae_next_tick_uv_idle();
  unload_ae_call_from_other_thread();
  AE_ASSERT(ae_ubf_uv_async != NULL); // TODO: testing.
  AE_CLOSE_UV_HANDLE(ae_ubf_uv_async);
  ae_ubf_uv_async = NULL;

  // Clear @_procs.
  rb_funcall2(AE_procs, method_clear, 0, NULL);
}


static
int destroy_handle(VALUE key, VALUE handle, VALUE in)
{
  AE_TRACE();

  ae_run_with_error_handler(call_handle_destroy, handle);
  return 0;
}


static
VALUE call_handle_destroy(VALUE handle)
{
  AE_TRACE();

  return rb_funcall2(handle, method_destroy, 0, NULL);
}


/** AE.is_running? method. */

static
VALUE AsyncEngine_is_running(VALUE self)
{
  AE_TRACE();

  return (AE_status == AE_RUNNING ? Qtrue : Qfalse);
}


/** ae_is_running_thread() and AE.is_running_thread methods. */

static
int ae_is_running_thread(void)
{
  AE_TRACE();

  return (rb_thread_current() == AE_thread ? 1 : 0);
}


static
VALUE AsyncEngine_is_running_thread(VALUE self)
{
  AE_TRACE();

  return (ae_is_running_thread() ? Qtrue : Qfalse);
}


/** ae_handle_error function. */

void ae_handle_error(VALUE error)
{
  AE_TRACE();
  VALUE error2;
  int error_tag;

  if ((! NIL_P(rb_ivar_get(mAsyncEngine, att_user_error_handler))) && rb_obj_is_kind_of(error, rb_eStandardError) == Qtrue) {
    rb_protect(ae_handle_error_with_rb_protect, error, &error_tag);
    if (error_tag) {
      error2 = rb_errinfo();
      rb_set_errinfo(Qnil);
      AE_DEBUG2("error %s rescued with rb_protect() while running the user error handler", rb_obj_classname(error2));
      rb_ivar_set(mAsyncEngine, att_exit_error, error2);
      ae_release_loop();
    }
  }
  else {
    AE_DEBUG2("error %s occurred with rb_protect(), releasing...", rb_obj_classname(error));
    /*
     * error could return Fixnum 8 when AE thread is killed with Thread#kill:
     *   https://github.com/ibc/AsyncEngine/issues/4
     * In this case convert to Interrupt.
     */
    if (FIXNUM_P(error))
      error = rb_exc_new2(rb_eInterrupt, "Thread killed (AsyncEngine workaround for https://github.com/ibc/AsyncEngine/issues/4)");

    rb_ivar_set(mAsyncEngine, att_exit_error, error);
    ae_release_loop();
  }
}


static
VALUE ae_handle_error_with_rb_protect(VALUE error)
{
  AE_TRACE();

  return rb_funcall2(rb_ivar_get(mAsyncEngine, att_user_error_handler), method_call, 1, &error);
}


/** AE.check_status method. */

static
VALUE AsyncEngine_check_status(VALUE self)
{
  AE_TRACE();

  AE_CHECK_STATUS();
  return Qtrue;
}


/** Function that generates AE::UV_ERRORS hash reading UV error list. */

static
VALUE generate_AE_UV_ERRORS_hash(void)
{
  AE_TRACE();
  VALUE hash = rb_hash_new();
  VALUE uv_error;
  VALUE args[3];

#define _AE_FILL_UV_ERRORS(val, name, s)  \
  args[0] = INT2FIX(val);  \
  args[1] = ID2SYM(rb_intern(#name));  \
  args[2] = rb_str_new2(s);  \
  uv_error = rb_class_new_instance(3, args, eAsyncEngineUvError);  \
  rb_hash_aset(hash, INT2FIX(val), uv_error);

  UV_ERRNO_MAP(_AE_FILL_UV_ERRORS)
#undef _AE_FILL_UV_ERRORS

  return hash;
}




void Init_asyncengine_ext()
{
  AE_TRACE();

  mProcess = rb_define_module("Process");
  mKernel = rb_define_module("Kernel");

  mAsyncEngine = rb_define_module("AsyncEngine");
  cAsyncEngineHandle = rb_define_class_under(mAsyncEngine, "Handle", rb_cObject);
  eAsyncEngineError = rb_define_class_under(mAsyncEngine, "Error", rb_eStandardError);
  eAsyncEngineUvError = rb_define_class_under(mAsyncEngine, "UvError", eAsyncEngineError);
  eAsyncEngineNotRunningError = rb_define_class_under(mAsyncEngine, "NotRunningError", eAsyncEngineError);

  rb_define_module_function(mAsyncEngine, "run", AsyncEngine_run, -1);
  rb_define_module_function(mAsyncEngine, "release_loop", AsyncEngine_release_loop, 0);
  rb_define_module_function(mAsyncEngine, "running?", AsyncEngine_is_running, 0);
  rb_define_module_function(mAsyncEngine, "running_thread?", AsyncEngine_is_running_thread, 0);
  rb_define_module_function(mAsyncEngine, "check_status", AsyncEngine_check_status, 0);  // TODO: Should be removed since all will be in C.
  // TODO: temporal methods (for debugging).
  rb_define_module_function(mAsyncEngine, "num_uv_active_handles", AsyncEngine_num_uv_active_handles, 0);
  rb_define_module_function(mAsyncEngine, "num_uv_active_reqs", AsyncEngine_num_uv_active_reqs, 0);

  att_handles = rb_intern("@_handles");
  att_procs = rb_intern("@_procs");
  att_next_tick_procs = rb_intern("@_next_tick_procs");
  att_call_from_other_thread_procs = rb_intern("@_call_from_other_thread_procs");
  att_exit_error = rb_intern("@_exit_error");
  att_user_error_handler = rb_intern("@_user_error_handler");
  att_on_exit_procs = rb_intern("@_on_exit_procs");

  method_destroy = rb_intern("destroy");
  method_clear = rb_intern("clear");
  method_next_tick = rb_intern("next_tick");
  method_call_from_other_thread = rb_intern("call_from_other_thread");
  method_raise = rb_intern("raise");
  method_call = rb_intern("call");

  init_rb_utilities();
  init_ae_handle_common();
  init_ae_utils();
  init_ae_ip_utils();
  init_ae_call_from_other_thread();
  init_ae_next_tick();
  init_ae_timer();
  init_ae_udp();
  init_ae_tcp();
  init_ae_dns();

  ae_ubf_uv_async = NULL;

  AE_UV_ERRORS = generate_AE_UV_ERRORS_hash();
  rb_const_set(mAsyncEngine, rb_intern("UV_ERRORS"), AE_UV_ERRORS);

  AE_mutex = rb_mutex_new();
  rb_gc_register_address(&AE_mutex);
  AE_uv_loop = NULL;
  AE_status = AE_STOPPED;
  AE_pid = -1;
}
