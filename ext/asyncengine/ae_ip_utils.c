#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_ip_utils.h"


/* TODO: Temporary hack: libuv should provide uv_inet_pton and uv_inet_ntop. */
#ifdef AE_OS_WINDOWS
  #include <inet_net_pton.h>
  #include <inet_ntop.h>
  #define ae_inet_pton ares_inet_pton
  #define ae_inet_ntop ares_inet_ntop
#else /* AE_OS_UNIX */
  #include <arpa/inet.h>
  #define ae_inet_pton inet_pton
  #define ae_inet_ntop inet_ntop
#endif


static VALUE mAsyncEngineIpUtils;


void init_ae_ip_utils()
{
  AE_TRACE();

  mAsyncEngineIpUtils = rb_define_module_under(mAsyncEngine, "IpUtils");

  rb_define_module_function(mAsyncEngineIpUtils, "ip_type", AsyncEngineIpUtils_ip_type, 1);
  rb_define_module_function(mAsyncEngineIpUtils, "compare_ips", AsyncEngineIpUtils_compare_ips, -1);

  symbol_ipv4 = ID2SYM(rb_intern("ipv4"));
  symbol_ipv6 = ID2SYM(rb_intern("ipv6"));
  symbol_ipv6_reference = ID2SYM(rb_intern("ipv6_reference"));
}


VALUE AsyncEngineIpUtils_ip_type(VALUE self, VALUE _rb_str)
{
  AE_TRACE();

  char *str;
  long len;

  if (TYPE(_rb_str) != T_STRING)
    rb_raise(rb_eTypeError, "argument must be a String");

  str = RSTRING_PTR(_rb_str);
  len = RSTRING_LEN(_rb_str);

  switch(ae_ip_parser_execute(str, len)) {
    case(ip_type_ipv4):
      return symbol_ipv4;
      break;
    case(ip_type_ipv6):
      return symbol_ipv6;
      break;
    case(ip_type_ipv6_reference):
      return symbol_ipv6_reference;
      break;
    default:
      return Qfalse;
      break;
  }
}


/*
 * Return 1 if both pure IP's are equal, 0 otherwise.
 */
static
int compare_pure_ips(char *ip1, size_t len1, enum_ip_type ip1_type, char *ip2, size_t len2, enum_ip_type ip2_type)
{
  AE_TRACE();

  struct in_addr in_addr1, in_addr2;
  struct in6_addr in6_addr1, in6_addr2;
  //INET6_ADDRSTRLEN = 46
  char _ip1[46], _ip2[46];

  // Not same IP type, return false.
  if (ip1_type != ip2_type)
    return 0;

  memcpy(_ip1, ip1, len1);
  _ip1[len1] = '\0';
  memcpy(_ip2, ip2, len2);
  _ip2[len2] = '\0';

  switch(ip1_type) {
    // Comparing IPv4 with IPv4.
    case(ip_type_ipv4):
      if (ae_inet_pton(AF_INET, _ip1, &in_addr1) == 0)  return 0;
      if (ae_inet_pton(AF_INET, _ip2, &in_addr2) == 0)  return 0;
      if (in_addr1.s_addr == in_addr2.s_addr)
        return 1;
      else
        return 0;
      break;
    // Comparing IPv6 with IPv6.
    case(ip_type_ipv6):
      if (ae_inet_pton(AF_INET6, _ip1, &in6_addr1) != 1)  return 0;
      if (ae_inet_pton(AF_INET6, _ip2, &in6_addr2) != 1)  return 0;
      if (memcmp(in6_addr1.s6_addr, in6_addr2.s6_addr, sizeof(in6_addr1.s6_addr)) == 0)
        return 1;
      else
        return 0;
      break;
    default:
      return 0;
      break;
  }
}


/*
 * Returns true if both IP's are equal (binary comparison). It allows comparison
 * between IPv4, IPv6 and IPv6 references.
 * Returns true when comparing an IPv6 with its IPv6 reference in case
 * parameter _allow_ipv6_reference_ is set to true, it returns false otherwise.
 *
 * Returns nil if at least one of the IP's is not valid IPv4, IPv6 or IPv6 reference.
 */
