/*
  A common layer for different SIMD intrinsics.
*/
#ifndef LIB_SIMD_H 
#define LIB_SIMD_H

#define USE_SSE 1

/*
The suffix implies the data type.
By default, the data type is float.
__SIMDi is for the integer.
__SIMDd is for the double.
*/

#ifdef  USE_SSE
// By default we support SSE2, and turn on the -msse2 GCC compiler flag 
#include <emmintrin.h>
#include "sse_mathfun.h"
typedef  __m128   __SIMD; 
typedef  __m128i  __SIMDi; 
typedef  __m128d  __SIMDd; 

#elif defined USE_AVX
#include <avxintrin.h>
#include "avx_mathfun.h"
typedef  __m256   __SIMD; 
typedef  __m256i  __SIMDi; 
typedef  __m256d  __SIMDd; 

#elif defined USE_IBM
typedef  vector float   __SIMD; 
typedef  vector int __SIMDi; 
typedef  vector double  __SIMDd; 

#endif

/*
The suffix name of each function is decided by the data type of operands.

_ps means "packed single-precision"
_pd means "packed double-precision"
_epi32 is for "packed integer" 
*/

#ifdef __cplusplus
extern "C" {
#endif
// Addition:  a = b + c  ==> a = _SIMD_add_ps(b,c)
__SIMD  _SIMD_add_ps(__SIMD, __SIMD);
__SIMDd _SIMD_add_pd(__SIMDd, __SIMDd);
__SIMDi _SIMD_add_epi32(__SIMDi, __SIMDi);

// subtraction:  a = b - c  ==> a = _SIMD_sub_ps(b,c)
__SIMD  _SIMD_sub_ps(__SIMD, __SIMD);
__SIMDd _SIMD_sub_pd(__SIMDd, __SIMDd);
__SIMDi _SIMD_sub_epi32(__SIMDi, __SIMDi);

// multiplication:  a = b * c  ==> a = _SIMD_mul_ps(b,c)
__SIMD  _SIMD_mul_ps(__SIMD, __SIMD);
__SIMDd _SIMD_mul_pd(__SIMDd, __SIMDd);
__SIMDi _SIMD_mul_epi32(__SIMDi, __SIMDi);

/* 
  ivision:  a = b / c  ==> a = _SIMD_div_ps(b,c)
  Integer is not supported for division.
*/
__SIMD  _SIMD_div_ps(__SIMD, __SIMD);
__SIMDd _SIMD_div_pd(__SIMDd, __SIMDd);

// multiply-add:  a = b * c + d  ==> a = _SIMD_madd_ps(b,c,d)
__SIMD  _SIMD_madd_ps(__SIMD, __SIMD, __SIMD);
__SIMDd _SIMD_madd_pd(__SIMDd, __SIMDd, __SIMDd);
__SIMDi _SIMD_madd_epi32(__SIMDi, __SIMDi, __SIMDi);

// multiply-sub:  a = b * c - d  ==> a = _SIMD_msub_ps(b,c,d)
__SIMD  _SIMD_msub_ps(__SIMD, __SIMD, __SIMD);
__SIMDd _SIMD_msub_pd(__SIMDd, __SIMDd, __SIMDd);
__SIMDi _SIMD_msub_epi32(__SIMDi, __SIMDi, __SIMDi);

// Sets all values packed in the SIMD operand to the same value.
__SIMD  _SIMD_splats_ps(float);
__SIMDd  _SIMD_splats_pd(double);
__SIMDi  _SIMD_splats_epi32(int);

// and:  a = b & c  ==> a = _SIMD_and_ps(b,c)
__SIMD  _SIMD_and_ps(__SIMD, __SIMD);
__SIMDd _SIMD_and_pd(__SIMDd, __SIMDd);

// and:  a = b | c  ==> a = _SIMD_or_ps(b,c)
__SIMD  _SIMD_or_ps(__SIMD, __SIMD);
__SIMDd _SIMD_or_pd(__SIMDd, __SIMDd);

// and:  a = b ^ c  ==> a = _SIMD_xor_ps(b,c)
__SIMD  _SIMD_xor_ps(__SIMD, __SIMD);
__SIMDd _SIMD_xor_pd(__SIMDd, __SIMDd);

// neg:  a = -a  ==> a = _SIMD_neg_ps(a)
__SIMD  _SIMD_neg_ps(__SIMD);
__SIMDd _SIMD_neg_pd(__SIMDd);

// SIMD select
__SIMD  _SIMD_sel_ps(__SIMD, __SIMD, void**);
__SIMDd  _SIMD_sel_pd(__SIMDd, __SIMDd, void**);

// SIMD cmpeq
void  _SIMD_cmpeq_ps(__SIMD, __SIMD, void**);
void  _SIMD_cmpeq_pd(__SIMDd, __SIMDd, void**);

// SIMD cmpne
void  _SIMD_cmpne_ps(__SIMD, __SIMD, void**);
void  _SIMD_cmpne_pd(__SIMDd, __SIMDd, void**);

// SIMD cmplt
void  _SIMD_cmplt_ps(__SIMD, __SIMD, void**);
void  _SIMD_cmplt_pd(__SIMDd, __SIMDd, void**);

// SIMD cmple
void  _SIMD_cmple_ps(__SIMD, __SIMD, void**);
void  _SIMD_cmple_pd(__SIMDd, __SIMDd, void**);

// SIMD cmpgt
void  _SIMD_cmpgt_ps(__SIMD, __SIMD, void**);
void  _SIMD_cmpgt_pd(__SIMDd, __SIMDd, void**);

// SIMD cmpge
void  _SIMD_cmpge_ps(__SIMD, __SIMD, void**);
void  _SIMD_cmpge_pd(__SIMDd, __SIMDd, void**);

// SIMD extract
float   _SIMD_extract_ps(__SIMD, int);
double  _SIMD_extract_pd(__SIMDd, int);
int _SIMD_extract_epi32(__SIMDi, int);

// SIMD sqrt
__SIMD   _SIMD_sqrt_ps(__SIMD);
__SIMDd  _SIMD_sqrt_pd(__SIMDd);

// SIMD abs
__SIMD  _SIMD_abs_ps(__SIMD);
__SIMDd _SIMD_abs_pd(__SIMDd);
__SIMDi _SIMD_abs_epi32(__SIMDi);

// SIMD max
__SIMD  _SIMD_max_ps(__SIMD, __SIMD);
__SIMDd _SIMD_max_pd(__SIMDd, __SIMDd);
__SIMDi _SIMD_max_epi32(__SIMDi, __SIMDi);

// SIMD max
__SIMD  _SIMD_min_ps(__SIMD, __SIMD);
__SIMDd _SIMD_min_pd(__SIMDd, __SIMDd);
__SIMDi _SIMD_min_epi32(__SIMDi, __SIMDi);

__SIMD   _SIMD_sin_ps(__SIMD);
__SIMD   _SIMD_cos_ps(__SIMD);
__SIMD   _SIMD_log_ps(__SIMD);
__SIMD   _SIMD_exp_ps(__SIMD);

#ifdef __cplusplus
}
#endif
#endif  // LIB_SIMD_H
