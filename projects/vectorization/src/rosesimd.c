/*
  Author: Pei-Hung Lin
  Contact: lin32@llnl.gov, phlin@cs.umn.edu

  Date Created       : July 26th, 2012

  This file provides runtime library functions.  
  The functions will map to the SIMD intrinsic functions used in different compilers. 
*/

//#include "rose_config.h"
#include "rose_simd.h"

#define USE_SSE 1

__SIMD _SIMD_add_ps(__SIMD a, __SIMD b)
{
#ifdef  USE_SSE
  return _mm_add_ps(a,b);
#elif defined USE_AVX
  return _m256_add_ps(a,b);
#elif defined USE_IBM
  return vec_add(a,b);
#endif
}

__SIMDd _SIMD_add_pd(__SIMDd a, __SIMDd b)
{
#ifdef  USE_SSE
  return _mm_add_pd(a,b);
#elif defined USE_AVX
  return _m256_add_ps(a,b);
#elif defined USE_IBM
  return vec_add(a,b);
#endif
}

__SIMDi _SIMD_add_epi32(__SIMDi a, __SIMDi b)
{
#ifdef  USE_SSE
  return _mm_add_epi32(a,b);
#elif defined USE_AVX
  return _m256_add_ps(a,b);
#elif defined USE_IBM
  return vec_add(a,b);
#endif
}

__SIMD _SIMD_sub_ps(__SIMD a, __SIMD b)
{
#ifdef  USE_SSE
  return _mm_sub_ps(a,b);
#elif defined USE_AVX
  return _m256_sub_ps(a,b);
#elif defined USE_IBM
  return vec_sub(a,b);
#endif
}

__SIMDd _SIMD_sub_pd(__SIMDd a, __SIMDd b)
{
#ifdef  USE_SSE
  return _mm_sub_pd(a,b);
#elif defined USE_AVX
  return _m256_sub_ps(a,b);
#elif defined USE_IBM
  return vec_sub(a,b);
#endif
}

__SIMDi _SIMD_sub_epi32(__SIMDi a, __SIMDi b)
{
#ifdef  USE_SSE
  return _mm_sub_epi32(a,b);
#elif defined USE_AVX
  return _m256_sub_ps(a,b);
#elif defined USE_IBM
  return vec_sub(a,b);
#endif
}

__SIMD _SIMD_mul_ps(__SIMD a, __SIMD b)
{
#ifdef  USE_SSE
  return _mm_mul_ps(a,b);
#elif defined USE_AVX
  return _m256_mul_ps(a,b);
#elif defined USE_IBM
  return vec_mul(a,b);
#endif
}

__SIMDd _SIMD_mul_pd(__SIMDd a, __SIMDd b)
{
#ifdef  USE_SSE
  return _mm_mul_pd(a,b);
#elif defined USE_AVX
  return _m256_mul_ps(a,b);
#elif defined USE_IBM
  return vec_mul(a,b);
#endif
}

__SIMDi _SIMD_mul_epi32(__SIMDi a, __SIMDi b)
{
#ifdef  USE_SSE
#ifdef __SSE4_1__  // modern CPU - use SSE 4.1
    return _mm_mullo_epi32(a, b);
#else               // old CPU - use SSE 2
    __m128i tmp1 = _mm_mul_epu32(a,b); /* mul 2,0*/
    __m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(a,4), _mm_srli_si128(b,4)); /* mul 3,1 */
    return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0))); /* shuffle results to [63..0] and pack */
#endif
#elif defined USE_AVX
  return _m256_mul_ps(a,b);
#elif defined USE_IBM
  return vec_mul(a,b);
#endif
}

__SIMD _SIMD_div_ps(__SIMD a, __SIMD b)
{
#ifdef  USE_SSE
  return _mm_div_ps(a,b);
#elif defined USE_AVX
  return _m256_div_ps(a,b);
#elif defined USE_IBM
  return vec_div(a,b);
#endif
}

__SIMDd _SIMD_div_pd(__SIMDd a, __SIMDd b)
{
#ifdef  USE_SSE
  return _mm_div_pd(a,b);
#elif defined USE_AVX
  return _m256_div_ps(a,b);
#elif defined USE_IBM
  return vec_div(a,b);
#endif
}
/*
__SIMDi _SIMD_div_epi32(__SIMDi a, __SIMDi b)
{
#ifdef  USE_SSE
  return _mm_div_epi32(a,b);
#elif defined USE_AVX
  return _m256_div_ps(a,b);
#elif defined USE_IBM
  return vec_div(a,b);
#endif
}
*/
__SIMD _SIMD_madd_ps(__SIMD a, __SIMD b, __SIMD c)
{
#ifdef  USE_SSE
  return _mm_add_ps(_mm_mul_ps(a,b),c);
#elif defined USE_AVX
  return _m256_madd_ps(a,b);
#elif defined USE_IBM
  return vec_madd(a,b);
#endif
}

__SIMDd _SIMD_madd_pd(__SIMDd a, __SIMDd b, __SIMDd c)
{
#ifdef  USE_SSE
  return _mm_add_pd(_mm_mul_pd(a,b),c);
#elif defined USE_AVX
  return _m256_madd_ps(a,b);
#elif defined USE_IBM
  return vec_madd(a,b);
#endif
}

__SIMDi _SIMD_madd_epi32(__SIMDi a, __SIMDi b, __SIMDi c)
{
#ifdef  USE_SSE
  return _SIMD_add_epi32(_SIMD_mul_epi32(a,b),c);
#elif defined USE_AVX
  return _m256_madd_ps(a,b);
#elif defined USE_IBM
  return vec_madd(a,b);
#endif
}

__SIMD _SIMD_msub_ps(__SIMD a, __SIMD b, __SIMD c)
{
#ifdef  USE_SSE
  return _mm_sub_ps(_mm_mul_ps(a,b),c);
#elif defined USE_AVX
  return _m256_msub_ps(a,b);
#elif defined USE_IBM
  return vec_msub(a,b);
#endif
}

__SIMDd _SIMD_msub_pd(__SIMDd a, __SIMDd b, __SIMDd c)
{
#ifdef  USE_SSE
  return _mm_sub_pd(_mm_mul_pd(a,b),c);
#elif defined USE_AVX
  return _m256_msub_ps(a,b);
#elif defined USE_IBM
  return vec_msub(a,b);
#endif
}

__SIMDi _SIMD_msub_epi32(__SIMDi a, __SIMDi b, __SIMDi c)
{
#ifdef  USE_SSE
  return _SIMD_sub_epi32(_SIMD_mul_epi32(a,b),c);
#elif defined USE_AVX
  return _m256_msub_ps(a,b);
#elif defined USE_IBM
  return vec_msub(a,b);
#endif
}

__SIMD _SIMD_splats_ps(float f)
{
#ifdef  USE_SSE
  return _mm_set1_ps(f);
#elif defined USE_AVX
  return _m256_set1_ps(f);
#elif defined USE_IBM
  return vec_splats(f);
#endif
}

__SIMDd _SIMD_splats_pd(double f)
{
#ifdef  USE_SSE
  return _mm_set1_pd(f);
#elif defined USE_AVX
  return _m256_set1_pd(f);
#elif defined USE_IBM
  return vec_splats(f);
#endif
}

__SIMDi _SIMD_splats_epi32(int i)
{
#ifdef  USE_SSE
  return _mm_set1_epi32(i);
#elif defined USE_AVX
  return _m256_set1_epi32(i);
#elif defined USE_IBM
  return vec_splats(i);
#endif
}
