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

static int is_ae_ready;
static int do_stop;


// TODO: Temporal function for debugging purposes, since active_handles
// will be removed soon from UV.
static
long _uv_num_active_handlers(void)
{
  ngx_queue_t *q;
  long num_active_handlers = 0;
  ngx_queue_foreach(q, &uv_default_loop()->active_handles) {
    num_active_handlers++;
  }
  return num_active_handlers;
}


VALUE AsyncEngine_num_uv_active_handles(VALUE self)
{
  AE_TRACE();

  return LONG2FIX(_uv_num_active_handlers());
}


VALUE AsyncEngine_init(VALUE self)
{
  AE_TRACE();

  AE_handles = rb_ivar_get(mAsyncEngine, att_handles);
  AE_blocks = rb_ivar_get(mAsyncEngine, att_blocks);
  AE_UV_ERRNOS = rb_const_get(mAsyncEngine, const_UV_ERRNOS);

  /* Load the UV idle (next tick) now. */
  load_ae_next_tick_uv_idle();

  /* Set is_ae_ready=1 so handles can be added. */
  is_ae_ready = 1;
  do_stop = 0;

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

  //uv_run(uv_default_loop());

  // Quiero poder parar si externamente seteo do_stop a 1. Y luego haré la limpieza de handles.
  // TEST Saghul (funca).
  // TODO: El día que cambien la estructura interna esto se va a la porra.
  uv_loop_t *loop =  uv_default_loop();
  while(!do_stop && (!ngx_queue_empty(&loop->active_handles) || !ngx_queue_empty(&loop->active_reqs))) {
    uv_run_once(loop);
  }
  do_stop = 0;

  AE_DEBUG("uv_run() terminates");

  is_ae_ready = 0;

  return Qtrue;
}


VALUE AsyncEngine_run_uv(VALUE self)
{
  AE_TRACE();

  rb_thread_call_without_gvl(run_uv_without_gvl, NULL, ae_ubf, NULL);

  AE_DEBUG("function terminates");
}


static
VALUE run_uv_once_without_gvl(void)
{
  AE_TRACE();

  /* Close the UV idle (next tick) now. */
  unload_ae_next_tick_uv_idle();

  /* Run UV loop (it blocks if there were handles in the given block). */
  AE_DEBUG("uv_run() starts...");
  uv_run_once(uv_default_loop());
  AE_DEBUG("uv_run() terminates");

  is_ae_ready = 0;

  return Qtrue;
}


/*
 * This method is called in the _ensure_ block of AsyncEngine.run() method
 * after AsyncEngine.destroy_ae_handles() to cause a single UV iteration in order
 * to invoke all the uv_close callbacks and free() the uv_handles.
 */
VALUE AsyncEngine_run_uv_once(VALUE self)
{
  AE_TRACE();

  /* There MUST NOT be UV active handles at this time, we enter here just to
   * iterate once for freeing closed UV handles not freed yet. */
  //AE_ASSERT(ngx_queue_empty(&uv_default_loop()->active_handles));
  // NOTE: If the blocks just contains a next_tick and raises, next_tick idle is
  // not removed by AE.destroy_ae_handles, so this assert would fail:
  //   AE.run { AE.next_tick { } ; RAISE_1 }
  // TODO: We need to assert that there are 1 or 0 active handles.
  // NOTE: uv_default_loop()->active_handles will be removed soon, so...
  AE_ASSERT(_uv_num_active_handlers() <= 1);

  rb_thread_call_without_gvl(run_uv_once_without_gvl, NULL, NULL, NULL);

  AE_DEBUG("function terminates");
}


VALUE AsyncEngine_stop_uv(VALUE self)
{
  AE_TRACE();

  do_stop = 1;
}




void Init_asyncengine_ext()
{
  AE_TRACE();

  mAsyncEngine = rb_define_module("AsyncEngine");

  rb_define_module_function(mAsyncEngine, "init", AsyncEngine_init, 0);
  rb_define_module_function(mAsyncEngine, "run_uv", AsyncEngine_run_uv, 0);
  rb_define_module_function(mAsyncEngine, "run_uv_once", AsyncEngine_run_uv_once, 0);
  rb_define_module_function(mAsyncEngine, "stop_uv", AsyncEngine_stop_uv, 0);
  rb_define_module_function(mAsyncEngine, "num_uv_active_handles", AsyncEngine_num_uv_active_handles, 0);

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
