#include "asyncengine_ruby.h"
#include "ae_ip_utils.h"


static VALUE mAsyncEngineUtils;


void init_ae_ip_utils()
{
  AE_TRACE();

  mAsyncEngineUtils = rb_define_module_under(mAsyncEngine, "Utils");

  rb_define_module_function(mAsyncEngineUtils, "ip_type", AsyncEngineUtils_ip_type, 1);
  rb_define_module_function(mAsyncEngineUtils, "compare_ips", AsyncEngineUtils_compare_ips, -1);

  symbol_ipv4 = ID2SYM(rb_intern("ipv4"));
  symbol_ipv6 = ID2SYM(rb_intern("ipv6"));
  symbol_ipv6_reference = ID2SYM(rb_intern("ipv6_reference"));
}


VALUE AsyncEngineUtils_ip_type(VALUE self, VALUE string)
{
  AE_TRACE();

  char *str;
  long len;

  if (TYPE(string) != T_STRING)
    rb_raise(rb_eTypeError, "argument must be a String");

  str = RSTRING_PTR(string);
  len = RSTRING_LEN(string);

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
VALUE AsyncEngineUtils_compare_ips(int argc, VALUE *argv, VALUE self)
{
  AE_TRACE();

  char *str1, *str2;
  long len1, len2;
  enum_ip_type ip1_type, ip2_type;
  int allow_ipv6_reference = 0;
  int ipv6_references_found = 0;

  if (argc < 2)
    rb_raise(rb_eArgError, "at least two arguments are required");

  if (TYPE(argv[0]) != T_STRING || TYPE(argv[1]) != T_STRING)
    rb_raise(rb_eTypeError, "first and second arguments must be String");

  if (argc >= 3 && TYPE(argv[2]) == T_TRUE)
    allow_ipv6_reference = 1;

  str1 = RSTRING_PTR(argv[0]);
  len1 = RSTRING_LEN(argv[0]);
  str2 = RSTRING_PTR(argv[1]);
  len2 = RSTRING_LEN(argv[1]);

  switch(ip1_type = ae_ip_parser_execute(str1, len1)) {
    case(ip_type_error):
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
    case(ip_type_error):
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
