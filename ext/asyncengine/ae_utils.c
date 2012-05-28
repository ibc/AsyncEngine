#include "asyncengine_ruby.h"
#include "ae_utils.h"
#include "ae_ip_utils.h"


VALUE mAsyncEngineUtils;
VALUE cAsyncEngineUtilsNetworkInterface;

static ID att_name;
static ID att_is_internal;
static ID att_ip_type;
static ID att_ip;


void init_ae_utils(void)
{
  mAsyncEngineUtils = rb_define_module_under(mAsyncEngine, "Utils");
  cAsyncEngineUtilsNetworkInterface = rb_define_class_under(mAsyncEngineUtils, "NetworkInterface", rb_cObject);

  rb_define_module_function(mAsyncEngineUtils, "get_hrtime", AsyncEngineUtils_get_hrtime, 0);
  rb_define_module_function(mAsyncEngineUtils, "get_total_memory", AsyncEngineUtils_get_total_memory, 0);
  rb_define_module_function(mAsyncEngineUtils, "get_free_memory", AsyncEngineUtils_get_free_memory, 0);
  rb_define_module_function(mAsyncEngineUtils, "get_loadavg", AsyncEngineUtils_get_loadavg, 0);
  rb_define_module_function(mAsyncEngineUtils, "get_uptime", AsyncEngineUtils_get_uptime, 0);
  rb_define_module_function(mAsyncEngineUtils, "get_network_interfaces", AsyncEngineUtils_get_network_interfaces, 0);

  att_name = rb_intern("@name");
  att_is_internal = rb_intern("@is_internal");
  att_ip_type = rb_intern("@ip_type");
  att_ip = rb_intern("@ip");
}


/*
 * Returns the current high-resolution real time. This is expressed in
 * nanoseconds. It is relative to an arbitrary time in the past. It is not
 * related to the time of day and therefore not subject to clock drift. The
 * primary use is for measuring performance between intervals.
 */
VALUE AsyncEngineUtils_get_hrtime(VALUE self)
{
  AE_TRACE();

  return LONG2FIX((long)uv_hrtime());
}


/* Gets total memory in bytes */
VALUE AsyncEngineUtils_get_total_memory(VALUE self)
{
  AE_TRACE();

  return LONG2FIX((long)uv_get_total_memory());
}


/* Gets free memory in bytes */
VALUE AsyncEngineUtils_get_free_memory(VALUE self)
{
  AE_TRACE();

  return LONG2FIX((long)uv_get_free_memory());
}


/* Returns an Array with loadavg data (Float values). */
VALUE AsyncEngineUtils_get_loadavg(VALUE self)
{
  AE_TRACE();

  double avg[3];
  uv_loadavg(avg);

  return rb_ary_new3(3, rb_float_new(avg[0]), rb_float_new(avg[1]), rb_float_new(avg[2]));
}


/* Returns system uptime value in seconds .*/
VALUE AsyncEngineUtils_get_uptime(VALUE self)
{
  AE_TRACE();

  double uptime;
  uv_err_t error;

  error = uv_uptime(&uptime);
  if (error.code)
    ae_raise_uv_error(error.code);

  return rb_float_new(uptime);
}


VALUE AsyncEngineUtils_get_network_interfaces(VALUE self)
{
  AE_TRACE();

  int i, num_interfaces;
  char ip[INET6_ADDRSTRLEN+1];
  uv_interface_address_t* interfaces;
  uv_err_t error;
  VALUE ae_network_interface, result_array;

  error = uv_interface_addresses(&interfaces, &num_interfaces);
  if (error.code)
    ae_raise_uv_error(error.code);

  result_array = rb_ary_new2(num_interfaces);

  for (i = 0; i < num_interfaces; i++) {
    ae_network_interface = rb_obj_alloc(cAsyncEngineUtilsNetworkInterface);
    rb_ivar_set(ae_network_interface, att_name, rb_str_new2(interfaces[i].name));
    rb_ivar_set(ae_network_interface, att_is_internal, (interfaces[i].is_internal ? Qtrue : Qfalse));

    if (interfaces[i].address.address4.sin_family == AF_INET) {
      rb_ivar_set(ae_network_interface, att_ip_type, symbol_ipv4);
      uv_ip4_name(&interfaces[i].address.address4, ip, INET_ADDRSTRLEN);
      rb_ivar_set(ae_network_interface, att_ip, rb_str_new2(ip));
    }
    else if (interfaces[i].address.address4.sin_family == AF_INET6) {
      rb_ivar_set(ae_network_interface, att_ip_type, symbol_ipv6);
      uv_ip6_name(&interfaces[i].address.address6, ip, INET6_ADDRSTRLEN);
      rb_ivar_set(ae_network_interface, att_ip, rb_str_new2(ip));
    }

    rb_ary_store(result_array, i, ae_network_interface);
  }

  uv_free_interface_addresses(interfaces, num_interfaces);

  return result_array;
}