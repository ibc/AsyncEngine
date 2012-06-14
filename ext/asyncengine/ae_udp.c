#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_ip_utils.h"
#include "ae_udp.h"


#define AE_UDP_DATAGRAM_MAX_SIZE 65536

#define GET_CDATA_FROM_SELF \
  struct_ae_udp_socket_cdata* cdata;  \
  Data_Get_Struct(self, struct_ae_udp_socket_cdata, cdata)

#define ENSURE_UV_HANDLE_EXISTS \
  if (! cdata->_uv_handle)  \
    return Qfalse;

#define GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN \
  GET_CDATA_FROM_SELF;  \
  ENSURE_UV_HANDLE_EXISTS


static VALUE cAsyncEngineUdpSocket;

static ID att_ip_type;

static ID method_on_datagram_received;


typedef struct {
  uv_udp_t *_uv_handle;
  uv_buf_t recv_buffer;
  enum_ip_type ip_type;
  int do_receive;
  int do_send;
  VALUE ae_handle;
  VALUE ae_handle_id;
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
  VALUE on_send_block_id;
} struct_udp_send_data;

struct udp_send_callback_data {
  VALUE on_send_block_id;
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

  // NOTE: No need to mark cdata->ae_handle since points to ourself, neither
  // cdata->ae_handle_id since it's a Fixnum.

  struct_ae_udp_socket_cdata* cdata = ALLOC(struct_ae_udp_socket_cdata);
  cdata->recv_buffer = uv_buf_init(ALLOC_N(char, AE_UDP_DATAGRAM_MAX_SIZE), AE_UDP_DATAGRAM_MAX_SIZE);
  cdata->_uv_handle = NULL;

  return Data_Wrap_Struct(klass, NULL, AsyncEngineUdpSocket_free, cdata);
}


void init_ae_udp()
{
  AE_TRACE();

  cAsyncEngineUdpSocket = rb_define_class_under(mAsyncEngine, "UDPSocket", cAsyncEngineHandle);

  rb_define_alloc_func(cAsyncEngineUdpSocket, AsyncEngineUdpSocket_alloc);
  rb_define_private_method(cAsyncEngineUdpSocket, "uv_handle_init", AsyncEngineUdpSocket_uv_handle_init, 2);
  rb_define_method(cAsyncEngineUdpSocket, "send_datagram", AsyncEngineUdpSocket_send_datagram, -1);
  rb_define_method(cAsyncEngineUdpSocket, "local_address", AsyncEngineUdpSocket_local_address, 0);
  rb_define_method(cAsyncEngineUdpSocket, "set_receiving", AsyncEngineUdpSocket_set_receiving, 1);
  rb_define_method(cAsyncEngineUdpSocket, "receiving?", AsyncEngineUdpSocket_is_receiving, 0);
  rb_define_method(cAsyncEngineUdpSocket, "set_sending", AsyncEngineUdpSocket_set_sending, 1);
  rb_define_method(cAsyncEngineUdpSocket, "sending?", AsyncEngineUdpSocket_is_sending, 0);
  rb_define_method(cAsyncEngineUdpSocket, "pause", AsyncEngineUdpSocket_pause, 0);
  rb_define_method(cAsyncEngineUdpSocket, "resume", AsyncEngineUdpSocket_resume, 0);
  rb_define_method(cAsyncEngineUdpSocket, "set_encoding_external", AsyncEngineUdpSocket_set_encoding_external, 0);
  rb_define_method(cAsyncEngineUdpSocket, "set_encoding_utf8", AsyncEngineUdpSocket_set_encoding_utf8, 0);
  rb_define_method(cAsyncEngineUdpSocket, "set_encoding_ascii", AsyncEngineUdpSocket_set_encoding_ascii, 0);
  rb_define_method(cAsyncEngineUdpSocket, "encoding", AsyncEngineUdpSocket_encoding, 0);
  //rb_define_method(cAsyncEngineUdpSocket, "set_broadcast", AsyncEngineUdpSocket_set_broadcast, 1);
  rb_define_method(cAsyncEngineUdpSocket, "alive?", AsyncEngineUdpSocket_is_alive, 0);
  rb_define_method(cAsyncEngineUdpSocket, "close", AsyncEngineUdpSocket_close, 0);
  rb_define_private_method(cAsyncEngineUdpSocket, "destroy", AsyncEngineUdpSocket_destroy, 0);

  att_ip_type = rb_intern("@_ip_type");

  method_on_datagram_received = rb_intern("on_datagram_received");
}


