#ifndef _AE_DEBUG_H
#define _AE_DEBUG_H


/* NOTE: Uncomment these lines for debugging during development. */
//#define AE_DO_TRACE
//#define AE_DO_DEBUG
#define AE_DO_WARN
#define AE_DO_ASSERT


#ifdef AE_DO_TRACE
#define AE_TRACE()  fprintf(stdout, "AE_TRACE: %s:%d:%s (GVL:%d)\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p())
#else
#define AE_TRACE()
#endif

#ifdef AE_DO_DEBUG
#define AE_DEBUG(desc)  fprintf(stderr, "AE_DEBUG: %s:%d:%s (GVL:%d)::  %s\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p(), desc)
#else
#define AE_DEBUG(desc)
#endif

#ifdef AE_DO_WARN
#define AE_WARN(desc)  fprintf(stderr, "AE_WARN: %s:%d:%s (GVL:%d)::  %s\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p(), desc)
#else
#define AE_WARN(desc)
#endif

#define AE_ABORT(desc)  fprintf(stderr, "AE_ABORT: %s:%d:%s (GVL:%d)::  %s\n", __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p(), desc); exit(1);

#ifdef AE_DO_ASSERT
#define AE_ASSERT(condition)  \
  do {  \
    if (!(condition)) {  \
      fprintf(stderr, "AE_ASSERT: %s:%d:%s (GVL:%d):: assertion `" #condition "' failed\n",  \
      __FILE__, __LINE__, __FUNCTION__, ruby_thread_has_gvl_p());  \
      abort();  \
    }  \
  } while(0)
#else
#define AE_ASSERT(condition)  condition
#endif


#endif  /* _AE_DEBUG_H */