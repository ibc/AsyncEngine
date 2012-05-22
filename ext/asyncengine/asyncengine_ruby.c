#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_async.h"
#include "ae_timer.h"
#include "ae_next_tick.h"
#include "ae_udp.h"
#include "ae_ip_utils.h"


static ID att_handles;
static ID att_blocks;
static ID const_UV_ERRNOS;


VALUE AsyncEngine_init(VALUE self)
{
  AE_TRACE();

  AE_handles = rb_ivar_get(mAsyncEngine, att_handles);
  AE_blocks = rb_ivar_get(mAsyncEngine, att_blocks);
  AE_UV_ERRNOS = rb_const_get(mAsyncEngine, const_UV_ERRNOS);

  return Qnil;
}


static
void ae_ubf_uv_async_callback(uv_async_t* handle, int status)
{
  AE_TRACE();

  // TODO: testing
  AE_ASSERT(! status);

  uv_close((uv_handle_t *)handle, ae_uv_handle_close_callback);

  AE_DEBUG("call rb_thread_call_with_gvl(rb_thread_check_ints, NULL)");
  rb_thread_call_with_gvl(rb_thread_check_ints, NULL);
}


static
void ae_ubf(void)
{
  AE_TRACE();

  /*
   * When a signal is received by a Ruby process running a blocking code (without GVL)
   * Ruby calls the ubf() function. But this ubf() function could be called also from
   * other thread (i.e. ae_tread.kill) so we don't know if the ubf() is being executing
   * in AE thread, nor if it has been called due a received signal or a Thread#kill.
   * Therefore, do nothing but check interrupts in Ruby land via a thread safe uv_async.
   */

  uv_async_t* ae_ubf_uv_async = ALLOC(uv_async_t);

  AE_ASSERT(! uv_async_init(uv_default_loop(), ae_ubf_uv_async, ae_ubf_uv_async_callback));
  AE_ASSERT(! uv_async_send(ae_ubf_uv_async));
}


static
VALUE run_uv_without_gvl(void)
{
  AE_TRACE();

  /* Run UV loop (it blocks if there were handles in the given block). */
  AE_DEBUG("uv_run() starts...");
  AE_ASSERT(! uv_run(uv_default_loop()));
  AE_DEBUG("uv_run() terminates");

  return Qtrue;
}


VALUE AsyncEngine_run_uv(VALUE self)
{
  AE_TRACE();

  rb_thread_call_without_gvl(run_uv_without_gvl, NULL, ae_ubf, NULL);

  AE_DEBUG("function terminates");
}


/*
 * This method is called in the _ensure_ block of AsyncEngine.run() method
 * after AsyncEngine.destroy_ae_handles() to cause a single UV iteration in order
 * to invoke all the uv_close callbacks and free() the uv_handles.
 */
VALUE AsyncEngine_run_uv_once(VALUE self)
{
  AE_TRACE();

  AE_DEBUG("uv_run_once() starts...");
  AE_ASSERT(! uv_run_once(uv_default_loop()));
  AE_DEBUG("uv_run_once() terminates");

  AE_DEBUG("function terminates");
}




void Init_asyncengine_ext()
{
  AE_TRACE();

  mAsyncEngine = rb_define_module("AsyncEngine");

  rb_define_module_function(mAsyncEngine, "init", AsyncEngine_init, 0);
  rb_define_module_function(mAsyncEngine, "run_uv", AsyncEngine_run_uv, 0);
  rb_define_module_function(mAsyncEngine, "run_uv_once", AsyncEngine_run_uv_once, 0);

  att_handles = rb_intern("@_handles");
  att_blocks = rb_intern("@_blocks");
  const_UV_ERRNOS = rb_intern("UV_ERRNOS");

  init_ae_handle_common();
  init_ae_async();
  init_ae_timer();
  init_ae_next_tick();
  init_ae_udp();
  init_ae_ip_utils();
}
