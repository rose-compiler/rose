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

__SIMDi _SIMD_splats_epi32(int32_t i)
{
#ifdef  USE_SSE
  return _mm_set1_epi32(i);
#elif defined USE_AVX
  return _m256_set1_epi32(i);
#elif defined USE_IBM
  return vec_splats(i);
#endif
}

/*
  V_SgAndOp
  int32_teger is not supported. 
*/
__SIMD _SIMD_and_ps(__SIMD a, __SIMD b)
{
#ifdef  USE_SSE
  return _mm_and_ps(a,b);
#elif defined USE_AVX
  return _m256_and_ps(a,b);
#elif defined USE_IBM
  return vec_and(a,b);
#endif
}

__SIMDd _SIMD_and_pd(__SIMDd a, __SIMDd b)
{
#ifdef  USE_SSE
  return _mm_and_pd(a,b);
#elif defined USE_AVX
  return _m256_and_ps(a,b);
#elif defined USE_IBM
  return vec_and(a,b);
#endif
}

/*
  V_SgBitOrOp
  int32_teger is not supported. 
*/
__SIMD _SIMD_or_ps(__SIMD a, __SIMD b)
{
#ifdef  USE_SSE
  return _mm_or_ps(a,b);
#elif defined USE_AVX
  return _m256_or_ps(a,b);
#elif defined USE_IBM
  return vec_or(a,b);
#endif
}

__SIMDd _SIMD_or_pd(__SIMDd a, __SIMDd b)
{
#ifdef  USE_SSE
  return _mm_or_pd(a,b);
#elif defined USE_AVX
  return _m256_or_ps(a,b);
#elif defined USE_IBM
  return vec_or(a,b);
#endif
}

/*
  V_SgBitXorOp
  integer is not supported. 
 
  todo: need to verify the support for double-precision
*/
__SIMD _SIMD_xor_ps(__SIMD a, __SIMD b)
{
#ifdef  USE_SSE
  return _mm_xor_ps(a,b);
#elif defined USE_AVX
  return _m256_xor_ps(a,b);
#elif defined USE_IBM
  return vec_xor(a,b);
#endif
}

__SIMDd _SIMD_xor_pd(__SIMDd a, __SIMDd b)
{
#ifdef  USE_SSE
  return _mm_xor_pd(a,b);
#elif defined USE_AVX
  return _m256_xor_ps(a,b);
#elif defined USE_IBM
  return vec_xor(a,b);
#endif
}

/*
  V_SgMinusOp
*/
__SIMD _SIMD_neg_ps(__SIMD a)
{
#ifdef  USE_SSE
  return _mm_xor_ps(a, _mm_set1_ps(-0.0f));
#elif defined USE_AVX
  return _mm256_xor_ps(a, _mm_set1_ps(-0.0f));
#elif defined USE_IBM
  return vec_neg(a);
#endif
}

__SIMDd _SIMD_neg_pd(__SIMDd a)
{
#ifdef  USE_SSE
  return _mm_xor_pd(a, _mm_set1_pd(-0.0f));
#elif defined USE_AVX
  return _mm256_xor_pd(a, _mm_set1_pd(-0.0f));
#elif defined USE_IBM
  return vec_neg(a);
#endif
}

/*
  SIMD selection
*/
__SIMD _SIMD_sel_ps(__SIMD a, __SIMD b, void** resultPtr)
{
#ifdef  USE_SSE
  __SIMD* result = (__SIMD*) (*resultPtr);
  return _mm_or_ps(_mm_andnot_ps(*result,a),_mm_and_ps(*result,b));
#elif defined USE_AVX
  __SIMD* result = (__SIMD*) resultPtr;
  return _mm256_or_ps(_mm256_andnot_ps(*result,a),_mm256_and_ps(*result,b));
#elif defined USE_IBM
  return vec_sel(a,b,c);
#endif
}

__SIMDd _SIMD_sel_pd(__SIMDd a, __SIMDd b, void** resultPtr)
{
#ifdef  USE_SSE
  __SIMDd* result = (__SIMDd*) (*resultPtr);
  return _mm_or_pd(_mm_andnot_pd(*result,a),_mm_and_pd(*result,b));
#elif defined USE_AVX
  __SIMDd* result = (__SIMDd*) resultPtr;
  return _mm256_or_pd(_mm256_andnot_pd(*result,a),_mm256_and_pd(*result,b));
#elif defined USE_IBM
  return vec_sel(a,b,c);
#endif
}

/*
  SIMD comparison: void _SIMD_cmp_eq(a,b,c)
  a and b are the arguements for comparison.  c will be the return result.
*/

// a == b
void _SIMD_cmpeq_ps(__SIMD a, __SIMD b, void** resultPtr)
{
  __SIMD* result = (__SIMD*)malloc(sizeof(__SIMD));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmpeq_ps(a,b);
#elif defined USE_AVX
  *result = _mm256_cmp_ps(a,b,0);
#elif defined USE_IBM
  *result = vec_cmpeq(a,b);
#endif
}

