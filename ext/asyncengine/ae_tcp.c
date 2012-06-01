#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_ip_utils.h"
#include "ae_tcp.h"


#define GET_CDATA_FROM_SELF  \
  struct_ae_tcp_socket_cdata* cdata;  \
  Data_Get_Struct(self, struct_ae_tcp_socket_cdata, cdata)

#define CHECK_UV_HANDLE_IS_OPEN  \
  if (! (cdata->_uv_handle))  \
    return Qfalse;

#define GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN  \
  GET_CDATA_FROM_SELF;  \
  CHECK_UV_HANDLE_IS_OPEN


static VALUE cAsyncEngineTcpSocket;

static ID method_on_connected;
static ID method_on_connection_error;
static ID method_on_data_received;
static ID method_on_disconnected;

typedef struct {
  int is_usable;  // 1 means that the user can call the Ruby methods of the object, 0 means that it cannot.
  uv_tcp_t *_uv_handle;
  int connected;
  enum_ip_type ip_type;
  int do_receive;
  VALUE ae_handle;
  VALUE ae_handle_id;
  enum_string_encoding encoding;
} struct_ae_tcp_socket_cdata;

struct _uv_tcp_connect_callback_params {
  uv_connect_t* req;
  int status;
};

struct _uv_tcp_read_callback_params {
  uv_stream_t* stream;
  ssize_t nread;
  uv_buf_t buf;
};

// Used for storing information about the last TCP connect callback.
static struct _uv_tcp_connect_callback_params last_uv_tcp_connect_callback_params;

// Used for storing information about the last TCP read callback.
static struct _uv_tcp_read_callback_params last_uv_tcp_read_callback_params;


/** Predeclaration of static functions. */

