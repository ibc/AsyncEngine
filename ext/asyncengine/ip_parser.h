#ifndef _AE_IP_PARSER_H
#define _AE_IP_PARSER_H


typedef enum {
  ip_type_no_ip = 0,
  ip_type_ipv4,
  ip_type_ipv6,
  ip_type_ipv6_reference,
} enum_ip_type;


enum_ip_type ae_ip_parser_execute(const char *str, int len);


#endif  /* _AE_IP_PARSER_H */