#include "asyncengine_ruby.h"
#include "ae_handle_common.h"


// Ruby modules/classes.
static VALUE mKernel;

// Ruby method names.
static ID method_call;
static ID method_handle_error;
static ID method_raise;

// C variable holding current proc number.
static long proc_long_id;


void init_ae_handle_common(void)
{
  AE_TRACE();

  mKernel = rb_define_module("Kernel");

  method_call = rb_intern("call");
  method_handle_error = rb_intern("handle_error");
  method_raise = rb_intern("raise");

  proc_long_id = 0;
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


VALUE ae_store_proc(VALUE proc)
{
  AE_TRACE();

  VALUE proc_id = LONG2FIX(++proc_long_id);
  rb_hash_aset(AE_procs, proc_id, proc);

  return proc_id;
}


VALUE ae_get_proc(VALUE proc_id)
{
  AE_TRACE();

  return rb_hash_aref(AE_procs, proc_id);
}


VALUE ae_remove_proc(VALUE proc_id)
{
  AE_TRACE();

  return rb_hash_delete(AE_procs, proc_id);
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

  if (NIL_P(ae_uv_error = rb_hash_aref(AE_UV_ERRORS, INT2FIX(uv_errno))))
    ae_uv_error = rb_hash_aref(AE_UV_ERRORS, INT2FIX(-1));

  return ae_uv_error;
}


VALUE ae_get_last_uv_error(void)
{
  AE_TRACE();
  VALUE ae_uv_error;

  if (NIL_P(ae_uv_error = rb_hash_aref(AE_UV_ERRORS, INT2FIX(uv_last_error(AE_uv_loop).code))))
    ae_uv_error = rb_hash_aref(AE_UV_ERRORS, INT2FIX(-1));

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


VALUE ae_proc_call_0(VALUE proc)
{
  AE_TRACE();

  AE_ASSERT(! NIL_P(proc));
  return rb_funcall2(proc, method_call, 0, NULL);
}


VALUE ae_proc_call_1(VALUE proc, VALUE param)
{
  AE_TRACE();

  AE_ASSERT(! NIL_P(proc));
  return rb_funcall2(proc, method_call, 1, &param);
}


VALUE ae_proc_call_2(VALUE proc, VALUE param1, VALUE param2)
{
  AE_TRACE();

  AE_ASSERT(! NIL_P(proc));
  return rb_funcall(proc, method_call, 2, param1, param2);
}


VALUE ae_proc_call_3(VALUE proc, VALUE param1, VALUE param2, VALUE param3)
{
  AE_TRACE();

  AE_ASSERT(! NIL_P(proc));
  return rb_funcall(proc, method_call, 3, param1, param2, param3);
}


/*
 * When any AsyncEngine handler runs a handle method having the GVL,
 * it must use this function, which can receive an optional VALUE parameter.
 */
VALUE ae_run_with_error_handler(void* function, VALUE param)
{
  AE_TRACE();

  VALUE ret, error;
  int error_tag;

  if (param)
    ret = rb_protect(function, (VALUE)param, &error_tag);
  else
    ret = rb_protect(function, Qnil, &error_tag);

  /*
   * If an error occurs while in function() it can be due:
   *
   * - An Exception (including SystemExit), this is "rescue-able" via "rescue Exception"
   *   and will run the "ensure" code if present. In this case rb_errinfo() returns the
   *   exact Exception object.
   *
   * - A Thread#kill. This is NOT "rescue-able" via "rescue Exception" but it WILL run
   *   the "ensure" code if present. In this case rb_errinfo() returns FIXNUM 8.
   *
   * So, check the class of the object returned by rb_errinfo(). If it's an Exception then
   * store it, release the loop and raise it. Otherwise (Thread#kill) then don't store the
   * exception returned by rb_errinfo() and just release the loop. Ruby will do the rest.
   */

  if (error_tag) {
    // NOTE: This could return Fixnum 8: https://github.com/ibc/AsyncEngine/issues/4,
    // so the error handler must check it.
    error = rb_errinfo();
    rb_set_errinfo(Qnil);

    // NOTE: While in RELEASING status ignore errors in user's provided callback/method.
    if (AE_status == AE_RELEASING) {
      AE_DEBUG2("error %s rescued while in RELEASING status, ignoring it", rb_obj_classname(error));
      return Qnil;
    }
    else {
      AE_DEBUG("error %s rescued, passing it to the error handler", rb_obj_classname(error));
      ae_handle_error(error);
      return Qnil;
    }
  }
  else
    return ret;
}


/*
 * When any AsyncEngine handler runs a Ruby callback, it must
 * use this function, which must be called without the GVL.
 * TODO: Allow passing a VALUE parameter...?
 */
VALUE ae_take_gvl_and_run_with_error_handler(void* function)
{
  AE_TRACE();

  return rb_thread_call_with_gvl(ae_run_with_error_handler, function);
}
