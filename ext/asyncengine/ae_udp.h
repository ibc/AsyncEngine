#ifndef AE_UDP_H
#define AE_UDP_H


void init_ae_udp(void);

static VALUE AsyncEngineUdpSocket_new(int argc, VALUE *argv, VALUE self);
static VALUE AsyncEngineUdpSocket_send_datagram(int argc, VALUE *argv, VALUE self);
static VALUE AsyncEngineUdpSocket_local_address(VALUE self);
static VALUE AsyncEngineUdpSocket_ip_type(VALUE self);
static VALUE AsyncEngineUdpSocket_pause(VALUE self);
static VALUE AsyncEngineUdpSocket_resume(VALUE self);
static VALUE AsyncEngineUdpSocket_is_paused(VALUE self);
static VALUE AsyncEngineUdpSocket_set_encoding_external(VALUE self);
static VALUE AsyncEngineUdpSocket_set_encoding_utf8(VALUE self);
static VALUE AsyncEngineUdpSocket_set_encoding_ascii(VALUE self);
static VALUE AsyncEngineUdpSocket_encoding(VALUE self);
static VALUE AsyncEngineUdpSocket_is_alive(VALUE self);
static VALUE AsyncEngineUdpSocket_close(VALUE self);
static VALUE AsyncEngineUdpSocket_destroy(VALUE self);


#endif  /* AE_UDP_H */