/*
  Test vectorization for multi-dimensional array
*/
#include "rose_simd.h" 

int main()
{
  int i_nom_1_strip_13;
  int i_nom_1;
  float a[64UL][32UL][16UL];
  __SIMD (*a_SIMD)[32UL][16UL / 4] = (__SIMD (*)[32UL][16UL / 4])a;
  float b[32UL][16UL];
  __SIMD (*b_SIMD)[16UL / 4] = (__SIMD (*)[16UL / 4])b;
  float c[16UL];
  __SIMD *c_SIMD = (__SIMD *)c;
  for (int k = 0; k < 64; k++) {
    for (int j = 0; j < 32; j++) {
      
#pragma SIMD
      for (i_nom_1 = 0, i_nom_1_strip_13 = i_nom_1; i_nom_1 <= 15; (i_nom_1 += 4 , i_nom_1_strip_13 += 1)) {
        a_SIMD[k][j][i_nom_1_strip_13] = _SIMD_splats_ps(5.f);
        b_SIMD[j][i_nom_1_strip_13] = _SIMD_splats_ps(2.f);
        c_SIMD[i_nom_1_strip_13] = _SIMD_splats_ps(4.f);
      }
    }
  }
  return 0;
}
