#ifndef AE_IP_UTILS_H
#define AE_IP_UTILS_H


#include "ip_parser.h"


VALUE symbol_ipv4;
VALUE symbol_ipv6;
VALUE symbol_ipv6_reference;


void init_ae_ip_utils();

VALUE AsyncEngineIpUtils_ip_type(VALUE self, VALUE string);
VALUE AsyncEngineIpUtils_compare_ips(int argc, VALUE *argv, VALUE self);

int ae_ip_utils_is_valid_port(int port);


#endif  /* AE_IP_UTILS_H */
