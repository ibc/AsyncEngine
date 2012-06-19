#ifndef AE_TIMER_H
#define AE_TIMER_H


void init_ae_timer(void);

static VALUE AsyncEngineTimer_new(int argc, VALUE *argv, VALUE self);
static VALUE AsyncEnginePeriodicTimer_new(int argc, VALUE *argv, VALUE self);
static VALUE AsyncEngineTimer_pause(VALUE self);
static VALUE AsyncEngineTimer_restart(int argc, VALUE *argv, VALUE self);
static VALUE AsyncEnginePeriodicTimer_restart(int argc, VALUE *argv, VALUE self);
static VALUE AsyncEngineTimer_delay(VALUE self);
static VALUE AsyncEnginePeriodicTimer_interval(VALUE self);
static VALUE AsyncEngineTimer_is_alive(VALUE self);
static VALUE AsyncEngineTimer_close(VALUE self);
static VALUE AsyncEngineTimer_destroy(VALUE self);


#endif  /* AE_TIMER_H */
