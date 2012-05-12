#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_timer.h"


static VALUE cAsyncEngineTimer;


typedef struct {
  uv_timer_t *_uv_handle;
  int periodic;
  VALUE rb_block_id;
  VALUE rb_ae_timer_id;
} struct_ae_timer_cdata;


void init_ae_timer()
{
  AE_TRACE();

  cAsyncEngineTimer = rb_define_class_under(mAsyncEngine, "Timer", rb_cObject);
  rb_define_module_function(mAsyncEngine, "_c_add_timer", AsyncEngine_c_add_timer, 4);
  rb_define_method(cAsyncEngineTimer, "cancel", AsyncEngineTimer_cancel, 0);
  rb_define_alias(cAsyncEngineTimer, "stop", "cancel");
  rb_define_private_method(cAsyncEngineTimer, "_c_set_interval", AsyncEngineTimer_c_set_interval, 1);
}


static
void deallocate_timer_handle(struct_ae_timer_cdata* cdata, int remove_handle)
{
  AE_TRACE();

  // Let the GC work.
  ae_remove_block(cdata->rb_block_id);
  if (remove_handle)
    ae_remove_handle(cdata->rb_ae_timer_id);
  // Close the timer so it's unreferenced by uv.
  uv_close((uv_handle_t *)cdata->_uv_handle, ae_uv_handle_close_callback);
  // Free memory.
  xfree(cdata);
}


static
void execute_timer_callback_with_gvl(uv_timer_t* handle)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata = (struct_ae_timer_cdata*)handle->data;
  VALUE block = ae_get_block(cdata->rb_block_id);
  int exception_tag;

  // Terminate the timer if it is not periodic.
  if (cdata->periodic == 0) {
    // If the timer has a ruby AE::Timer instance then set its attribute
    // @_handle_terminated to true.
    if (! NIL_P(cdata->rb_ae_timer_id))
      rb_ivar_set(ae_remove_handle(cdata->rb_ae_timer_id), att_handle_terminated, Qtrue);
    deallocate_timer_handle(cdata, 0);
  }

  ae_protect_block_call(block, &exception_tag);

  if (exception_tag)
    ae_handle_exception(exception_tag);
}


static
void timer_callback(uv_timer_t* handle, int status)
{
  AE_TRACE();

  rb_thread_call_with_gvl(execute_timer_callback_with_gvl, handle);
}


VALUE AsyncEngine_c_add_timer(VALUE self, VALUE rb_delay, VALUE rb_interval, VALUE block, VALUE instance)
{
  AE_TRACE();

  uv_timer_t* _uv_handle = ALLOC(uv_timer_t);
  struct_ae_timer_cdata* cdata = ALLOC(struct_ae_timer_cdata);
  long delay, interval;

  cdata->_uv_handle = _uv_handle;

  delay = NUM2LONG(rb_delay);
  if (NIL_P(rb_interval)) {
    interval = 0;
    cdata->periodic = 0;
  }
  else {
    interval = NUM2LONG(rb_interval);
    if (interval == 0)  interval = 1;
    cdata->periodic = 1;
  }

  // Save the block from being GC'd.
  cdata->rb_block_id = ae_store_block(block);

  if (NIL_P(instance))
    cdata->rb_ae_timer_id = Qnil;
  else {
    // Save the AE timer from being GC'd.
    cdata->rb_ae_timer_id = ae_store_handle(instance);
    // Store CDATA within the instance.
    rb_ivar_set(instance, att_cdata, Data_Wrap_Struct(cAsyncEngineCData, NULL, NULL, cdata));
  }

  // Initialize.
  uv_timer_init(uv_default_loop(), _uv_handle);
  _uv_handle->data = cdata;

  uv_timer_start(_uv_handle, timer_callback, delay, interval);

  return Qtrue;
}


VALUE AsyncEngineTimer_cancel(VALUE self)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata;

  if (! NIL_P(rb_ivar_get(self, att_handle_terminated)))
    return Qfalse;
  rb_ivar_set(self, att_handle_terminated, Qtrue);

  Data_Get_Struct(rb_ivar_get(self, att_cdata), struct_ae_timer_cdata, cdata);

  // Stop timer.
  uv_timer_stop(cdata->_uv_handle);

  // Terminate the timer.
  deallocate_timer_handle(cdata, 1);

  return Qtrue;
}


VALUE AsyncEngineTimer_c_set_interval(VALUE self, VALUE rb_interval)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata;
  long interval;

  if (! NIL_P(rb_ivar_get(self, att_handle_terminated)))
    return Qfalse;

  Data_Get_Struct(rb_ivar_get(self, att_cdata), struct_ae_timer_cdata, cdata);

  interval = NUM2LONG(rb_interval);
  if (interval == 0)  interval = 1;

  uv_timer_set_repeat(cdata->_uv_handle, interval);
  return rb_interval;
}