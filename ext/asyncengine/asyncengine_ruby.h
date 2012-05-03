#ifndef ASYNCENGINE_RUBY_H
#define ASYNCENGINE_RUBY_H


#include <ruby.h>
#include <libuv/include/uv.h>  // NOTE: Not needed if dir_config() line is enabled in extconf.rb.
//#include <uv.h>              // so this line becomes enough.


// Uncomment this line for enabling TRACE() function.
//#define AE_DEBUG
#ifdef AE_DEBUG
#define AE_TRACE()  fprintf(stderr, "AE_TRACE: %s:%d:%s\n", __FILE__, __LINE__, __FUNCTION__)
#else
#define AE_TRACE()
#endif


VALUE mAsyncEngine;

// A permanent uv idle for next_tick.
uv_idle_t* ae_next_tick_uv_idle;


#endif  /* #ifndef ASYNCENGINE_RUBY_H */