static
void destroy(struct_ae_udp_socket_cdata* cdata)
{
  AE_TRACE();

  uv_close((uv_handle_t *)cdata->_uv_handle, ae_uv_handle_close_callback);
  cdata->_uv_handle = NULL;
  // Let the GC work.
  ae_remove_handle(cdata->ae_handle_id);
}


static
uv_buf_t _uv_udp_recv_alloc_callback(uv_handle_t* handle, size_t suggested_size)
{
  AE_TRACE();

  return ((struct_ae_udp_socket_cdata*)handle->data)->recv_buffer;
}


static
VALUE ae_udp_recv_callback(void)
{
  AE_TRACE();

  struct_ae_udp_socket_cdata* cdata = (struct_ae_udp_socket_cdata*)last_udp_recv_callback_data.handle->data;
  VALUE _rb_datagram, _rb_array_ip_port, _rb_src_ip, _rb_src_port;

  // In utilities.h:  ae_rb_str_new(char* ptr, long len, enum_string_encoding enc, int tainted)
  _rb_datagram = ae_rb_str_new(last_udp_recv_callback_data.buf.base, last_udp_recv_callback_data.nread, cdata->encoding, 1);

  AE_ASSERT(! NIL_P(_rb_array_ip_port = ae_ip_utils_get_ip_port((struct sockaddr_storage *)last_udp_recv_callback_data.addr, cdata->ip_type)));

  return rb_funcall(cdata->ae_handle, method_on_datagram_received, 3, _rb_datagram,
                    rb_ary_entry(_rb_array_ip_port, 0), rb_ary_entry(_rb_array_ip_port, 1));
}


static
void _uv_udp_recv_callback(uv_udp_t* handle, ssize_t nread, uv_buf_t buf, struct sockaddr* addr, unsigned flags)
{
  AE_TRACE();

  struct_ae_udp_socket_cdata* cdata = handle->data;

  if (nread == 0) return;

  // uv.h: -1 if a transmission error was detected. So ignore it.
  if (nread == -1) {
    AE_WARN("nread == -1");
    return;
  }

  if (! cdata->do_receive) return;

  // Store UDP recv information in the last_udp_recv_callback_data struct.
  last_udp_recv_callback_data.handle = handle;
  last_udp_recv_callback_data.nread = nread;
  last_udp_recv_callback_data.buf = buf;
  last_udp_recv_callback_data.addr = addr;
  last_udp_recv_callback_data.flags = flags;

  ae_take_gvl_and_run_with_error_handler(ae_udp_recv_callback);
}


