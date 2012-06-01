#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_next_tick.h"


static uv_idle_t* ae_next_tick_uv_idle;

static ID method_execute_next_ticks;


void init_ae_next_tick(void)
{
  AE_TRACE();

  rb_define_module_function(mAsyncEngine, "_c_next_tick", AsyncEngine_c_next_tick, 0);

  method_execute_next_ticks = rb_intern("execute_next_ticks");

  ae_next_tick_uv_idle = NULL;
}


void load_ae_next_tick_uv_idle(void)
{
  AE_TRACE();

  if (ae_next_tick_uv_idle) {
    AE_DEBUG("ae_next_tick_uv_idle == NULL");
    return;
  }

  ae_next_tick_uv_idle = ALLOC(uv_idle_t);
  AE_ASSERT(! uv_idle_init(AE_uv_loop, ae_next_tick_uv_idle));
}


void unload_ae_next_tick_uv_idle(void)
{
  AE_TRACE();

  if (! ae_next_tick_uv_idle) {
    AE_DEBUG("ae_next_tick_uv_idle != NULL");
    return;
  }

  uv_close((uv_handle_t *)ae_next_tick_uv_idle, ae_uv_handle_close_callback);
  ae_next_tick_uv_idle = NULL;
}


static
void ae_next_tick_callback_with_gvl(void)
{
  AE_TRACE();

  rb_funcall2(mAsyncEngine, method_execute_next_ticks, 0, NULL);
}


static
void _uv_idle_callback(uv_idle_t* handle, int status)
{
  AE_TRACE();

  AE_DEBUG("uv_idle_stop(handle);");
  uv_idle_stop(handle);

  rb_thread_call_with_gvl(ae_next_tick_callback_with_gvl, NULL);
}


VALUE AsyncEngine_c_next_tick(VALUE self)
{
  AE_TRACE();

  AE_ASSERT(ae_next_tick_uv_idle != NULL);

  if (! uv_is_active((uv_handle_t *)ae_next_tick_uv_idle)) {
    AE_DEBUG("ae_next_tick_uv_idle is NOT active => uv_idle_start()");
    uv_idle_start(ae_next_tick_uv_idle, _uv_idle_callback);
  }
  else
    AE_DEBUG("ae_next_tick_uv_idle is active => do nothing");
  return Qtrue;
}