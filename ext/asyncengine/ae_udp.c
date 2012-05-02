#include "asyncengine_ruby.h"
#include "ae_handle_common.h"
#include "ae_udp.h"


void init_ae_udp()
{
  rb_define_module_function(mAsyncEngine, "test_ipv4", AsyncEngine_test_ipv4, 2);
}


VALUE AsyncEngine_test_ipv4(VALUE self, VALUE rb_ip, VALUE rb_port)
{
  char* ip = StringValueCStr(rb_ip);
  int port = FIX2INT(rb_port);
  struct sockaddr_in addr;
  
  //printf("AsyncEngine_ip4():  ip=%s, port=%d\n", ip, port);
  /* Convert string ip addresses to binary structures */
  
  //UV_EXTERN struct sockaddr_in uv_ip4_addr(const char* ip, int port);
  addr = uv_ip4_addr(ip, port);
  
}