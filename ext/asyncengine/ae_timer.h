#ifndef AE_TIMER_H
#define AE_TIMER_H


VALUE AsyncEngine_c_add_timer(VALUE self, VALUE delay, VALUE interval, VALUE callback, VALUE instance);
VALUE AsyncEngineTimer_cancel(VALUE self);
VALUE AsyncEngineTimer_c_set_interval(VALUE self, VALUE interval);


#endif  /* AE_TIMER_H */
