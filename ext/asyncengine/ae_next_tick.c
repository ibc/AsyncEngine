#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_next_tick.h"


static uv_idle_t* ae_next_tick_uv_idle;

static ID id_method_execute_next_ticks;


void init_ae_next_tick()
{
  AE_TRACE();

  rb_define_module_function(mAsyncEngine, "_c_next_tick", AsyncEngine_c_next_tick, 0);

  id_method_execute_next_ticks = rb_intern("execute_next_ticks");

  ae_next_tick_uv_idle = NULL;
}


void load_ae_next_tick_uv_idle()
{
  AE_TRACE();

  if (ae_next_tick_uv_idle)
    return;

  ae_next_tick_uv_idle = ALLOC(uv_idle_t);
  uv_idle_init(uv_default_loop(), ae_next_tick_uv_idle);
  uv_unref((uv_handle_t *)ae_next_tick_uv_idle);
}


void unload_ae_next_tick_uv_idle()
{
  AE_TRACE();

  if (! ae_next_tick_uv_idle)
    return;

  // Referece again ae_uv_prepare so it can be properly closed.
  uv_ref((uv_handle_t *)ae_next_tick_uv_idle);
  uv_close((uv_handle_t *)ae_next_tick_uv_idle, ae_uv_handle_close_callback);
  ae_next_tick_uv_idle = NULL;
}


static
void execute_next_tick_callback_with_gvl()
{
  AE_TRACE();

  rb_funcall2(mAsyncEngine, id_method_execute_next_ticks, 0, NULL);
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

  load_ae_next_tick_uv_idle();

  if (! uv_is_active((uv_handle_t *)ae_next_tick_uv_idle)) {
    uv_idle_start(ae_next_tick_uv_idle, _uv_idle_callback);
    uv_ref((uv_handle_t *)ae_next_tick_uv_idle);
  }
  return Qtrue;
}