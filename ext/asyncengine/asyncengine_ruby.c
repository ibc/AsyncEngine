#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_async.h"
#include "ae_timer.h"
#include "ae_next_tick.h"
#include "ae_udp.h"
#include "ae_ip_utils.h"


static _uv_is_running;
static do_stop;

uv_prepare_t *ae_uv_prepare;
uv_check_t *ae_uv_check;

static ID method_destroy;


static
VALUE stop_ae_from_kill(VALUE ignore)
{
  AE_TRACE();

  AE_DEBUG("running AE.destroy");

  rb_funcall2(mAsyncEngine, method_destroy, 0, NULL);

  return Qnil;
}


static
void ae_uv_prepare_callback(uv_prepare_t* handle, int status)
{
  AE_TRACE();

  if (do_stop) {
    AE_DEBUG("do_stop == 1");
    do_stop = 0;
    ae_execute_function_with_gvl_and_protect(stop_ae_from_kill, Qnil);
  }

  // Check received interruptions in Ruby land.
  rb_thread_call_with_gvl(rb_thread_check_ints, NULL);
}


static
void load_ae_uv_prepare(void)
{
  AE_TRACE();

  if (ae_uv_prepare)
    return;

  ae_uv_prepare = ALLOC(uv_prepare_t);
  uv_prepare_init(uv_default_loop(), ae_uv_prepare);
  uv_prepare_start(ae_uv_prepare, ae_uv_prepare_callback);
  uv_unref((uv_handle_t*)ae_uv_prepare);
}


static
void unload_ae_uv_prepare(void)
{
  AE_TRACE();

  if (! ae_uv_prepare)
    return;

  uv_ref((uv_handle_t*)ae_uv_prepare);
  uv_close((uv_handle_t *)ae_uv_prepare, ae_uv_handle_close_callback);
  ae_uv_prepare = NULL;
}


static
void ae_uv_check_callback(uv_check_t* handle, int status)
{
  AE_TRACE();

  if (do_stop) {
    AE_DEBUG("do_stop == 1");
    do_stop = 0;
    ae_execute_function_with_gvl_and_protect(stop_ae_from_kill, Qnil);
  }
}


static
void load_ae_uv_check(void)
{
  AE_TRACE();

  if (ae_uv_check)
    return;

  ae_uv_check = ALLOC(uv_check_t);
  uv_check_init(uv_default_loop(), ae_uv_check);
  uv_check_start(ae_uv_check, ae_uv_check_callback);
  uv_unref((uv_handle_t*)ae_uv_check);
}


static
void unload_ae_uv_check(void)
{
  AE_TRACE();

  if (! ae_uv_check)
    return;
  
  uv_ref((uv_handle_t*)ae_uv_check);
  uv_close((uv_handle_t *)ae_uv_check, ae_uv_handle_close_callback);
  ae_uv_check = NULL;
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

  _uv_is_running = 1;

  /* Load the AE prepare and check handles */
  load_ae_uv_prepare();
  load_ae_uv_check();

  assert(! uv_run(uv_default_loop()));

  AE_DEBUG("uv_run() exits");

  unload_ae_uv_prepare();
  unload_ae_uv_check();

  /* Unload the av_uv_idle_next_tick handle (if AE.next_tick was used at least once). */
  // TODO: peta, ver bug_uv_run_2.rb.
  //unload_ae_next_tick_uv_idle();

  _uv_is_running = 0;

  AE_DEBUG("function terminates");

  return Qtrue;
}


static
void ubf_ae_thread_killed(void)
{
  AE_TRACE();

  AE_DEBUG("setting do_stop = 1");

  do_stop = 1;

  // TODO:  No me mola demasiado, antes no hacía falta pero
  // ahora al recibir un Interrupt no termina uv_run !!!
  //_uv_is_running = 0;

  AE_DEBUG("function terminates");
}


VALUE AsyncEngine_c_run(VALUE self)
{
  AE_TRACE();

  return rb_thread_call_without_gvl(run_uv_without_gvl, NULL, ubf_ae_thread_killed, NULL);
}


VALUE AsyncEngine_c_stop(VALUE self)
{
  AE_TRACE();

  do_stop = 1;
}


VALUE AsyncEngine_is_running(VALUE self)
{
  AE_TRACE();

  if (_uv_is_running)
    return Qtrue;
  else
    return Qfalse;
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
  rb_define_module_function(mAsyncEngine, "_c_stop", AsyncEngine_c_stop, 0);
  rb_define_module_function(mAsyncEngine, "running?", AsyncEngine_is_running, 0);

  rb_define_module_function(mAsyncEngine, "gvl?", AsyncEngine_has_gvl, 0);

  method_destroy = rb_intern("destroy");

  init_ae_handle_common();
  init_ae_async();
  init_ae_timer();
  init_ae_next_tick();
  init_ae_udp();
  init_ae_ip_utils();

  _uv_is_running = 0;
  do_stop = 0;

  ae_uv_prepare = NULL;
  ae_uv_check = NULL;
}