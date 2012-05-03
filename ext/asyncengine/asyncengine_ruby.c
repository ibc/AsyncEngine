#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_timer.h"
#include "ae_next_tick.h"
#include "ae_udp.h"


static uv_prepare_t *av_uv_prepare;


static
void prepare_callback(uv_prepare_t* handle, int status)
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

  ret = uv_run(uv_default_loop());

  // Referece again av_uv_prepare and av_uv_idle_next_tick so they can be properly closed.
  uv_ref(uv_default_loop());
  uv_close((uv_handle_t *)av_uv_prepare, ae_handle_close_callback_0);
  //uv_ref(uv_default_loop());
  //uv_close((uv_handle_t *)ae_next_tick_uv_idle, ae_handle_close_callback_0);

  if (! ret) {
    return Qtrue;
  }
  else
    return Qfalse;
}


VALUE AsyncEngine_c_run(VALUE self)
{
  AE_TRACE();
  av_uv_prepare = ALLOC(uv_prepare_t);

  uv_prepare_init(uv_default_loop(), av_uv_prepare);
  uv_prepare_start(av_uv_prepare, prepare_callback);
  uv_unref(uv_default_loop());

  return rb_thread_call_without_gvl(run_uv_without_gvl, NULL, RUBY_UBF_IO, NULL);
}


/*
 * Returns the number of handlers in the loop.
 * NOTE: The returned number is the real number of handles minus 1 (the av_uv_prepare handle).
 */
VALUE AsyncEngine_num_handles(VALUE self)
{
  return INT2FIX(uv_loop_refcount(uv_default_loop()) - 1);
}




void Init_asyncengine_ext()
{
  AE_TRACE();

  mAsyncEngine = rb_define_module("AsyncEngine");

  rb_define_module_function(mAsyncEngine, "_c_run", AsyncEngine_c_run, 0);
  rb_define_module_function(mAsyncEngine, "num_handles", AsyncEngine_num_handles, 0);

  init_ae_handle_common();
  init_ae_timer();
  init_ae_next_tick();
  init_ae_udp();
}