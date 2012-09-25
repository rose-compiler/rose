/*
  Test vectorization for FMA instructions.
  a = b * c + d ==> a = _SIMD_madd_ps(b,c,d);
*/
#include "rose_simd.h" 

int main()
{
  int i_nom_1_strip_12;
  int i_nom_1;
  float a[16UL];
  __SIMD *a_SIMD;
  float b[16UL];
  __SIMD *b_SIMD;
  float c[16UL];
  __SIMD *c_SIMD;
  int n = 16;
  __SIMDi n_SIMD;
  float as;
  __SIMD as_SIMD;
  float bs;
  __SIMD bs_SIMD;
  float cs;
  __SIMD cs_SIMD;
  
#pragma SIMD
  c_SIMD = ((__SIMD *)c);
  b_SIMD = ((__SIMD *)b);
  a_SIMD = ((__SIMD *)a);
  n_SIMD = _SIMD_splats_epi32(n);
  as_SIMD = _SIMD_splats_ps(as);
  bs_SIMD = _SIMD_splats_ps(bs);
  for (i_nom_1 = 0, i_nom_1_strip_12 = i_nom_1; i_nom_1 <= n - 1; (i_nom_1 += 4 , i_nom_1_strip_12 += 1)) {
    a_SIMD[i_nom_1_strip_12] = _SIMD_madd_ps(a_SIMD[i_nom_1_strip_12],b_SIMD[i_nom_1_strip_12],c_SIMD[i_nom_1_strip_12]);
    a_SIMD[i_nom_1_strip_12] = _SIMD_msub_ps(a_SIMD[i_nom_1_strip_12],b_SIMD[i_nom_1_strip_12],c_SIMD[i_nom_1_strip_12]);
    a_SIMD[i_nom_1_strip_12] = _SIMD_madd_ps(a_SIMD[i_nom_1_strip_12],b_SIMD[i_nom_1_strip_12],c_SIMD[i_nom_1_strip_12]);
    a_SIMD[i_nom_1_strip_12] = _SIMD_neg_ps(_SIMD_msub_ps(a_SIMD[i_nom_1_strip_12],b_SIMD[i_nom_1_strip_12],c_SIMD[i_nom_1_strip_12]));
    cs_SIMD = (_SIMD_madd_pd(as_SIMD,bs_SIMD,_SIMD_splats_pd(1.)));
  }
  return 0;
}
