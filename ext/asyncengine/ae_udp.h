#ifndef AE_UDP_H
#define AE_UDP_H


void init_ae_udp();

VALUE AsyncEngineUdpSocket_uv_handle_init(VALUE self, VALUE ip, VALUE port);
VALUE AsyncEngineUdpSocket_send_datagram(VALUE self, VALUE rb_ip, VALUE rb_port, VALUE data);
VALUE AsyncEngineUdpSocket_close(VALUE self);
VALUE AsyncEngineUdpSocket_is_alive(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_external(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_utf8(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_ascii(VALUE self);
VALUE AsyncEngineUdpSocket_destroy(VALUE self);


#endif  /* AE_UDP_H */