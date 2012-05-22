#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_async.h"


typedef struct {
  uv_async_t* handle;
} struct_async_callback_data;


// Used for storing information about the last async callback.
static struct_async_callback_data last_async_callback_data;


void init_ae_async()
{
  AE_TRACE();

  rb_define_module_function(mAsyncEngine, "_c_call_from_other_thread", AsyncEngine_c_call_from_other_thread, 1);
}


static
VALUE ae_async_callback(VALUE ignore)
{
  AE_TRACE();

  VALUE block = ae_remove_block((VALUE)last_async_callback_data.handle->data);

  uv_close((uv_handle_t *)last_async_callback_data.handle, ae_uv_handle_close_callback);

  return ae_block_call_0(block);
}


static
void _uv_async_callback(uv_async_t* handle, int status)
{
  AE_TRACE();

  // TODO: testing
  AE_ASSERT(! status);

  last_async_callback_data.handle = handle;

  ae_execute_function_with_gvl_and_protect(ae_async_callback, Qnil);
}


VALUE AsyncEngine_c_call_from_other_thread(VALUE self, VALUE block)
{
  AE_TRACE();

  uv_async_t* _uv_async = ALLOC(uv_async_t);

  AE_ASSERT(! uv_async_init(uv_default_loop(), _uv_async, _uv_async_callback));

  // Store just the block id in the handle data field.
  _uv_async->data = (void *)ae_store_block(block);

  // TODO: ¿puede devolver error esto? Parece que no:
  //   https://github.com/joyent/libuv/issues/419
  AE_ASSERT(! uv_async_send(_uv_async));

  return Qtrue;
}