VALUE AsyncEngineIpUtils_compare_ips(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();

  char *str1, *str2;
  long len1, len2;
  enum_ip_type ip1_type, ip2_type;
  int allow_ipv6_reference = 0;
  int ipv6_references_found = 0;

  AE_RB_CHECK_NUM_ARGS(2,3);

  if (TYPE(argv[0]) != T_STRING || TYPE(argv[1]) != T_STRING)
    rb_raise(rb_eTypeError, "first and second arguments must be String");

  if (argc == 3 && TYPE(argv[2]) == T_TRUE)
    allow_ipv6_reference = 1;

  str1 = RSTRING_PTR(argv[0]);
  len1 = RSTRING_LEN(argv[0]);
  str2 = RSTRING_PTR(argv[1]);
  len2 = RSTRING_LEN(argv[1]);

  switch(ip1_type = ae_ip_parser_execute(str1, len1)) {
    case(ip_type_no_ip):
      return Qnil;
      break;
    case(ip_type_ipv6_reference):
      ipv6_references_found++;
      str1 += 1;
      len1 -= 2;
      ip1_type = ip_type_ipv6;
      break;
    default:
      break;
  }
  switch(ip2_type = ae_ip_parser_execute(str2, len2)) {
    case(ip_type_no_ip):
      return Qnil;
      break;
    case(ip_type_ipv6_reference):
      ipv6_references_found++;
      str2 += 1;
      len2 -= 2;
      ip2_type = ip_type_ipv6;
      break;
    default:
      break;
  }

  if (ipv6_references_found == 1 && allow_ipv6_reference == 0)
    return Qfalse;

  if (compare_pure_ips(str1, len1, ip1_type, str2, len2, ip2_type))
    return Qtrue;
  else
    return Qfalse;
}


int ae_ip_utils_is_valid_port(int port)
{
  AE_TRACE();

  if (port >= 0 && port <= 65535)
    return 1;
  else
    return 0;
}


/*
 * Parameters:
 * - addr: pointer to a struct sockaddr_storage.
 * - ip_type: if it's ip_type_no_ip then the IP will be parsed.
 * 
 * Returns a Ruby Array with three elements:
 * - ip: Ruby String.
 * - port: Ruby Fixnum.
 * If an error occurs (in ae_inet_ntop() function) nil is returned.
 */
VALUE ae_ip_utils_get_ip_port(struct sockaddr_storage *addr, enum_ip_type ip_type)
{
  AE_TRACE();

  struct sockaddr_in *addr4;
  struct sockaddr_in6 *addr6;
  char ip[INET6_ADDRSTRLEN + 1];
  int port;
  VALUE _rb_array_ip_port;

  switch(ip_type) {
    case ip_type_ipv4:
      addr4 = (struct sockaddr_in*)addr;
      if (! ae_inet_ntop(AF_INET, &addr4->sin_addr, ip, INET_ADDRSTRLEN)) {
        AE_WARN("ae_inet_ntop() failed");
        return Qnil;
      }
      port = (int)htons(addr4->sin_port);
      break;
    case ip_type_ipv6:
      addr6 = (struct sockaddr_in6*)addr;
      if (! ae_inet_ntop(AF_INET6, &addr6->sin6_addr, ip, INET6_ADDRSTRLEN)) {
        AE_WARN("ae_inet_ntop() failed");
        return Qnil;
      }
      port = (int)htons(addr6->sin6_port);
      break;
    default:
      AE_ASSERT("invalid ip_type");
  }

  _rb_array_ip_port = rb_ary_new2(2);
  rb_ary_store(_rb_array_ip_port, 0, rb_str_new2(ip));
  rb_ary_store(_rb_array_ip_port, 1, INT2FIX(port));

  return _rb_array_ip_port;
}
