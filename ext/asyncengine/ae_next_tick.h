#ifndef AE_NEXT_TICK_H
#define AE_NEXT_TICK_H


void init_ae_next_tick(void);
void load_ae_next_tick_uv_idle(void);
void unload_ae_next_tick_uv_idle(void);

VALUE AsyncEngine_c_next_tick(VALUE self);


#endif  /* AE_NEXT_TICK_H */
