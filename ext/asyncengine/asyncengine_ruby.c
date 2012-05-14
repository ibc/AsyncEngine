#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_timer.h"
#include "ae_next_tick.h"
#include "ae_udp.h"
#include "ae_ip_utils.h"


static _uv_is_running;
static uv_prepare_t *ae_uv_prepare;


static
void _uv_prepare_callback(uv_prepare_t* handle, int status)
{
  AE_TRACE();

  // Check received interruptions in Ruby land.
  rb_thread_call_with_gvl(rb_thread_check_ints, NULL);
}


static
VALUE stop_uv_without_gvl(void* param)
{
  AE_TRACE();

  int ret;

  // TODO: buffff
  if (! _uv_is_running) {
    printf("NOTICE: stop_uv_without_gvl(): uv_run is NOT running !!!\n");
    return Qfalse;
  }

  // Referece again ae_uv_prepare so it can be properly closed.
  uv_ref(uv_default_loop());
  uv_close((uv_handle_t *)ae_uv_prepare, ae_uv_handle_close_callback);

  assert(! uv_run(uv_default_loop()));

  _uv_is_running = 0;

  return Qtrue;
}


VALUE AsyncEngine_c_stop(VALUE self)
{
  AE_TRACE();

  return rb_thread_call_without_gvl(stop_uv_without_gvl, NULL, RUBY_UBF_IO, NULL);
}


static
VALUE run_uv_without_gvl(void* param)
{
  AE_TRACE();

  int ret;

  // TODO: buffff
  if (_uv_is_running) {
    printf("CRITICAL: run_uv_without_gvl(): uv_run is already running !!!\n");
    return Qfalse;
  }

  /* Load the AE prepare handle */
  ae_uv_prepare = ALLOC(uv_prepare_t);
  uv_prepare_init(uv_default_loop(), ae_uv_prepare);
  uv_prepare_start(ae_uv_prepare, _uv_prepare_callback);
  uv_unref(uv_default_loop());

  _uv_is_running = 1;

  assert(! uv_run(uv_default_loop()));

  printf("DBG: run_uv_without_gvl(): uv_run exits\n");
  printf("DBG: run_uv_without_gvl(): [1] uv_loop_refcount(uv_default_loop()): %d\n", uv_loop_refcount(uv_default_loop()));

  // If AsyncEngine has been properly stopped due to the lack of active handles,
  // then free the uv_prepare handle.
  // But if it has been terminated by AE.stop (so _uv_is_running is already 0)
  // then the work is already done.
  if (_uv_is_running) {
    // Referece again ae_uv_prepare so it can be properly closed.
    uv_ref(uv_default_loop());
    uv_close((uv_handle_t *)ae_uv_prepare, ae_uv_handle_close_callback);
  }
  printf("DBG: run_uv_without_gvl(): setting _uv_is_running = 0\n");
  printf("DBG: run_uv_without_gvl(): [2] uv_loop_refcount(uv_default_loop()): %d\n", uv_loop_refcount(uv_default_loop()));
  _uv_is_running = 0;

  return Qtrue;
}


static
VALUE terminate_uv(void)
{
  AE_TRACE();

  printf("NOTICE: terminate_uv() !!!\n");
  if (rb_thread_interrupted(rb_thread_current()))
    printf("INFO: terminate_uv(): rb_thread_interrupted() => true\n");

  // NOTE: En teoría terminate_uv() se ejecuta sin GVL (ver rb_thread_blocking_region()), pero
  // al ejecutar esto me da error "[BUG] rb_thread_call_with_gvl: called by a thread which has GVL."
  //return rb_thread_call_with_gvl(terminate_uv_with_gvl, NULL);

  return rb_funcall2(mAsyncEngine, rb_intern("ae_thread_killed"), 0, NULL);
}


VALUE AsyncEngine_c_run(VALUE self)
{
  AE_TRACE();

  //return rb_thread_call_without_gvl(run_uv_without_gvl, NULL, RUBY_UBF_IO, NULL);
  return rb_thread_call_without_gvl(run_uv_without_gvl, NULL, terminate_uv, NULL);
}


VALUE AsyncEngine_is_running(VALUE self)
{
  AE_TRACE();

  if (_uv_is_running)
    return Qtrue;
  else
    return Qfalse;
}


/*
 * Returns the number of handlers in the loop.
 */
VALUE AsyncEngine_num_handles(VALUE self)
{
  AE_TRACE();

  return INT2FIX(uv_loop_refcount(uv_default_loop()));
}




void Init_asyncengine_ext()
{
  AE_TRACE();

  mAsyncEngine = rb_define_module("AsyncEngine");

  rb_define_module_function(mAsyncEngine, "_c_run", AsyncEngine_c_run, 0);
  rb_define_module_function(mAsyncEngine, "_c_stop", AsyncEngine_c_stop, 0);
  rb_define_module_function(mAsyncEngine, "running?", AsyncEngine_is_running, 0);
  rb_define_module_function(mAsyncEngine, "num_handles", AsyncEngine_num_handles, 0);

  init_ae_handle_common();
  init_ae_timer();
  init_ae_next_tick();
  init_ae_udp();
  init_ae_ip_utils();

  _uv_is_running = 0;
}