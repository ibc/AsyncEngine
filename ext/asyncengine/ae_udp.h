#ifndef AE_UDP_H
#define AE_UDP_H


void init_ae_udp(void);

VALUE AsyncEngineUdpSocket_uv_handle_init(VALUE self, VALUE _rb_bind_ip, VALUE _rb_bind_port);
VALUE AsyncEngineUdpSocket_send_datagram(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineUdpSocket_local_address(VALUE self);
VALUE AsyncEngineUdpSocket_peer_address(VALUE self);
VALUE AsyncEngineUdpSocket_reply_datagram(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineUdpSocket_set_receiving(VALUE self, VALUE allow);
VALUE AsyncEngineUdpSocket_is_receiving(VALUE self);
VALUE AsyncEngineUdpSocket_set_sending(VALUE self, VALUE allow);
VALUE AsyncEngineUdpSocket_is_sending(VALUE self);
VALUE AsyncEngineUdpSocket_pause(VALUE self);
VALUE AsyncEngineUdpSocket_resume(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_external(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_utf8(VALUE self);
VALUE AsyncEngineUdpSocket_set_encoding_ascii(VALUE self);
VALUE AsyncEngineUdpSocket_encoding(VALUE self);
//VALUE AsyncEngineUdpSocket_set_broadcast(VALUE self, VALUE on);
VALUE AsyncEngineUdpSocket_is_alive(VALUE self);
VALUE AsyncEngineUdpSocket_close(VALUE self);
VALUE AsyncEngineUdpSocket_destroy(VALUE self);


#endif  /* AE_UDP_H */