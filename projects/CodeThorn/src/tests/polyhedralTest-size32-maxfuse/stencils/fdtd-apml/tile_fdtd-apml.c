/**
 * fdtd-apml.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#   define CZ ARRAYSIZE
#   define CYM ARRAYSIZE
#   define CXM ARRAYSIZE
# define _PB_CZ ARRAYSIZE
# define _PB_CYM ARRAYSIZE
# define _PB_CXM ARRAYSIZE

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int cz = 16;
  int cym = 16;
  int cxm = 16;
/* Variable declaration/allocation. */
  double mui;
  double ch;
  double Ax[16 + 1][16 + 1];
  double Ry[16 + 1][16 + 1];
  double clf[16 + 1][16 + 1];
  double tmp[16 + 1][16 + 1];
  double Bza[16 + 1][16 + 1][16 + 1];
  double Ex[16 + 1][16 + 1][16 + 1];
  double Ey[16 + 1][16 + 1][16 + 1];
  double Hz[16 + 1][16 + 1][16 + 1];
  double czm[16 + 1];
  double czp[16 + 1];
  double cxmh[16 + 1];
  double cxph[16 + 1];
  double cymh[16 + 1];
  double cyph[16 + 1];
  int iz;
  int iy;
  int ix;
  
#pragma scop
{
    int c0;
    int c1;
    int c4;
{
      int c2;
      int c3;
      int c7;
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c7 = 0; c7 <= 15; c7++) {
            clf[c2][c3] = Ex[c2][c3][c7] - Ex[c2][c3 + 1][c7] + Ey[c2][c3][c7 + 1] - Ey[c2][c3][c7];
            tmp[c2][c3] = cymh[c3] / cyph[c3] * Bza[c2][c3][c7] - ch / cyph[c3] * clf[c2][c3];
            Hz[c2][c3][c7] = cxmh[c7] / cxph[c7] * Hz[c2][c3][c7] + mui * czp[c2] / cxph[c7] * tmp[c2][c3] - mui * czm[c2] / cxph[c7] * Bza[c2][c3][c7];
            Bza[c2][c3][c7] = tmp[c2][c3];
          }
          clf[c2][c3] = Ex[c2][c3][16] - Ex[c2][c3 + 1][16] + Ry[c2][c3] - Ey[c2][c3][16];
          tmp[c2][c3] = cymh[c3] / cyph[c3] * Bza[c2][c3][16] - ch / cyph[c3] * clf[c2][c3];
          Hz[c2][c3][16] = cxmh[16] / cxph[16] * Hz[c2][c3][16] + mui * czp[c2] / cxph[16] * tmp[c2][c3] - mui * czm[c2] / cxph[16] * Bza[c2][c3][16];
          Bza[c2][c3][16] = tmp[c2][c3];
          for (c7 = 0; c7 <= 15; c7++) {
            clf[c2][c3] = Ex[c2][16][c7] - Ax[c2][c7] + Ey[c2][16][c7 + 1] - Ey[c2][16][c7];
            tmp[c2][c3] = cymh[16] / cyph[c3] * Bza[c2][c3][c7] - ch / cyph[c3] * clf[c2][c3];
            Hz[c2][16][c7] = cxmh[c7] / cxph[c7] * Hz[c2][16][c7] + mui * czp[c2] / cxph[c7] * tmp[c2][c3] - mui * czm[c2] / cxph[c7] * Bza[c2][16][c7];
            Bza[c2][16][c7] = tmp[c2][c3];
          }
          clf[c2][c3] = Ex[c2][16][16] - Ax[c2][16] + Ry[c2][16] - Ey[c2][16][16];
          tmp[c2][c3] = cymh[16] / cyph[16] * Bza[c2][16][16] - ch / cyph[16] * clf[c2][c3];
          Hz[c2][16][16] = cxmh[16] / cxph[16] * Hz[c2][16][16] + mui * czp[c2] / cxph[16] * tmp[c2][c3] - mui * czm[c2] / cxph[16] * Bza[c2][16][16];
          Bza[c2][16][16] = tmp[c2][c3];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
