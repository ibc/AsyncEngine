#ifndef ASYNCENGINE_RUBY_H
#define ASYNCENGINE_RUBY_H


#include <ruby.h>
#include "libuv/include/uv.h"


// Uncomment these lines for debugging and developming.
//#define AE_DO_TRACE
//#define AE_DO_DEBUG
#define AE_DO_WARN
#define AE_DO_ASSERT

// TODO: Pongo los fprintf() a escribir en stderr en vez de stderr, ya que si no peta
// un montón, no lo entiendo !!!

#ifdef AE_DO_TRACE
  #define AE_TRACE()  fprintf(stderr, "AE_TRACE: %s:%d:%s (GVL:%d)\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p())
#else
  #define AE_TRACE()
#endif

#ifdef AE_DO_DEBUG
  #define AE_DEBUG(desc)  fprintf(stderr, "AE_DEBUG: %s:%d:%s (GVL:%d)::  %s\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p(), desc)
#else
  #define AE_DEBUG(desc)
#endif

#ifdef AE_DO_WARN
#define AE_WARN(desc)  fprintf(stderr, "AE_WARN: %s:%d:%s (GVL:%d)::  %s\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p(), desc)
#else
  #define AE_WARN(desc)
#endif

#define AE_ABORT(desc)  fprintf(stderr, "AE_ABORT: %s:%d:%s (GVL:%d)::  %s\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p(), desc); exit(1);

#ifdef AE_DO_ASSERT
  #define AE_ASSERT(condition)  \
      do {  \
        if (!(condition)) {  \
          fprintf(stderr, "AE_ASSERT: %s:%d:%s (GVL:%d):: assertion `" #condition "' failed\n",  \
                  __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p());  \
          abort();  \
        }  \
      } while(0)
#else
  #define AE_ASSERT(condition)  condition
#endif


VALUE mAsyncEngine;
VALUE eAsyncEngineError;

VALUE AE_handles;
VALUE AE_blocks;
VALUE AE_UV_ERRNOS;


#endif  /* #ifndef ASYNCENGINE_RUBY_H */