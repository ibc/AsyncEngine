#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_ip_utils.h"
#include "ae_udp.h"


#define AE_UDP_DATAGRAM_MAX_SIZE 65536


static VALUE cAsyncEngineUdpSocket;

static ID method_on_datagram_received;


enum flags {
  PAUSED = 1 << 1
};

typedef struct {
  uv_udp_t *_uv_handle;
  uv_buf_t recv_buffer;
  enum_ip_type ip_type;
  unsigned int flags;
  enum_string_encoding encoding;
  VALUE ae_handle;
  VALUE ae_handle_id;
} struct_cdata;

struct _uv_recv_callback_data {
  struct_cdata* cdata;
  ssize_t nread;
  struct sockaddr* addr;
  unsigned flags;
};

typedef struct {
  char *datagram;
  VALUE on_send_proc_id;
} struct_send_data;

struct _uv_send_callback_data {
  VALUE on_send_proc_id;
  int status;
};


// Used for storing information about the last UDP recv callback.
static struct _uv_recv_callback_data last_uv_recv_callback_data;

// Used for storing information about the last UDP send callback.
static struct _uv_send_callback_data last_uv_send_callback_data;


/** Predeclaration of static functions. */

static VALUE AsyncEngineUdpSocket_alloc(VALUE klass);
static void AsyncEngineUdpSocket_mark(struct_cdata* cdata);
static void AsyncEngineUdpSocket_free(struct_cdata* cdata);
static void init_instance(VALUE self, enum_ip_type ip_type, char *bind_ip, int bind_port);
static uv_buf_t _uv_alloc_callback(uv_handle_t* handle, size_t suggested_size);
static void _uv_recv_callback(uv_udp_t* handle, ssize_t nread, uv_buf_t buf, struct sockaddr* addr, unsigned flags);
static VALUE _ae_recv_callback(void);
static void _uv_send_callback(uv_udp_send_t* req, int status);
static VALUE _ae_send_callback(void);
static void close_handle(struct_cdata *cdata);


void init_ae_udp()
{
  AE_TRACE();

  cAsyncEngineUdpSocket = rb_define_class_under(mAsyncEngine, "UDPSocket", cAsyncEngineHandle);

  rb_define_alloc_func(cAsyncEngineUdpSocket, AsyncEngineUdpSocket_alloc);

  rb_define_singleton_method(cAsyncEngineUdpSocket, "new", AsyncEngineUdpSocket_new, -1);

  rb_define_method(cAsyncEngineUdpSocket, "send_datagram", AsyncEngineUdpSocket_send_datagram, -1);
  rb_define_method(cAsyncEngineUdpSocket, "local_address", AsyncEngineUdpSocket_local_address, 0);
  rb_define_method(cAsyncEngineUdpSocket, "ip_type", AsyncEngineUdpSocket_ip_type, 0);
  rb_define_method(cAsyncEngineUdpSocket, "pause", AsyncEngineUdpSocket_pause, 0);
  rb_define_method(cAsyncEngineUdpSocket, "resume", AsyncEngineUdpSocket_resume, 0);
  rb_define_method(cAsyncEngineUdpSocket, "paused?", AsyncEngineUdpSocket_is_paused, 0);
  rb_define_method(cAsyncEngineUdpSocket, "set_encoding_external", AsyncEngineUdpSocket_set_encoding_external, 0);
  rb_define_method(cAsyncEngineUdpSocket, "set_encoding_utf8", AsyncEngineUdpSocket_set_encoding_utf8, 0);
  rb_define_method(cAsyncEngineUdpSocket, "set_encoding_ascii", AsyncEngineUdpSocket_set_encoding_ascii, 0);
  rb_define_method(cAsyncEngineUdpSocket, "encoding", AsyncEngineUdpSocket_encoding, 0);
  rb_define_method(cAsyncEngineUdpSocket, "alive?", AsyncEngineUdpSocket_is_alive, 0);
  rb_define_method(cAsyncEngineUdpSocket, "close", AsyncEngineUdpSocket_close, 0);
  rb_define_private_method(cAsyncEngineUdpSocket, "destroy", AsyncEngineUdpSocket_destroy, 0);

  method_on_datagram_received = rb_intern("on_datagram_received");
}


/** Class alloc, mark and free functions. */

