#ifndef ASYNCENGINE_RUBY_H
#define ASYNCENGINE_RUBY_H


#include <ruby.h>


// Uncomment this line for enabling TRACE() function.
//#define AE_DO_TRACE
#define AE_DO_DEBUG

#ifdef AE_DO_TRACE
  #define AE_TRACE()  fprintf(stderr, "AE_TRACE: %s:%d:%s (GVL: %d)\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p())
#else
  #define AE_TRACE()
#endif

#ifdef AE_DO_DEBUG
  #define AE_DEBUG(desc)  fprintf(stderr, "AE_DEBUG: %s:%d:%s (GVL: %d):  %s\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p(), desc)
#else
  #define AE_DEBUG(desc)
#endif


VALUE mAsyncEngine;


#endif  /* #ifndef ASYNCENGINE_RUBY_H */