#include "asyncengine_ruby.h"
#include "ae_handle_common.h"


// Ruby modules/classes.
static VALUE mKernel;

// Ruby method names.
static ID method_call;
static ID method_handle_exception;
static ID method_raise;

// C variable holding current block number.
static long block_long_id;


void init_ae_handle_common()
{
  AE_TRACE();

  mKernel = rb_define_module("Kernel");

  method_call = rb_intern("call");
  method_handle_exception = rb_intern("handle_exception");
  method_raise = rb_intern("raise");

  block_long_id = 0;
}


VALUE ae_store_handle(VALUE ae_handle)
{
  AE_TRACE();

  VALUE ae_handle_id = rb_obj_id(ae_handle);
  rb_hash_aset(AE_handles, ae_handle_id, ae_handle);

  return ae_handle_id;
}


VALUE ae_get_handle(VALUE ae_handle_id)
{
  AE_TRACE();

  return rb_hash_aref(AE_handles, ae_handle_id);
}


VALUE ae_remove_handle(VALUE ae_handle_id)
{
  AE_TRACE();

  return rb_hash_delete(AE_handles, ae_handle_id);
}


VALUE ae_store_block(VALUE block)
{
  AE_TRACE();

  VALUE block_id = LONG2FIX(++block_long_id);
  rb_hash_aset(AE_blocks, block_id, block);

  return block_id;
}


VALUE ae_get_block(VALUE block_id)
{
  AE_TRACE();

  return rb_hash_aref(AE_blocks, block_id);
}


VALUE ae_remove_block(VALUE block_id)
{
  AE_TRACE();

  return rb_hash_delete(AE_blocks, block_id);
}


void ae_uv_handle_close_callback(uv_handle_t* handle)
{
  AE_TRACE();

  xfree(handle);
}


VALUE ae_get_uv_error(int uv_errno)
{
  AE_TRACE();

  VALUE ae_uv_error;

  if (NIL_P(ae_uv_error = rb_hash_aref(AE_UV_ERRNOS, INT2FIX(uv_errno))))
    ae_uv_error = rb_hash_aref(AE_UV_ERRNOS, INT2FIX(-1));

  return ae_uv_error;
}


VALUE ae_get_last_uv_error(void)
{
  AE_TRACE();

  VALUE ae_uv_error;

  if (NIL_P(ae_uv_error = rb_hash_aref(AE_UV_ERRNOS, INT2FIX(uv_last_error(uv_default_loop()).code))))
    ae_uv_error = rb_hash_aref(AE_UV_ERRNOS, INT2FIX(-1));

  return ae_uv_error;
}


void ae_raise_last_uv_error(void)
{
  AE_TRACE();

  VALUE ae_uv_error = ae_get_last_uv_error();

  rb_funcall2(mKernel, method_raise, 1, &ae_uv_error);
}


VALUE ae_block_call_0(VALUE rb_block)
{
  AE_TRACE();

  // This could occur if for example the user calls AE.stop and inmediatelly sets a UV async. The
  // UV async callback would not find it block since it has been removed by AE.stop.
  if (NIL_P(rb_block)) {
    AE_WARN("called with nil as rb_block");
    return Qfalse;
  }
  return rb_funcall2(rb_block, method_call, 0, NULL);
}


VALUE ae_block_call_1(VALUE rb_block, VALUE param)
{
  AE_TRACE();

  if (NIL_P(rb_block)) {
    AE_WARN("called with nil as rb_block");
    return Qfalse;
  }
  return rb_funcall2(rb_block, method_call, 1, &param);
}


typedef struct {
  function_with_gvl_and_protect function;
  VALUE rb_param;
} struct_function_with_gvl_and_protect;


// TODO: jsut for considering it... due to the bug in bug_uv_run_2.rb
// static
// void ae_handle_exception(int exception_tag)
// {
//   AE_TRACE();
//   
//   // rb_errinfo() gives the current exception object in this thread.
//   VALUE exception = rb_errinfo();
//   
//   // Just check the exception in the user provided AE.exception_handler block if
//   // it is a StandardError or LoadError. Otherwise raise it and terminate.
//   if (rb_obj_is_kind_of(exception, rb_eStandardError) == Qtrue ||
//     rb_obj_is_kind_of(exception, rb_eLoadError) == Qtrue)
//   {
//     rb_funcall2(mAsyncEngine, method_handle_exception, 1, &exception);
//     // Dissable the current thread exception.
//     rb_set_errinfo(Qnil);
//   }
//   else
//     rb_jump_tag(exception_tag);
// }


static
VALUE execute_function_with_protect(struct_function_with_gvl_and_protect *data)
{
  AE_TRACE();

  int exception_tag;
  VALUE ret;

  ret = rb_protect(data->function, data->rb_param, &exception_tag);

  // If an exception occurred then call AsyncEngine.handle_exception(exception).
  if (exception_tag) {
    VALUE exception = rb_errinfo();
    rb_funcall2(mAsyncEngine, method_handle_exception, 1, &exception);
  }

  return ret;
}


/*
 * Executes the given function taking the GVL and using rb_protect(), and passes
 * rb_param as a single argument to the given function.
 */
VALUE ae_execute_function_with_gvl_and_protect(function_with_gvl_and_protect function, VALUE rb_param)
{
  AE_TRACE();

  struct_function_with_gvl_and_protect data;
  data.function = function;
  data.rb_param = rb_param;

  return rb_thread_call_with_gvl(execute_function_with_protect, &data);
}