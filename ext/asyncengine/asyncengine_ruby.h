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


// Public methods.
VALUE AsyncEngine_store_handle(VALUE);
VALUE AsyncEngine_get_handle(VALUE);
void AsyncEngine_remove_handle(VALUE);


#endif  /* #ifndef ASYNCENGINE_RUBY_H */