#ifndef _AE_DEBUG_H
#define _AE_DEBUG_H


#include <stdio.h>


/* NOTE: Uncomment these lines for debugging during development. */
#define AE_DO_TRACE
#define AE_DO_TRACE2
#define AE_DO_DEBUG
#define AE_DO_DEBUG2
#define AE_DO_WARN
#define AE_DO_ASSERT


#define _AE_DEBUG_STR  "%s:%d:%s()"
#define _AE_DEBUG_STR_WITH_DESC  _AE_DEBUG_STR ":: "
#define _AE_DEBUG_ARG  __FILE__, __LINE__, __FUNCTION__
#define _AE_DEBUG_PRINT(...)  fprintf(stderr, __VA_ARGS__)

#ifdef AE_DO_TRACE
#define AE_TRACE()  _AE_DEBUG_PRINT("AE_TRACE: " _AE_DEBUG_STR "\n", _AE_DEBUG_ARG)
#else
#define AE_TRACE()
#endif

#ifdef AE_DO_TRACE2
#define AE_TRACE2()  _AE_DEBUG_PRINT("AE_TRACE2: " _AE_DEBUG_STR "\n", _AE_DEBUG_ARG)
#else
#define AE_TRACE2()
#endif

#ifdef AE_DO_DEBUG
#define AE_DEBUG(_desc, ...)  _AE_DEBUG_PRINT("AE_DEBUG: " _AE_DEBUG_STR_WITH_DESC _desc "\n", _AE_DEBUG_ARG, ##__VA_ARGS__)
#else
#define AE_DEBUG(_desc, ...)
#endif

#ifdef AE_DO_DEBUG2
#define AE_DEBUG2(_desc, ...)  _AE_DEBUG_PRINT("AE_DEBUG2: " _AE_DEBUG_STR_WITH_DESC _desc "\n", _AE_DEBUG_ARG, ##__VA_ARGS__)
#else
#define AE_DEBUG2(_desc, ...)
#endif

#ifdef AE_DO_WARN
#define AE_WARN(_desc, ...)  _AE_DEBUG_PRINT("AE_WARN: " _AE_DEBUG_STR_WITH_DESC _desc "\n", _AE_DEBUG_ARG, ##__VA_ARGS__)
#else
#define AE_WARN(_desc, ...)
#endif

#define AE_ABORT(_desc, ...)  _AE_DEBUG_PRINT("AE_ABORT: " _AE_DEBUG_STR_WITH_DESC _desc "\n", _AE_DEBUG_ARG, ##__VA_ARGS__); exit(1);

#ifdef AE_DO_ASSERT
#define AE_ASSERT(_condition)  \
  do {  \
    if (!(_condition)) {  \
      _AE_DEBUG_PRINT("AE_ASSERT: " _AE_DEBUG_STR_WITH_DESC "assertion `" #_condition "' failed\n", _AE_DEBUG_ARG);  \
      abort();  \
    }  \
  } while(0)
#else
#define AE_ASSERT(_condition)  _condition
#endif


#endif  /* _AE_DEBUG_H */