VALUE AsyncEngineUdpSocket_uv_handle_init(VALUE self, VALUE _rb_bind_ip, VALUE _rb_bind_port)
{
  AE_TRACE();

  char *bind_ip;
  int bind_port;
  enum_ip_type ip_type;

  GET_CDATA_FROM_SELF;

  if (! RB_TYPE_P(_rb_bind_ip, T_STRING)) {
    destroy(cdata);
    rb_raise(rb_eTypeError, "bind IP must be a String");
  }
  if (! FIXNUM_P(_rb_bind_port)) {
    destroy(cdata);
    rb_raise(rb_eTypeError, "bind port must be a Fixnum");
  }

  switch(ip_type = ae_ip_parser_execute(RSTRING_PTR(_rb_bind_ip), RSTRING_LEN(_rb_bind_ip))) {
    case ip_type_ipv4:
      rb_ivar_set(self, att_ip_type, symbol_ipv4);
      break;
    case ip_type_ipv6:
      rb_ivar_set(self, att_ip_type, symbol_ipv6);
      break;
    default:
      destroy(cdata);
      rb_raise(rb_eTypeError, "not valid IPv4 or IPv6");
  }

  bind_ip = StringValueCStr(_rb_bind_ip);
  bind_port = FIX2INT(_rb_bind_port);

  if (! ae_ip_utils_is_valid_port(bind_port)) {
    destroy(cdata);
    rb_raise(rb_eArgError, "invalid bind port value");
  }

  cdata->_uv_handle = ALLOC(uv_udp_t);
  cdata->ae_handle = self;
  cdata->ae_handle_id = ae_store_handle(self); // Avoid GC.
  cdata->ip_type = ip_type;
  cdata->do_receive = 0;  // It's set below.
  cdata->do_send = 1;
  cdata->encoding = string_encoding_ascii;

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

  // Start receiving.
  if (uv_udp_recv_start(cdata->_uv_handle, _uv_udp_recv_alloc_callback, _uv_udp_recv_callback)) {
    destroy(cdata);
    ae_raise_last_uv_error();
  }
  cdata->do_receive = 1;

  return self;
}


static
VALUE ae_udp_send_callback(void)
{
  AE_TRACE();

  VALUE _rb_on_send_block = ae_remove_block(last_udp_send_callback_data.on_send_block_id);

  if (! NIL_P(_rb_on_send_block)) {
    if (! last_udp_send_callback_data.status)
      return ae_block_call_1(_rb_on_send_block, Qnil);
    else
      return ae_block_call_1(_rb_on_send_block, ae_get_last_uv_error());
  }
  // This can occur if the UDP handle is closed or destroyed before the send callback.
  else {
    AE_WARN("ae_remove_block(last_udp_send_callback_data.on_send_block_id) returned Qnil");
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
  if (! NIL_P(send_data->on_send_block_id)) {
    last_udp_send_callback_data.on_send_block_id = send_data->on_send_block_id;
    last_udp_send_callback_data.status = status;
    do_on_send = 1;
  }

  xfree(req);
  xfree(send_data->datagram);
  xfree(send_data);

  if (do_on_send)
    ae_take_gvl_and_run_with_error_handler(ae_udp_send_callback);
}


/*
 * Arguments:
 * 1) datagram
 * 2) ip
 * 3) port
 * 4) proc (optional)
 */
VALUE AsyncEngineUdpSocket_send_datagram(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();

  char *ip;
  int port;
  uv_buf_t buffer;
  char *datagram;
  int datagram_len;
  uv_udp_send_t* _uv_udp_send_req;
  struct_udp_send_data* send_data;
  VALUE _rb_datagram, _rb_ip, _rb_port, _rb_block;

  AE_RB_CHECK_NUM_ARGS(3,4);
  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;
  AE_RB_GET_BLOCK_OR_PROC(4, _rb_block);

  if (! cdata->do_send)
    return Qfalse;

  _rb_datagram = argv[0];
  _rb_ip = argv[1];
  _rb_port = argv[2];

  Check_Type(_rb_datagram, T_STRING);
  Check_Type(_rb_ip, T_STRING);

  if (cdata->ip_type != ae_ip_parser_execute(RSTRING_PTR(_rb_ip), RSTRING_LEN(_rb_ip)))
    rb_raise(rb_eTypeError, "invalid destination IP family");

  ip = StringValueCStr(_rb_ip);
  port = FIX2INT(_rb_port);

  if (! ae_ip_utils_is_valid_port(port))
    rb_raise(rb_eArgError, "invalid destination port value");

  datagram_len = RSTRING_LEN(_rb_datagram);
  datagram = ALLOC_N(char, datagram_len);
  memcpy(datagram, RSTRING_PTR(_rb_datagram), datagram_len);

  send_data = ALLOC(struct_udp_send_data);
  send_data->datagram = datagram;

  if (! NIL_P(_rb_block))
    send_data->on_send_block_id = ae_store_block(_rb_block);
  else
    send_data->on_send_block_id = Qnil;

  _uv_udp_send_req = ALLOC(uv_udp_send_t);
  _uv_udp_send_req->data = send_data;

  buffer = uv_buf_init(datagram, datagram_len);

  switch(cdata->ip_type) {
    case ip_type_ipv4:
      AE_ASSERT(! uv_udp_send(_uv_udp_send_req, cdata->_uv_handle, &buffer, 1, uv_ip4_addr(ip, port), _uv_udp_send_callback));
      break;
    case ip_type_ipv6:
      AE_ASSERT(! uv_udp_send6(_uv_udp_send_req, cdata->_uv_handle, &buffer, 1, uv_ip6_addr(ip, port), _uv_udp_send_callback));
      break;
  }

  return Qtrue;
}


VALUE AsyncEngineUdpSocket_local_address(VALUE self)
{
  AE_TRACE();

  struct sockaddr_storage local_addr;
  int len = sizeof(local_addr);
  VALUE _rb_array_ip_port;

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  if (uv_udp_getsockname(cdata->_uv_handle, (struct sockaddr*)&local_addr, &len))
    ae_raise_last_uv_error();

  if (! NIL_P(_rb_array_ip_port = ae_ip_utils_get_ip_port(&local_addr, cdata->ip_type)))
    return _rb_array_ip_port;
  else
    rb_raise(eAsyncEngineError, "error getting local address");
}


VALUE AsyncEngineUdpSocket_set_receiving(VALUE self, VALUE allow)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  if (TYPE(allow) == T_TRUE)
    cdata->do_receive = 1;
  else
    cdata->do_receive = 0;

  return Qtrue;
}