static void AsyncEngineTcpSocket_free(void* cdata);
static VALUE AsyncEngineTcpSocket_alloc(VALUE klass);
static int init_instance(VALUE self, enum_ip_type ip_type, char* dest_ip, int dest_port, char* bind_ip, int bind_port);
static void close_uv_handle(uv_tcp_t* handle);
static void _uv_tcp_connect_callback(uv_connect_t* req, int status);
static uv_buf_t _uv_tcp_read_alloc_callback(uv_handle_t* handle, size_t suggested_size);
static void _uv_tcp_read_callback(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
static VALUE _ae_tcp_connect_callback(void);
static void _uv_tcp_connect_callback(uv_connect_t* req, int status);
static void _uv_tcp_read_callback(uv_stream_t* stream, ssize_t nread, uv_buf_t buf);
static VALUE _ae_tcp_read_callback(void);


void init_ae_tcp()
{
  AE_TRACE();

  cAsyncEngineTcpSocket = rb_define_class_under(mAsyncEngine, "TCPSocket", cAsyncEngineHandle);

  rb_define_alloc_func(cAsyncEngineTcpSocket, AsyncEngineTcpSocket_alloc);
  rb_define_singleton_method(cAsyncEngineTcpSocket, "new", AsyncEngineTcpSocket_new, -1);

//   rb_define_method(cAsyncEngineTcpSocket, "send_data", AsyncEngineTcpSocket_send_data, -1);
//   rb_define_method(cAsyncEngineTcpSocket, "connected?", AsyncEngineTcpSocket_is_connected, 0);
  rb_define_method(cAsyncEngineTcpSocket, "local_address", AsyncEngineTcpSocket_local_address, 0);
//   rb_define_method(cAsyncEngineTcpSocket, "peer_address", AsyncEngineTcpSocket_peer_address, 0);
//   rb_define_method(cAsyncEngineTcpSocket, "alive?", AsyncEngineTcpSocket_is_alive, 0);
  rb_define_method(cAsyncEngineTcpSocket, "close", AsyncEngineTcpSocket_close, 0);
  rb_define_private_method(cAsyncEngineTcpSocket, "destroy", AsyncEngineTcpSocket_destroy, 0);
 
  method_on_connected = rb_intern("on_connected");
  method_on_connection_error = rb_intern("on_connection_error");
  method_on_data_received = rb_intern("on_data_received");
  method_on_disconnected = rb_intern("on_disconnected");
}


/** Class alloc and free functions. */

static
VALUE AsyncEngineTcpSocket_alloc(VALUE klass)
{
  AE_TRACE2();

  struct_ae_tcp_socket_cdata* cdata = ALLOC(struct_ae_tcp_socket_cdata);

  return Data_Wrap_Struct(klass, NULL, AsyncEngineTcpSocket_free, cdata);
}


static void AsyncEngineTcpSocket_free(void *cdata)
{
  AE_TRACE2();

  xfree(cdata);
}


/** Class new() method. */

VALUE AsyncEngineTcpSocket_new(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE2();

  char *dest_ip, *bind_ip;
  int dest_ip_len, bind_ip_len;
  int dest_port, bind_port;
  enum_ip_type ip_type, bind_ip_type;
  int error;
  VALUE instance;

  ae_ensure_ready_for_handles();

  AE_RB_CHECK_NUM_ARGS(2,4);

  // Parameter 1: destination IP.
  if (TYPE(argv[0]) != T_STRING)
    rb_raise(rb_eTypeError, "destination IP must be a String");
  dest_ip = StringValueCStr(argv[0]);
  dest_ip_len = RSTRING_LEN(argv[0]);

  // Parameter 2: destination port.
  if (! FIXNUM_P(argv[1]))
    rb_raise(rb_eTypeError, "destination port must be a Fixnum");
  dest_port = FIX2INT(argv[1]);
  if (! ae_ip_utils_is_valid_port(dest_port))
    rb_raise(rb_eArgError, "invalid destination port value");

  // Parameter 3: bind IP.
  if (argc >= 3) {
    if (TYPE(argv[2]) != T_STRING)
      rb_raise(rb_eTypeError, "bind IP must be a String");
    bind_ip = StringValueCStr(argv[2]);
    bind_ip_len = RSTRING_LEN(argv[2]);

    // Parameter 4: bind port.
    if (argc == 4) {
      if (! FIXNUM_P(argv[3]))
        rb_raise(rb_eTypeError, "bind port must be a Fixnum");
      bind_port = FIX2INT(argv[3]);
      if (! ae_ip_utils_is_valid_port(bind_port))
        rb_raise(rb_eArgError, "invalid bind port value");
    }
    else
      bind_port = 0;
  }
  else {
    bind_ip = NULL;
    bind_port = 0;
  }

  // TODO: Allow domains y blablablablablablabla puto sag.
  // Parse destination IP.
  if ((ip_type = ae_ip_parser_execute(dest_ip, dest_ip_len)) == ip_type_no_ip)
    rb_raise(rb_eTypeError, "destination IP is not valid IPv4 or IPv6");

  // Parse bind IP.
  if (bind_ip) {
    if ((bind_ip_type = ae_ip_parser_execute(bind_ip, bind_ip_len)) == ip_type_no_ip)
      rb_raise(rb_eTypeError, "bind IP is not valid IPv4 or IPv6");
    else if (bind_ip_type != ip_type)
      rb_raise(rb_eTypeError, "bind IP does not belong to the same IP family of the destination IP");
  }

  // Allocate the Ruby instance.
  instance = rb_obj_alloc(cAsyncEngineTcpSocket);

  // Init the UV stuff within the instance.
  if (error = init_instance(instance, ip_type, dest_ip, dest_port, bind_ip, bind_port))
    ae_raise_uv_error(error);

  return instance;
}


static
int init_instance(VALUE self, enum_ip_type ip_type, char *dest_ip, int dest_port, char *bind_ip, int bind_port)
{
  AE_TRACE2();

  uv_tcp_t *_uv_handle;
  uv_connect_t *_uv_tcp_connect_req;
  int ret;

  // Create and init the UV handle.
  _uv_handle = ALLOC(uv_tcp_t);
  AE_ASSERT(! uv_tcp_init(AE_uv_loop, _uv_handle));

  // Bind to the given IP (and optional port) if given.
  if (bind_ip) {
    switch(ip_type) {
      case ip_type_ipv4:
        ret = uv_tcp_bind(_uv_handle, uv_ip4_addr(bind_ip, bind_port));
        break;
      case ip_type_ipv6:
        ret = uv_tcp_bind6(_uv_handle, uv_ip6_addr(bind_ip, bind_port));
        break;
    }
    if (ret) {
      close_uv_handle(_uv_handle);  // TODO
      return ae_get_last_uv_error_int();
    }
  }

  // Connect.
  _uv_tcp_connect_req = ALLOC(uv_connect_t);

  switch(ip_type) {
    case ip_type_ipv4:
      ret = uv_tcp_connect(_uv_tcp_connect_req, _uv_handle, uv_ip4_addr(dest_ip, dest_port), _uv_tcp_connect_callback);
      break;
    case ip_type_ipv6:
      ret = uv_tcp_connect6(_uv_tcp_connect_req, _uv_handle, uv_ip6_addr(dest_ip, dest_port), _uv_tcp_connect_callback);
      break;
  }
  if (ret) {
    close_uv_handle(_uv_handle);  // TODO
    xfree(_uv_tcp_connect_req);
    return ae_get_last_uv_error_int();
  }

  // Start receiving.
  AE_ASSERT(! uv_read_start((uv_stream_t*)_uv_handle, _uv_tcp_read_alloc_callback, _uv_tcp_read_callback));

  GET_CDATA_FROM_SELF;

  // Fill cdata struct.
  cdata->_uv_handle = _uv_handle;
  cdata->connected = 0;
  cdata->is_usable = 1;
  cdata->ip_type = ip_type;
  cdata->ae_handle = self;
  cdata->ae_handle_id = ae_store_handle(self); // Avoid GC.  // TODO
  cdata->do_receive = 1;
  cdata->encoding = string_encoding_ascii;

  // Fill data field of the UV handle.
  cdata->_uv_handle->data = (void *)cdata;

  return 0;
}


static
void close_uv_handle(uv_tcp_t* handle)
{
  AE_TRACE2();

  uv_close((uv_handle_t *)handle, ae_uv_handle_close_callback);
}


/** TCP connect callback. */

static
void _uv_tcp_connect_callback(uv_connect_t* req, int status)
{
  AE_TRACE2();

  uv_tcp_t* _uv_handle = (uv_tcp_t*)req->handle;
  struct_ae_tcp_socket_cdata* cdata = (struct_ae_tcp_socket_cdata*)_uv_handle->data;

  last_uv_tcp_connect_callback_params.req = req;
  last_uv_tcp_connect_callback_params.status = status;

  if (! status)
    cdata->connected = 1;
  // If it's a connection error and uv_is_closing() == 1 it means that the AE handle
  // was closed by the application, so don't close the UV handle again. Otherwise
  // close the UV handle.
  else if (! uv_is_closing((const uv_handle_t*)_uv_handle)) {
    close_uv_handle(cdata->_uv_handle);
    cdata->_uv_handle = NULL;
  }

  ae_execute_in_ruby_land(_ae_tcp_connect_callback);
}


static
VALUE _ae_tcp_connect_callback(void)
{
  AE_TRACE2();

  uv_tcp_t* _uv_handle = (uv_tcp_t*)last_uv_tcp_connect_callback_params.req->handle;
  struct_ae_tcp_socket_cdata* cdata = (struct_ae_tcp_socket_cdata*)_uv_handle->data;
  VALUE error;

  xfree(last_uv_tcp_connect_callback_params.req);

  // Connection succedded, so call on_connected() callback in the instance.
  if (! last_uv_tcp_connect_callback_params.status) {
    rb_funcall2(cdata->ae_handle, method_on_connected, 0, NULL);
  }
  // Connection failed.
  else {
    error = ae_get_last_uv_error();  // TODO: Custom error?
    ae_remove_handle(cdata->ae_handle_id);
    rb_funcall2(cdata->ae_handle, method_on_connection_error, 1, &error);
  }

  return Qnil;
}


/** TCP read callback. */

static
uv_buf_t _uv_tcp_read_alloc_callback(uv_handle_t* handle, size_t suggested_size)
{
  AE_TRACE();

  return uv_buf_init(ALLOC_N(char, suggested_size), suggested_size);
}


static
void _uv_tcp_read_callback(uv_stream_t* stream, ssize_t nread, uv_buf_t buf)
{
  AE_TRACE();

  uv_tcp_t *_uv_handle = (uv_tcp_t *)stream;
  struct_ae_tcp_socket_cdata* cdata = (struct_ae_tcp_socket_cdata*)_uv_handle->data;

  last_uv_tcp_read_callback_params.stream = stream;
  last_uv_tcp_read_callback_params.nread = nread;
  last_uv_tcp_read_callback_params.buf = buf;

  /*
   * nread 0:  libuv requested a buffer through the alloc callback but then decided
   *           that it didn't need that buffer.
   * nread -1: disconnecton, so close the UV handle.
   *
   * In any case,  read alloc cb *may* be previously called so check the buffer pointer
   * and in case it's not NULL, free it.
   */
  if (nread == 0) {
    if (buf.base)
      xfree(buf.base);
  }
  else if (nread == -1) {
    if (buf.base)
      xfree(buf.base);
    // Once the connection is established, if the app calls to uv_close() this tcp_read_callback
    // could be called with nread=0, but never with nread=-1.
    close_uv_handle(cdata->_uv_handle);
    cdata->_uv_handle = NULL;
    ae_execute_in_ruby_land(_ae_tcp_read_callback);
  }
  else {
    if (cdata->do_receive)
      ae_execute_in_ruby_land(_ae_tcp_read_callback);
    else
      xfree(buf.base);
  }
}


static
VALUE _ae_tcp_read_callback(void)
{
  AE_TRACE();

  uv_tcp_t *_uv_handle = (uv_tcp_t *)last_uv_tcp_read_callback_params.stream;
  struct_ae_tcp_socket_cdata* cdata = (struct_ae_tcp_socket_cdata*)_uv_handle->data;
  VALUE _rb_data;
  VALUE error;

  // Data received.
  if (last_uv_tcp_read_callback_params.nread > 0) {
    _rb_data = ae_rb_str_new(last_uv_tcp_read_callback_params.buf.base, last_uv_tcp_read_callback_params.nread, cdata->encoding, 1);
    xfree(last_uv_tcp_read_callback_params.buf.base);
    rb_funcall2(cdata->ae_handle, method_on_data_received, 1, &_rb_data);
  }
  // Otherwise it's -1 (EOF, disconnection).
  else {
    error = ae_get_last_uv_error();  // TODO: Custom error?
    ae_remove_handle(cdata->ae_handle_id);
    rb_funcall2(cdata->ae_handle, method_on_disconnected, 1, &error);
  }

  return Qnil;
}





/** local_address() and peer_address() methods. */

VALUE AsyncEngineTcpSocket_local_address(VALUE self)
{
  AE_TRACE2();

  struct sockaddr_storage local_addr;
  int len = sizeof(local_addr);
  VALUE _rb_array_ip_port;

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  if (uv_tcp_getsockname(cdata->_uv_handle, (struct sockaddr*)&local_addr, &len))
    ae_raise_last_uv_error();

  if (! NIL_P(_rb_array_ip_port = ae_ip_utils_get_ip_port(&local_addr, cdata->ip_type)))
    return _rb_array_ip_port;
  else
    rb_raise(eAsyncEngineError, "error getting local address");
}


/** close() and destroy() methods. */

VALUE AsyncEngineTcpSocket_close(VALUE self)
{
  AE_TRACE2();

  VALUE error = Qnil;

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  close_uv_handle(cdata->_uv_handle);
  cdata->_uv_handle = NULL;

  // If it's connected we must manually call to on_disconnected() callback.
  if (cdata->connected) {
    ae_remove_handle(cdata->ae_handle_id);
    // NOTE: This should be safe. Ruby wont GC the handle here since the object is
    // in the stack.
    rb_funcall2(cdata->ae_handle, method_on_disconnected, 1, &error);
  }

  return Qtrue;
}


VALUE AsyncEngineTcpSocket_destroy(VALUE self)
{
  AE_TRACE2();

  VALUE error = Qnil;

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  close_uv_handle(cdata->_uv_handle);
  cdata->_uv_handle = NULL;

  // If it's connected we must manually call to on_disconnected() callback.
  if (cdata->connected) {
    ae_remove_handle(cdata->ae_handle_id);
    rb_funcall2(cdata->ae_handle, method_on_disconnected, 1, &error);
  }

  return Qtrue;
}
