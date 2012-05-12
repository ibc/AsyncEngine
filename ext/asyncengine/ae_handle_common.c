#include "asyncengine_ruby.h"
#include "ae_handle_common.h"


// Global variables defined in asyncengine_ruby.c.
extern VALUE mAsyncEngine;

// C variable holding current block number.
static long block_id;
// C variable holding current handle number.
static long handle_id;

// Ruby class for saving C data inside.
VALUE cAsyncEngineCData;

// Ruby attributes.
static ID att_blocks;
static ID att_handles;
ID att_cdata;
ID att_handle_terminated;

// Ruby method names.
static ID method_call;
static ID method_handle_exception;


void init_ae_handle_common()
{
  AE_TRACE();

  cAsyncEngineCData = rb_define_class_under(mAsyncEngine, "CData", rb_cObject);

  att_blocks = rb_intern("@_blocks");
  att_handles = rb_intern("@_handles");
  att_cdata = rb_intern("@_cdata");
  att_handle_terminated = rb_intern("@_handle_terminated");

  method_call = rb_intern("call");
  method_handle_exception = rb_intern("handle_exception");

  block_id = 0;
  handle_id = 0;
}


VALUE ae_store_handle(VALUE rb_handle)
{
  AE_TRACE();

  VALUE rb_handle_id = LONG2FIX(++handle_id);

  rb_hash_aset(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id, rb_handle);
  return rb_handle_id;
}


VALUE ae_get_handle(VALUE rb_handle_id)
{
  AE_TRACE();

  return rb_hash_aref(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id);
}


VALUE ae_remove_handle(VALUE rb_handle_id)
{
  AE_TRACE();

  return rb_hash_delete(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_id);
}


VALUE ae_store_block(VALUE rb_block)
{
  AE_TRACE();

  VALUE rb_block_id = LONG2FIX(++block_id);

  rb_hash_aset(rb_ivar_get(mAsyncEngine, att_blocks), rb_block_id, rb_block);
  return rb_block_id;
}


VALUE ae_get_block(VALUE rb_block_id)
{
  AE_TRACE();

  return rb_hash_aref(rb_ivar_get(mAsyncEngine, att_blocks), rb_block_id);
}


VALUE ae_remove_block(VALUE rb_block_id)
{
  AE_TRACE();

  return rb_hash_delete(rb_ivar_get(mAsyncEngine, att_blocks), rb_block_id);
}


void ae_handle_exception(int exception_tag)
{
  AE_TRACE();

  // rb_errinfo() gives the current exception object in this thread.
  VALUE exception = rb_errinfo();

  // Just check the exception in the user provided AE.exception_handler block if
  // it is a StandardError. Otherwise raise it and terminate.
  if (rb_obj_is_kind_of(exception, rb_eStandardError) == Qtrue) {
    rb_funcall(mAsyncEngine, method_handle_exception, 1, exception);
    // Dissable the current thread exception.
    rb_set_errinfo(Qnil);
  }
  else
    rb_jump_tag(exception_tag);
}


void ae_uv_handle_close_callback(uv_handle_t* handle)
{
  AE_TRACE();

  xfree(handle);
}


static
VALUE wrapper_rb_funcall_block_call(VALUE block)
{
  AE_TRACE();

  return rb_funcall2(block, method_call, 0, NULL);
}


VALUE ae_protect_block_call(VALUE block, int *exception_tag)
{
  AE_TRACE();

  *exception_tag;

  return rb_protect(wrapper_rb_funcall_block_call, block, exception_tag);
}




typedef struct {
  execute_method_with_protect method_with_protect;
  void *param;
} struct_method_with_protect_data;


static
void execute_method_with_gvl(struct_method_with_protect_data data)
{
  AE_TRACE();

  int exception_tag;
  VALUE param;

  if (data.param)
    param = (VALUE)(data.param);
  else
    param = Qnil;

  rb_protect(data.method_with_protect, param, &exception_tag);

  if (exception_tag)
    ae_handle_exception(exception_tag);
}


// param MUST be NULL or VALUE.
void execute_method_with_gvl_and_protect(execute_method_with_protect method_with_protect, void *param)
{
  AE_TRACE();

  struct_method_with_protect_data data;
  data.method_with_protect = method_with_protect;
  data.param = param;

  rb_thread_call_with_gvl(execute_method_with_gvl, data);
}