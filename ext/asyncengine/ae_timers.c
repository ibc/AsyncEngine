#include "asyncengine_ruby.h"
#include "ae_timers.h"


// Global variables defined in asyncengine_ruby.c.
extern VALUE cAsyncEngineCData;
extern ID id_method_call;


typedef struct {
  int periodic;
  uv_timer_t *_uv_handle;
  VALUE rb_handle_id;
} struct_AsyncEngine_timer_handle;


static
void timer_close_cb(uv_handle_t* _uv_handle)
{
  AE_TRACE();
  xfree(_uv_handle);
}


static
void terminate_timer(uv_timer_t* _uv_handle)
{
  AE_TRACE();
  struct_AsyncEngine_timer_handle* cdata = (struct_AsyncEngine_timer_handle*)_uv_handle->data;

  // Let the GC work.
  AsyncEngine_remove_handle(cdata->rb_handle_id);
  // Free memory.
  xfree(cdata);
  // Close the timer so it's unreferenced by uv.
  uv_close((uv_handle_t *)_uv_handle, timer_close_cb);
}


static
void execute_callback_with_gvl(void *param)
{
  AE_TRACE();
  struct_AsyncEngine_timer_handle* cdata = (struct_AsyncEngine_timer_handle*)param;

  rb_funcall(AsyncEngine_get_handle(cdata->rb_handle_id), id_method_call, 0, 0);
}


static
void AsyncEngine_timer_callback(uv_timer_t* _uv_handle, int status)
{
  AE_TRACE();
  struct_AsyncEngine_timer_handle* cdata = (struct_AsyncEngine_timer_handle*)_uv_handle->data;

  // Run callback.
  rb_thread_call_with_gvl(execute_callback_with_gvl, cdata);

  // Terminate the timer if it is not periodic.
  if (cdata->periodic == 0)
    terminate_timer(_uv_handle);
}


VALUE AsyncEngine_c_add_timer(VALUE self, VALUE rb_delay, VALUE rb_interval, VALUE callback)
{
  AE_TRACE();
  uv_timer_t* _uv_handle = ALLOC(uv_timer_t);
  struct_AsyncEngine_timer_handle* cdata = ALLOC(struct_AsyncEngine_timer_handle);
  long delay, interval;

  delay = NUM2LONG(rb_delay);
  if (NIL_P(rb_interval)) {
    interval = 0;
    cdata->periodic = 0;
  }
  else {
    interval = NUM2LONG(rb_interval);
    cdata->periodic = 1;
  }

  cdata->_uv_handle = _uv_handle;

  // Save the callback from being GC'd.
  cdata->rb_handle_id = AsyncEngine_store_handle(callback);

  // Initialize.
  uv_timer_init(uv_default_loop(), _uv_handle);
  _uv_handle->data = cdata;

  uv_timer_start(_uv_handle, AsyncEngine_timer_callback, delay, interval);

  return Data_Wrap_Struct(cAsyncEngineCData, NULL, NULL, cdata);
}


VALUE AsyncEngineTimer_cancel(VALUE self)
{
  AE_TRACE();
  struct_AsyncEngine_timer_handle* cdata;
  Data_Get_Struct(rb_iv_get(self, "@_c_data"), struct_AsyncEngine_timer_handle, cdata);

  // Stop timer.
  uv_timer_stop(cdata->_uv_handle);

  // Terminate the timer.
  terminate_timer(cdata->_uv_handle);

  return Qtrue;
}


VALUE AsyncEngineTimer_c_set_interval(VALUE self, VALUE rb_interval)
{
  AE_TRACE();
  struct_AsyncEngine_timer_handle* cdata;
  Data_Get_Struct(rb_iv_get(self, "@_c_data"), struct_AsyncEngine_timer_handle, cdata);

  uv_timer_set_repeat(cdata->_uv_handle, NUM2LONG(rb_interval));
  return rb_interval;
}