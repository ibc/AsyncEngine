#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_udp.h"
#include "ae_ip_utils.h"


#define AE_UDP_DATAGRAM_MAX_SIZE 65536


static VALUE cAsyncEngineUdpSocket;

static ID att_bind_ip;
static ID att_bind_port;
static ID att_ip_type;

static ID method_on_received_datagram;


typedef struct {
  uv_udp_t *_uv_handle;
  uv_buf_t recv_buffer;
  enum_ip_type ip_type;
  VALUE rb_ae_udp_socket;
  VALUE rb_ae_udp_socket_id;
  enum_string_encoding encoding;
} struct_ae_udp_socket_cdata;

struct udp_recv_callback_data {
  uv_udp_t* handle;
  ssize_t nread;
  uv_buf_t buf;
  struct sockaddr* addr;
  unsigned flags;
};

typedef struct {
  char *datagram;
  VALUE rb_on_send_block_id;
} struct_udp_send_data;

struct udp_send_callback_data {
  VALUE rb_on_send_block_id;
  int status;
};


// Used for storing information about the last UDP recv callback.
static struct udp_recv_callback_data last_udp_recv_callback_data;

// Used for storing information about the last UDP send callback.
static struct udp_send_callback_data last_udp_send_callback_data;


static void AsyncEngineUdpSocket_free(void *cdata)
{
  AE_TRACE();

  xfree(((struct_ae_udp_socket_cdata*)cdata)->recv_buffer.base);
  xfree(cdata);
}


VALUE AsyncEngineUdpSocket_alloc(VALUE klass)
{
  AE_TRACE();

  // NOTE: No need to mark cdata->rb_ae_udp_socket since points to ourself, neither
  // cdata->rb_ae_udp_socket_id since it's a Fixnum.

  struct_ae_udp_socket_cdata* cdata = ALLOC(struct_ae_udp_socket_cdata);
  cdata->recv_buffer = uv_buf_init(ALLOC_N(char, AE_UDP_DATAGRAM_MAX_SIZE), AE_UDP_DATAGRAM_MAX_SIZE);

  return Data_Wrap_Struct(klass, NULL, AsyncEngineUdpSocket_free, cdata);
}


void init_ae_udp()
{
  AE_TRACE();

  cAsyncEngineUdpSocket = rb_define_class_under(mAsyncEngine, "UDPSocket", rb_cObject);

  rb_define_alloc_func(cAsyncEngineUdpSocket, AsyncEngineUdpSocket_alloc);
  rb_define_private_method(cAsyncEngineUdpSocket, "uv_handle_init", AsyncEngineUdpSocket_uv_handle_init, 2);
  rb_define_method(cAsyncEngineUdpSocket, "send_datagram", AsyncEngineUdpSocket_send_datagram, 3);
  rb_define_method(cAsyncEngineUdpSocket, "close", AsyncEngineUdpSocket_close, 0);
  rb_define_method(cAsyncEngineUdpSocket, "alive?", AsyncEngineUdpSocket_is_alive, 0);
  rb_define_method(cAsyncEngineUdpSocket, "set_encoding_external", AsyncEngineUdpSocket_set_encoding_external, 0);
  rb_define_method(cAsyncEngineUdpSocket, "set_encoding_utf8", AsyncEngineUdpSocket_set_encoding_utf8, 0);
  rb_define_method(cAsyncEngineUdpSocket, "set_encoding_ascii", AsyncEngineUdpSocket_set_encoding_ascii, 0);
  rb_define_private_method(cAsyncEngineUdpSocket, "destroy", AsyncEngineUdpSocket_destroy, 0);

  att_bind_ip = rb_intern("@_bind_ip");
  att_bind_port = rb_intern("@_bind_port");
  att_ip_type = rb_intern("@_ip_type");

  method_on_received_datagram = rb_intern("on_received_datagram");
}


static
void destroy(struct_ae_udp_socket_cdata* cdata)
{
  AE_TRACE();

  uv_close((uv_handle_t *)cdata->_uv_handle, ae_uv_handle_close_callback);
  // Set the handle field to NULL.
  cdata->_uv_handle = NULL;
  // Let the GC work.
  ae_remove_handle(cdata->rb_ae_udp_socket_id);
}


static
uv_buf_t _uv_udp_recv_alloc_callback(uv_handle_t* handle, size_t suggested_size)
{
  AE_TRACE();

  return ((struct_ae_udp_socket_cdata*)handle->data)->recv_buffer;
}


