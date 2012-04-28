#ifndef AE_TIMERS_H
#define AE_TIMERS_H


VALUE AsyncEngine_c_add_timer(VALUE self, VALUE delay, VALUE interval, VALUE callback);
VALUE AsyncEngineTimer_cancel(VALUE self);


#endif  /* AE_TIMERS_H */
