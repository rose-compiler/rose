

#ifndef _XMMINTRIN_H_INCLUDED
   #warning "XMMINTRIN_H_INCLUDED is NOT defined!"
#else
   #warning "XMMINTRIN_H_INCLUDED IS defined!"
#endif

// This macro is defined for ROSE, and not defined for the backend compiler (GNU g++ version 4.9.3).
#ifndef _XMMINTRIN_H_INCLUDED
#warning _XMMINTRIN_H_INCLUDED not defined

/* DQ (7/22/2018): Added error cpp directive. */
/* #warning "_XMMINTRIN_H_INCLUDED is not defined!" */

// DQ (7/22/2018): The required function is defined in xmmintrin.h NOT mmintrin.h, and
// DQ (7/22/2018): _XMMINTRIN_H_INCLUDED can't be defined before the call to #include <xmmintrin.h>
// #include <xmmintrin.h>
#include <xmmintrin.h>
// #include <mmintrin.h>
// #include <mm_malloc.h>

// #define _XMMINTRIN_H_INCLUDED
// #include <mmintrin.h>
// #include <mm_malloc.h>

// typedef float __m128 __attribute__ ((__vector_size__ (16), __may_alias__));
// typedef float __v4sf __attribute__ ((__vector_size__ (16)));
/* static __inline __m128 __attribute__((__always_inline__))
   XXX_mm_set_ps (const float __Z, const float __Y, const float __X, const float __W) */
/* int XXX_mm_set_ps (const float __Z, const float __Y, const float __X, const float __W) */
/* int foobar (const float __Z, const float __Y, const float __X, const float __W) */
int
foobar (int i)
   {
  // return __extension__ (__m128)(__v4sf){ __W };
     return 0;
   }

#else
// #include <mmintrin.h>
// #include <mm_malloc.h>
// #include <xmmintrin.h>
#endif /* _XMMINTRIN_H_INCLUDED */

void func1 () 
   {
  // _mm_set_ps((float)0.0, (float)0.0, (float)0.0, (float)0.0);
  // foobar(0.0F, 0.0F, 0.0F, 0.0F);
#if 0
     foobar(0); 
#else
     _mm_set_ps(0.0, 0.0, 0.0, 0.0);
#endif
   }

