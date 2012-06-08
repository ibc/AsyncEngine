#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_next_tick.h"


static ID att_next_ticks;

static ID method_execute_next_ticks;

static uv_idle_t* ae_next_tick_uv_idle;


void init_ae_next_tick(void)
{
  AE_TRACE();

  rb_define_module_function(mAsyncEngine, "next_tick", AsyncEngine_next_tick, -1);

  att_next_ticks = rb_intern("@_next_ticks");

  method_execute_next_ticks = rb_intern("execute_next_ticks");

  ae_next_tick_uv_idle = NULL;
}


void load_ae_next_tick_uv_idle(void)
{
  AE_TRACE();

  if (ae_next_tick_uv_idle) {
    AE_WARN("ae_next_tick_uv_idle already loaded !!!");
    return;
  }

  ae_next_tick_uv_idle = ALLOC(uv_idle_t);
  AE_ASSERT(! uv_idle_init(AE_uv_loop, ae_next_tick_uv_idle));
}


void unload_ae_next_tick_uv_idle(void)
{
  AE_TRACE();

  if (! ae_next_tick_uv_idle) {
    AE_WARN("ae_next_tick_uv_idle already unloaded");
    return;
  }

  uv_close((uv_handle_t *)ae_next_tick_uv_idle, ae_uv_handle_close_callback);
  ae_next_tick_uv_idle = NULL;
}


static
void ae_next_tick_callback_with_gvl(void)
{
  AE_TRACE();

  rb_funcall2(mAsyncEngine, method_execute_next_ticks, 0, NULL);
}


static
void _uv_idle_callback(uv_idle_t* handle, int status)
{
  AE_TRACE();

  AE_DEBUG("uv_idle_stop(handle);");
  uv_idle_stop(handle);

  rb_thread_call_with_gvl(ae_next_tick_callback_with_gvl, NULL);
}


void ae_next_tick(VALUE _rb_block)
{
  AE_TRACE2();

  rb_ary_push(rb_ivar_get(mAsyncEngine, att_next_ticks), _rb_block);

  if (! uv_is_active((uv_handle_t *)ae_next_tick_uv_idle))
    uv_idle_start(ae_next_tick_uv_idle, _uv_idle_callback);
}


VALUE AsyncEngine_next_tick(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE2();

  VALUE _rb_block;

  ae_check_running();

  AE_ASSERT(ae_next_tick_uv_idle != NULL);  // TODO: TMP

  AE_RB_CHECK_NUM_ARGS(0,1);
  AE_RB_GET_BLOCK_OR_PROC(1, _rb_block);

  if (NIL_P(_rb_block))
    rb_raise(rb_eArgError, "no block given");

  ae_next_tick(_rb_block);

  return Qtrue;
}