static
VALUE ae_udp_socket_recv_callback(VALUE ignore)
{
  AE_TRACE();

  struct_ae_udp_socket_cdata* cdata = (struct_ae_udp_socket_cdata*)last_udp_recv_callback_data.handle->data;

  // In utilities.h:  ae_rb_str_new(char* ptr, long len, enum_string_encoding enc, int tainted)
  VALUE rb_datagram = ae_rb_str_new(last_udp_recv_callback_data.buf.base, last_udp_recv_callback_data.nread, cdata->encoding, 1);

  return rb_funcall2(cdata->rb_ae_udp_socket, method_on_received_datagram, 1, &rb_datagram);
}


static
void _uv_udp_recv_callback(uv_udp_t* handle, ssize_t nread, uv_buf_t buf, struct sockaddr* addr, unsigned flags)
{
  AE_TRACE();

  if (nread == 0) return;

  if (nread == -1) {
    // TODO: Que hacemos?
    printf("ERROR: udp_recv_callback: nread = %d\n", (int)nread);
    return;
  }

  //printf("DBG: udp_recv_allback: nread = %d\n", (int)nread);

  // Store UDP recv information in the last_udp_recv_callback_data struct.
  last_udp_recv_callback_data.handle = handle;
  last_udp_recv_callback_data.nread = nread;
  last_udp_recv_callback_data.buf = buf;
  last_udp_recv_callback_data.addr = addr;
  last_udp_recv_callback_data.flags = flags;

  ae_execute_in_ruby_land(ae_udp_socket_recv_callback);
}


VALUE AsyncEngineUdpSocket_uv_handle_init(VALUE self, VALUE rb_bind_ip, VALUE rb_bind_port)
{
  AE_TRACE();

  char *bind_ip = StringValueCStr(rb_bind_ip);
  int bind_port = FIX2INT(rb_bind_port);
  enum_ip_type ip_type;
  struct_ae_udp_socket_cdata* cdata;

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

  if (! ae_ip_utils_is_valid_port(bind_port))
    rb_raise(rb_eArgError, "invalid port value");

  rb_ivar_set(self, att_bind_ip, rb_bind_ip);
  rb_ivar_set(self, att_bind_port, rb_bind_port);

  Data_Get_Struct(self, struct_ae_udp_socket_cdata, cdata);

  cdata->_uv_handle = ALLOC(uv_udp_t);
  cdata->rb_ae_udp_socket = self;
  cdata->rb_ae_udp_socket_id = ae_store_handle(self); // Avoid GC.
  cdata->ip_type = ip_type;
  cdata->encoding = string_encoding_external;

  AE_ASSERT(! uv_udp_init(AE_uv_loop, cdata->_uv_handle));

  // NOTE: If we set the data field *before* uv_udp_init() then such a function set data to NULL !!!
  cdata->_uv_handle->data = cdata;

  switch(ip_type) {
    case ip_type_ipv4:
      if (uv_udp_bind(cdata->_uv_handle, uv_ip4_addr(bind_ip, bind_port), 0)) {
        destroy(cdata);
        ae_raise_last_uv_error();
      }
      break;
    case ip_type_ipv6:
      // TODO: UDP flags en IPv6 puede ser que si. Solo vale 0 o UV_UDP_IPV6ONLY.
      if (uv_udp_bind6(cdata->_uv_handle, uv_ip6_addr(bind_ip, bind_port), UV_UDP_IPV6ONLY)) {
        destroy(cdata);
        ae_raise_last_uv_error();
      }
      break;
  }

  AE_ASSERT(! uv_udp_recv_start(cdata->_uv_handle, _uv_udp_recv_alloc_callback, _uv_udp_recv_callback));
  return self;
}


static
VALUE ae_udp_send_callback(VALUE ignore)
{
  AE_TRACE();

  VALUE rb_on_send_block = ae_remove_block(last_udp_send_callback_data.rb_on_send_block_id);

  if (! NIL_P(rb_on_send_block)) {
    if (! last_udp_send_callback_data.status)
      return ae_block_call_1(rb_on_send_block, Qnil);
    else
      return ae_block_call_1(rb_on_send_block, ae_get_last_uv_error());
  }
  else {
    AE_WARN("ae_remove_block(last_udp_send_callback_data.rb_on_send_block_id) returned Qnil");
    return Qnil;
  }
}


static
void _uv_udp_send_callback(uv_udp_send_t* req, int status)
{
  AE_TRACE();

  struct_udp_send_data* send_data = req->data;
  int do_on_send = 0;

  // Block was provided so must call it with success or error param.
  if (! NIL_P(send_data->rb_on_send_block_id)) {
    last_udp_send_callback_data.rb_on_send_block_id = send_data->rb_on_send_block_id;
    last_udp_send_callback_data.status = status;
    do_on_send = 1;
  }

  xfree(req);
  xfree(send_data->datagram);
  xfree(send_data);

  if (do_on_send)
    ae_execute_in_ruby_land(ae_udp_send_callback);
}


