#include "asyncengine_ruby.h"
#include "ae_timer.h"
#include "ae_next_tick.h"


// C variable holding current callback number.
static long callback_id = 0;

// AsyncEngine Ruby modules and classes.
VALUE mAsyncEngine;
static VALUE cAsyncEngineTimer;

// Ruby class for saving C data inside.
VALUE cAsyncEngineCData;

// Ruby attributes.
static ID att_callbacks;
ID att_next_ticks;
ID att_c_data;
ID att_handle_terminated;

// Ruby method names.
ID id_method_call;
ID id_method_execute_next_ticks;



VALUE AsyncEngine_store_callback(VALUE callback)
{
  AE_TRACE();
  VALUE rb_callback_id = LONG2FIX(++callback_id);

  rb_hash_aset(rb_ivar_get(mAsyncEngine, att_callbacks), rb_callback_id, callback);
  return rb_callback_id;
}


VALUE AsyncEngine_get_callback(VALUE rb_callback_id)
{
  AE_TRACE();
  return rb_hash_aref(rb_ivar_get(mAsyncEngine, att_callbacks), rb_callback_id);
}


VALUE AsyncEngine_remove_callback(VALUE rb_callback_id)
{
  AE_TRACE();
  return rb_hash_delete(rb_ivar_get(mAsyncEngine, att_callbacks), rb_callback_id);
}


static
void prepare_close_cb(uv_handle_t* handle)
{
  AE_TRACE();
  xfree(handle);
}


static
void prepare_cb(uv_prepare_t* handle, int status)
{
  AE_TRACE();

  // Check received interruptions in Ruby land.
  rb_thread_call_with_gvl(rb_thread_check_ints, NULL);

  // If this uv_prepare is the only existing handle, then terminate the loop.
  if (uv_loop_refcount(uv_default_loop()) == 1)
    uv_close((uv_handle_t *)handle, prepare_close_cb);
}


static
VALUE run_uv_without_gvl(void* param)
{
  AE_TRACE();
  if (! uv_run(uv_default_loop()))
    return Qtrue;
  else
    return Qfalse;
}


VALUE AsyncEngine_c_start(VALUE self)
{
  AE_TRACE();
  uv_prepare_t *_uv_prepare = ALLOC(uv_prepare_t);

  uv_prepare_init(uv_default_loop(), _uv_prepare);
  uv_prepare_start(_uv_prepare, prepare_cb);

  return rb_thread_call_without_gvl(run_uv_without_gvl, NULL, RUBY_UBF_IO, NULL);
}


VALUE AsyncEngine_num_handles(VALUE self)
{
  return INT2FIX(uv_loop_refcount(uv_default_loop()));
}




void Init_asyncengine_ext()
{
  AE_TRACE();
  mAsyncEngine = rb_define_module("AsyncEngine");
  cAsyncEngineCData = rb_define_class_under(mAsyncEngine, "CData", rb_cObject);

  rb_define_module_function(mAsyncEngine, "_c_start", AsyncEngine_c_start, 0);
  rb_define_module_function(mAsyncEngine, "num_handles", AsyncEngine_num_handles, 0);

  // Timers.
  cAsyncEngineTimer = rb_define_class_under(mAsyncEngine, "Timer", rb_cObject);
  rb_define_module_function(mAsyncEngine, "_c_add_timer", AsyncEngine_c_add_timer, 4);
  rb_define_method(cAsyncEngineTimer, "cancel", AsyncEngineTimer_cancel, 0);
  rb_define_alias(cAsyncEngineTimer, "stop", "cancel");
  rb_define_private_method(cAsyncEngineTimer, "_c_set_interval", AsyncEngineTimer_c_set_interval, 1);

  // Next tick.
  rb_define_module_function(mAsyncEngine, "_c_next_tick", AsyncEngine_c_next_tick, 0);
  
  // Attribute and method names.
  att_callbacks = rb_intern("@callbacks");
  att_next_ticks = rb_intern("@next_ticks");
  att_c_data = rb_intern("@_c_data");
  att_handle_terminated = rb_intern("@handle_terminated");
  id_method_call = rb_intern("call");
  id_method_execute_next_ticks = rb_intern("execute_next_ticks");
}