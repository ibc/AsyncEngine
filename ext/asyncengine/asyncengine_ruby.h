#ifndef ASYNCENGINE_RUBY_H
#define ASYNCENGINE_RUBY_H


#include <ruby.h>
#include "libuv/include/uv.h"
#include "rb_utilities.h"
#include "debug.h"


VALUE mAsyncEngine;
VALUE cAsyncEngineHandle;
VALUE eAsyncEngineError;

uv_loop_t *AE_uv_loop;
ngx_queue_t AE_handles_q;  // TODO: fuera

VALUE AE_handles;
VALUE AE_blocks;
VALUE AE_UV_ERRNOS;


#endif  /* ASYNCENGINE_RUBY_H */