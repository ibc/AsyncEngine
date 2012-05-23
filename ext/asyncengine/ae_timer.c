#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_timer.h"


static VALUE cAsyncEngineTimer;


typedef struct {
  VALUE ae_handle_id;
  VALUE block;
  uv_timer_t *_uv_handle;
  int periodic;
  long delay;
  long interval;
} struct_ae_timer_cdata;


typedef struct {
  uv_timer_t* handle;
} struct_timer_callback_data;


// Used for storing information about the last timer callback.
static struct_timer_callback_data last_timer_callback_data;


static void AsyncEngineTimer_mark(void *ptr)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata = (struct_ae_timer_cdata*)ptr;

  // This tells Ruby not to GC cdata->block if the Timer instance
  // continues alive in Ruby land.
  if (cdata->_uv_handle)
    rb_gc_mark(cdata->block);
}


static void AsyncEngineTimer_free(void *cdata)
{
  AE_TRACE();

  xfree(cdata);
}


VALUE AsyncEngineTimer_alloc(VALUE klass)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata = ALLOC(struct_ae_timer_cdata);

  /* IMPORTANT: Set the _uv_handle to NULL right now since GC could
   * call our mark() function before cdata->block is set.
   */
  cdata->_uv_handle = NULL;

  return Data_Wrap_Struct(klass, AsyncEngineTimer_mark, AsyncEngineTimer_free, cdata);
}


void init_ae_timer()
{
  AE_TRACE();

  cAsyncEngineTimer = rb_define_class_under(mAsyncEngine, "Timer", rb_cObject);

  rb_define_alloc_func(cAsyncEngineTimer, AsyncEngineTimer_alloc);
  rb_define_private_method(cAsyncEngineTimer, "uv_handle_init", AsyncEngineTimer_uv_handle_init, 3);
  rb_define_method(cAsyncEngineTimer, "stop", AsyncEngineTimer_stop, 0);
  rb_define_private_method(cAsyncEngineTimer, "_c_restart", AsyncEngineTimer_c_restart, 2);
  rb_define_method(cAsyncEngineTimer, "alive?", AsyncEngineTimer_is_alive, 0);
  rb_define_method(cAsyncEngineTimer, "delay", AsyncEngineTimer_delay, 0);
  rb_define_method(cAsyncEngineTimer, "interval", AsyncEngineTimer_interval, 0);
  rb_define_method(cAsyncEngineTimer, "cancel", AsyncEngineTimer_cancel, 0);
  rb_define_private_method(cAsyncEngineTimer, "destroy", AsyncEngineTimer_destroy, 0);
}


static
void destroy(struct_ae_timer_cdata* cdata)
{
  AE_TRACE();

  uv_close((uv_handle_t *)cdata->_uv_handle, ae_uv_handle_close_callback);
  // Set the handle field to NULL.
  cdata->_uv_handle = NULL;
  // Let the GC work.
  ae_remove_handle(cdata->ae_handle_id);
}


static
VALUE ae_timer_callback(VALUE ignore)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata = (struct_ae_timer_cdata*)last_timer_callback_data.handle->data;

  // Terminate the timer if it is not periodic.
  if (cdata->periodic == 0)
    destroy(cdata);

  return ae_block_call_0(cdata->block);
}


static
void _uv_timer_callback(uv_timer_t* handle, int status)
{
  AE_TRACE();

  // TODO: testing
  AE_ASSERT(! status);

  last_timer_callback_data.handle = handle;

  ae_execute_function_with_gvl_and_protect(ae_timer_callback, Qnil);
}


VALUE AsyncEngineTimer_uv_handle_init(VALUE self, VALUE rb_delay, VALUE rb_interval, VALUE block)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata;

  Data_Get_Struct(self, struct_ae_timer_cdata, cdata);

  if ((cdata->delay = NUM2LONG(rb_delay)) <= 0)
    cdata->delay = 1;

  if (NIL_P(rb_interval)) {
    cdata->interval = 0;
    cdata->periodic = 0;
  }
  else {
    if (! NIL_P(rb_interval)) {
      if ((cdata->interval = NUM2LONG(rb_interval)) <= 0)
        cdata->interval = 1;
    }
    cdata->periodic = 1;
  }

  cdata->block = block;
  cdata->_uv_handle = ALLOC(uv_timer_t);

  // Avoid GC.
  cdata->ae_handle_id = ae_store_handle(self);

  // Initialize the UV handle.
  uv_timer_init(AE_uv_loop, cdata->_uv_handle);

  // Store the cdata within the UV handle.
  cdata->_uv_handle->data = cdata;

  // Run the timer.
  uv_timer_start(cdata->_uv_handle, _uv_timer_callback, cdata->delay, cdata->interval);

  return self;
}


VALUE AsyncEngineTimer_stop(VALUE self)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata;

  Data_Get_Struct(self, struct_ae_timer_cdata, cdata);
  if (! cdata->_uv_handle)
    return Qfalse;

  if (uv_is_active((uv_handle_t*)cdata->_uv_handle)) {
    uv_timer_stop(cdata->_uv_handle);
    return Qtrue;
  }
  else
    return Qnil;
}


VALUE AsyncEngineTimer_c_restart(VALUE self, VALUE rb_delay, VALUE rb_interval)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata;

  Data_Get_Struct(self, struct_ae_timer_cdata, cdata);
  if (! cdata->_uv_handle)
    return Qfalse;

  if (uv_is_active((uv_handle_t*)cdata->_uv_handle))
    uv_timer_stop(cdata->_uv_handle);

  if (! NIL_P(rb_delay)) {
    if ((cdata->delay = NUM2LONG(rb_delay)) <= 0)
      cdata->delay = 1;
  }

  if (! NIL_P(rb_interval)) {
    if ((cdata->interval = NUM2LONG(rb_interval)) <= 0)
      cdata->interval = 1;
  }

  uv_timer_start(cdata->_uv_handle, _uv_timer_callback, cdata->delay, cdata->interval);

  return Qtrue;
}


VALUE AsyncEngineTimer_is_alive(VALUE self)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata;

  Data_Get_Struct(self, struct_ae_timer_cdata, cdata);
  if (! cdata->_uv_handle)
    return Qfalse;

  return Qtrue;
}


VALUE AsyncEngineTimer_delay(VALUE self)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata;

  Data_Get_Struct(self, struct_ae_timer_cdata, cdata);
  if (! cdata->_uv_handle)
    return Qfalse;

  return LONG2NUM(cdata->delay);
}


VALUE AsyncEngineTimer_interval(VALUE self)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata;

  Data_Get_Struct(self, struct_ae_timer_cdata, cdata);
  if (! cdata->_uv_handle)
    return Qfalse;

  return LONG2NUM((long)uv_timer_get_repeat(cdata->_uv_handle));
}


VALUE AsyncEngineTimer_cancel(VALUE self)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata;

  Data_Get_Struct(self, struct_ae_timer_cdata, cdata);
  if (! cdata->_uv_handle)
    return Qfalse;

  if (uv_is_active((uv_handle_t*)cdata->_uv_handle))
    uv_timer_stop(cdata->_uv_handle);

  destroy(cdata);

  return Qtrue;
}


VALUE AsyncEngineTimer_destroy(VALUE self)
{
  AE_TRACE();

  struct_ae_timer_cdata* cdata;

  Data_Get_Struct(self, struct_ae_timer_cdata, cdata);
  if (! cdata->_uv_handle)
    return Qfalse;

  destroy(cdata);
  return Qtrue;
}
