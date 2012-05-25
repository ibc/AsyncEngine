#ifndef AE_UDP_H
#define AE_UDP_H


void init_ae_udp();

VALUE AsyncEngineUdpSocket_uv_handle_init(VALUE self, VALUE _rb_bind_ip, VALUE _rb_bind_port);
VALUE AsyncEngineUdpSocket_send_datagram(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineUdpSocket_close(VALUE self);
VALUE AsyncEngineUdpSocket_is_alive(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_external(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_utf8(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_ascii(VALUE self);
VALUE AsyncEngineUdpSocket_destroy(VALUE self);


#endif  /* AE_UDP_H */