#ifndef _AE_DEBUG_H
#define _AE_DEBUG_H


#include <stdio.h>


/* NOTE: Uncomment these lines for debugging during development. */
//#define AE_DO_TRACE
#define AE_DO_TRACE2
//#define AE_DO_DEBUG
#define AE_DO_DEBUG2
#define AE_DO_WARN
#define AE_DO_ASSERT


#ifdef HAVE_RUBY_THREAD_HAS_GVL_P
#define AE_SHOW_THREAD_HAS_GVL_PART1 (GVL:%d)
#define AE_SHOW_THREAD_HAS_GVL_PART2 " , ruby_thread_has_gvl_p()"
#else
#define AE_SHOW_THREAD_HAS_GVL_PART1
#define AE_SHOW_THREAD_HAS_GVL_PART2
#endif


#ifdef AE_DO_TRACE
#define AE_TRACE()  fprintf(stdout, "AE_TRACE: %s:%d:%s\n", __FILE__, __LINE__, __FUNCTION__)
#else
#define AE_TRACE()
#endif

#ifdef AE_DO_TRACE2
#define AE_TRACE2()  fprintf(stdout, "AE_TRACE2: %s:%d:%s\n", __FILE__, __LINE__, __FUNCTION__)
#else
#define AE_TRACE2()
#endif

#ifdef AE_DO_DEBUG
#define AE_DEBUG(desc)  fprintf(stdout, "AE_DEBUG: %s:%d:%s::  %s\n", __FILE__, __LINE__, __FUNCTION__, desc)
#else
#define AE_DEBUG(desc)
#endif

#ifdef AE_DO_DEBUG2
#define AE_DEBUG2(desc)  fprintf(stdout, "AE_DEBUG2: %s:%d:%s::  %s\n", __FILE__, __LINE__, __FUNCTION__, desc)
#else
#define AE_DEBUG2(desc)
#endif

#ifdef AE_DO_WARN
#define AE_WARN(desc)  fprintf(stderr, "AE_WARN: %s:%d:%s::  %s\n", __FILE__, __LINE__, __FUNCTION__, desc)
#else
#define AE_WARN(desc)
#endif

#define AE_ABORT(desc)  fprintf(stderr, "AE_ABORT: %s:%d:%s::  %s\n", __FILE__, __LINE__, __FUNCTION__, desc); exit(1);

#ifdef AE_DO_ASSERT
#define AE_ASSERT(condition)  \
  do {  \
    if (!(condition)) {  \
      fprintf(stderr, "AE_ASSERT: %s:%d:%s:: assertion `" #condition "' failed\n",  \
      __FILE__, __LINE__, __FUNCTION__);  \
      abort();  \
    }  \
  } while(0)
#else
#define AE_ASSERT(condition)  condition
#endif

// TEST
#define KK2(...)  fprintf(stderr, __VA_ARGS__)


#endif  /* _AE_DEBUG_H */