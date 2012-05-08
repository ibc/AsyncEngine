#ifndef AE_HANDLE_COMMON_H
#define AE_HANDLE_COMMON_H


#include "libuv/include/uv.h"  // NOTE: Not needed if dir_config() line is enabled in extconf.rb.
//#include <uv.h>              // so this line becomes enough.
#include <assert.h>


#define AE_FIXNUM_UV_LAST_ERROR()  INT2FIX(uv_last_error(uv_default_loop()).code)


VALUE cAsyncEngineCData;

ID att_cdata;
ID att_handle_terminated;
ID id_method_call;

void init_ae_handle_common();

VALUE ae_store_handle(VALUE);
VALUE ae_get_handle(VALUE);
VALUE ae_remove_handle(VALUE);

VALUE ae_store_block(VALUE);
VALUE ae_get_block(VALUE);
VALUE ae_remove_block(VALUE);

void ae_handle_exception(int);
void ae_uv_handle_close_callback_0(uv_handle_t*);
int ae_protect_block_call_0(VALUE);


#endif  /* AE_HANDLE_COMMON_H */