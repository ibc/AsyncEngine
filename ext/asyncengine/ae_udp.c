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
} struct_ae_udp_socket_cdata;


typedef struct {
  char *datagram;
  VALUE rb_on_send_error_block_id;
} struct_ae_udp_socket_send_data;


typedef struct {
  uv_udp_t* handle;
  ssize_t nread;
  uv_buf_t buf;
  struct sockaddr* addr;
  unsigned flags;
} struct_ae_udp_socket_recv_callback_data;


// Used for storing information about the last UDP recv callback.
static struct_ae_udp_socket_recv_callback_data last_udp_recv_callback_data;


static void AsyncEngineUdpSocket_free(void *cdata)
{
  AE_TRACE();

  printf("DBG: AsyncEngineUdpSocket_free()\n");

  xfree(((struct_ae_udp_socket_cdata*)cdata)->recv_buffer.base);
  xfree(cdata);
}


VALUE AsyncEngineUdpSocket_alloc(VALUE klass)
{
  AE_TRACE();

  struct_ae_udp_socket_cdata* cdata = ALLOC(struct_ae_udp_socket_cdata);
  cdata->recv_buffer = uv_buf_init(ALLOC_N(char, AE_UDP_DATAGRAM_MAX_SIZE), AE_UDP_DATAGRAM_MAX_SIZE);

  return Data_Wrap_Struct(klass, NULL, AsyncEngineUdpSocket_free, cdata);
}


void init_ae_udp()
{
  AE_TRACE();

  cAsyncEngineUdpSocket = rb_define_class_under(mAsyncEngine, "UDPSocket", rb_cObject);

  rb_define_alloc_func(cAsyncEngineUdpSocket, AsyncEngineUdpSocket_alloc);
  rb_define_private_method(cAsyncEngineUdpSocket, "_c_init_udp_socket", AsyncEngineUdpSocket_c_init_udp_socket, 2);
  rb_define_method(cAsyncEngineUdpSocket, "send_datagram", AsyncEngineUdpSocket_send_datagram, 3);
  rb_define_method(cAsyncEngineUdpSocket, "close", AsyncEngineUdpSocket_close, 0);
  rb_define_private_method(cAsyncEngineUdpSocket, "destroy", AsyncEngineUdpSocket_destroy, 0);

  att_bind_ip = rb_intern("@_bind_ip");
  att_bind_port = rb_intern("@_bind_port");
  att_ip_type = rb_intern("@_ip_type");

  method_on_received_datagram = rb_intern("on_received_datagram");
}


static
void terminate(struct_ae_udp_socket_cdata* cdata)
{
  AE_TRACE();
  printf("DBG: terminate()\n");

  // Close the UDP handle so it's unreferenced by uv.
  uv_close((uv_handle_t *)cdata->_uv_handle, ae_uv_handle_close_callback);
  // Set the handle field to NULL.
  cdata->_uv_handle = NULL;
  // Let the GC work.
  ae_remove_handle(cdata->rb_ae_udp_socket_id);
}


/*
 * The same as terminate() but it does not remove the instance from the
 * hash of handles. This private method MUST be called by AE over all the
 * existing handles in the hash when the user invokes AE.stop or when an
 * exception raises. It allows uv_run() to exit.
 */
VALUE AsyncEngineUdpSocket_destroy(VALUE self)
{
  AE_TRACE();
  printf("DBG: AsyncEngineUdpSocket_destroy()\n");

  struct_ae_udp_socket_cdata* cdata;

  Data_Get_Struct(self, struct_ae_udp_socket_cdata, cdata);

  // Close the UDP handle so it's unreferenced by uv.
  uv_close((uv_handle_t *)cdata->_uv_handle, ae_uv_handle_close_callback);
  // Set the handle field to NULL.
  cdata->_uv_handle = NULL;

  return Qtrue;
}


static
uv_buf_t _uv_udp_recv_alloc_callback(uv_handle_t* handle, size_t suggested_size)
{
  AE_TRACE();

  //printf("DBG: uv_alloc_cb: suggested_size = %d\n", (int)suggested_size);

  return ((struct_ae_udp_socket_cdata*)handle->data)->recv_buffer;
}


static
VALUE ae_udp_socket_recv_callback(VALUE ignore)
{
  AE_TRACE();

  struct_ae_udp_socket_cdata* cdata = (struct_ae_udp_socket_cdata*)last_udp_recv_callback_data.handle->data;
  VALUE rb_datagram = RB_STR_TAINTED_UTF8_NEW(last_udp_recv_callback_data.buf.base, last_udp_recv_callback_data.nread);

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

  ae_execute_function_with_gvl_and_protect(ae_udp_socket_recv_callback, Qnil);
}


