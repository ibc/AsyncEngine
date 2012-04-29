#include "asyncengine_ruby.h"
#include "ae_timers.h"
#include <signal.h>


// C variable holding current handle number.
static long handle_id = 0;

// AsyncEngine Ruby modules and classes.
static VALUE mAsyncEngine;
static VALUE cAsyncEngineTimer;

// Rucy class for saving C data within a Ruby object.
static VALUE cAsyncEngineCPointer;

// Ruby attributes.
static ID att_handles;
static ID att_trapped_signals;

// Ruby method names.
static ID id_method_call;



long AsyncEngine_next_handle_id()
{
  ++handle_id;
}


VALUE AsyncEngine_c_get_mAsyncEngine() { return mAsyncEngine; }
VALUE AsyncEngine_c_get_cAsyncEngineCPointer() { return cAsyncEngineCPointer; }


VALUE AsyncEngine_store_handle(VALUE handle)
{
  VALUE rb_handle_id = LONG2FIX(++handle_id);

  rb_hash_aset(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id, handle);
  return rb_handle_id;
}


VALUE AsyncEngine_get_handle(VALUE rb_handle_id)
{
  return rb_hash_aref(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id);
}


void AsyncEngine_remove_handle(VALUE rb_handle_id)
{
  rb_hash_delete(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id);
}


static
VALUE run_uv_without_gvl(void* param)
{
  if (! uv_run(uv_default_loop()))
    return Qtrue;
  else
    return Qfalse;
}


static void check_rb_ints_with_gvl(void *param) { rb_thread_check_ints(); }

static
void prepare_signals_cb(uv_prepare_t* handle, int status)
{
  printf("DBG: prepare_signals_cb()\n");
  if (rb_thread_interrupted(rb_thread_current())) {
    printf("DBG: prepare_signals_cb(): rb_thread_interrupted() => true;\n");
    rb_thread_call_with_gvl(check_rb_ints_with_gvl, NULL);
  }
}



VALUE AsyncEngine_c_start(VALUE self)
{
  uv_prepare_t *_uv_prepare_signals = ALLOC(uv_prepare_t);

  uv_prepare_init(uv_default_loop(), _uv_prepare_signals);
  uv_prepare_start(_uv_prepare_signals, prepare_signals_cb);

  return rb_thread_call_without_gvl(run_uv_without_gvl, NULL, RUBY_UBF_IO, NULL);
}




void Init_asyncengine_ext()
{
  mAsyncEngine = rb_define_module("AsyncEngine");
  cAsyncEngineCPointer = rb_define_class_under(mAsyncEngine, "CPointer", rb_cObject);

  rb_define_module_function(mAsyncEngine, "_c_start", AsyncEngine_c_start, 0);

  /* Timers */
  cAsyncEngineTimer = rb_define_class_under(mAsyncEngine, "Timer", rb_cObject);
  rb_define_module_function(mAsyncEngine, "_c_add_timer", AsyncEngine_c_add_timer, 3);
  rb_define_method(cAsyncEngineTimer, "cancel", AsyncEngineTimer_cancel, 0);

  /* Attribute and method names */
  att_handles = rb_intern("@handles");
  att_trapped_signals = rb_intern("@trapped_signals");
  id_method_call = rb_intern("call");
}