static
VALUE AsyncEngineUdpSocket_alloc(VALUE klass)
{
  AE_TRACE();

  struct_cdata* cdata = ALLOC(struct_cdata);
  cdata->recv_buffer = uv_buf_init(ALLOC_N(char, AE_UDP_DATAGRAM_MAX_SIZE), AE_UDP_DATAGRAM_MAX_SIZE);
  cdata->_uv_handle = NULL;

  return Data_Wrap_Struct(klass, AsyncEngineUdpSocket_mark, AsyncEngineUdpSocket_free, cdata);
}


static
void AsyncEngineUdpSocket_mark(struct_cdata* cdata)
{
  AE_TRACE();

  // TODO: hash de send_datagram...
}


static
void AsyncEngineUdpSocket_free(struct_cdata* cdata)
{
  AE_TRACE();

  xfree(cdata->recv_buffer.base);
  xfree(cdata);
}


/** UDPSocket.new() method.
 *
 * Arguments:
 * - bind IP (String).
 * - bind port (Fixnum).
 */
VALUE AsyncEngineUdpSocket_new(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();
  char *bind_ip;
  int bind_ip_len;
  int bind_port;
  enum_ip_type ip_type;
  VALUE instance;

  AE_CHECK_STATUS();
  AE_RB_CHECK_NUM_ARGS(2,2);

  // Parameter 1: bind IP.
  if (! RB_TYPE_P(argv[0], T_STRING))
    rb_raise(rb_eTypeError, "bind IP must be a String");
  bind_ip = StringValueCStr(argv[0]);
  bind_ip_len = RSTRING_LEN(argv[0]);

  // Parameter 2: bind port.
  if (! FIXNUM_P(argv[1]))
    rb_raise(rb_eTypeError, "bind port must be a Fixnum");
  bind_port = FIX2INT(argv[1]);
  if (! ae_ip_utils_is_valid_port(bind_port))
    rb_raise(rb_eArgError, "invalid bind port value");

  if ((ip_type = ae_ip_parser_execute(bind_ip, bind_ip_len)) == ip_type_no_ip)
    rb_raise(rb_eTypeError, "bind IP is not valid IPv4 or IPv6");

  // Allocate the Ruby instance.
  instance = rb_obj_alloc(self);

  // Init the UV stuff within the instance.
  init_instance(instance, ip_type, bind_ip, bind_port);

  return instance;
}


static
void init_instance(VALUE self, enum_ip_type ip_type, char *bind_ip, int bind_port)
{
  AE_TRACE();
  uv_udp_t *_uv_handle;
  int r;

  // Create and init the UV handle.
  _uv_handle = ALLOC(uv_udp_t);
  r = uv_udp_init(AE_uv_loop, _uv_handle);
  if (r != 0) {
    xfree(_uv_handle);
    ae_raise_last_uv_error();
  }

  GET_CDATA_FROM_SELF;

  // Bind.
  switch(ip_type) {
    case ip_type_ipv4:
      r = uv_udp_bind(_uv_handle, uv_ip4_addr(bind_ip, bind_port), 0);
      break;
    case ip_type_ipv6:
      r = uv_udp_bind6(_uv_handle, uv_ip6_addr(bind_ip, bind_port), UV_UDP_IPV6ONLY);
      break;
  }
  if (r != 0) {
    close_handle(cdata);
    ae_raise_last_uv_error();
  }

  // Start receiving.
  r = uv_udp_recv_start(_uv_handle, _uv_alloc_callback, _uv_recv_callback);
  if (r != 0) {
    close_handle(cdata);
    ae_raise_last_uv_error();
  }

  // Fill cdata struct.
  cdata->_uv_handle = _uv_handle;
  cdata->ip_type = ip_type;
  cdata->flags = 0;
  cdata->encoding = string_encoding_ascii;
  cdata->ae_handle = self;
  cdata->ae_handle_id = ae_store_handle(self); // Avoid GC.

  // Fill data field of the UV handle.
  cdata->_uv_handle->data = (void *)cdata;
}


static
uv_buf_t _uv_alloc_callback(uv_handle_t* handle, size_t suggested_size)
{
  AE_TRACE();

  return ((struct_cdata*)handle->data)->recv_buffer;
}


