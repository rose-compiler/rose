/*
  Test vectorization for simple arithmetic statements using integer operands.
*/
#include "rose_simd.h" 

int main()
{
  int i_nom_1_strip_10;
  int i_nom_1;
  int a[16UL];
  __SIMDi *a_SIMD = (__SIMDi *)a;
  int b[16UL];
  __SIMDi *b_SIMD = (__SIMDi *)b;
  int c[16UL];
  __SIMDi *c_SIMD = (__SIMDi *)c;
  int n = 16;
  __SIMDi n_SIMD;
  
#pragma SIMD
  n_SIMD = _SIMD_splats_epi32(n);
  for (i_nom_1 = 0, i_nom_1_strip_10 = i_nom_1; i_nom_1 <= n - 1; (i_nom_1 += 4 , i_nom_1_strip_10 += 1)) {
    c_SIMD[i_nom_1_strip_10] = _SIMD_add_epi32(a_SIMD[i_nom_1_strip_10],b_SIMD[i_nom_1_strip_10]);
    c_SIMD[i_nom_1_strip_10] = _SIMD_sub_epi32(a_SIMD[i_nom_1_strip_10],b_SIMD[i_nom_1_strip_10]);
    c_SIMD[i_nom_1_strip_10] = _SIMD_mul_epi32(a_SIMD[i_nom_1_strip_10],b_SIMD[i_nom_1_strip_10]);
  }
  return 0;
}