VALUE AsyncEngineUdpSocket_send_datagram(VALUE self, VALUE rb_data, VALUE rb_ip, VALUE rb_port)
{
  AE_TRACE();

  char *ip;
  int port;
  uv_buf_t buffer;
  char *datagram;
  int datagram_len;
  uv_udp_send_t* _uv_req;
  struct_udp_send_data* send_data;
  struct_ae_udp_socket_cdata* cdata;

  Data_Get_Struct(self, struct_ae_udp_socket_cdata, cdata);

  if (! cdata->_uv_handle) {
    if (rb_block_given_p())
      ae_block_call_1(rb_block_proc(), ae_get_uv_error(31));
    return Qfalse;
  }

  ip = StringValueCStr(rb_ip);
  port = FIX2INT(rb_port);

  if (cdata->ip_type != ae_ip_parser_execute(RSTRING_PTR(rb_ip), RSTRING_LEN(rb_ip)))
    rb_raise(rb_eTypeError, "invalid destination IP family");

  if (! ae_ip_utils_is_valid_port(port))
    rb_raise(rb_eArgError, "invalid port value");

  Check_Type(rb_data, T_STRING);

  datagram_len = RSTRING_LEN(rb_data);
  datagram = ALLOC_N(char, datagram_len);
  memcpy(datagram, RSTRING_PTR(rb_data), datagram_len);

  send_data = ALLOC(struct_udp_send_data);
  send_data->datagram = datagram;

  if (rb_block_given_p())
    send_data->rb_on_send_block_id = ae_store_block(rb_block_proc());
  else
    send_data->rb_on_send_block_id = Qnil;

  _uv_req = ALLOC(uv_udp_send_t);
  _uv_req->data = send_data;

  buffer = uv_buf_init(datagram, datagram_len);

  // TODO uv_udp_send() parece que devuelve siempre 0 aunque le pases un puerto destino 0 (que
  // luego se traduce en error en el callback).
  // Segun los src solo devuelve error si el handle no esta bindeado (el nuestro ya lo está) o si no hay
  // más memoria para un alloc que hace, bufff. Un AE_ASSERT y va que arde.
  switch(cdata->ip_type) {
    case ip_type_ipv4:
      AE_ASSERT(! uv_udp_send(_uv_req, cdata->_uv_handle, &buffer, 1, uv_ip4_addr(ip, port), _uv_udp_send_callback));
      break;
    case ip_type_ipv6:
      AE_ASSERT(! uv_udp_send6(_uv_req, cdata->_uv_handle, &buffer, 1, uv_ip6_addr(ip, port), _uv_udp_send_callback));
      break;
  }

  return Qtrue;
}


VALUE AsyncEngineUdpSocket_close(VALUE self)
{
  AE_TRACE();

  struct_ae_udp_socket_cdata* cdata;

  Data_Get_Struct(self, struct_ae_udp_socket_cdata, cdata);
  if (! cdata->_uv_handle)
    return Qfalse;

  destroy(cdata);
  return Qtrue;
}


VALUE AsyncEngineUdpSocket_is_alive(VALUE self)
{
  AE_TRACE();

  struct_ae_udp_socket_cdata* cdata;

  Data_Get_Struct(self, struct_ae_udp_socket_cdata, cdata);
  if (! cdata->_uv_handle)
    return Qfalse;

  return Qtrue;
}


static
VALUE ae_set_external_encoding(VALUE self, enum_string_encoding encoding)
{
  AE_TRACE();

  struct_ae_udp_socket_cdata* cdata;

  Data_Get_Struct(self, struct_ae_udp_socket_cdata, cdata);
  if (! cdata->_uv_handle)
    return Qfalse;

  cdata->encoding = encoding;
  return Qtrue;
}


VALUE AsyncEngineUdpSocket_set_encoding_external(VALUE self)
{
  AE_TRACE();

  return ae_set_external_encoding(self, string_encoding_external);
}


VALUE AsyncEngineUdpSocket_set_encoding_utf8(VALUE self)
{
  AE_TRACE();

  return ae_set_external_encoding(self, string_encoding_utf8);
}


VALUE AsyncEngineUdpSocket_set_encoding_ascii(VALUE self)
{
  AE_TRACE();

  return ae_set_external_encoding(self, string_encoding_ascii);
}


VALUE AsyncEngineUdpSocket_destroy(VALUE self)
{
  AE_TRACE();

  struct_ae_udp_socket_cdata* cdata;

  Data_Get_Struct(self, struct_ae_udp_socket_cdata, cdata);
  if (! cdata->_uv_handle)
    return Qfalse;

  destroy(cdata);
  return Qtrue;
}
