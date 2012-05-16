#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_timer.h"
#include "ae_next_tick.h"
#include "ae_udp.h"
#include "ae_ip_utils.h"


static _uv_is_running;
static _uv_do_destroy;

static ID method_stop;


static
void ae_uv_prepare_callback(uv_prepare_t* handle, int status)
{
  AE_TRACE();

  // Check received interruptions in Ruby land.
  rb_thread_call_with_gvl(rb_thread_check_ints, NULL);
}


static
VALUE run_uv_without_gvl(void* param)
{
  AE_TRACE();

  int ret;

  // This should NEVER occur, but it occurs at some time after various Ctrl+C:
  //   loop do AE.run { AE::Timer.new(0) { puts "TIMER" } } end
  assert(! _uv_is_running);
//   if (_uv_is_running) {
//     AE_TRACE3("uv_run is already running, return false");
//     return Qfalse;
//   }

  /* Load the AE prepare handle */
  uv_prepare_t *ae_uv_prepare = ALLOC(uv_prepare_t);
  uv_prepare_init(uv_default_loop(), ae_uv_prepare);
  uv_prepare_start(ae_uv_prepare, ae_uv_prepare_callback);
  uv_unref(uv_default_loop());

  /* Load the AE next_tick idle handle */
  load_ae_next_tick_uv_idle();

  _uv_is_running = 1;

  /* We need to run uv at least once for the ae_next_tick_uv_idle. */
  // TODO: OSTRAS QUE NO HACE FALTA !!!
  //assert(! uv_run_once(uv_default_loop()));
  assert(! uv_run(uv_default_loop()));

  /* It's important that _uv_do_destroy is set to 0 here. If it's set before it
   * would be set to 1 by the previous uv_run_once() and would block in this simple
   * case:
   * 
   *   AE.run do
   *     AE.next_tick { AE.next_tick { } }
   *     AE.next_tick { AE.stop }
   *  end
   *
   * This is because the next_tick (uv_prepare) is executed before the async_uv.
   */
  // TODO: OSTRAS QUE NO HACE FALTA !!!
  //_uv_do_destroy = 0;

  // TODO: OSTRAS QUE NO HACE FALTA !!!
  //while (uv_loop_refcount(uv_default_loop()) > 0 && ! _uv_do_destroy)
  //  assert(! uv_run_once(uv_default_loop()));

  AE_TRACE3("UV exits");

  /* Referece again ae_uv_prepare so it can be properly closed. */
  uv_ref(uv_default_loop());
  uv_close((uv_handle_t *)ae_uv_prepare, ae_uv_handle_close_callback);

  /* Same for the av_uv_idle_next_tick handle. */
  unload_ae_next_tick_uv_idle();

  _uv_is_running = 0;

  AE_TRACE3("function terminates");

  return Qtrue;
}


static
void ubf_ae_thread_killed(void)
{
  AE_TRACE();
  AE_TRACE3("rb_funcall2(mAsyncEngine, method_stop, 0, NULL) starts");

  // Call AE.stop which closes all the active UV handles and that allows uv_run() to terminate.
  // NOTE: We have the GVL now (ruby_thread_has_gvl_p() says that) but, do we have it in any case?
  rb_funcall2(mAsyncEngine, method_stop, 0, NULL);

  AE_TRACE3("rb_funcall2(mAsyncEngine, method_stop, 0, NULL) terminates");

//   AE_TRACE3("waiting for _uv_is_running == 0");
//   while(_uv_is_running) {
//     printf(".");
//     rb_thread_schedule();
//   }
//   printf("\n");

  AE_TRACE3("function terminates");
}


VALUE AsyncEngine_c_run(VALUE self)
{
  AE_TRACE();

  // TODO: future battle...
  return rb_thread_call_without_gvl(run_uv_without_gvl, NULL, ubf_ae_thread_killed, NULL);
  //return rb_thread_call_without_gvl(run_uv_without_gvl, NULL, RUBY_UBF_IO, NULL);
}


static
void ae_uv_async_callback(uv_async_t* handle, int status)
{
  AE_TRACE();

  printf("-------- ae_uv_async_callback\n");
  uv_close((uv_handle_t *)handle, ae_uv_handle_close_callback);
}


VALUE AsyncEngine_c_destroy(VALUE self)
{
  AE_TRACE();

  // If the user calls twice AE.stop, don't repeat.
  if (_uv_do_destroy)
    return Qnil;

  // If the user calls AE.stop while AE is not running, don't store the handle.
  if (!_uv_is_running)
    return Qfalse;

  /* Load the AE async handle (for AE.stop) */
  uv_async_t *ae_uv_async = ALLOC(uv_async_t);

  _uv_do_destroy = 1;

  /* If we don't set _uv_is_running = 0 here, when the following code terminates AE.num_handles
   * returns 1.
   *
   *   AE.next_tick { AE.next_tick { AE.stop } }
   */
  _uv_is_running = 0;

  uv_async_init(uv_default_loop(), ae_uv_async, ae_uv_async_callback);
  uv_async_send(ae_uv_async);

  return Qtrue;
}


VALUE AsyncEngine_is_running(VALUE self)
{
  AE_TRACE();

  if (_uv_is_running)
    return Qtrue;
  else
    return Qfalse;
}


/*
 * Returns the number of handlers in the loop.
 */
VALUE AsyncEngine_num_handles(VALUE self)
{
  AE_TRACE();

  return INT2FIX(uv_loop_refcount(uv_default_loop()));
}


VALUE AsyncEngine_has_gvl(VALUE self)
{
  return INT2FIX(ruby_thread_has_gvl_p());
}


void Init_asyncengine_ext()
{
  AE_TRACE();

  mAsyncEngine = rb_define_module("AsyncEngine");

  rb_define_module_function(mAsyncEngine, "_c_run", AsyncEngine_c_run, 0);
  rb_define_module_function(mAsyncEngine, "_c_destroy", AsyncEngine_c_destroy, 0);
  rb_define_module_function(mAsyncEngine, "running?", AsyncEngine_is_running, 0);
  rb_define_module_function(mAsyncEngine, "num_handles", AsyncEngine_num_handles, 0);

  rb_define_module_function(mAsyncEngine, "gvl?", AsyncEngine_has_gvl, 0);

  method_stop = rb_intern("stop");

  init_ae_handle_common();
  init_ae_timer();
  init_ae_next_tick();
  init_ae_udp();
  init_ae_ip_utils();

  _uv_is_running = 0;
}