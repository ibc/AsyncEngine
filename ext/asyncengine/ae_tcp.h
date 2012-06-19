#ifndef AE_TCP_H
#define AE_TCP_H


void init_ae_tcp(void);

static VALUE AsyncEngineTcpSocket_new(int argc, VALUE *argv, VALUE self);
static VALUE AsyncEngineTcpSocket_send_data(int argc, VALUE *argv, VALUE self);
static VALUE AsyncEngineTcpSocket_local_address(VALUE self);
static VALUE AsyncEngineTcpSocket_peer_address(VALUE self);
static VALUE AsyncEngineTcpSocket_set_connect_timeout(VALUE self, VALUE timeout);
static VALUE AsyncEngineTcpSocket_status(VALUE self);
static VALUE AsyncEngineTcpSocket_is_connected(VALUE self);
static VALUE AsyncEngineTcpSocket_is_alive(VALUE self);
static VALUE AsyncEngineTcpSocket_close(VALUE self);
static VALUE AsyncEngineTcpSocket_close_gracefully(int argc, VALUE *argv, VALUE self);
static VALUE AsyncEngineTcpSocket_destroy(VALUE self);


#endif  /* AE_TCP_H */