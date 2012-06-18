#ifndef AE_TIMER_H
#define AE_TIMER_H


void init_ae_timer(void);

VALUE AsyncEngineTimer_new(int argc, VALUE *argv, VALUE self);
VALUE AsyncEnginePeriodicTimer_new(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineTimer_pause(VALUE self);
VALUE AsyncEngineTimer_restart(int argc, VALUE *argv, VALUE self);
VALUE AsyncEnginePeriodicTimer_restart(int argc, VALUE *argv, VALUE self);
VALUE AsyncEngineTimer_delay(VALUE self);
VALUE AsyncEnginePeriodicTimer_interval(VALUE self);
VALUE AsyncEngineTimer_is_alive(VALUE self);
VALUE AsyncEngineTimer_close(VALUE self);
VALUE AsyncEngineTimer_destroy(VALUE self);


#endif  /* AE_TIMER_H */
