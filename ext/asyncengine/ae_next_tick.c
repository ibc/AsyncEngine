#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_next_tick.h"


static ID att_next_tick_procs;

static uv_idle_t* ae_next_tick_uv_idle;


/** Pre-declaration of static functions. */

static void _uv_idle_callback(uv_idle_t* handle, int status);
static VALUE _ae_idle_callback(void);


void init_ae_next_tick(void)
{
  AE_TRACE();

  rb_define_module_function(mAsyncEngine, "next_tick", AsyncEngine_next_tick, -1);

  att_next_tick_procs = rb_intern("@_next_tick_procs");

  ae_next_tick_uv_idle = NULL;
}


void load_ae_next_tick_uv_idle(void)
{
  AE_TRACE();
  int r;

  AE_ASSERT(ae_next_tick_uv_idle == NULL);

  ae_next_tick_uv_idle = ALLOC(uv_idle_t);
  r = uv_idle_init(AE_uv_loop, ae_next_tick_uv_idle);
  AE_ASSERT(r == 0);
}


void unload_ae_next_tick_uv_idle(void)
{
  AE_TRACE();

  AE_ASSERT(ae_next_tick_uv_idle != NULL);

  AE_CLOSE_UV_HANDLE(ae_next_tick_uv_idle);
  ae_next_tick_uv_idle = NULL;
}


static
VALUE AsyncEngine_next_tick(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();
  VALUE proc;
  int r;

  AE_ASSERT(ae_next_tick_uv_idle != NULL);

  AE_CHECK_STATUS();
  AE_RB_CHECK_NUM_ARGS(0,1);
  AE_RB_ENSURE_BLOCK_OR_PROC(1, proc);

  rb_ary_push(rb_ivar_get(mAsyncEngine, att_next_tick_procs), proc);
  if (! uv_is_active((uv_handle_t *)ae_next_tick_uv_idle)) {
    r = uv_idle_start(ae_next_tick_uv_idle, _uv_idle_callback);
    AE_ASSERT(r == 0);
  }

  return Qtrue;
}


static
void _uv_idle_callback(uv_idle_t* handle, int status)
{
  AE_TRACE();

  uv_idle_stop(handle);
  ae_take_gvl_and_run_with_error_handler(_ae_idle_callback);
}


static
VALUE _ae_idle_callback(void)
{
  AE_TRACE();
  VALUE procs;
  long i;

  AE_ASSERT(AE_status == AE_RUNNING);

  // procs = @_next_tick_procs
  procs = rb_ivar_get(mAsyncEngine, att_next_tick_procs);
  // @_next_tick_procs = []
  rb_ivar_set(mAsyncEngine, att_next_tick_procs, rb_ary_new());

  // Iterate procs Array and call each proc.
  for(i=0 ; i<RARRAY_LEN(procs) ; i++) {
    // It could occur that one of these procs is a release_loop() method, so
    // stop iterating.
    if (AE_status != AE_RUNNING)
      break;
    ae_run_with_error_handler(ae_proc_call_0, rb_ary_entry(procs, i));
  }
  procs = Qnil;

  return Qnil;
}
