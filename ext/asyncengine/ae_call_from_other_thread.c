#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_call_from_other_thread.h"


static ID att_call_from_other_thread_procs;

static ID method_execute_call_from_other_thread_procs;
static ID method_call;

static uv_async_t* ae_call_from_other_thread_uv_async;


/** Pre-declaration of static functions. */

static void _uv_async_callback(uv_async_t* handle, int status);
static VALUE _ae_async_callback(void);


void init_ae_call_from_other_thread(void)
{
  AE_TRACE();

  rb_define_module_function(mAsyncEngine, "call_from_other_thread", AsyncEngine_call_from_other_thread, -1);

  att_call_from_other_thread_procs = rb_intern("@_call_from_other_thread_procs");

  method_execute_call_from_other_thread_procs = rb_intern("execute_call_from_other_thread_procs");
  method_call = rb_intern("call");

  ae_call_from_other_thread_uv_async = NULL;
}


void load_ae_call_from_other_thread(void)
{
  AE_TRACE();
  int r;

  AE_ASSERT(ae_call_from_other_thread_uv_async == NULL);

  ae_call_from_other_thread_uv_async = ALLOC(uv_async_t);
  r = uv_async_init(AE_uv_loop, ae_call_from_other_thread_uv_async, _uv_async_callback);
  AE_ASSERT(r == 0);
}


void unload_ae_call_from_other_thread(void)
{
  AE_TRACE();

  AE_CLOSE_UV_HANDLE(ae_call_from_other_thread_uv_async);
  ae_call_from_other_thread_uv_async = NULL;
}


VALUE AsyncEngine_call_from_other_thread(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();

  VALUE _rb_proc;
  int r;

  AE_CHECK_STATUS();

  AE_RB_CHECK_NUM_ARGS(0,1);
  AE_RB_ENSURE_BLOCK_OR_PROC(1, _rb_proc);

  rb_ary_push(rb_ivar_get(mAsyncEngine, att_call_from_other_thread_procs), _rb_proc);

  r = uv_async_send(ae_call_from_other_thread_uv_async);
  AE_ASSERT(r == 0);

  return Qtrue;
}


static
void _uv_async_callback(uv_async_t* handle, int status)
{
  AE_TRACE();

  AE_ASSERT(! status);  // TODO: testing
  ae_take_gvl_and_run_with_error_handler(_ae_async_callback);
}


static
VALUE _ae_async_callback(void)
{
  AE_TRACE();

  VALUE procs;
  long i;

  AE_ASSERT(AE_status == AE_RUNNING);

  // procs = @_att_call_from_other_thread_procs
  procs = rb_ivar_get(mAsyncEngine, att_call_from_other_thread_procs);
  // @_att_call_from_other_thread_procs = []
  rb_ivar_set(mAsyncEngine, att_call_from_other_thread_procs, rb_ary_new());

  // Iterate procs Array and call each proc.
  for(i=0 ; i<RARRAY_LEN(procs) ; i++) {
    ae_run_with_error_handler(ae_block_call_0, rb_ary_entry(procs, i));
  }
  procs = Qnil;

  return Qnil;
}
