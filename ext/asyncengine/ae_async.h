#ifndef AE_ASYNC_H
#define AE_ASYNC_H


void init_ae_async();

VALUE AsyncEngine_c_call_from_other_thread(VALUE self, VALUE block);


#endif  /* AE_ASYNC_H */
