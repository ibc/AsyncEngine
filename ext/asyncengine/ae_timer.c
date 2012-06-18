#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_timer.h"


static VALUE cAsyncEngineTimer;
static VALUE cAsyncEnginePeriodicTimer;


typedef struct {
  VALUE ae_handle_id;
  VALUE on_fire_proc;
  uv_timer_t *_uv_handle;
  int periodic;
  long delay;
  long interval;
} struct_cdata;

struct timer_callback_data {
  struct_cdata* cdata;
};


// Used for storing information about the last timer callback.
static struct timer_callback_data last_timer_callback_data;


/** Predeclaration of static functions. */

static VALUE AsyncEngineTimer_alloc(VALUE klass);
static void AsyncEngineTimer_mark(struct_cdata* cdata);
static void AsyncEngineTimer_free(struct_cdata* cdata);
static void init_instance(VALUE self, long delay, long interval, VALUE proc);
static void _uv_timer_callback(uv_timer_t* handle, int status);
static VALUE _ae_timer_callback(void);
static void close_handle(struct_cdata *cdata);


void init_ae_timer()
{
  AE_TRACE();

  cAsyncEngineTimer = rb_define_class_under(mAsyncEngine, "Timer", cAsyncEngineHandle);
  cAsyncEnginePeriodicTimer = rb_define_class_under(mAsyncEngine, "PeriodicTimer", cAsyncEngineTimer);

  rb_define_alloc_func(cAsyncEngineTimer, AsyncEngineTimer_alloc);

  rb_define_singleton_method(cAsyncEngineTimer, "new", AsyncEngineTimer_new, -1);
  rb_define_singleton_method(cAsyncEnginePeriodicTimer, "new", AsyncEnginePeriodicTimer_new, -1);

  rb_define_method(cAsyncEngineTimer, "pause", AsyncEngineTimer_pause, 0);
  rb_define_method(cAsyncEngineTimer, "restart", AsyncEngineTimer_restart, -1);
  rb_define_method(cAsyncEnginePeriodicTimer, "restart", AsyncEnginePeriodicTimer_restart, -1);
  rb_define_method(cAsyncEngineTimer, "alive?", AsyncEngineTimer_is_alive, 0);
  rb_define_method(cAsyncEngineTimer, "delay", AsyncEngineTimer_delay, 0);
  rb_define_method(cAsyncEnginePeriodicTimer, "interval", AsyncEnginePeriodicTimer_interval, 0);
  rb_define_method(cAsyncEngineTimer, "alive?", AsyncEngineTimer_is_alive, 0);
  rb_define_method(cAsyncEngineTimer, "close", AsyncEngineTimer_close, 0);
  rb_define_alias(cAsyncEngineTimer, "stop", "close");
  rb_define_private_method(cAsyncEngineTimer, "destroy", AsyncEngineTimer_destroy, 0);
}


/** Class alloc, mark and free functions. */

static
VALUE AsyncEngineTimer_alloc(VALUE klass)
{
  AE_TRACE();

  struct_cdata* cdata = ALLOC(struct_cdata);

  /* IMPORTANT: Set the _uv_handle to NULL right now since GC could
   * call our mark() function before cdata->on_fire_proc is set.
   */
  cdata->_uv_handle = NULL;

  return Data_Wrap_Struct(klass, AsyncEngineTimer_mark, AsyncEngineTimer_free, cdata);
}


static
void AsyncEngineTimer_mark(struct_cdata* cdata)
{
  AE_TRACE();

  // This tells Ruby not to GC cdata->on_fire_proc if the Timer
  // instance continues alive in Ruby land.
  if (cdata->_uv_handle)
    rb_gc_mark(cdata->on_fire_proc);
}


static
void AsyncEngineTimer_free(struct_cdata* cdata)
{
  AE_TRACE();

  xfree(cdata);
}


/** Timer.new() method.
 *
 * Arguments:
 * - delay (Float).
 * - proc (Proc) (optional).
 *
 * Block optional.
 */
VALUE AsyncEngineTimer_new(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();
  long delay;
  VALUE proc, instance;

  AE_CHECK_STATUS();
  AE_RB_CHECK_NUM_ARGS(1,2);
  AE_RB_ENSURE_BLOCK_OR_PROC(2, proc);

  // Parameter 1: delay.
  delay = (long)(NUM2DBL(argv[0]) * 1000);
  if (delay < 1)
    delay = 1;

  // Allocate the Ruby instance.
  instance = rb_obj_alloc(self);

  // Init the UV stuff within the instance.
  init_instance(instance, delay, 0, proc);

  return instance;
}


/** PeriodicTimer.new() method.
 *
 * Arguments:
 * - interval (Float).
 * - delay (Float) (optional).
 * - proc (Proc) (optional).
 *
 * Block optional.
 */
VALUE AsyncEnginePeriodicTimer_new(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();
  long interval, delay;
  VALUE proc, instance;

  AE_CHECK_STATUS();
  AE_RB_CHECK_NUM_ARGS(1,3);
  AE_RB_ENSURE_BLOCK_OR_PROC(3, proc);

  // Parameter 1: interval.
  interval = (long)(NUM2DBL(argv[0]) * 1000);
  if (interval < 1)
    interval = 1;

  // Parameter 2: delay (optional).
  if (argc >= 2 && ! NIL_P(argv[1])) {
    delay = (long)(NUM2DBL(argv[1]) * 1000);
    if (delay < 1)
      delay = 1;
  }
  else
    delay = interval;

  // Allocate the Ruby instance.
  instance = rb_obj_alloc(self);

  // Init the UV stuff within the instance.
  init_instance(instance, delay, interval, proc);

  return instance;
}


