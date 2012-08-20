/*
  Test vectorization for FMA instructions.
  a = b * c + d ==> a = _SIMD_madd_ps(b,c,d);
*/
#include "rose_simd.h" 

int main()
{
  int i_nom_1_strip_11;
  int i_nom_1;
  float a[16UL];
  __SIMD *a_SIMD;
  float b[16UL];
  __SIMD *b_SIMD;
  float c[16UL];
  __SIMD *c_SIMD;
  int n = 16;
  float as;
  __SIMD as_SIMD;
  float bs;
  __SIMD bs_SIMD;
  float cs;
  __SIMD cs_SIMD;
  c_SIMD = ((__SIMD *)c);
  b_SIMD = ((__SIMD *)b);
  a_SIMD = ((__SIMD *)a);
  for (i_nom_1 = 0, i_nom_1_strip_11 = i_nom_1; i_nom_1 <= n - 1; (i_nom_1 += 4 , i_nom_1_strip_11 += 1)) {
    a_SIMD[i_nom_1_strip_11] = _SIMD_madd_ps(a_SIMD[i_nom_1_strip_11],b_SIMD[i_nom_1_strip_11],c_SIMD[i_nom_1_strip_11]);
    a_SIMD[i_nom_1_strip_11] = _SIMD_msub_ps(a_SIMD[i_nom_1_strip_11],b_SIMD[i_nom_1_strip_11],c_SIMD[i_nom_1_strip_11]);
    a_SIMD[i_nom_1_strip_11] = _SIMD_madd_ps(a_SIMD[i_nom_1_strip_11],b_SIMD[i_nom_1_strip_11],c_SIMD[i_nom_1_strip_11]);
    a_SIMD[i_nom_1_strip_11] = -_SIMD_msub_ps(a_SIMD[i_nom_1_strip_11],b_SIMD[i_nom_1_strip_11],c_SIMD[i_nom_1_strip_11]);
    cs_SIMD = _SIMD_madd_ps(as_SIMD,bs_SIMD,_SIMD_splats_ps(1));
  }
  return 0;
}
