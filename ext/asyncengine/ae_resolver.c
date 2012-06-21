#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_resolver.h"


static VALUE cAsyncEngineResolver;


typedef struct {
  VALUE on_result_proc_id;
} struct_getaddrinfo_data;

struct _uv_getaddrinfo_callback_data {
  int status;
  struct addrinfo* res;
  VALUE on_result_proc_id;
};


// Used for storing information about the last getaddrinfo() callback.
static struct _uv_getaddrinfo_callback_data last_uv_getaddrinfo_callback_data;


/** Predeclaration of static functions. */

static void _uv_getaddrinfo_callback(uv_getaddrinfo_t* handle, int status, struct addrinfo* res);
static VALUE _ae_getaddrinfo_callback(void);


void init_ae_resolver()
{
  AE_TRACE();

  cAsyncEngineResolver = rb_define_class_under(mAsyncEngine, "Resolver", cAsyncEngineHandle);

  rb_define_singleton_method(cAsyncEngineResolver, "resolve", AsyncEngineResolver_resolve, -1);
}


/** Resolver.resolve() class method.
 *
 * Arguments:
 * - hostname (String).
 * - IP family (Fixnum), valid values: Socket::AF_INET, Socket::AF_INET6, Socket::AF_UNSPEC (optional).
 * - Callback (Proc) (optional).
 *
 * Block optional.
 */

static
VALUE AsyncEngineResolver_resolve(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();
  uv_getaddrinfo_t* _uv_handle;
  struct_getaddrinfo_data* data;
  char *hostname;
  struct addrinfo addrinfo_hints;
  VALUE proc;
  int r;

  AE_CHECK_STATUS();
  AE_RB_CHECK_NUM_ARGS(1,3);
  AE_RB_ENSURE_BLOCK_OR_PROC(3, proc);

  // Parameter 1: hostname.
  if (! RB_TYPE_P(argv[0], T_STRING))
    rb_raise(rb_eTypeError, "hostname must be a String");
  hostname = StringValueCStr(argv[0]);

  memset(&addrinfo_hints, 0, sizeof(addrinfo_hints));
  addrinfo_hints.ai_socktype = SOCK_STREAM;  // Avoid duplicated results (with some IP but different socktype).

  // Parameter 2: IP family (optional).
  if (argc > 1 && ! NIL_P(argv[1])) {
    if (! FIXNUM_P(argv[1]))
      rb_raise(rb_eTypeError, "IP family must be a Fixnum");
    addrinfo_hints.ai_family = FIX2INT(argv[1]);
  }

  _uv_handle = ALLOC(uv_getaddrinfo_t);
  data = ALLOC(struct_getaddrinfo_data);
  _uv_handle->data = (void*)data;

  data->on_result_proc_id = ae_store_proc(proc);

  r = uv_getaddrinfo(AE_uv_loop, _uv_handle, &_uv_getaddrinfo_callback, hostname, NULL, &addrinfo_hints);
  if (r != 0) {
    ae_remove_proc(data->on_result_proc_id);
    xfree(data);
    xfree(_uv_handle);
    ae_raise_last_uv_error();
  }

  return Qtrue;
}


static
void _uv_getaddrinfo_callback(uv_getaddrinfo_t* handle, int status, struct addrinfo* res)
{
  AE_TRACE();
  struct_getaddrinfo_data* data = (struct_getaddrinfo_data*)handle->data;

  last_uv_getaddrinfo_callback_data.status = status;
  last_uv_getaddrinfo_callback_data.res = res;
  last_uv_getaddrinfo_callback_data.on_result_proc_id = data->on_result_proc_id;

  ae_take_gvl_and_run_with_error_handler(_ae_getaddrinfo_callback);
}


static
VALUE _ae_getaddrinfo_callback(void)
{
  AE_TRACE();
  struct addrinfo *ptr;
  struct sockaddr_in *addr4;
  struct sockaddr_in6 *addr6;
  char ip[INET6_ADDRSTRLEN];
  VALUE ips;
  VALUE proc = ae_remove_proc(last_uv_getaddrinfo_callback_data.on_result_proc_id);

  if (last_uv_getaddrinfo_callback_data.status == 0) {
    ips = rb_ary_new();
    for (ptr = last_uv_getaddrinfo_callback_data.res; ptr; ptr = ptr->ai_next) {
      if (ptr->ai_addrlen == 0)
        continue;
      switch (ptr->ai_addr->sa_family) {
        case AF_INET:
          addr4 = (struct sockaddr_in*)ptr->ai_addr;
          uv_ip4_name(addr4, ip, INET_ADDRSTRLEN);
          rb_ary_push(ips, rb_str_new2(ip));
          break;
        case AF_INET6:
          addr6 = (struct sockaddr_in6*)ptr->ai_addr;
          uv_ip6_name(addr6, ip, INET6_ADDRSTRLEN);
          rb_ary_push(ips, rb_str_new2(ip));
          break;
      }
    }
    return ae_proc_call_2(proc, Qnil, ips);
  }
  else
    return ae_proc_call_2(proc, ae_get_last_uv_error(), Qnil);
}
