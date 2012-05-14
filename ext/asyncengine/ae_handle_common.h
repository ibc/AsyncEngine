#ifndef AE_HANDLE_COMMON_H
#define AE_HANDLE_COMMON_H


#include "libuv/include/uv.h"  // NOTE: Not needed if dir_config() line is enabled in extconf.rb.
//#include <uv.h>              // so this line becomes enough.
#include <assert.h>


#define AE_FIXNUM_LAST_UV_ERRNO()  INT2FIX(uv_last_error(uv_default_loop()).code)
#define RB_STR_UTF8_NEW(s, len) (rb_enc_str_new(s, len, rb_utf8_encoding()))
#define RB_STR_TAINTED_UTF8_NEW(s, len) (rb_external_str_new_with_enc(s, len, rb_utf8_encoding()))


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

VALUE ae_get_uv_error(int uv_errno);
VALUE ae_get_last_uv_error(void);
void ae_raise_last_uv_error(void);

VALUE ae_block_call_0(VALUE rb_block);
VALUE ae_block_call_1(VALUE rb_block, VALUE param);

typedef VALUE (*function_with_gvl_and_protect)(VALUE rb_param);
VALUE ae_execute_function_with_gvl_and_protect(function_with_gvl_and_protect function, VALUE rb_param);


#endif  /* AE_HANDLE_COMMON_H */