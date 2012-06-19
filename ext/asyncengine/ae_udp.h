#ifndef AE_UDP_H
#define AE_UDP_H


void init_ae_udp(void);

VALUE AsyncEngineUdpSocket_new(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineUdpSocket_send_datagram(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineUdpSocket_local_address(VALUE self);
VALUE AsyncEngineUdpSocket_ip_type(VALUE self);
VALUE AsyncEngineUdpSocket_pause(VALUE self);
VALUE AsyncEngineUdpSocket_resume(VALUE self);
VALUE AsyncEngineUdpSocket_is_paused(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_external(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_utf8(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_ascii(VALUE self);
VALUE AsyncEngineUdpSocket_encoding(VALUE self);
VALUE AsyncEngineUdpSocket_is_alive(VALUE self);
VALUE AsyncEngineUdpSocket_close(VALUE self);
VALUE AsyncEngineUdpSocket_destroy(VALUE self);


#endif  /* AE_UDP_H */