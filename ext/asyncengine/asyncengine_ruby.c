#include "asyncengine_ruby.h"
#include "ae_timers.h"
#include <signal.h>


// C variable holding current handle number.
static long handle_id = 0;

// AsyncEngine Ruby modules and classes.
extern VALUE mAsyncEngine;
static VALUE cAsyncEngineTimer;

// Ruby class for saving C data inside.
extern VALUE cAsyncEngineCData;

// Ruby attributes.
static ID att_handles;

// Ruby method names.
extern ID id_method_call;



long AsyncEngine_next_handle_id()
{
  ++handle_id;
}



VALUE AsyncEngine_store_handle(VALUE handle)
{
  TRACE();
  VALUE rb_handle_id = LONG2FIX(++handle_id);

  rb_hash_aset(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id, handle);
  return rb_handle_id;
}


VALUE AsyncEngine_get_handle(VALUE rb_handle_id)
{
  TRACE();
  return rb_hash_aref(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id);
}


void AsyncEngine_remove_handle(VALUE rb_handle_id)
{
  TRACE();
  rb_hash_delete(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id);
}


static
void prepare_signals_cb(uv_prepare_t* handle, int status)
{
  rb_thread_call_with_gvl(rb_thread_check_ints, NULL);
}


static
VALUE run_uv_without_gvl(void* param)
{
  TRACE();
  if (! uv_run(uv_default_loop()))
    return Qtrue;
  else
    return Qfalse;
}


VALUE AsyncEngine_c_start(VALUE self)
{
  TRACE();
  uv_prepare_t *_uv_prepare_signals = ALLOC(uv_prepare_t);

  uv_prepare_init(uv_default_loop(), _uv_prepare_signals);
  uv_prepare_start(_uv_prepare_signals, prepare_signals_cb);

  return rb_thread_call_without_gvl(run_uv_without_gvl, NULL, RUBY_UBF_IO, NULL);
}




void Init_asyncengine_ext()
{
  TRACE();
  mAsyncEngine = rb_define_module("AsyncEngine");
  cAsyncEngineCData = rb_define_class_under(mAsyncEngine, "CData", rb_cObject);

  rb_define_module_function(mAsyncEngine, "_c_start", AsyncEngine_c_start, 0);

  // Timers.
  cAsyncEngineTimer = rb_define_class_under(mAsyncEngine, "Timer", rb_cObject);
  rb_define_module_function(mAsyncEngine, "_c_add_timer", AsyncEngine_c_add_timer, 3);
  rb_define_method(cAsyncEngineTimer, "cancel", AsyncEngineTimer_cancel, 0);

  // Attribute and method names.
  att_handles = rb_intern("@handles");
  id_method_call = rb_intern("call");
}