static
void _uv_recv_callback(uv_udp_t* handle, ssize_t nread, uv_buf_t buf, struct sockaddr* addr, unsigned flags)
{
  AE_TRACE();

  struct_cdata* cdata =  (struct_cdata*)handle->data;

  // Don't execute the callback when AsyncEngine is releasing.
  if (AE_status == AE_RELEASING)
    return;

  if (nread == 0)
    return;

  // uv.h: -1 if a transmission error was detected, ignore it.
  if (nread == -1) {
    AE_WARN("nread == -1");  // TODO: tmp
    return;
  }

  // Ignore datagram when paused.
  if (cdata->flags & PAUSED)
    return;

  last_uv_recv_callback_data.cdata = cdata;
  last_uv_recv_callback_data.nread = nread;
  last_uv_recv_callback_data.addr = addr;
  last_uv_recv_callback_data.flags = flags;

  ae_take_gvl_and_run_with_error_handler(_ae_recv_callback);
}


static
VALUE _ae_recv_callback(void)
{
  AE_TRACE();

  struct_cdata* cdata = last_uv_recv_callback_data.cdata;
  VALUE datagram, array_ip_port, _rb_src_ip, _rb_src_port;

  datagram = ae_rb_str_new(cdata->recv_buffer.base,
                           last_uv_recv_callback_data.nread, cdata->encoding, 1);
  array_ip_port = ae_ip_utils_get_ip_port((struct sockaddr_storage *)last_uv_recv_callback_data.addr,
                                          cdata->ip_type);

  return rb_funcall(cdata->ae_handle, method_on_datagram_received, 3, datagram,
                    rb_ary_entry(array_ip_port, 0), rb_ary_entry(array_ip_port, 1));
}


/** UDPSocket#send_datagram() method.
 * 
 * Arguments:
 * - datagram (String).
 * - destination IP (String).
 * - destination port (Fixnum).
 * - proc (Proc) (optional).
 *
 * Block optional.
 */
VALUE AsyncEngineUdpSocket_send_datagram(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();
  char *_rb_datagram, *datagram, *ip;
  int datagram_len, ip_len, port;
  uv_buf_t buffer;
  uv_udp_send_t* _uv_udp_send_req;
  struct_send_data* send_data;
  VALUE proc;
  int r;

  AE_RB_CHECK_NUM_ARGS(3,4);
  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;
  AE_RB_GET_BLOCK_OR_PROC(4, proc);

  // Parameter 1: data.
  if (! RB_TYPE_P(argv[0], T_STRING))
    rb_raise(rb_eTypeError, "datagram must be a String");
  _rb_datagram = RSTRING_PTR(argv[0]);
  datagram_len = RSTRING_LEN(argv[0]);

  // Parameter 2: destination IP.
  if (! RB_TYPE_P(argv[1], T_STRING))
    rb_raise(rb_eTypeError, "destination IP must be a String");
  ip = StringValueCStr(argv[1]);
  ip_len = RSTRING_LEN(argv[1]);

  // Parameter 3: destination port.
  if (! FIXNUM_P(argv[2]))
    rb_raise(rb_eTypeError, "destination port must be a Fixnum");
  port = FIX2INT(argv[2]);
  if (! ae_ip_utils_is_valid_port(port))
    rb_raise(rb_eArgError, "invalid destination port value");

  if (cdata->ip_type != ae_ip_parser_execute(ip, ip_len))
    rb_raise(rb_eTypeError, "invalid destination IP family");

  datagram = ALLOC_N(char, datagram_len);
  memcpy(datagram, _rb_datagram, datagram_len);

  send_data = ALLOC(struct_send_data);
  send_data->datagram = datagram;

  if (! NIL_P(proc))
    send_data->on_send_proc_id = ae_store_block(proc);
  else
    send_data->on_send_proc_id = Qnil;

  _uv_udp_send_req = ALLOC(uv_udp_send_t);
  _uv_udp_send_req->data = (void *)send_data;

  buffer = uv_buf_init(datagram, datagram_len);

  switch(cdata->ip_type) {
    case ip_type_ipv4:
      r = uv_udp_send(_uv_udp_send_req, cdata->_uv_handle, &buffer, 1, uv_ip4_addr(ip, port), _uv_send_callback);
      break;
    case ip_type_ipv6:
      r =  uv_udp_send6(_uv_udp_send_req, cdata->_uv_handle, &buffer, 1, uv_ip6_addr(ip, port), _uv_send_callback);
      break;
  }
  if (r != 0) {
    xfree(datagram);
    xfree(send_data);
    xfree(_uv_udp_send_req);
    ae_raise_last_uv_error();
  }

  return Qtrue;
}


