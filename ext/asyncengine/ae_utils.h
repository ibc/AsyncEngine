#ifndef AE_UTILS_H
#define AE_UTILS_H


void init_ae_utils(void);

VALUE AsyncEngineUtils_get_hrtime(VALUE self);
VALUE AsyncEngineUtils_get_total_memory(VALUE self);
VALUE AsyncEngineUtils_get_free_memory(VALUE self);
VALUE AsyncEngineUtils_get_loadavg(VALUE self);
VALUE AsyncEngineUtils_get_uptime(VALUE self);
VALUE AsyncEngineUtils_get_network_interfaces(VALUE self);


#endif  /* AE_UTILS_H */