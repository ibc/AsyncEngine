#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_ip_utils.h"
#include "ae_tcp_connection.h"


#define GET_CDATA_FROM_SELF \
  struct_ae_tcp_socket_cdata* cdata;  \
  Data_Get_Struct(self, struct_ae_tcp_socket_cdata, cdata)

#define ENSURE_UV_HANDLE_EXISTS \
  if (! cdata->_uv_handle)  \
    return Qfalse;

#define GET_CDATA_FROM_SELF_AND_ENSURE_UV_HANDLE_EXISTS \
  GET_CDATA_FROM_SELF;  \
  ENSURE_UV_HANDLE_EXISTS


static VALUE cAsyncEngineTcpSocket;

static ID att_ip_type;

static ID method_on_connected;
static ID method_on_connection_error;
static ID method_on_data_received;


typedef struct {
  uv_tcp_t *_uv_handle;
  //uv_buf_t recv_buffer;  // TODO: si?
  enum_ip_type ip_type;
  int connected;
  //int do_receive;
  //int do_send;
  VALUE ae_handle;
  VALUE ae_handle_id;
  enum_string_encoding encoding;
} struct_ae_tcp_socket_cdata;

struct tcp_connect_callback_data {
  uv_tcp_t* handle;
  int status;
};


// Used for storing information about the last TCP connect callback.
static struct tcp_connect_callback_data last_tcp_connect_callback_data;


static void AsyncEngineTcpSocket_free(void *cdata)
{
  AE_TRACE();

  //xfree(((struct_ae_tcp_socket_cdata*)cdata)->recv_buffer.base);
  xfree(cdata);
}


VALUE AsyncEngineTcpSocket_alloc(VALUE klass)
{
  AE_TRACE();

  struct_ae_tcp_socket_cdata* cdata = ALLOC(struct_ae_tcp_socket_cdata);
  //cdata->recv_buffer = uv_buf_init(ALLOC_N(char, AE_UDP_DATAGRAM_MAX_SIZE), AE_UDP_DATAGRAM_MAX_SIZE);
  cdata->_uv_handle = NULL;

  return Data_Wrap_Struct(klass, NULL, AsyncEngineTcpSocket_free, cdata);
}


void init_ae_tcp_connection()
{
  AE_TRACE();

  cAsyncEngineTcpSocket = rb_define_class_under(mAsyncEngine, "TCPSocket", cAsyncEngineHandle);

  rb_define_alloc_func(cAsyncEngineTcpSocket, AsyncEngineTcpSocket_alloc);
  rb_define_private_method(cAsyncEngineTcpSocket, "uv_handle_init", AsyncEngineTcpSocket_uv_handle_init, 2);
  rb_define_method(cAsyncEngineTcpSocket, "alive?", AsyncEngineTcpSocket_is_alive, 0);
  rb_define_method(cAsyncEngineTcpSocket, "close", AsyncEngineTcpSocket_close, 0);
  rb_define_private_method(cAsyncEngineTcpSocket, "destroy", AsyncEngineTcpSocket_destroy, 0);

  att_ip_type = rb_intern("@_ip_type");

  method_on_connected = rb_intern("on_connected");
  method_on_connection_error = rb_intern("on_connection_error");
  method_on_data_received = rb_intern("on_data_received");
}


static
void destroy(struct_ae_tcp_socket_cdata* cdata)
{
  AE_TRACE();

  uv_close((uv_handle_t *)cdata->_uv_handle, ae_uv_handle_close_callback);
  cdata->_uv_handle = NULL;
  // Let the GC work.
  ae_remove_handle(cdata->ae_handle_id);
}


