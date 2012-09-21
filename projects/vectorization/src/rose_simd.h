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
typedef  vector int32_t __SIMDi; 
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
extern __SIMD  _SIMD_add_ps(__SIMD, __SIMD);
extern __SIMDd _SIMD_add_pd(__SIMDd, __SIMDd);
extern __SIMDi _SIMD_add_epi32(__SIMDi, __SIMDi);

// subtraction:  a = b - c  ==> a = _SIMD_sub_ps(b,c)
extern __SIMD  _SIMD_sub_ps(__SIMD, __SIMD);
extern __SIMDd _SIMD_sub_pd(__SIMDd, __SIMDd);
extern __SIMDi _SIMD_sub_epi32(__SIMDi, __SIMDi);

// multiplication:  a = b * c  ==> a = _SIMD_mul_ps(b,c)
extern __SIMD  _SIMD_mul_ps(__SIMD, __SIMD);
extern __SIMDd _SIMD_mul_pd(__SIMDd, __SIMDd);
extern __SIMDi _SIMD_mul_epi32(__SIMDi, __SIMDi);

/* 
  ivision:  a = b / c  ==> a = _SIMD_div_ps(b,c)
  Integer is not supported for division.
*/
extern __SIMD  _SIMD_div_ps(__SIMD, __SIMD);
extern __SIMDd _SIMD_div_pd(__SIMDd, __SIMDd);

// multiply-add:  a = b * c + d  ==> a = _SIMD_madd_ps(b,c,d)
extern __SIMD  _SIMD_madd_ps(__SIMD, __SIMD, __SIMD);
extern __SIMDd _SIMD_madd_pd(__SIMDd, __SIMDd, __SIMDd);
extern __SIMDi _SIMD_madd_epi32(__SIMDi, __SIMDi, __SIMDi);

// multiply-sub:  a = b * c - d  ==> a = _SIMD_msub_ps(b,c,d)
extern __SIMD  _SIMD_msub_ps(__SIMD, __SIMD, __SIMD);
extern __SIMDd _SIMD_msub_pd(__SIMDd, __SIMDd, __SIMDd);
extern __SIMDi _SIMD_msub_epi32(__SIMDi, __SIMDi, __SIMDi);

// Sets all values packed in the SIMD operand to the same value.
extern __SIMD  _SIMD_splats_ps(float);
extern __SIMDd  _SIMD_splats_pd(double);
extern __SIMDi  _SIMD_splats_epi32(int32_t);

// and:  a = b & c  ==> a = _SIMD_and_ps(b,c)
extern __SIMD  _SIMD_and_ps(__SIMD, __SIMD);
extern __SIMDd _SIMD_and_pd(__SIMDd, __SIMDd);

// and:  a = b | c  ==> a = _SIMD_or_ps(b,c)
extern __SIMD  _SIMD_or_ps(__SIMD, __SIMD);
extern __SIMDd _SIMD_or_pd(__SIMDd, __SIMDd);

// and:  a = b ^ c  ==> a = _SIMD_xor_ps(b,c)
extern __SIMD  _SIMD_xor_ps(__SIMD, __SIMD);
extern __SIMDd _SIMD_xor_pd(__SIMDd, __SIMDd);

// neg:  a = -a  ==> a = _SIMD_neg_ps(a)
extern __SIMD  _SIMD_neg_ps(__SIMD);
extern __SIMDd _SIMD_neg_pd(__SIMDd);

// SIMD select
extern __SIMD  _SIMD_sel_ps(__SIMD, __SIMD, void**);
extern __SIMDd  _SIMD_sel_pd(__SIMDd, __SIMDd, void**);

// SIMD cmpeq
extern void  _SIMD_cmpeq_ps(__SIMD, __SIMD, void**);
extern void  _SIMD_cmpeq_pd(__SIMDd, __SIMDd, void**);

// SIMD cmpne
extern void  _SIMD_cmpne_ps(__SIMD, __SIMD, void**);
extern void  _SIMD_cmpne_pd(__SIMDd, __SIMDd, void**);

// SIMD cmplt
extern void  _SIMD_cmplt_ps(__SIMD, __SIMD, void**);
extern void  _SIMD_cmplt_pd(__SIMDd, __SIMDd, void**);

// SIMD cmple
extern void  _SIMD_cmple_ps(__SIMD, __SIMD, void**);
extern void  _SIMD_cmple_pd(__SIMDd, __SIMDd, void**);

// SIMD cmpgt
extern void  _SIMD_cmpgt_ps(__SIMD, __SIMD, void**);
extern void  _SIMD_cmpgt_pd(__SIMDd, __SIMDd, void**);

// SIMD cmpge
extern void  _SIMD_cmpge_ps(__SIMD, __SIMD, void**);
extern void  _SIMD_cmpge_pd(__SIMDd, __SIMDd, void**);

// SIMD extract
extern float   _SIMD_extract_ps(__SIMD, int32_t);
extern double  _SIMD_extract_pd(__SIMDd, int32_t);
extern int32_t _SIMD_extract_epi32(__SIMDi, int32_t);

#ifdef __cplusplus
}
#endif
#endif  // LIB_SIMD_H
