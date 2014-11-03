/*
  Test vectorization for multi-dimensional array
*/
#include "rose_simd.h" 

int main()
{
  int i_nom_1_strip_13;
  int i_nom_1;
  float a[64][32][18];
  float b[32][16];
  float c[16];
  __SIMD __SIMDtmp0__;
  __SIMD __constant1__ = _SIMD_splats_ps(5.f);
  __SIMD __SIMDtmp2__;
  __SIMD __constant3__ = _SIMD_splats_ps(2.f);
  __SIMD __SIMDtmp4__;
  __SIMD __constant5__ = _SIMD_splats_ps(4.f);
  __SIMD __SIMDtmp6__;
  for (int k = 0; k < 64; k++) {
    for (int j = 0; j < 32; j++) {
      
#pragma SIMD
      for (i_nom_1 = 0, i_nom_1_strip_13 = i_nom_1; i_nom_1 <= 15; (i_nom_1 += 4 , i_nom_1_strip_13 += 1)) {
        __SIMDtmp4__ = _SIMD_load(&b[j][i_nom_1 + 1]);
        __SIMDtmp0__ = __constant1__;
        __SIMDtmp2__ = __constant3__;
        __SIMDtmp6__ = _SIMD_add_ps(__SIMDtmp4__,__constant5__);
        _SIMD_st(&a[k][j][i_nom_1 - 1],__SIMDtmp0__);
        _SIMD_st(&b[j][i_nom_1 - 1],__SIMDtmp2__);
        _SIMD_st(&c[i_nom_1 + 1],__SIMDtmp6__);
      }
      for (int i = 4 * (16 / 4); i < 16; i++) {
        a[k][j][i - 1] = 5.f;
        b[j][i - 1] = 2.f;
        c[i + 1] = b[j][i + 1] + 4.f;
      }
    }
  }
}
