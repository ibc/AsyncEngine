#ifndef IP_UTILS_H
#define IP_UTILS_H


#include <stdlib.h>


enum enum_ip_type {
  ip_type_ipv4 = 1,
  ip_type_ipv6,
  ip_type_ipv6_reference,
  ip_type_error
};


enum enum_ip_type ip_utils_parser_execute(const char *str, size_t len);


#endif
