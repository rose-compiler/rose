/* The Intel API is flexible enough that we must allow aliasing with other vector types, and their scalar components.  */

#ifdef __INTEL_COMPILER
  #include "xmmintrin.h"
  #define __inline inline
#else
// DQ (1/25/2017): I think this should be (__vector_size__ (16) for at least GNU 4.8 in C++11 mode.
// typedef float __m128 __attribute__ ((__vector_size__ (16), __may_alias__));
// typedef float __m128 __attribute__ ((__vector_size__ (32), __may_alias__));
  typedef float __m128 __attribute__ ((__vector_size__ (16), __may_alias__));
#endif

#ifdef __INTEL_COMPILER
static __inline __m128 __attribute__((__always_inline__)) _mm_setzero_ps_rose (void);
#else
static __inline __m128 __attribute__((__always_inline__)) _mm_setzero_ps_rose (void)
{
// DQ (8/22/2011): Modified header for ROSE
// return __extension__ (__m128){ 0.0f, 0.0f, 0.0f, 0.0f };
// DQ (8/29/2015): This is not unparsed properly for the Intel compiler.
   return __extension__ (__m128){ 0.0f };
}
#endif
