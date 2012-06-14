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
      AE_DEBUG("AE running, let's go with the new handle...");  \
      break;  \
    case AE_RELEASING:  \
      AE_DEBUG("AE releasing, return false");  \
      return Qfalse;  \
      break;  \
    case AE_STOPPED:  \
      AE_DEBUG("AE stopped, raise");  \
      rb_raise(eAsyncEngineNotRunningError, "AsyncEngine is not running");  \
      break;  \
    default:  \
      AE_ABORT("unknown AE_status ???");  \
      break;  \
  }


VALUE mAsyncEngine;
VALUE cAsyncEngineHandle;
VALUE eAsyncEngineError;
VALUE eAsyncEngineUvError;
VALUE eAsyncEngineNotRunningError;
VALUE eAsyncEngineStillReleasingError;

VALUE AE_handles;
VALUE AE_blocks;
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