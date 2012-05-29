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
static ID method_on_disconnected;


typedef struct {
  uv_tcp_t *_uv_handle;
  //uv_buf_t recv_buffer;  // TODO: si?
  enum_ip_type ip_type;
  int connected;
  int do_receive;
  int do_send;
  VALUE ae_handle;
  VALUE ae_handle_id;
  enum_string_encoding encoding;
} struct_ae_tcp_socket_cdata;

struct tcp_connect_callback_data {
  uv_tcp_t* handle;
  int status;
};

struct tcp_read_callback_data {
  uv_stream_t* stream;
  ssize_t nread;
  uv_buf_t buf;
};

typedef struct {
  char *data;
  VALUE on_write_block_id;
} struct_tcp_write_data;

struct tcp_write_callback_data {
  VALUE on_write_block_id;
  int status;
};


// Used for storing information about the last TCP connect callback.
static struct tcp_connect_callback_data last_tcp_connect_callback_data;

// Used for storing information about the last TCP read callback.
static struct tcp_read_callback_data last_tcp_read_callback_data;

// Used for storing information about the last TCP write callback.
static struct tcp_write_callback_data last_tcp_write_callback_data;


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
  rb_define_private_method(cAsyncEngineTcpSocket, "uv_handle_init", AsyncEngineTcpSocket_uv_handle_init, 4);
  rb_define_method(cAsyncEngineTcpSocket, "send_data", AsyncEngineTcpSocket_send_data, -1);
  rb_define_method(cAsyncEngineTcpSocket, "local_address", AsyncEngineTcpSocket_local_address, 0);
  rb_define_method(cAsyncEngineTcpSocket, "peer_address", AsyncEngineTcpSocket_peer_address, 0);
  rb_define_method(cAsyncEngineTcpSocket, "alive?", AsyncEngineTcpSocket_is_alive, 0);
  rb_define_method(cAsyncEngineTcpSocket, "connected?", AsyncEngineTcpSocket_is_connected, 0);
  rb_define_method(cAsyncEngineTcpSocket, "close", AsyncEngineTcpSocket_close, 0);
  rb_define_private_method(cAsyncEngineTcpSocket, "destroy", AsyncEngineTcpSocket_destroy, 0);

  att_ip_type = rb_intern("@_ip_type");

  method_on_connected = rb_intern("on_connected");
  method_on_connection_error = rb_intern("on_connection_error");
  method_on_data_received = rb_intern("on_data_received");
  method_on_disconnected = rb_intern("on_disconnected");
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

  cdata->connected = 1;

  if (! last_tcp_connect_callback_data.status)
    return rb_funcall2(cdata->ae_handle, method_on_connected, 0, NULL);
  else {
    error = ae_get_last_uv_error();
    rb_funcall2(cdata->ae_handle, method_on_connection_error, 1, &error);
    destroy(cdata);
    return Qnil;
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


static
uv_buf_t _uv_tcp_read_alloc_callback(uv_handle_t* handle, size_t suggested_size)
{
  AE_TRACE();

  return uv_buf_init(ALLOC_N(char, suggested_size), suggested_size);
}


static
VALUE ae_tcp_read_callback(VALUE ignore)
{
  AE_TRACE();

  struct_ae_tcp_socket_cdata* cdata = (struct_ae_tcp_socket_cdata*)last_tcp_read_callback_data.stream->data;
  VALUE _rb_data;

  if (last_tcp_read_callback_data.nread > 0) {
    if (cdata->do_receive && cdata->_uv_handle)
      _rb_data = ae_rb_str_new(last_tcp_read_callback_data.buf.base, last_tcp_read_callback_data.nread, cdata->encoding, 1);
    xfree(last_tcp_read_callback_data.buf.base);
    if (cdata->do_receive && cdata->_uv_handle)
      rb_funcall2(cdata->ae_handle, method_on_data_received, 1, &_rb_data);
  }
  // Otherwise it's -1 (EOF, disconnection).
  else {
    destroy(cdata);
    // TODO: Habrá que pasar algún parámetro para decir que es desconexión remota.
    rb_funcall2(cdata->ae_handle, method_on_disconnected, 0, NULL);
  }

  return Qnil;
}


static
void _uv_tcp_read_callback(uv_stream_t* stream, ssize_t nread, uv_buf_t buf)
{
  AE_TRACE();

  // Store TCP read information in the last_tcp_read_callback_data struct.
  last_tcp_read_callback_data.stream = stream;
  last_tcp_read_callback_data.nread = nread;
  last_tcp_read_callback_data.buf = buf;

  switch(nread) {
    // Disconnection.
    case -1:
      printf("_uv_tcp_read_callback(): nread = -1 => disconnection\n");
      // TODO: AE.on_disconnected (with GVL !!!) and destroy.
      // O sea, llamar a ae_tcp_read_callback() y él mirará que nread == -1.
      ae_execute_in_ruby_land(ae_tcp_read_callback);
      break;

    // libuv requested a buffer through the alloc callback but then decided
    // that it didn't need that buffer.
    case 0:
      printf("_uv_tcp_read_callback(): nread = 0 => free buffer and ignore\n");
      // TODO: is ok?
      xfree(buf.base);
      break;

    default:
      ae_execute_in_ruby_land(ae_tcp_read_callback);
  }
}


VALUE AsyncEngineTcpSocket_uv_handle_init(VALUE self, VALUE _rb_bind_ip, VALUE _rb_bind_port, VALUE _rb_dest_ip, VALUE _rb_dest_port)
{
  AE_TRACE();

  char *bind_ip;
  int bind_port;
  int do_bind;
  char *dest_ip;
  int dest_port;
  enum_ip_type ip_type, bind_ip_type;
  uv_connect_t *_uv_tcp_connect_req;

  GET_CDATA_FROM_SELF;

  if (TYPE(_rb_dest_ip) != T_STRING) {
    destroy(cdata);
    rb_raise(rb_eTypeError, "destination IP must be a String");
  }
  if (TYPE(_rb_dest_port) != T_FIXNUM) {
    destroy(cdata);
    rb_raise(rb_eTypeError, "destination port must be a Fixnum");
  }

  switch(ip_type = ae_ip_parser_execute(RSTRING_PTR(_rb_dest_ip), RSTRING_LEN(_rb_dest_ip))) {
    case ip_type_ipv4:
      rb_ivar_set(self, att_ip_type, symbol_ipv4);
      break;
    case ip_type_ipv6:
      rb_ivar_set(self, att_ip_type, symbol_ipv6);
      break;
    default:
      destroy(cdata);
      rb_raise(rb_eTypeError, "destination IP is not valid IPv4 or IPv6");
  }

  dest_ip = StringValueCStr(_rb_dest_ip);
  dest_port = FIX2INT(_rb_dest_port);

  if (! ae_ip_utils_is_valid_port(dest_port)) {
    destroy(cdata);
    rb_raise(rb_eArgError, "invalid destination port value");
  }

  if (! NIL_P(_rb_bind_ip)) {
    if (TYPE(_rb_bind_ip) != T_STRING) {
      destroy(cdata);
      rb_raise(rb_eTypeError, "bind IP must be a String");
    }
    if (TYPE(_rb_bind_port) != T_FIXNUM) {
      destroy(cdata);
      rb_raise(rb_eTypeError, "port must be a Fixnum");
    }

    bind_ip = StringValueCStr(_rb_bind_ip);
    bind_port = FIX2INT(_rb_bind_port);

    bind_ip_type = ae_ip_parser_execute(RSTRING_PTR(_rb_bind_ip), RSTRING_LEN(_rb_bind_ip));
    if (bind_ip_type == ip_type_error) {
      destroy(cdata);
      rb_raise(rb_eTypeError, "bind IP is not valid IPv4 or IPv6");
    }
    else if (bind_ip_type != ip_type) {
      destroy(cdata);
      rb_raise(rb_eTypeError, "bind IP and destination IP don't belong to same IP family");
    }

    if (! ae_ip_utils_is_valid_port(bind_port)) {
      destroy(cdata);
      rb_raise(rb_eArgError, "invalid bind port value");
    }

    do_bind = 1;
  }
  else
    do_bind = 0;

  cdata->_uv_handle = ALLOC(uv_tcp_t);
  cdata->connected = 0;
  cdata->ae_handle = self;
  cdata->ae_handle_id = ae_store_handle(self); // Avoid GC.
  cdata->ip_type = ip_type;
  cdata->do_receive = 0;  // It's set below.
  cdata->do_send = 1;
  cdata->encoding = string_encoding_ascii;

  AE_ASSERT(! uv_tcp_init(AE_uv_loop, cdata->_uv_handle));

  cdata->_uv_handle->data = cdata;

  if (do_bind) {
    switch(ip_type) {
      case ip_type_ipv4:
        if (uv_tcp_bind(cdata->_uv_handle, uv_ip4_addr(bind_ip, bind_port))) {
          destroy(cdata);
          ae_raise_last_uv_error();
        }
        break;
      case ip_type_ipv6:
        if (uv_tcp_bind6(cdata->_uv_handle, uv_ip6_addr(bind_ip, bind_port))) {
          destroy(cdata);
          ae_raise_last_uv_error();
        }
        break;
    }
  }

  // Connect.
  _uv_tcp_connect_req = ALLOC(uv_connect_t);

  // TODO: Si falla uv_tcp_connect(), ¿debo free el req? Lo mismo para TCP/UDP al hacer el bind.
  // Pues parece que no, ya que si lo pongo se hace doble free() y peta!!!
  switch(ip_type) {
    case ip_type_ipv4:
      if (uv_tcp_connect(_uv_tcp_connect_req, cdata->_uv_handle, uv_ip4_addr(dest_ip, dest_port), _uv_tcp_connect_callback)) {
        destroy(cdata);
        //xfree(_uv_tcp_connect_req);
        ae_raise_last_uv_error();
      }
      break;
    case ip_type_ipv6:
      if (uv_tcp_connect6(_uv_tcp_connect_req, cdata->_uv_handle, uv_ip6_addr(dest_ip, dest_port), _uv_tcp_connect_callback)) {
        destroy(cdata);
        //xfree(_uv_tcp_connect_req);
        ae_raise_last_uv_error();
      }
      break;
  }

  // Start receiving.
  if (uv_read_start((uv_stream_t*)cdata->_uv_handle, _uv_tcp_read_alloc_callback, _uv_tcp_read_callback)) {
    destroy(cdata);
    //xfree(_uv_tcp_connect_req);
    ae_raise_last_uv_error();
  }
  cdata->do_receive = 1;

  return self;
}


static
VALUE ae_tcp_write_callback(VALUE ignore)
{
  AE_TRACE();

  VALUE rb_on_write_block = ae_remove_block(last_tcp_write_callback_data.on_write_block_id);

  if (! NIL_P(rb_on_write_block)) {
    if (! last_tcp_write_callback_data.status)
      return ae_block_call_1(rb_on_write_block, Qnil);
    else
      return ae_block_call_1(rb_on_write_block, ae_get_last_uv_error());
  }
  // This can occur if the TCP handle is closed or destroyed before the write callback.
  else {
    AE_WARN("ae_remove_block(last_tcp_write_callback_data.on_write_block_id) returned Qnil");
    return Qnil;
  }
}


static
void _uv_write_callback(uv_write_t* req, int status)
{
  AE_TRACE();

  printf("DBG: _uv_write_callback(): status=%d\n", status);

  struct_tcp_write_data* write_data = req->data;
  int do_on_write = 0;

  // Block was provided so must call it with success or error param.
  if (! NIL_P(write_data->on_write_block_id)) {
    last_tcp_write_callback_data.on_write_block_id = write_data->on_write_block_id;
    last_tcp_write_callback_data.status = status;
    do_on_write = 1;
  }

  xfree(req);
  xfree(write_data->data);
  xfree(write_data);

  if (do_on_write)
    ae_execute_in_ruby_land(ae_tcp_write_callback);
}


VALUE AsyncEngineTcpSocket_send_data(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();

  uv_buf_t buffer;
  char *data;
  int data_len;
  uv_write_t* _uv_write_req;
  struct_tcp_write_data* write_data;
  VALUE _rb_data, _rb_block;

  AE_RB_CHECK_NUM_ARGS(1,2);
  AE_RB_GET_BLOCK_OR_PROC(2, _rb_block);

  GET_CDATA_FROM_SELF;
  if (! cdata->_uv_handle) {
    if (! NIL_P(_rb_block))
      ae_block_call_1(_rb_block, ae_get_uv_error(AE_UV_ERRNO_SOCKET_NOT_CONNECTED));
    return Qfalse;
  }

  if (! cdata->do_send)
    return Qfalse;

  _rb_data = argv[0];

  Check_Type(_rb_data, T_STRING);

  data_len = RSTRING_LEN(_rb_data);
  data = ALLOC_N(char, data_len);
  memcpy(data, RSTRING_PTR(_rb_data), data_len);

  write_data = ALLOC(struct_tcp_write_data);
  write_data->data = data;

  if (! NIL_P(_rb_block))
    write_data->on_write_block_id = ae_store_block(_rb_block);
  else
    write_data->on_write_block_id = Qnil;

  _uv_write_req = ALLOC(uv_write_t);
  _uv_write_req->data = write_data;

  buffer = uv_buf_init(data, data_len);

  // TODO: Can it return error? If so I should free the buffer, req, and so...
  AE_ASSERT(! uv_write(_uv_write_req, (uv_stream_t *)cdata->_uv_handle, &buffer, 1, _uv_write_callback));

  return Qtrue;
}


VALUE AsyncEngineTcpSocket_local_address(VALUE self)
{
  AE_TRACE();

  struct sockaddr_storage local_addr;
  int len = sizeof(local_addr);
  VALUE _rb_array_ip_port;

  GET_CDATA_FROM_SELF_AND_ENSURE_UV_HANDLE_EXISTS;

  if (uv_tcp_getsockname(cdata->_uv_handle, (struct sockaddr*)&local_addr, &len))
    ae_raise_last_uv_error();

  if (! NIL_P(_rb_array_ip_port = ae_ip_utils_get_ip_port(&local_addr, cdata->ip_type)))
    return _rb_array_ip_port;
  else
    rb_raise(eAsyncEngineError, "error getting local address");
}


VALUE AsyncEngineTcpSocket_peer_address(VALUE self)
{
  AE_TRACE();

  struct sockaddr_storage local_addr;
  int len = sizeof(local_addr);
  VALUE _rb_array_ip_port;

  GET_CDATA_FROM_SELF_AND_ENSURE_UV_HANDLE_EXISTS;

  // Return false if the connection is not yet established.
  if (! cdata->connected)
    return Qfalse;

  if (uv_tcp_getpeername(cdata->_uv_handle, (struct sockaddr*)&local_addr, &len))
    ae_raise_last_uv_error();

  if (! NIL_P(_rb_array_ip_port = ae_ip_utils_get_ip_port(&local_addr, cdata->ip_type)))
    return _rb_array_ip_port;
  else
    rb_raise(eAsyncEngineError, "error getting peer address");
}


VALUE AsyncEngineTcpSocket_is_connected(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_ENSURE_UV_HANDLE_EXISTS;

  if (cdata->connected)
    return Qtrue;
  else
    return Qfalse;
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