#ifndef AE_TCP_H
#define AE_TCP_H


void init_ae_tcp(void);

VALUE AsyncEngineTcpSocket_uv_handle_init(VALUE self, VALUE _rb_bind_ip, VALUE _rb_bind_port, VALUE _rb_dest_ip, VALUE _rb_dest_port);
VALUE AsyncEngineTcpSocket_send_data(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineTcpSocket_local_address(VALUE self);
VALUE AsyncEngineTcpSocket_peer_address(VALUE self);
VALUE AsyncEngineTcpSocket_is_connected(VALUE self);
VALUE AsyncEngineTcpSocket_is_alive(VALUE self);
VALUE AsyncEngineTcpSocket_close(VALUE self);
VALUE AsyncEngineTcpSocket_destroy(VALUE self);
// TODO: añadir método #connected?


#endif  /* AE_TCP_H */