VALUE AsyncEngineUdpSocket_c_init_udp_socket(VALUE self, VALUE rb_bind_ip, VALUE rb_bind_port)
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

  rb_ivar_set(self, att_bind_ip, rb_bind_ip);
  rb_ivar_set(self, att_bind_port, rb_bind_port);

  Data_Get_Struct(self, struct_ae_udp_socket_cdata, cdata);

  cdata->_uv_handle = ALLOC(uv_udp_t);
  cdata->rb_ae_udp_socket = self;
  cdata->rb_ae_udp_socket_id = ae_store_handle(self); // Avoid GC.
  cdata->ip_type = ip_type;

  assert(! uv_udp_init(uv_default_loop(), cdata->_uv_handle));

  // NOTE: If we set the data field *before* uv_udp_init() then such a function set data to NULL !!!
  cdata->_uv_handle->data = cdata;

  switch(ip_type) {
    case ip_type_ipv4:
      if (uv_udp_bind(cdata->_uv_handle, uv_ip4_addr(bind_ip, bind_port), 0)) {
        terminate(cdata);
        ae_raise_last_uv_error();
      }
      break;
    case ip_type_ipv6:
      // TODO: UDP flags en IPv6 puede ser que si. Solo vale 0 o UV_UDP_IPV6ONLY.
      if (uv_udp_bind6(cdata->_uv_handle, uv_ip6_addr(bind_ip, bind_port), UV_UDP_IPV6ONLY)) {
        terminate(cdata);
        ae_raise_last_uv_error();
      }
      break;
  }

  assert(! uv_udp_recv_start(cdata->_uv_handle, _uv_udp_recv_alloc_callback, _uv_udp_recv_callback));

  return self;
}


static
VALUE ae_udp_send_error_callback(VALUE rb_on_send_error_block)
{
  AE_TRACE();

  return ae_block_call_1(rb_on_send_error_block, ae_get_last_uv_error());
}


static
void _uv_udp_send_callback(uv_udp_send_t* req, int status)
{
  AE_TRACE();

  //printf("DBG: _uv_udp_send_callback(): status = %i\n", status);

  struct_ae_udp_socket_send_data* send_data = req->data;
  VALUE rb_on_send_error_block;
  int do_error = 0;

  // TODO: Ojo, estoy eliminando un elemento de un Hash sin tener el GVL !!!
  // It could crash:
  // (1) rb_hash_delete() can call Ruby's #hash method for each elements.
  // (2) If another thread access to the hash simultaneously, it will be crash.
  // But in my case I use Fixnum as hash key, so no problem :)
  if (! NIL_P(send_data->rb_on_send_error_block_id)) {
    rb_on_send_error_block = ae_remove_block(send_data->rb_on_send_error_block_id);
    if (status)
      do_error = 1;
  }

  xfree(send_data->datagram);
  xfree(send_data);
  xfree(req);

  if (do_error)
    ae_execute_function_with_gvl_and_protect(ae_udp_send_error_callback, rb_on_send_error_block);
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
  struct_ae_udp_socket_send_data* send_data;
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

  Check_Type(rb_data, T_STRING);

  datagram_len = RSTRING_LEN(rb_data);
  datagram = ALLOC_N(char, datagram_len);
  memcpy(datagram, RSTRING_PTR(rb_data), datagram_len);

  send_data = ALLOC(struct_ae_udp_socket_send_data);
  send_data->datagram = datagram;

  // TODO
  if (rb_block_given_p())
    send_data->rb_on_send_error_block_id = ae_store_block(rb_block_proc());
  else
    send_data->rb_on_send_error_block_id = Qnil;

  _uv_req = ALLOC(uv_udp_send_t);
  _uv_req->data = send_data;

  buffer = uv_buf_init(datagram, datagram_len);

  // TODO uv_udp_send() parece que devuelve siempre 0 aunque le pases un puerto destino 0 (que
  // luego se traduce en error en el callback).
  // Segun los src solo devuelve error si el handle no esta bindeado (el nuestro ya lo está) o si no hay
  // más memoria para un alloc que hace, bufff. Un assert y va que arde.
  switch(cdata->ip_type) {
    case ip_type_ipv4:
      assert(! uv_udp_send(_uv_req, cdata->_uv_handle, &buffer, 1, uv_ip4_addr(ip, port), _uv_udp_send_callback));
      break;
    case ip_type_ipv6:
      assert(! uv_udp_send6(_uv_req, cdata->_uv_handle, &buffer, 1, uv_ip6_addr(ip, port), _uv_udp_send_callback));
      break;
  }

  return Qtrue;
}


VALUE AsyncEngineUdpSocket_close(VALUE self)
{
  AE_TRACE();

  struct_ae_udp_socket_cdata* cdata;

  Data_Get_Struct(self, struct_ae_udp_socket_cdata, cdata);

  if (cdata->_uv_handle) {
    terminate(cdata);
    return Qtrue;
  }
  else
    return Qfalse;
}