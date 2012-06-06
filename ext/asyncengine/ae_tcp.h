#ifndef AE_TCP_H
#define AE_TCP_H


void init_ae_tcp(void);

VALUE AsyncEngineTcpSocket_new(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineTcpSocket_send_data(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineTcpSocket_local_address(VALUE self);
VALUE AsyncEngineTcpSocket_peer_address(VALUE self);
VALUE AsyncEngineTcpSocket_set_connect_timeout(VALUE self, VALUE timeout);
VALUE AsyncEngineTcpSocket_status(VALUE self);
VALUE AsyncEngineTcpSocket_is_connected(VALUE self);
VALUE AsyncEngineTcpSocket_is_alive(VALUE self);
VALUE AsyncEngineTcpSocket_close(VALUE self);
VALUE AsyncEngineTcpSocket_close_gracefully(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineTcpSocket_destroy(VALUE self);


#endif  /* AE_TCP_H */