void _SIMD_cmpeq_pd(__SIMDd a, __SIMDd b, void** resultPtr)
{
  __SIMDd* result = (__SIMDd*)malloc(sizeof(__SIMDd));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmpeq_pd(a,b);
#elif defined USE_AVX
  *result = _mm256_cmp(a,b,0);
#elif defined USE_IBM
  *result = vec_cmpeq(a,b);
#endif
}

// a != b
void _SIMD_cmpne_ps(__SIMD a, __SIMD b, void** resultPtr)
{
  __SIMD* result = (__SIMD*)malloc(sizeof(__SIMD));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmpneq_ps(a,b);
#elif defined USE_AVX
  *result = _mm256_cmp_ps(a,b,4);
#elif defined USE_IBM
  *result = vec_xor(vec_cmpeq(a,b));
#endif
}

void _SIMD_cmpne_pd(__SIMDd a, __SIMDd b, void** resultPtr)
{
  __SIMDd* result = (__SIMDd*)malloc(sizeof(__SIMDd));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmpneq_pd(a,b);
#elif defined USE_AVX
  *result = _mm256_cmpneq_pd(a,b,4);
#elif defined USE_IBM
  *result = vec_xor(vec_cmpeq(a,b));
#endif
}

// a < b
void _SIMD_cmplt_ps(__SIMD a, __SIMD b, void** resultPtr)
{
  __SIMD* result = (__SIMD*)malloc(sizeof(__SIMD));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmplt_ps(a,b);
#elif defined USE_AVX
  *result = _mm256_cmp_ps(a,b,17);
#elif defined USE_IBM
  *result = vec_cmplt(a,b);
#endif
}

void _SIMD_cmplt_pd(__SIMDd a, __SIMDd b, void** resultPtr)
{
  __SIMDd* result = (__SIMDd*)malloc(sizeof(__SIMDd));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmplt_pd(a,b);
#elif defined USE_AVX
  *result = _mm256_cmp(a,b,17);
#elif defined USE_IBM
  *result = vec_cmplt(a,b);
#endif
}

// a <= b
void _SIMD_cmple_ps(__SIMD a, __SIMD b, void** resultPtr)
{
  __SIMD* result = (__SIMD*)malloc(sizeof(__SIMD));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmple_ps(a,b);
#elif defined USE_AVX
  *result = _mm256_cmp_ps(a,b,18);
#elif defined USE_IBM
  *result = vec_cmple(a,b);
#endif
}

void _SIMD_cmple_pd(__SIMDd a, __SIMDd b, void** resultPtr)
{
  __SIMDd* result = (__SIMDd*)malloc(sizeof(__SIMDd));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmple_pd(a,b);
#elif defined USE_AVX
  *result = _mm256_cmp(a,b,18);
#elif defined USE_IBM
  *result = vec_cmple(a,b);
#endif
}

// a > b
void _SIMD_cmpgt_ps(__SIMD a, __SIMD b, void** resultPtr)
{
  __SIMD* result = (__SIMD*)malloc(sizeof(__SIMD));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmpgt_ps(a,b);
#elif defined USE_AVX
  *result = _mm256_cmp_ps(a,b,30);
#elif defined USE_IBM
  *result = vec_cmpgt(a,b);
#endif
}

void _SIMD_cmpgt_pd(__SIMDd a, __SIMDd b, void** resultPtr)
{
  __SIMDd* result = (__SIMDd*)malloc(sizeof(__SIMDd));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmple_pd(a,b);
#elif defined USE_AVX
  *result = _mm256_cmp(a,b,30);
#elif defined USE_IBM
  *result = vec_cmpgt(a,b);
#endif
}

// a >= b
void _SIMD_cmpge_ps(__SIMD a, __SIMD b, void** resultPtr)
{
  __SIMD* result = (__SIMD*)malloc(sizeof(__SIMD));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmpge_ps(a,b);
#elif defined USE_AVX
  *result = _mm256_cmp_ps(a,b,29);
#elif defined USE_IBM
  *result = vec_cmpge(a,b);
#endif
}

void _SIMD_cmpge_pd(__SIMDd a, __SIMDd b, void** resultPtr)
{
  __SIMDd* result = (__SIMDd*)malloc(sizeof(__SIMDd));
  *resultPtr = result;
#ifdef  USE_SSE
  *result = _mm_cmpge_pd(a,b);
#elif defined USE_AVX
  *result = _mm256_cmp(a,b,29);
#elif defined USE_IBM
  *result = vec_cmpge(a,b);
#endif
}

// extract scalar from SIMD operand
float _SIMD_extract_ps(__SIMD a, int32_t i)
{
#if defined USE_IBM
  return vec_extract(a,i);
#else
  return *(((float*)&a)+i);
#endif
}

double _SIMD_extract_pd(__SIMDd a, int32_t i)
{
#if defined USE_IBM
  return vec_extract(a,i);
#else
  return *(((double*)&a)+i);
#endif
}

int32_t _SIMD_extract_epi32(__SIMDi a, int32_t i)
{
#if defined USE_IBM
  return vec_extract(a,i);
#else
  return *(((int32_t*)&a)+i);
#endif
}
