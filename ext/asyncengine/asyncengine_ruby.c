#include "asyncengine_ruby.h"
#include "ae_timers.h"
#include <signal.h>


// C variable holding current handle number.
static long handle_id = 0;

// AsyncEngine Ruby modules and classes.
static VALUE mAsyncEngine;
static VALUE cAsyncEngineTimer;

// Ruby class for saving C data inside.
VALUE cAsyncEngineCData;

// Ruby attributes.
static ID att_handles;
ID att_c_data;
ID att_handle_terminated;

// Ruby method names.
ID id_method_call;



long AsyncEngine_next_handle_id()
{
  ++handle_id;
}



VALUE AsyncEngine_store_handle(VALUE handle)
{
  AE_TRACE();
  VALUE rb_handle_id = LONG2FIX(++handle_id);

  rb_hash_aset(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id, handle);
  return rb_handle_id;
}


VALUE AsyncEngine_get_handle(VALUE rb_handle_id)
{
  AE_TRACE();
  return rb_hash_aref(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id);
}


void AsyncEngine_remove_handle(VALUE rb_handle_id)
{
  AE_TRACE();
  rb_hash_delete(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id);
}


static
void prepare_signals_cb(uv_prepare_t* handle, int status)
{
  rb_thread_call_with_gvl(rb_thread_check_ints, NULL);
}


// static
// void prepare_signals_close_cb(uv_handle_t* _uv_handle)
// {
//   AE_TRACE();
//   printf("------- prepare_signals_close_cb \n");
//   xfree(_uv_handle);
// }


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
  VALUE ret;
  uv_prepare_t *_uv_prepare_signals = ALLOC(uv_prepare_t);

  uv_prepare_init(uv_default_loop(), _uv_prepare_signals);
  uv_prepare_start(_uv_prepare_signals, prepare_signals_cb);
  // Don't count the prepare handle.
  uv_unref(uv_default_loop());

  ret = rb_thread_call_without_gvl(run_uv_without_gvl, NULL, RUBY_UBF_IO, NULL);

  // TODO: See TODO file
  //uv_close((uv_handle_t *)_uv_prepare_signals, prepare_signals_close_cb);
  //xfree(_uv_prepare_signals);
  return ret;
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
  rb_define_module_function(mAsyncEngine, "_c_add_timer", AsyncEngine_c_add_timer, 3);
  rb_define_method(cAsyncEngineTimer, "cancel", AsyncEngineTimer_cancel, 0);
  rb_define_alias(cAsyncEngineTimer, "stop", "cancel");
  rb_define_method(cAsyncEngineTimer, "_c_set_interval", AsyncEngineTimer_c_set_interval, 1);

  // Attribute and method names.
  att_handles = rb_intern("@handles");
  att_c_data = rb_intern("@_c_data");
  att_handle_terminated = rb_intern("@handle_terminated");
  id_method_call = rb_intern("call");
}