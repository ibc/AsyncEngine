#include "asyncengine_ruby.h"
#include "ae_handle_common.h"


// Global variables defined in asyncengine_ruby.c.
extern VALUE mAsyncEngine;

// C variable holding current block number.
static long block_id = 0;

// Ruby class for saving C data inside.
VALUE cAsyncEngineCData;

// Ruby attributes.
static ID att_blocks;
ID att_c_data;
ID att_handle_terminated;

// Ruby method names.
ID id_method_call;
ID id_handle_error;


void init_ae_handle_common()
{
  cAsyncEngineCData = rb_define_class_under(mAsyncEngine, "CData", rb_cObject);

  att_blocks = rb_intern("@_blocks");
  att_c_data = rb_intern("@_c_data");
  att_handle_terminated = rb_intern("@_handle_terminated");

  id_method_call = rb_intern("call");
  id_handle_error = rb_intern("handle_error");
}



VALUE AsyncEngine_store_block(VALUE block)
{
  AE_TRACE();
  VALUE rb_block_id = LONG2FIX(++block_id);

  rb_hash_aset(rb_ivar_get(mAsyncEngine, att_blocks), rb_block_id, block);
  return rb_block_id;
}


VALUE AsyncEngine_get_block(VALUE rb_block_id)
{
  AE_TRACE();
  return rb_hash_aref(rb_ivar_get(mAsyncEngine, att_blocks), rb_block_id);
}


VALUE AsyncEngine_remove_block(VALUE rb_block_id)
{
  AE_TRACE();
  return rb_hash_delete(rb_ivar_get(mAsyncEngine, att_blocks), rb_block_id);
}


void AsyncEngine_handle_exception()
{
  AE_TRACE();

  // rb_errinfo() gives the current exception object in this thread.
  rb_funcall(mAsyncEngine, id_handle_error, 1, rb_errinfo());
  // Dissable the current thread exception.
  rb_set_errinfo(Qnil);
}


void handle_close_callback_1(uv_handle_t* handle)
{
  AE_TRACE();
  xfree(handle);
}


static VALUE wrapper_rb_funcall_0(VALUE block)
{
  rb_funcall(block, id_method_call, 0, 0);
  return Qnil;
}


int ae_protect_block_call_0(VALUE block)
{
  int exception = 0;

  rb_protect(wrapper_rb_funcall_0, block, &exception);
  return exception;
}
