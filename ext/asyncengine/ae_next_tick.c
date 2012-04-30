#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_next_tick.h"


// C variable indicating whether there are pending next_ticks.
static int pending_next_ticks;

static ID id_method_execute_next_ticks;


void init_ae_next_tick()
{
  pending_next_ticks = 0;

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
  uv_close((uv_handle_t *)handle, handle_close_callback_1);
  pending_next_ticks = 0;
  rb_thread_call_with_gvl(execute_next_tick_with_gvl, NULL);
}


VALUE AsyncEngine_c_next_tick(VALUE self)
{
  AE_TRACE();
  if (pending_next_ticks == 0) {
    uv_idle_t* _uv_handle = ALLOC(uv_idle_t);
    uv_idle_init(uv_default_loop(), _uv_handle);
    pending_next_ticks = 1;
    uv_idle_start(_uv_handle, next_tick_callback);
  }
  return Qtrue;
}