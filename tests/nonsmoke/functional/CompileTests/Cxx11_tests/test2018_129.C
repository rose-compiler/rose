#ifndef _XMMINTRIN_H_INCLUDED
   #warning "XMMINTRIN_H_INCLUDED is NOT defined!"
#else
   #warning "XMMINTRIN_H_INCLUDED IS defined!"
#endif


#ifndef _XMMINTRIN_H_INCLUDED
#warning _XMMINTRIN_H_INCLUDED not defined
// #define _XMMINTRIN_H_INCLUDED
#include <mmintrin.h>
#include <mm_malloc.h>
typedef float __m128 __attribute__ ((__vector_size__ (16), __may_alias__));
typedef float __v4sf __attribute__ ((__vector_size__ (16)));

// Definition for GNU g++ (but will not be output in the CPP block where _XMMINTRIN_H_INCLUDED is different then when ROSE processed the file.
// static __inline __m128 __attribute__((__always_inline__))
// XXX_mm_set_ps (const int __Z)
int
X_mm_set_ps (const int __Z)
   {
  // return __extension__ (__m128)(__v4sf){ __W };
     return 0;
   }
#else

// Definition for ROSE
int
X_mm_set_ps (const int __Z)
   {
     return 0;
   }

#endif /* _XMMINTRIN_H_INCLUDED */

void func1 () 
   {
  // _mm_set_ps(0.0, 0.0, 0.0, 0.0);
  // _mm_set_ps(0.0F, 0.0F, 0.0F, 0.0F);
     X_mm_set_ps(0);
   }

