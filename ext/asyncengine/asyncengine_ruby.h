#ifndef ASYNCENGINE_RUBY_H
#define ASYNCENGINE_RUBY_H


#include <ruby.h>
#include <libuv/include/uv.h>  // No hace falta por el dir_config() en extconf.rb.
//#include <uv.h>


VALUE AsyncEngine_c_get_mAsyncEngine();
VALUE AsyncEngine_c_get_cAsyncEngineCPointer();
VALUE AsyncEngine_store_handle(VALUE);
VALUE AsyncEngine_get_handle(VALUE);
void AsyncEngine_remove_handle(VALUE);


#endif  /* #ifndef ASYNCENGINE_RUBY_H */