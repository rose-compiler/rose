/* The Intel API is flexible enough that we must allow aliasing with other vector types, and their scalar components.  */

#ifdef __INTEL_COMPILER
//  #include "xmmintrin.h"
#ifdef USE_ROSE_BACKEND
// Make sure that the Intel compiler backend is seeing the include file where __m128 is defined.
  #include "xmmintrin.h"
#else
// A declaration of __m128 can only appear in xmmintrin.h (enforced by Intel compiler).
  typedef float __m128 __attribute__ ((__vector_size__ (16), __may_alias__));
#endif
// Intel backend compiler requires __inline to be only on the function 
// declaration, not the defining function (EDG does not require this).
  static __inline __m128 __attribute__((__always_inline__)) _mm_setzero_ps_rose (void);
#else
// Debugging to make sure we are using the Intel compiler macro.
// #error "Intel compiler not being used"
  typedef float __m128 __attribute__ ((__vector_size__ (16), __may_alias__));
#endif

// DQ (8/29/2015): __inline is not supported in the Intel compiler.
static __m128 __attribute__((__always_inline__)) _mm_setzero_ps_rose (void)
{
// DQ (8/22/2011): Modified header for ROSE
// return __extension__ (__m128){ 0.0f, 0.0f, 0.0f, 0.0f };
// DQ (8/29/2015): This is not unparsed properly for the Intel compiler.
   return __extension__ (__m128){ 0.0f };
}
