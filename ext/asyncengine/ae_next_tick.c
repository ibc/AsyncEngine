#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_next_tick.h"


static ID id_method_execute_next_ticks;


void init_ae_next_tick()
{
  ae_next_tick_uv_idle = ALLOC(uv_idle_t);
  uv_idle_init(uv_default_loop(), ae_next_tick_uv_idle);
  uv_unref(uv_default_loop());

  rb_define_module_function(mAsyncEngine, "_c_next_tick", AsyncEngine_c_next_tick, 0);

  id_method_execute_next_ticks = rb_intern("execute_next_ticks");
}


static
void execute_next_tick_with_gvl()
{
  AE_TRACE();
  rb_funcall(mAsyncEngine, id_method_execute_next_ticks, 0, 0);
}


static
void next_tick_callback(uv_idle_t* handle, int status)
{
  AE_TRACE();

  uv_idle_stop(handle);
  uv_unref(uv_default_loop());

  rb_thread_call_with_gvl(execute_next_tick_with_gvl, NULL);
}


VALUE AsyncEngine_c_next_tick(VALUE self)
{
  AE_TRACE();

  if (! uv_is_active((uv_handle_t *)ae_next_tick_uv_idle)) {
    uv_idle_start(ae_next_tick_uv_idle, next_tick_callback);
    uv_ref(uv_default_loop());
  }
  return Qtrue;
}