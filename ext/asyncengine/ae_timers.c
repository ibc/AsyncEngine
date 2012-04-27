#include "asyncengine.h"
#include "ae_timers.h"


VALUE AsyncEngine_c_add_timer(VALUE self, VALUE timeout, VALUE interval, VALUE callback)
{
  long handle_number = AsyncEngine_next_handle();
  uv_timer_t* handle = (uv_timer_t*)malloc(sizeof(uv_timer_t));

  // Save the callback from being GC'd.
  rb_gc_register_address(&callback);
  
}