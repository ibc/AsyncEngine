#ifndef AE_UDP_H
#define AE_UDP_H


void init_ae_udp();

VALUE AsyncEngineUdpSocket_c_init_udp_socket(VALUE self, VALUE ip, VALUE port);
VALUE AsyncEngineUdpSocket_send_datagram(VALUE self, VALUE rb_ip, VALUE rb_port, VALUE data);

VALUE AsyncEngineUdpSocket_close(VALUE self);


#endif  /* AE_UDP_H */