#include "asyncengine_ruby.h"
#include "ae_next_tick.h"


// Global variables defined in asyncengine_ruby.c.
extern VALUE mAsyncEngine;
extern ID id_method_call;
extern ID id_method_execute_next_ticks;

// C variable indicating whether there are pending next_ticks.
static int pending_next_ticks = 0;


static
void idle_close_cb(uv_handle_t* handle)
{
  AE_TRACE();
  xfree(handle);
}


static
void execute_callback_with_gvl()
{
  AE_TRACE();
  rb_funcall(mAsyncEngine, id_method_execute_next_ticks, 0, 0);
}


static
void next_tick_cb(uv_idle_t* handle, int status)
{
  uv_idle_stop(handle);
  uv_close((uv_handle_t *)handle, idle_close_cb);
  pending_next_ticks = 0;
  rb_thread_call_with_gvl(execute_callback_with_gvl, NULL);
}


VALUE AsyncEngine_c_next_tick(VALUE self)
{
  if (pending_next_ticks == 0) {
    uv_idle_t* _uv_handle = ALLOC(uv_idle_t);
    uv_idle_init(uv_default_loop(), _uv_handle);
    pending_next_ticks = 1;
    uv_idle_start(_uv_handle, next_tick_cb);
  }
  return Qtrue;
}