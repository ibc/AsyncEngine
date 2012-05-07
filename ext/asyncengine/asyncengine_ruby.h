#ifndef ASYNCENGINE_RUBY_H
#define ASYNCENGINE_RUBY_H


#include <ruby.h>


// Uncomment this line for enabling TRACE() function.
//#define AE_DEBUG
#ifdef AE_DEBUG
#define AE_TRACE()  fprintf(stderr, "AE_TRACE: %s:%d:%s\n", __FILE__, __LINE__, __FUNCTION__)
#else
#define AE_TRACE()
#endif


VALUE mAsyncEngine;


#endif  /* #ifndef ASYNCENGINE_RUBY_H */