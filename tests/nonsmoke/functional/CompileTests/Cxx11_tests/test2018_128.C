#ifndef _XMMINTRIN_H_INCLUDED
   #warning "XMMINTRIN_H_INCLUDED is NOT defined!"
#else
   #warning "XMMINTRIN_H_INCLUDED IS defined!"
#endif


#ifndef _XMMINTRIN_H_INCLUDED
#warning _XMMINTRIN_H_INCLUDED not defined
#define _XMMINTRIN_H_INCLUDED
#include <mmintrin.h>
#include <mm_malloc.h>
typedef float __m128 __attribute__ ((__vector_size__ (16), __may_alias__));
typedef float __v4sf __attribute__ ((__vector_size__ (16)));
static __inline __m128 __attribute__((__always_inline__))
_mm_set_ps (const float __Z, const float __Y, const float __X, const float __W)
   {
     return __extension__ (__m128)(__v4sf){ __W };
   }
#endif /* _XMMINTRIN_H_INCLUDED */

void func1 () 
   {
  // _mm_set_ps(0.0, 0.0, 0.0, 0.0);
     _mm_set_ps(0.0F, 0.0F, 0.0F, 0.0F);
   }

