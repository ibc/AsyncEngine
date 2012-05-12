#ifndef AE_HANDLE_COMMON_H
#define AE_HANDLE_COMMON_H


#include "libuv/include/uv.h"  // NOTE: Not needed if dir_config() line is enabled in extconf.rb.
//#include <uv.h>              // so this line becomes enough.
#include <assert.h>


#define AE_FIXNUM_LAST_UV_ERRNO()  INT2FIX(uv_last_error(uv_default_loop()).code)


// Ruby class for saving C data inside.
VALUE cAsyncEngineCData;

ID att_cdata;
ID att_handle_terminated; // TODO: Fuera, hacer como en UDP y meterlo en el cdata struct.

void init_ae_handle_common(void);

VALUE ae_store_handle(VALUE);
VALUE ae_get_handle(VALUE);
VALUE ae_remove_handle(VALUE);

VALUE ae_store_block(VALUE);
VALUE ae_get_block(VALUE);
VALUE ae_remove_block(VALUE);

void ae_uv_handle_close_callback(uv_handle_t*);

void ae_raise_last_uv_errno(void);

VALUE ae_block_call_0(VALUE rb_block);
VALUE ae_block_call_1(VALUE rb_block, VALUE param);

typedef VALUE (*function_with_gvl_and_protect)(VALUE rb_param);
VALUE ae_execute_function_with_gvl_and_protect(function_with_gvl_and_protect function, VALUE rb_param);


#endif  /* AE_HANDLE_COMMON_H */