static
void _uv_send_callback(uv_udp_send_t* req, int status)
{
  AE_TRACE();

  struct_send_data* send_data = (struct_send_data*)req->data;
  int do_on_send = 0;

  // Block was provided so must call it with success or error param.
  if (! NIL_P(send_data->on_send_proc_id)) {
    last_uv_send_callback_data.on_send_proc_id = send_data->on_send_proc_id;
    last_uv_send_callback_data.status = status;
    do_on_send = 1;
  }

  xfree(req);
  xfree(send_data->datagram);
  xfree(send_data);

  if (do_on_send)
    ae_take_gvl_and_run_with_error_handler(_ae_send_callback);
}


static
VALUE _ae_send_callback(void)
{
  AE_TRACE();

  VALUE proc = ae_remove_block(last_uv_send_callback_data.on_send_proc_id);

  // Don't execute the callback when AsyncEngine is releasing.
  if (AE_status == AE_RELEASING)
    return Qnil;

  if (last_uv_send_callback_data.status == 0)
    return ae_block_call_1(proc, Qnil);
  else
    return ae_block_call_1(proc, ae_get_last_uv_error());
}


/** UDPSocket#local_address() method. */

VALUE AsyncEngineUdpSocket_local_address(VALUE self)
{
  AE_TRACE();
  struct sockaddr_storage local_addr;
  int len = sizeof(local_addr);
  int r;

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  r = uv_udp_getsockname(cdata->_uv_handle, (struct sockaddr*)&local_addr, &len);
  if (r != 0)
    ae_raise_last_uv_error();

  return ae_ip_utils_get_ip_port(&local_addr, cdata->ip_type);
}


/** UDPSocket#ip_type() method. */

VALUE AsyncEngineUdpSocket_ip_type(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  return ae_ip_type_to_rb_symbol(cdata->ip_type);
}


/** UDPSocket#pause() method. */

VALUE AsyncEngineUdpSocket_pause(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  cdata->flags |= PAUSED;
  return Qtrue;
}


/** UDPSocket#resume() method. */

VALUE AsyncEngineUdpSocket_resume(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  cdata->flags &= ~PAUSED;
  return Qtrue;
}


/** UDPSocket#paused?() method. */

VALUE AsyncEngineUdpSocket_is_paused(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  if (cdata->flags & PAUSED)
    return Qtrue;
  else
    return Qfalse;
}


/** UDPSocket#set_encoding_external() method. */

VALUE AsyncEngineUdpSocket_set_encoding_external(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  cdata->encoding = string_encoding_external;
  return Qtrue;
}


/** UDPSocket#set_encoding_utf8() method. */

VALUE AsyncEngineUdpSocket_set_encoding_utf8(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  cdata->encoding = string_encoding_utf8;
  return Qtrue;
}


/** UDPSocket#set_encoding_ascii() method. */

VALUE AsyncEngineUdpSocket_set_encoding_ascii(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  cdata->encoding = string_encoding_ascii;
  return Qtrue;
}


/** UDPSocket#encoding() method. */

VALUE AsyncEngineUdpSocket_encoding(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  return ae_encoding_to_rb_symbol(cdata->encoding);
}


/** UDPSocket#alive?() method. */

VALUE AsyncEngineUdpSocket_is_alive(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  return Qtrue;
}


/** UDPSocket#close() method. */

VALUE AsyncEngineUdpSocket_close(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  close_handle(cdata);
  return Qtrue;
}


/** UDPSocket#destroy() private method. */

VALUE AsyncEngineUdpSocket_destroy(VALUE self)
{
  AE_TRACE();

  GET_CDATA_FROM_SELF_AND_CHECK_UV_HANDLE_IS_OPEN;

  close_handle(cdata);
  return Qtrue;
}


static
void close_handle(struct_cdata *cdata)
{
  AE_TRACE();

  AE_CLOSE_UV_HANDLE(cdata->_uv_handle);
  cdata->_uv_handle = NULL;
  ae_remove_handle(cdata->ae_handle_id);
}