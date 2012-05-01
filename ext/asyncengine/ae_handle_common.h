#ifndef AE_HANDLE_COMMON_H
#define AE_HANDLE_COMMON_H


VALUE cAsyncEngineCData;
ID att_c_data;
ID att_handle_terminated;
ID id_method_call;

void init_ae_handle_common();

VALUE ae_store_callback(VALUE);
VALUE ae_get_callback(VALUE);
VALUE ae_remove_callback(VALUE);

void ae_manage_exception();

void ae_handle_close_callback_0(uv_handle_t*);
int ae_protect_block_call_0(VALUE);


#endif  /* AE_HANDLE_COMMON_H */