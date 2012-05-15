#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_timer.h"
#include "ae_next_tick.h"
#include "ae_udp.h"
#include "ae_ip_utils.h"


static _uv_is_running;

static uv_prepare_t *ae_uv_prepare;

static ID method_stop;


static
void _uv_prepare_callback(uv_prepare_t* handle, int status)
{
  AE_TRACE();

  // Check received interruptions in Ruby land.
  rb_thread_call_with_gvl(rb_thread_check_ints, NULL);
}


static
VALUE run_uv_without_gvl(void* param)
{
  AE_TRACE();

  int ret;

  if (_uv_is_running) {
    AE_TRACE3("uv_run is already running, return false");
    return Qfalse;
  }

  /* Load the AE prepare handle */
  ae_uv_prepare = ALLOC(uv_prepare_t);
  uv_prepare_init(uv_default_loop(), ae_uv_prepare);
  uv_prepare_start(ae_uv_prepare, _uv_prepare_callback);
  uv_unref(uv_default_loop());

  /* Load the AE next_tick idle handle */
  load_ae_next_tick_uv_idle();

  _uv_is_running = 1;

  assert(! uv_run(uv_default_loop()));

  AE_TRACE3("uv_run() exits");

  // Referece again ae_uv_prepare so it can be properly closed.
  uv_ref(uv_default_loop());
  uv_close((uv_handle_t *)ae_uv_prepare, ae_uv_handle_close_callback);

  // Same for the av_uv_idle_next_tick handle.
  unload_ae_next_tick_uv_idle();

  _uv_is_running = 0;

  AE_TRACE3("function terminates");

  return Qtrue;
}


static
void ubf_ae_thread_killed(void)
{
  AE_TRACE();
  AE_TRACE3("rb_funcall2(mAsyncEngine, method_stop, 0, NULL) starts");

  // Call AE.stop which closes all the active UV handles and that allows uv_run() to terminate.
  // NOTE: We have the GVL now (ruby_thread_has_gvl_p() says that) but, do we have it in any case?
  rb_funcall2(mAsyncEngine, method_stop, 0, NULL);

  AE_TRACE3("rb_funcall2(mAsyncEngine, method_stop, 0, NULL) terminates");

  AE_TRACE3("waiting for _uv_is_running == 0");
  while(_uv_is_running) {
    printf(".");
    rb_thread_schedule();
  }
  printf("\n");

  AE_TRACE3("function terminates");
}


VALUE AsyncEngine_c_run(VALUE self)
{
  AE_TRACE();

  return rb_thread_call_without_gvl(run_uv_without_gvl, NULL, ubf_ae_thread_killed, NULL);
  //return rb_thread_call_without_gvl(run_uv_without_gvl, NULL, RUBY_UBF_IO, NULL);
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


VALUE AsyncEngine_has_gvl(VALUE self)
{
  return INT2FIX(ruby_thread_has_gvl_p());
}


void Init_asyncengine_ext()
{
  AE_TRACE();

  mAsyncEngine = rb_define_module("AsyncEngine");

  rb_define_module_function(mAsyncEngine, "_c_run", AsyncEngine_c_run, 0);
  rb_define_module_function(mAsyncEngine, "running?", AsyncEngine_is_running, 0);
  rb_define_module_function(mAsyncEngine, "num_handles", AsyncEngine_num_handles, 0);

  rb_define_module_function(mAsyncEngine, "gvl?", AsyncEngine_has_gvl, 0);

  method_stop = rb_intern("stop");

  init_ae_handle_common();
  init_ae_timer();
  init_ae_next_tick();
  init_ae_udp();
  init_ae_ip_utils();

  _uv_is_running = 0;
}