#ifndef ASYNCENGINE_RUBY_H
#define ASYNCENGINE_RUBY_H


#include <ruby.h>
#include "libuv/include/uv.h"
#include "rb_utilities.h"
#include "debug.h"


/*
 * This macro behaves as follows:
 * - Returns false if AE is releasing.
 * - Raises a AE::NotRunningError exception if stopped.
 * - Does nothing if AE is running.
 */
#define AE_CHECK_STATUS()  \
  switch(AE_status) {  \
    case AE_RUNNING:  \
      AE_DEBUG("AsyncEngine status: RUNNING => let's go");  \
      break;  \
    case AE_RELEASING:  \
      AE_DEBUG("AsyncEngine status: RELEASING => return Qfalse");  \
      return Qfalse;  \
      break;  \
    case AE_STOPPED:  \
      AE_DEBUG("AsyncEngine status: STOPPED => raise error");  \
      rb_raise(eAsyncEngineNotRunningError, "AsyncEngine is not running");  \
      break;  \
    default:  \
      AE_ABORT("AsyncEngine status: unknown => abort!");  \
      break;  \
  }


VALUE mAsyncEngine;
VALUE cAsyncEngineHandle;
VALUE eAsyncEngineError;
VALUE eAsyncEngineUvError;
VALUE eAsyncEngineNotRunningError;

VALUE AE_handles;
VALUE AE_procs;
VALUE AE_UV_ERRORS;

enum ae_status {
  AE_RUNNING = 1,
  AE_RELEASING = 2,
  AE_STOPPED = 3
};

enum ae_status AE_status;

uv_loop_t *AE_uv_loop;


void ae_handle_error(VALUE error);


#endif  /* ASYNCENGINE_RUBY_H */