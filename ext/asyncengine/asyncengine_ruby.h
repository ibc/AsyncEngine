#ifndef ASYNCENGINE_RUBY_H
#define ASYNCENGINE_RUBY_H


#include <ruby.h>
#include <libuv/include/uv.h>  // NOTE: Not needed if dir_config() line is enabled in extconf.rb.
//#include <uv.h>              // so this line becomes enough.


// Global variables (used in other files with "extern").
VALUE mAsyncEngine;
VALUE cAsyncEngineCData;
ID id_method_call;

// Public methods.
VALUE AsyncEngine_store_handle(VALUE);
VALUE AsyncEngine_get_handle(VALUE);
void AsyncEngine_remove_handle(VALUE);


#endif  /* #ifndef ASYNCENGINE_RUBY_H */