#ifndef _AE_IP_PARSER_H
#define _AE_IP_PARSER_H_H


typedef enum {
  ip_type_ipv4 = 1,
  ip_type_ipv6,
  ip_type_ipv6_reference,
  ip_type_error
} enum_ip_type;


enum_ip_type ae_ip_parser_execute(const char *str, int len);


#endif  /* _AE_IP_PARSER_H */