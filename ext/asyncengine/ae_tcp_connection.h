#ifndef AE_TCP_CONNECTION_H
#define AE_TCP_CONNECTION_H


void init_ae_tcp_connection(void);

VALUE AsyncEngineTcpSocket_uv_handle_init(VALUE self, VALUE _rb_dest_ip, VALUE _rb_dest_port);

VALUE AsyncEngineTcpSocket_local_address(VALUE self);
VALUE AsyncEngineTcpSocket_peer_address(VALUE self);

VALUE AsyncEngineTcpSocket_is_alive(VALUE self);
VALUE AsyncEngineTcpSocket_close(VALUE self);
VALUE AsyncEngineTcpSocket_destroy(VALUE self);


#endif  /* AE_TCP_CONNECTION_H */