#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_next_tick.h"


static ID method_execute_next_ticks;


void init_ae_next_tick()
{
  AE_TRACE();

  rb_define_module_function(mAsyncEngine, "_c_next_tick", AsyncEngine_c_next_tick, 0);

  method_execute_next_ticks = rb_intern("execute_next_ticks");
}


static
void execute_next_tick_callback_with_gvl()
{
  AE_TRACE();

  rb_funcall2(mAsyncEngine, method_execute_next_ticks, 0, NULL);
}


static
void _uv_idle_callback(uv_idle_t* handle, int status)
{
  AE_TRACE();

  uv_idle_stop(handle);
  uv_unref((uv_handle_t *)handle);

  rb_thread_call_with_gvl(execute_next_tick_callback_with_gvl, NULL);
}


VALUE AsyncEngine_c_next_tick(VALUE self)
{
  AE_TRACE();

  if (! uv_is_active((uv_handle_t *)ae_next_tick_uv_idle)) {
    uv_idle_start(ae_next_tick_uv_idle, _uv_idle_callback);
    uv_ref((uv_handle_t *)ae_next_tick_uv_idle);
  }
  return Qtrue;
}