#ifndef AE_HANDLE_COMMON_H
#define AE_HANDLE_COMMON_H


#define AE_CLOSE_UV_HANDLE(handle)  \
  uv_close((uv_handle_t *)handle, ae_uv_handle_close_callback);

#define GET_CDATA_FROM_SELF  \
  struct_cdata* cdata;  \
  Data_Get_Struct(self, struct_cdata, cdata)

#define CHECK_UV_HANDLE_IS_OPEN  \
  if (! (cdata->_uv_handle))  \
    return Qfalse;

#define GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN  \
  GET_CDATA_FROM_SELF;  \
  CHECK_UV_HANDLE_IS_OPEN


void init_ae_handle_common(void);

VALUE ae_store_handle(VALUE);
VALUE ae_get_handle(VALUE);
VALUE ae_remove_handle(VALUE);

VALUE ae_store_proc(VALUE);
VALUE ae_get_proc(VALUE);
VALUE ae_remove_proc(VALUE);

void ae_uv_handle_close_callback(uv_handle_t*);
int ae_get_last_uv_error_int(void);
VALUE ae_get_uv_error(int uv_errno);
VALUE ae_get_last_uv_error(void);
void ae_raise_uv_error(int uv_errno);
void ae_raise_last_uv_error(void);

VALUE ae_proc_call_0(VALUE proc);
VALUE ae_proc_call_1(VALUE proc, VALUE param);

VALUE ae_run_with_error_handler(void* function, VALUE param);
VALUE ae_take_gvl_and_run_with_error_handler(void* function);


#endif  /* AE_HANDLE_COMMON_H */