#include "asyncengine.h"
#include "ae_timers.h"
#include <signal.h>


// C variables.
long handle_number = 0;

// Ruby modules/classes.
static VALUE mAsyncEngine;

// Ruby attributes.
static ID att_handles;
static ID att_trapped_signals;

// Ruby method names.
static ID id_method_call;



inline long AsyncEngine_next_handle(void)
{
  return ++handle_number;
}


static void signal_handler(int signal_number)
{
  VALUE trapped_signals = rb_ivar_get(mAsyncEngine, att_trapped_signals);
  rb_funcall(rb_hash_lookup(trapped_signals, INT2FIX(signal_number)), id_method_call, 0, 0);
  // TODO: Mejor usar rb_block_call(obj, id_each, 0, 0, min_ii, (VALUE)&result);
  // aunque creo que no hay en 1.8
  // NOTA: Que va, no es cierto que sea mas rapido ni nada.
}


VALUE AsyncEngine_c_add_handle(VALUE self, VALUE handle)
{
  VALUE rb_handle_number = LONG2FIX(++handle_number);

  rb_hash_aset(rb_ivar_get(mAsyncEngine, att_handles), rb_handle_number, handle);

  return rb_handle_number;
}


VALUE AsyncEngine_trap_signal(VALUE self, VALUE rb_signal_number)
{
  // TODO: Check http://linux.die.net/man/2/sigaction (recommended by http://linux.die.net/man/2/signal).
  // TODO: En teoria devuelve EINVAL si el numero de signal pasado no existe, pero me da error porque
  // no hay EINVAL en signal.h.
  if (signal(FIX2INT(rb_signal_number), signal_handler) < 0)
    return Qfalse;
  return Qtrue;
}



VALUE AsyncEngine_c_start(VALUE self)
{
  //uv_ref(uv_default_loop());  // TODO: Es una ñapa temporal !!!

  // Esto lo hacemos en Ruby.
//   if (rb_block_given_p()) {
//     rb_yield(Qnil);
//   }

  if (! uv_run(uv_default_loop()))
    return Qtrue;
  else
    return Qfalse;
}


void Init_asyncengine_ext()
{
  mAsyncEngine = rb_define_module("AsyncEngine");
  rb_define_module_function(mAsyncEngine, "_c_start", AsyncEngine_c_start, 0);
  rb_define_module_function(mAsyncEngine, "_c_add_handle", AsyncEngine_c_add_handle, 1);
  rb_define_module_function(mAsyncEngine, "trap_signal", AsyncEngine_trap_signal, 1);

  /* Timers */
  rb_define_module_function(mAsyncEngine, "_c_add_timer", AsyncEngine_c_add_timer, 3);

  att_handles = rb_intern("@handles");
  att_trapped_signals = rb_intern("@trapped_signals");

  id_method_call = rb_intern("call");
}