static
void init_instance(VALUE self, long delay, long interval, VALUE proc)
{
  AE_TRACE();
  uv_timer_t *_uv_handle;
  int r;

  // Create and init the UV handle.
  _uv_handle = ALLOC(uv_timer_t);
  r = uv_timer_init(AE_uv_loop, _uv_handle);
  if (r != 0) {
    xfree(_uv_handle);
    ae_raise_last_uv_error();
  }

  // Fill cdata struct.
  GET_CDATA_FROM_SELF;
  cdata->_uv_handle = _uv_handle;
  cdata->delay = delay;
  cdata->interval = interval;
  if (interval)
    cdata->periodic = 1;
  else
    cdata->periodic = 0;
  cdata->on_fire_proc = proc;
  cdata->ae_handle_id = ae_store_handle(self);  // Avoid GC.

  // Fill data field of the UV handle.
  cdata->_uv_handle->data = (void *)cdata;

  // Run the timer.
  r = uv_timer_start(_uv_handle, _uv_timer_callback, delay, interval);
  if (r != 0) {
    close_handle(cdata);
    ae_raise_last_uv_error();
  }
}


static
void _uv_timer_callback(uv_timer_t* handle, int status)
{
  AE_TRACE();

  last_timer_callback_data.cdata = (struct_cdata*)handle->data;
  ae_take_gvl_and_run_with_error_handler(_ae_timer_callback);
}


static
VALUE _ae_timer_callback(void)
{
  AE_TRACE();

  struct_cdata* cdata = last_timer_callback_data.cdata;

  // Terminate the timer if it is not periodic.
  if (cdata->periodic == 0)
    close_handle(cdata);

  return ae_block_call_0(cdata->on_fire_proc);
}


/** Timer#pause() method. */

VALUE AsyncEngineTimer_pause(VALUE self)
{
  AE_TRACE();
  int r;

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  r = uv_timer_stop(cdata->_uv_handle);
  if (r != 0)
    ae_raise_last_uv_error();

  return Qtrue;
}


/**
 * Timer#restart() method.
 *
 * Arguments:
 * - New delay (Float) (optional). If not set, previous delay is used.
 */

VALUE AsyncEngineTimer_restart(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();
  long delay;
  int r;

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;
  AE_RB_CHECK_NUM_ARGS(0,1);

  // Parameter 1: delay.
  if (argc == 1 && ! NIL_P(argv[0])) {
    delay = (long)(NUM2DBL(argv[0]) * 1000);
    if (delay < 1)
      delay = 1;
    cdata->delay = delay;
  }

  r = uv_timer_stop(cdata->_uv_handle);
  if (r != 0)
    ae_raise_last_uv_error();

  r = uv_timer_start(cdata->_uv_handle, _uv_timer_callback, cdata->delay, 0);
  if (r != 0)
    ae_raise_last_uv_error();

  return Qtrue;
}


/**
 * PeriodicTimer#restart() method.
 *
 * Arguments:
 * - New interval (Float) (optional). If not set, previous interval is used.
 * - New delay (Float) (optional). If not set, new interval value is used,
 *   or previous delay value if no new interval is set.
 */

VALUE AsyncEnginePeriodicTimer_restart(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();
  long interval, delay;
  int r;

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;
  AE_RB_CHECK_NUM_ARGS(0,2);

  // Parameter 1: interval (optional).
  if (argc >= 1 && ! NIL_P(argv[0])) {
    interval = (long)(NUM2DBL(argv[0]) * 1000);
    if (interval < 1)
      interval = 1;
    cdata->interval = interval;

    // Parameter 2: delay (optional).
    if (argc == 2 && ! NIL_P(argv[1])) {
      delay = (long)(NUM2DBL(argv[1]) * 1000);
      if (delay < 1)
        delay = 1;
      cdata->delay = delay;
    }
    else
      cdata->delay = interval;
  }

  r = uv_timer_stop(cdata->_uv_handle);
  if (r != 0)
    ae_raise_last_uv_error();

  r = uv_timer_start(cdata->_uv_handle, _uv_timer_callback, cdata->delay, cdata->interval);
  if (r != 0)
    ae_raise_last_uv_error();

  return Qtrue;
}


/** Timer#delay() method. */

VALUE AsyncEngineTimer_delay(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  return rb_float_new((double)(cdata->delay / 1000.0));
}


/** PeriodicTimer#interval() method. */

VALUE AsyncEnginePeriodicTimer_interval(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  return rb_float_new((double)(uv_timer_get_repeat(cdata->_uv_handle) / 1000.0));
}


/** Timer#alive?() method. */

VALUE AsyncEngineTimer_is_alive(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  return Qtrue;
}


/** Timer#close() method. */

VALUE AsyncEngineTimer_close(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  close_handle(cdata);
  return Qtrue;
}


/** Timer#destroy() private method. */

VALUE AsyncEngineTimer_destroy(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  close_handle(cdata);
  return Qtrue;
}


static
void close_handle(struct_cdata *cdata)
{
  AE_TRACE();

  AE_CLOSE_UV_HANDLE(cdata->_uv_handle);
  cdata->_uv_handle = NULL;
  ae_remove_handle(cdata->ae_handle_id);
}
