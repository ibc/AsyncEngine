#ifndef ASYNCENGINE_RUBY_H
#define ASYNCENGINE_RUBY_H


#include <ruby.h>


// Uncomment this line for enabling TRACE() function.
//#define AE_DEBUG1
//#define AE_DEBUG2
//#define AE_DEBUG3

#ifdef AE_DEBUG1
#define AE_TRACE()  fprintf(stderr, "AE_TRACE: %s:%d:%s (GVL: %d)\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p())
#else
#define AE_TRACE()
#endif

#ifdef AE_DEBUG2
#define AE_TRACE2(desc)  fprintf(stderr, "AE_TRACE2: %s:%d:%s (GVL: %d):  %s\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p(), desc)
#else
#define AE_TRACE2(desc)
#endif

#ifdef AE_DEBUG3
#define AE_TRACE3(desc)  fprintf(stderr, "AE_TRACE3: %s:%d:%s (GVL: %d):  %s\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p(), desc)
#else
#define AE_TRACE3(desc)
#endif



VALUE mAsyncEngine;


#endif  /* #ifndef ASYNCENGINE_RUBY_H */