#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_udp.h"


static VALUE mAsyncEngineUdpSocket;


typedef struct {
  uv_udp_t *_uv_handle;
  VALUE rb_instance;  // Points to the owner AE::Timer (if there is).
} struct_ae_udp_cdata;


void init_ae_udp()
{
  AE_TRACE();

  mAsyncEngineUdpSocket = rb_define_module_under(mAsyncEngine, "UDPSocket");

  rb_define_private_method(mAsyncEngineUdpSocket, "_c_init_udp_socket", AsyncEngineUdpSocket_c_init_udp_socket, 3);
}


static
void deallocate_udp_handle(struct_ae_udp_cdata* cdata)
{
  AE_TRACE();
  printf("--- deallocate_udp_handle()\n");

  // Let the GC work.
  //ae_remove_block(cdata->rb_block_id);
  // Close the timer so it's unreferenced by uv.
  uv_close((uv_handle_t *)cdata->_uv_handle, ae_uv_handle_close_callback_0);
  // Free memory.
  xfree(cdata);
}


VALUE AsyncEngineUdpSocket_c_init_udp_socket(VALUE self, VALUE rb_ip_type, VALUE rb_bind_ip, VALUE rb_bind_port)
{
  AE_TRACE();

  char *bind_ip = StringValueCStr(rb_bind_ip);
  int bind_port = FIX2INT(rb_bind_port);
  struct sockaddr_in bind_addr;
  struct_ae_udp_cdata* cdata = ALLOC(struct_ae_udp_cdata);

  cdata->_uv_handle = ALLOC(uv_udp_t);
  cdata->rb_instance = self;

  Data_Wrap_Struct(rb_obj_class(self), NULL, NULL, cdata);

  bind_addr = uv_ip4_addr(bind_ip, bind_port);
  assert(! uv_udp_init(uv_default_loop(), cdata->_uv_handle));
  if (uv_udp_bind(cdata->_uv_handle, bind_addr, 0)) {
    deallocate_udp_handle(cdata);
    return AE_FIXNUM_UV_LAST_ERROR();
  }

  return Qtrue;
}