VALUE AsyncEngineUdpSocket_is_receiving(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  if (cdata->do_receive)
    return Qtrue;
  else
    return Qfalse;
}


VALUE AsyncEngineUdpSocket_set_sending(VALUE self, VALUE allow)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  if (TYPE(allow) == T_TRUE)
    cdata->do_send = 1;
  else
    cdata->do_send = 0;

  return Qtrue;
}


VALUE AsyncEngineUdpSocket_is_sending(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  if (cdata->do_send)
    return Qtrue;
  else
    return Qfalse;
}


VALUE AsyncEngineUdpSocket_pause(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  cdata->do_receive = 0;
  cdata->do_send = 0;

  return Qtrue;
}


VALUE AsyncEngineUdpSocket_resume(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  cdata->do_receive = 1;
  cdata->do_send = 1;

  return Qtrue;
}


static
VALUE ae_set_external_encoding(VALUE self, enum_string_encoding encoding)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

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


VALUE AsyncEngineUdpSocket_encoding(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  return ae_get_rb_encoding(cdata->encoding);
}


// TODO: No idea how this stuf works.
// VALUE AsyncEngineUdpSocket_set_broadcast(VALUE self, VALUE on)
// {
//   AE_TRACE();
// 
//   GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;
// 
//   if (TYPE(on) == T_TRUE) {
//     if (uv_udp_set_broadcast(cdata->_uv_handle, 1))
//       return Qtrue;
//     else
//       ae_raise_last_uv_error();
//   }
//   else {
//     if (uv_udp_set_broadcast(cdata->_uv_handle, 0))
//       return Qtrue;
//     else
//       ae_raise_last_uv_error();
//   }
// }


VALUE AsyncEngineUdpSocket_is_alive(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  return Qtrue;
}


VALUE AsyncEngineUdpSocket_close(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  destroy(cdata);
  return Qtrue;
}


VALUE AsyncEngineUdpSocket_destroy(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  destroy(cdata);
  return Qtrue;
}
