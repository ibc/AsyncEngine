#ifndef _AE_IP_COMMON_H
#define _AE_IP_COMMON_H


/* TODO: Temporary hack: libuv should provide uv_inet_pton and uv_inet_ntop. */
//#ifdef AE_WINDOWS
//  #include <inet_net_pton.h>
//  #include <inet_ntop.h>
//  #define ae_inet_pton ares_inet_pton
//  #define ae_inet_ntop ares_inet_ntop
//#else /* __POSIX__ */
  #include <arpa/inet.h>
  #define ae_inet_pton inet_pton
  #define ae_inet_ntop inet_ntop
//#endif


typedef enum {
  ip_type_ipv4 = 1,
  ip_type_ipv6,
  ip_type_ipv6_reference,
  ip_type_error
} enum_ip_type;


#endif  /* _AE_IP_COMMON_H */