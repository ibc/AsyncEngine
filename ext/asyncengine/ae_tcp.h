#ifndef AE_TCP_H
#define AE_TCP_H


void init_ae_tcp(void);

VALUE AsyncEngineTcpSocket_new(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineTcpSocket_send_data(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineTcpSocket_local_address(VALUE self);
VALUE AsyncEngineTcpSocket_peer_address(VALUE self);
VALUE AsyncEngineTcpSocket_is_connected(VALUE self);
VALUE AsyncEngineTcpSocket_is_alive(VALUE self);
VALUE AsyncEngineTcpSocket_close(VALUE self);
VALUE AsyncEngineTcpSocket_destroy(VALUE self);


// TODO: peer_address() a veces hace raise :ENOTCONN, socket is not connected (UV_ERRNO: 31). Esto puede ser
// porque se ejecuta la función #peer_address() antes del callback de read con nread=-1. Algo hay que hacer.


#endif  /* AE_TCP_H */