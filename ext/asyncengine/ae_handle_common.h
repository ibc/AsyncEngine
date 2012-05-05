#ifndef AE_HANDLE_COMMON_H
#define AE_HANDLE_COMMON_H


VALUE cAsyncEngineCData;
ID att_cdata;
ID att_handle_terminated;
ID id_method_call;

void init_ae_handle_common();

VALUE ae_store_block(VALUE);
VALUE ae_get_block(VALUE);
VALUE ae_remove_block(VALUE);
void ae_manage_exception(int);
void ae_uv_handle_close_callback_0(uv_handle_t*);
int ae_protect_block_call_0(VALUE);


#endif  /* AE_HANDLE_COMMON_H */