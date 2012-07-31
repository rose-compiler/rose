/*
  A common layer for different SIMD intrinsics.
*/
#ifndef LIB_SIMD_H 
#define LIB_SIMD_H

#define USE_SSE 1

#ifdef  USE_SSE
#include <xmmintrin.h>
typedef  __m128   __SIMD; 
typedef  __m128i  __SIMDi; 
typedef  __m128d  __SIMDd; 

#elif defined USE_AVX
#include <avxintrin.h>
typedef  __m256   __SIMD; 
typedef  __m256i  __SIMDi; 
typedef  __m256d  __SIMDd; 

#elif defined USE_IBM
typedef  vector float   __SIMD; 
typedef  vector int     __SIMDi; 
typedef  vector double  __SIMDd; 

#endif

#ifdef __cplusplus
extern "C" {
#endif

extern __SIMD  _SIMD_add_ps(__SIMD, __SIMD);
extern __SIMDd _SIMD_add_pd(__SIMDd, __SIMDd);
extern __SIMDi _SIMD_add_epi32(__SIMDi, __SIMDi);

extern __SIMD  _SIMD_sub_ps(__SIMD, __SIMD);
extern __SIMDd _SIMD_sub_pd(__SIMDd, __SIMDd);
extern __SIMDi _SIMD_sub_epi32(__SIMDi, __SIMDi);

extern __SIMD  _SIMD_mul_ps(__SIMD, __SIMD);
extern __SIMDd _SIMD_mul_pd(__SIMDd, __SIMDd);
extern __SIMDi _SIMD_mul_epi32(__SIMDi, __SIMDi);

extern __SIMD  _SIMD_div_ps(__SIMD, __SIMD);
extern __SIMDd _SIMD_div_pd(__SIMDd, __SIMDd);
//extern __SIMD _SIMD_div_epi32(__SIMDi, __SIMDi);

extern __SIMD  _SIMD_madd_ps(__SIMD, __SIMD, __SIMD);
extern __SIMDd _SIMD_madd_pd(__SIMDd, __SIMDd, __SIMDd);
extern __SIMDi _SIMD_madd_epi32(__SIMDi, __SIMDi, __SIMDi);

extern __SIMD  _SIMD_msub_ps(__SIMD, __SIMD, __SIMD);
extern __SIMDd _SIMD_msub_pd(__SIMDd, __SIMDd, __SIMDd);
extern __SIMDi _SIMD_msub_epi32(__SIMDi, __SIMDi, __SIMDi);

extern __SIMD  _SIMD_splats(float);

#ifdef __cplusplus
}
#endif
#endif  // LIB_SIMD_H
