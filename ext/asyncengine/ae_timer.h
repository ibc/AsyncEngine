#ifndef AE_TIMER_H
#define AE_TIMER_H


void init_ae_timer();

VALUE AsyncEngineTimer_uv_handle_init(VALUE self, VALUE rb_delay, VALUE rb_interval, VALUE block);
VALUE AsyncEngineTimer_stop(VALUE self);
VALUE AsyncEngineTimer_c_restart(VALUE self, VALUE rb_delay, VALUE rb_interval);
VALUE AsyncEngineTimer_delay(VALUE self);
VALUE AsyncEngineTimer_interval(VALUE self);
VALUE AsyncEngineTimer_cancel(VALUE self);
VALUE AsyncEngineTimer_is_alive(VALUE self);
VALUE AsyncEngineTimer_destroy(VALUE self);


#endif  /* AE_TIMER_H */
