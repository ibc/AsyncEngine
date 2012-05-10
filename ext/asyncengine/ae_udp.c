#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_udp.h"
#include "ae_ip_utils.h"


static VALUE cAsyncEngineUdpSocket;

static ID att_bind_ip;
static ID att_bind_port;
static ID att_ip_type;


typedef struct {
  enum_ip_type ip_type;
  uv_udp_t *_uv_handle;
  VALUE rb_ae_udp_socket_id;
} struct_ae_udp_cdata;


typedef struct {
  char *datagram;
  int datagram_len;
} struct_ae_udp_send_data;


static void AsyncEngineUdpSocket_free(void *cdata)
{
  AE_TRACE();

  printf("--- AsyncEngineUdpSocket_free()\n");

  // NOTE: The cdata struct is freed via Ruby GC.
  if(cdata)
    xfree(cdata);
}


VALUE AsyncEngineUdpSocket_alloc(VALUE klass)
{
  AE_TRACE();

  printf("--- AsyncEngineUdpSocket_alloc()\n");

  struct_ae_udp_cdata* cdata = ALLOC(struct_ae_udp_cdata);

  return Data_Wrap_Struct(klass, NULL, AsyncEngineUdpSocket_free, cdata);
}


void init_ae_udp()
{
  AE_TRACE();

  cAsyncEngineUdpSocket = rb_define_class_under(mAsyncEngine, "UDPSocket", rb_cObject);

  rb_define_alloc_func(cAsyncEngineUdpSocket, AsyncEngineUdpSocket_alloc);
  rb_define_private_method(cAsyncEngineUdpSocket, "_c_init_udp_socket", AsyncEngineUdpSocket_c_init_udp_socket, 2);
  rb_define_method(cAsyncEngineUdpSocket, "send_datagram", AsyncEngineUdpSocket_send_datagram, 3);

  att_bind_ip = rb_intern("@_bind_ip");
  att_bind_port = rb_intern("@_bind_port");
  att_ip_type = rb_intern("@_ip_type");
}


static
void deallocate_udp_handle(struct_ae_udp_cdata* cdata)
{
  AE_TRACE();
  printf("--- deallocate_udp_handle()\n");

  // Let the GC work. TODO: Debe quitarlo del hash de udp handles de AE (salvo que haya fallado al crearse !!!).
  ae_remove_handle(cdata->rb_ae_udp_socket_id);
  // Close the UDP handle so it's unreferenced by uv.
  uv_close((uv_handle_t *)cdata->_uv_handle, ae_uv_handle_close_callback_0);
}


VALUE AsyncEngineUdpSocket_c_init_udp_socket(VALUE self, VALUE rb_bind_ip, VALUE rb_bind_port)
{
  AE_TRACE();

  char *bind_ip = StringValueCStr(rb_bind_ip);
  int bind_port = FIX2INT(rb_bind_port);
  enum_ip_type ip_type;
  struct_ae_udp_cdata* cdata;

  switch(ip_type = ae_ip_parser_execute(RSTRING_PTR(rb_bind_ip), RSTRING_LEN(rb_bind_ip))) {
    case ip_type_ipv4:
      rb_ivar_set(self, att_ip_type, symbol_ipv4);
      break;
    case ip_type_ipv6:
      rb_ivar_set(self, att_ip_type, symbol_ipv6);
      break;
    default:
      rb_raise(rb_eTypeError, "not valid IPv4 or IPv6");
  }

  rb_ivar_set(self, att_bind_ip, rb_bind_ip);
  rb_ivar_set(self, att_bind_port, rb_bind_port);

  Data_Get_Struct(self, struct_ae_udp_cdata, cdata);

  cdata->_uv_handle = ALLOC(uv_udp_t);
  cdata->rb_ae_udp_socket_id = ae_store_handle(self);
  cdata->ip_type = ip_type;

  assert(! uv_udp_init(uv_default_loop(), cdata->_uv_handle));

  switch(ip_type) {
    case ip_type_ipv4:
      if (uv_udp_bind(cdata->_uv_handle, uv_ip4_addr(bind_ip, bind_port), 0)) {
        deallocate_udp_handle(cdata);
        return AE_FIXNUM_UV_LAST_ERROR();
      }
      break;
    case ip_type_ipv6:
      // TODO: UDP flags en IPv6 puede ser que si. Solo vale 0 o UV_UDP_IPV6ONLY.
      if (uv_udp_bind6(cdata->_uv_handle, uv_ip6_addr(bind_ip, bind_port), UV_UDP_IPV6ONLY)) {
        deallocate_udp_handle(cdata);
        return AE_FIXNUM_UV_LAST_ERROR();
      }
      break;
  }

  return Qtrue;
}


static
void udp_send_callback(uv_udp_send_t* req, int status)
{
  AE_TRACE();

  if (status)
    printf("ERROR: udp_send_callback() => error = %d\n", uv_last_error(uv_default_loop()).code);

  struct_ae_udp_send_data* send_data = req->data;
  xfree(send_data->datagram);
  xfree(send_data);
  xfree(req);
}


VALUE AsyncEngineUdpSocket_send_datagram(VALUE self, VALUE rb_ip, VALUE rb_port, VALUE rb_data)
{
  AE_TRACE();

  char *ip = StringValueCStr(rb_ip);
  int port = FIX2INT(rb_port);
  uv_buf_t buffer;
  char *datagram;
  int datagram_len;
  uv_udp_send_t* _uv_req;
  struct_ae_udp_send_data* send_data;
  struct_ae_udp_cdata* cdata;
  enum_ip_type ip_type;

  Data_Get_Struct(self, struct_ae_udp_cdata, cdata);

  ip_type = ae_ip_parser_execute(RSTRING_PTR(rb_ip), RSTRING_LEN(rb_ip));
  if (ip_type != cdata->ip_type)
    rb_raise(rb_eTypeError, "invalid destination IP family");

  Check_Type(rb_data, T_STRING);

  datagram_len = RSTRING_LEN(rb_data);
  datagram = ALLOC_N(char, datagram_len);
  memcpy(datagram, RSTRING_PTR(rb_data), datagram_len);

  buffer = uv_buf_init(datagram, datagram_len);

  _uv_req = ALLOC(uv_udp_send_t);
  send_data = ALLOC(struct_ae_udp_send_data);

  send_data->datagram = datagram;
  send_data->datagram_len = datagram_len;

  _uv_req->data = send_data;

  // TODO uv_udp_send() parece que devuelve siempre 0 aunque le pases un puerto destino 0 (que
  // luego se traduce en error en el callback).
  // Segun los src solo devuelve error si el handle no esta bindeado (el nuestro ya lo está) o si no hay
  // más memoria para un alloc que hace, bufff. Un assert y va que arde.
  switch(ip_type) {
    case ip_type_ipv4:
      assert(! uv_udp_send(_uv_req, cdata->_uv_handle, &buffer, 1, uv_ip4_addr(ip, port), udp_send_callback));
      break;
    case ip_type_ipv6:
      assert(! uv_udp_send6(_uv_req, cdata->_uv_handle, &buffer, 1, uv_ip6_addr(ip, port), udp_send_callback));
      break;
  }

  // TODO: Si está chapao el socket habrá que devolver false no?
  return Qtrue;
}