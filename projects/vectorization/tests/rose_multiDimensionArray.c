/*
  Test vectorization for multi-dimensional array
*/
#include "rose_simd.h" 

int main()
{
  int k_nom_3;
  int j_nom_2;
  int i_nom_1_strip_12;
  int i_nom_1;
  float a[64UL][32UL][16UL];
  __SIMD (*a_SIMD)[32UL][16UL / 4];
  float b[32UL][16UL];
  __SIMD (*b_SIMD)[16UL / 4];
  float c[16UL];
  __SIMD *c_SIMD;
  c_SIMD = ((__SIMD *)c);
  b_SIMD = ((__SIMD (*)[16UL / 4])b);
  a_SIMD = ((__SIMD (*)[32UL][16UL / 4])a);
  for (k_nom_3 = 0; k_nom_3 <= 63; k_nom_3 += 1) {
    for (j_nom_2 = 0; j_nom_2 <= 31; j_nom_2 += 1) {
      for (i_nom_1 = 0, i_nom_1_strip_12 = i_nom_1; i_nom_1 <= 15; (i_nom_1 += 4 , i_nom_1_strip_12 += 1)) {
        a_SIMD[k_nom_3][j_nom_2][i_nom_1_strip_12] = _SIMD_splats_ps(5.f);
        b_SIMD[j_nom_2][i_nom_1_strip_12] = _SIMD_splats_ps(2.f);
        c_SIMD[i_nom_1_strip_12] = _SIMD_splats_ps(4.f);
      }
    }
  }
  return 0;
}