static
VALUE ae_tcp_connect_callback(VALUE ignore)
{
  AE_TRACE();

  struct_ae_tcp_socket_cdata* cdata = (struct_ae_tcp_socket_cdata*)last_tcp_connect_callback_data.handle->data;
  VALUE error;

  // Don't run the Ruby callback if the TCP socket was closed/destroyed before
  // the connection or connection error happened.
  ENSURE_UV_HANDLE_EXISTS;

  //if (! cdata->do_receive)
  //  return Qnil;

  if (! last_tcp_connect_callback_data.status)
    return rb_funcall2(cdata->ae_handle, method_on_connected, 0, NULL);
  else {
    error = ae_get_last_uv_error();
    return rb_funcall2(cdata->ae_handle, method_on_connection_error, 1, &error);
    // TODO: Qutiar de handles entonces! o destroy!
  }
}


static
void _uv_tcp_connect_callback(uv_connect_t* req, int status)
{
  AE_TRACE();

  printf("DBG: _uv_tcp_connect_callback(): status=%d\n", status);

  last_tcp_connect_callback_data.handle = (uv_tcp_t*)req->handle;
  last_tcp_connect_callback_data.status = status;

  xfree(req);

  ae_execute_in_ruby_land(ae_tcp_connect_callback);
}


VALUE AsyncEngineTcpSocket_uv_handle_init(VALUE self, VALUE _rb_dest_ip, VALUE _rb_dest_port)
{
  AE_TRACE();

  char *dest_ip = StringValueCStr(_rb_dest_ip);
  int dest_port = FIX2INT(_rb_dest_port);
  enum_ip_type ip_type;
  uv_connect_t *_uv_tcp_connect_req;

  GET_CDATA_FROM_SELF;

  switch(ip_type = ae_ip_parser_execute(RSTRING_PTR(_rb_dest_ip), RSTRING_LEN(_rb_dest_ip))) {
    case ip_type_ipv4:
      rb_ivar_set(self, att_ip_type, symbol_ipv4);
      break;
    case ip_type_ipv6:
      rb_ivar_set(self, att_ip_type, symbol_ipv6);
      break;
    default:
      rb_raise(rb_eTypeError, "not valid IPv4 or IPv6");
  }

  if (! ae_ip_utils_is_valid_port(dest_port))
    rb_raise(rb_eArgError, "invalid port value");

  cdata->_uv_handle = ALLOC(uv_tcp_t);
  cdata->connected = 0;
  cdata->ae_handle = self;
  cdata->ae_handle_id = ae_store_handle(self); // Avoid GC.
  cdata->ip_type = ip_type;
  //cdata->do_receive = 0;  // It's set below.
  //cdata->do_send = 1;
  //cdata->encoding = string_encoding_ascii;

  AE_ASSERT(! uv_tcp_init(AE_uv_loop, cdata->_uv_handle));

  cdata->_uv_handle->data = cdata;

  // TODO: uv_read_start() here.

   _uv_tcp_connect_req = ALLOC(uv_connect_t);

  switch(ip_type) {
    case ip_type_ipv4:
      if (uv_tcp_connect(_uv_tcp_connect_req, cdata->_uv_handle, uv_ip4_addr(dest_ip, dest_port), _uv_tcp_connect_callback)) {
        destroy(cdata);
        ae_raise_last_uv_error();
      }
      break;
    case ip_type_ipv6:
      if (uv_tcp_connect6(_uv_tcp_connect_req, cdata->_uv_handle, uv_ip6_addr(dest_ip, dest_port), _uv_tcp_connect_callback)) {
        destroy(cdata);
        ae_raise_last_uv_error();
      }
      break;
  }

  return self;
}


VALUE AsyncEngineTcpSocket_is_alive(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_ENSURE_UV_HANDLE_EXISTS;

  return Qtrue;
}


VALUE AsyncEngineTcpSocket_close(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_ENSURE_UV_HANDLE_EXISTS;

  destroy(cdata);
  return Qtrue;
}


VALUE AsyncEngineTcpSocket_destroy(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_ENSURE_UV_HANDLE_EXISTS;

  destroy(cdata);
  return Qtrue;
}