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


void init_ae_handle_common(void)
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


int ae_get_last_uv_error_int(void)
{
  AE_TRACE();

  return uv_last_error(AE_uv_loop).code;
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

  if (NIL_P(ae_uv_error = rb_hash_aref(AE_UV_ERRNOS, INT2FIX(uv_last_error(AE_uv_loop).code))))
    ae_uv_error = rb_hash_aref(AE_UV_ERRNOS, INT2FIX(-1));

  return ae_uv_error;
}


void ae_raise_uv_error(int uv_errno)
{
  AE_TRACE();

  VALUE ae_uv_error = ae_get_uv_error(uv_errno);

  rb_funcall2(mKernel, method_raise, 1, &ae_uv_error);
}


void ae_raise_last_uv_error(void)
{
  AE_TRACE();

  VALUE ae_uv_error = ae_get_last_uv_error();

  rb_funcall2(mKernel, method_raise, 1, &ae_uv_error);
}


VALUE ae_block_call_0(VALUE _rb_block)
{
  AE_TRACE();

  // This could occur if for example the user calls AE.stop and inmediatelly sets a UV async. The
  // UV async callback would not find it block since it has been removed by AE.stop.
  if (NIL_P(_rb_block)) {
    AE_WARN("called with nil as _rb_block");
    return Qfalse;
  }
  return rb_funcall2(_rb_block, method_call, 0, NULL);
}


VALUE ae_block_call_1(VALUE _rb_block, VALUE param)
{
  AE_TRACE();

  if (NIL_P(_rb_block)) {
    AE_WARN("called with nil as _rb_block");
    return Qfalse;
  }
  return rb_funcall2(_rb_block, method_call, 1, &param);
}


static
VALUE execute_function_with_glv_and_rb_protect(void* function)
{
  AE_TRACE();

  int error_tag = 0;
  VALUE ret;
  VALUE exception;

  ret = rb_protect(function, Qnil, &error_tag);

  /*
   * If an exception occurs while in function() it can be due:
   * 
   * - An Exception (including SystemExit), this is "rescue-able" via "rescue Exception"
   *   and will run the "ensure" code if present. In this case rb_errinfo() gets the
   *   exact Exception object.
   *
   * - A Thread#kill. This is NOT "rescue-able" via "rescue Exception" but it WILL run
   *   the "ensure" code if present. In this case rb_errinfo() returns FIXNUM 8 (it maybe
   *   different, no idea).
   *
   * So, check the class of the object returned by rb_errinfo(). If it's an Exception then
   * store it, release the loop and raise it. Otherwise (Thread#kill) then don't store the
   * exception returned by rb_errinfo() and just release the loop. Ruby will do the rest.
   */
  if (error_tag) {
    if (error_tag == 8) AE_WARN("******************************************************************** error_tag == 8");

    VALUE exception = rb_errinfo();
    rb_set_errinfo(Qnil);
    printf("*** DBG: execute_function_with_glv_and_rb_protect():  exception.class: %s\n", rb_obj_classname(exception));

    // Call AsyncEngine.handle_exception().
    return rb_funcall2(mAsyncEngine, method_handle_exception, 1, &exception);
  }
  // Otherwise just return the VALUE returned by rb_protec() above.
  else
    return ret;
}


/*
 * Executes the given function taking the GVL and using rb_protect().
 */
VALUE ae_execute_in_ruby_land(void* function)
{
  AE_TRACE();

  return rb_thread_call_with_gvl(execute_function_with_glv_and_rb_protect, function);
}


/*
 * Executes the given function using rb_protect() and ignoring any error/exception.
 * NOTE: This function MUST be called with the GVL.
 */
VALUE ae_safe_run_ruby_function(void* function)
{
  AE_TRACE2();

  int error_tag;

  rb_protect(function, Qnil, &error_tag);

  if (error_tag) {
    rb_set_errinfo(Qnil);  // TODO: sure?
    AE_WARN("error rescued by rb_protect() and ignored while executing the function");
  }
}