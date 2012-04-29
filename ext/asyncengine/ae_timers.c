#include "asyncengine_ruby.h"
#include "ae_timers.h"


typedef struct {
  int periodic;
  uv_timer_t *_uv_handle;
  VALUE rb_handle_id;
} struct_AsyncEngine_timer_handle;


static
void execute_callback_with_gvl(void *param)
{
  struct_AsyncEngine_timer_handle* data = (struct_AsyncEngine_timer_handle*)param;

  rb_funcall(AsyncEngine_get_handle(data->rb_handle_id), rb_intern("call"), 0, 0);
}


static
void AsyncEngine_timer_callback(uv_timer_t* _uv_handle, int status)
{
  struct_AsyncEngine_timer_handle* data = (struct_AsyncEngine_timer_handle*)_uv_handle->data;

  // Run callback.
  rb_thread_call_with_gvl(execute_callback_with_gvl, data);

  // Free the timer if it is not periodic.
  if (data->periodic == 0) {
    // Let the GC work.
    AsyncEngine_remove_handle(data->rb_handle_id);
    // Free memory.
    xfree(data);
    xfree(_uv_handle);
  }
}


VALUE AsyncEngine_c_add_timer(VALUE self, VALUE rb_delay, VALUE rb_interval, VALUE callback)
{
  uv_timer_t* _uv_handle = ALLOC(uv_timer_t);
  struct_AsyncEngine_timer_handle* data = ALLOC(struct_AsyncEngine_timer_handle);
  long delay, interval;

  delay = NUM2LONG(rb_delay);
  if (NIL_P(rb_interval)) {
    interval = 0;
    data->periodic = 0;
  }
  else {
    interval = NUM2LONG(rb_interval);
    data->periodic = 1;
  }

  //data->callback = callback;
  data->_uv_handle = _uv_handle;

  // Save the callback from being GC'd.
  data->rb_handle_id = AsyncEngine_store_handle(callback);

  // Initialize.
  uv_timer_init(uv_default_loop(), _uv_handle);
  _uv_handle->data = data;

  uv_timer_start(_uv_handle, AsyncEngine_timer_callback, delay, interval);

  return Data_Wrap_Struct(AsyncEngine_c_get_cAsyncEngineCPointer(), NULL, NULL, data);
}


VALUE AsyncEngineTimer_cancel(VALUE self)
{
  // Load data.
  struct_AsyncEngine_timer_handle* data;
  Data_Get_Struct(rb_iv_get(self, "@_c_data"), struct_AsyncEngine_timer_handle, data);

  // Stop timer.
  uv_timer_stop(data->_uv_handle);

  // Let the GC work.
  AsyncEngine_remove_handle(data->rb_handle_id);
  // Free memory.
  xfree(data);
  
  return Qtrue;
}