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
    for (c0 = 0; c0 <= 15; c0++) {
      for (c1 = 0; c1 <= 15; c1++) {
        for (c4 = 0; c4 <= 15; c4++) {
          clf[c0][c1] = Ex[c0][c1][c4] - Ex[c0][c1 + 1][c4] + Ey[c0][c1][c4 + 1] - Ey[c0][c1][c4];
          tmp[c0][c1] = cymh[c1] / cyph[c1] * Bza[c0][c1][c4] - ch / cyph[c1] * clf[c0][c1];
          Hz[c0][c1][c4] = cxmh[c4] / cxph[c4] * Hz[c0][c1][c4] + mui * czp[c0] / cxph[c4] * tmp[c0][c1] - mui * czm[c0] / cxph[c4] * Bza[c0][c1][c4];
          Bza[c0][c1][c4] = tmp[c0][c1];
        }
        clf[c0][c1] = Ex[c0][c1][16] - Ex[c0][c1 + 1][16] + Ry[c0][c1] - Ey[c0][c1][16];
        tmp[c0][c1] = cymh[c1] / cyph[c1] * Bza[c0][c1][16] - ch / cyph[c1] * clf[c0][c1];
        Hz[c0][c1][16] = cxmh[16] / cxph[16] * Hz[c0][c1][16] + mui * czp[c0] / cxph[16] * tmp[c0][c1] - mui * czm[c0] / cxph[16] * Bza[c0][c1][16];
        Bza[c0][c1][16] = tmp[c0][c1];
        for (c4 = 0; c4 <= 15; c4++) {
          clf[c0][c1] = Ex[c0][16][c4] - Ax[c0][c4] + Ey[c0][16][c4 + 1] - Ey[c0][16][c4];
          tmp[c0][c1] = cymh[16] / cyph[c1] * Bza[c0][c1][c4] - ch / cyph[c1] * clf[c0][c1];
          Hz[c0][16][c4] = cxmh[c4] / cxph[c4] * Hz[c0][16][c4] + mui * czp[c0] / cxph[c4] * tmp[c0][c1] - mui * czm[c0] / cxph[c4] * Bza[c0][16][c4];
          Bza[c0][16][c4] = tmp[c0][c1];
        }
        clf[c0][c1] = Ex[c0][16][16] - Ax[c0][16] + Ry[c0][16] - Ey[c0][16][16];
        tmp[c0][c1] = cymh[16] / cyph[16] * Bza[c0][16][16] - ch / cyph[16] * clf[c0][c1];
        Hz[c0][16][16] = cxmh[16] / cxph[16] * Hz[c0][16][16] + mui * czp[c0] / cxph[16] * tmp[c0][c1] - mui * czm[c0] / cxph[16] * Bza[c0][16][16];
        Bza[c0][16][16] = tmp[c0][c1];
      }
    }
  }
  
#pragma endscop
  return 0;
}
