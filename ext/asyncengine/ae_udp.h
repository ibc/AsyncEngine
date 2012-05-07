#ifndef AE_UDP_H
#define AE_UDP_H


void init_ae_udp();

VALUE AsyncEngineUdpSocket_c_init_udp_socket(VALUE self, VALUE ip_type, VALUE ip, VALUE port);


#endif  /* AE_UDP_H */