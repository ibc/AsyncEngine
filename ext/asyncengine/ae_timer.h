#ifndef AE_TIMER_H
#define AE_TIMER_H


void init_ae_timer(void);

VALUE AsyncEngineTimer_uv_handle_init(VALUE self, VALUE _rb_delay, VALUE _rb_interval, VALUE _rb_block);
VALUE AsyncEngineTimer_stop(VALUE self);
VALUE AsyncEngineTimer_c_restart(VALUE self, VALUE _rb_delay, VALUE _rb_interval);
VALUE AsyncEngineTimer_delay(VALUE self);
VALUE AsyncEngineTimer_interval(VALUE self);
VALUE AsyncEngineTimer_is_alive(VALUE self);
VALUE AsyncEngineTimer_cancel(VALUE self);
VALUE AsyncEngineTimer_destroy(VALUE self);


#endif  /* AE_TIMER_H */
