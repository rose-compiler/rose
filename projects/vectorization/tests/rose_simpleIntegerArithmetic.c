/*
  Test vectorization for simple arithmetic statements using integer operands.
*/
#include "rose_simd.h" 

int main()
{
  int i_nom_1_strip_9;
  int i_nom_1;
  int a[16UL];
  __SIMDi *a_SIMD;
  int b[16UL];
  __SIMDi *b_SIMD;
  int c[16UL];
  __SIMDi *c_SIMD;
  int n = 16;
  b_SIMD = ((__SIMDi *)b);
  a_SIMD = ((__SIMDi *)a);
  c_SIMD = ((__SIMDi *)c);
  for (i_nom_1 = 0, i_nom_1_strip_9 = i_nom_1; i_nom_1 <= n - 1; (i_nom_1 += 4 , i_nom_1_strip_9 += 1)) {
    c_SIMD[i_nom_1_strip_9] = _SIMD_add_epi32(a_SIMD[i_nom_1_strip_9],b_SIMD[i_nom_1_strip_9]);
    c_SIMD[i_nom_1_strip_9] = _SIMD_sub_epi32(a_SIMD[i_nom_1_strip_9],b_SIMD[i_nom_1_strip_9]);
    c_SIMD[i_nom_1_strip_9] = _SIMD_mul_epi32(a_SIMD[i_nom_1_strip_9],b_SIMD[i_nom_1_strip_9]);
  }
  return 0;
}
