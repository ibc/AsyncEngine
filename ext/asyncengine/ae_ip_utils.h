#ifndef AE_IP_UTILS_H
#define AE_IP_UTILS_H


#include "ip_parser.h"


VALUE symbol_ipv4;
VALUE symbol_ipv6;
VALUE symbol_ipv6_reference;


void init_ae_ip_utils();

VALUE AsyncEngineUtils_ip_type(VALUE self, VALUE string);
VALUE AsyncEngineUtils_compare_ips(int argc, VALUE *argv, VALUE self);

// VALUE Utils_is_ip(VALUE self, VALUE string);
// VALUE Utils_is_pure_ip(VALUE self, VALUE string);
// VALUE Utils_ip_type(VALUE self, VALUE string);
// VALUE Utils_compare_ips(VALUE self, VALUE string1, VALUE string2);
// VALUE Utils_compare_pure_ips(VALUE self, VALUE string1, VALUE string2);
// VALUE Utils_normalize_ipv6(int argc, VALUE *argv, VALUE self);
// VALUE Utils_normalize_host(int argc, VALUE *argv, VALUE self);
// VALUE Utils_to_pure_ip(VALUE self, VALUE string);


#endif  /* AE_IP_UTILS_H */
