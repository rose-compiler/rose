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
    int c5;
    int c3;
    int c1;
    for (c1 = 0; c1 <= 15; c1++) {
      for (c3 = 0; c3 <= 15; c3++) {
        for (c5 = 0; c5 <= 15; c5++) {
          clf[c1][c3] = Ex[c1][c3][c5] - Ex[c1][c3 + 1][c5] + Ey[c1][c3][c5 + 1] - Ey[c1][c3][c5];
          tmp[c1][c3] = cymh[c3] / cyph[c3] * Bza[c1][c3][c5] - ch / cyph[c3] * clf[c1][c3];
          Hz[c1][c3][c5] = cxmh[c5] / cxph[c5] * Hz[c1][c3][c5] + mui * czp[c1] / cxph[c5] * tmp[c1][c3] - mui * czm[c1] / cxph[c5] * Bza[c1][c3][c5];
          Bza[c1][c3][c5] = tmp[c1][c3];
        }
        clf[c1][c3] = Ex[c1][c3][16] - Ex[c1][c3 + 1][16] + Ry[c1][c3] - Ey[c1][c3][16];
        tmp[c1][c3] = cymh[c3] / cyph[c3] * Bza[c1][c3][16] - ch / cyph[c3] * clf[c1][c3];
        Hz[c1][c3][16] = cxmh[16] / cxph[16] * Hz[c1][c3][16] + mui * czp[c1] / cxph[16] * tmp[c1][c3] - mui * czm[c1] / cxph[16] * Bza[c1][c3][16];
        Bza[c1][c3][16] = tmp[c1][c3];
        for (c5 = 0; c5 <= 15; c5++) {
          clf[c1][c3] = Ex[c1][16][c5] - Ax[c1][c5] + Ey[c1][16][c5 + 1] - Ey[c1][16][c5];
          tmp[c1][c3] = cymh[16] / cyph[c3] * Bza[c1][c3][c5] - ch / cyph[c3] * clf[c1][c3];
          Hz[c1][16][c5] = cxmh[c5] / cxph[c5] * Hz[c1][16][c5] + mui * czp[c1] / cxph[c5] * tmp[c1][c3] - mui * czm[c1] / cxph[c5] * Bza[c1][16][c5];
          Bza[c1][16][c5] = tmp[c1][c3];
        }
        clf[c1][c3] = Ex[c1][16][16] - Ax[c1][16] + Ry[c1][16] - Ey[c1][16][16];
        tmp[c1][c3] = cymh[16] / cyph[16] * Bza[c1][16][16] - ch / cyph[16] * clf[c1][c3];
        Hz[c1][16][16] = cxmh[16] / cxph[16] * Hz[c1][16][16] + mui * czp[c1] / cxph[16] * tmp[c1][c3] - mui * czm[c1] / cxph[16] * Bza[c1][16][16];
        Bza[c1][16][16] = tmp[c1][c3];
      }
    }
  }
  
#pragma endscop
  return 0;
}
