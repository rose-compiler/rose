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
    int c6;
    int c5;
    int c3;
    int c2;
    int c1;
#pragma omp parallel for private(c2, c3, c5, c6)
    for (c1 = 0; c1 <= 2; c1++) {
      for (c2 = 0; c2 <= 2; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c5 = 7 * c2; c5 <= ((15 < 7 * c2 + 6?15 : 7 * c2 + 6)); c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c6 = 7 * c1; c6 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c6++) {
              clf[c6][c5] = Ex[c6][c5][c3] - Ex[c6][c5 + 1][c3] + Ey[c6][c5][c3 + 1] - Ey[c6][c5][c3];
              tmp[c6][c5] = cymh[c5] / cyph[c5] * Bza[c6][c5][c3] - ch / cyph[c5] * clf[c6][c5];
              Hz[c6][c5][c3] = cxmh[c3] / cxph[c3] * Hz[c6][c5][c3] + mui * czp[c6] / cxph[c3] * tmp[c6][c5] - mui * czm[c6] / cxph[c3] * Bza[c6][c5][c3];
              Bza[c6][c5][c3] = tmp[c6][c5];
            }
          }
        }
      }
    }
#pragma omp parallel for private(c2, c5, c6)
    for (c1 = 0; c1 <= 2; c1++) {
      for (c2 = 0; c2 <= 2; c2++) {
        for (c5 = 7 * c2; c5 <= ((15 < 7 * c2 + 6?15 : 7 * c2 + 6)); c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c6 = 7 * c1; c6 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c6++) {
            clf[c6][c5] = Ex[c6][c5][16] - Ex[c6][c5 + 1][16] + Ry[c6][c5] - Ey[c6][c5][16];
            tmp[c6][c5] = cymh[c5] / cyph[c5] * Bza[c6][c5][16] - ch / cyph[c5] * clf[c6][c5];
            Hz[c6][c5][16] = cxmh[16] / cxph[16] * Hz[c6][c5][16] + mui * czp[c6] / cxph[16] * tmp[c6][c5] - mui * czm[c6] / cxph[16] * Bza[c6][c5][16];
            Bza[c6][c5][16] = tmp[c6][c5];
          }
        }
      }
    }
#pragma omp parallel for private(c2, c3, c5, c6)
    for (c1 = 0; c1 <= 2; c1++) {
      for (c2 = 0; c2 <= 2; c2++) {
        for (c3 = 0; c3 <= 15; c3++) {
          for (c5 = 7 * c2; c5 <= ((15 < 7 * c2 + 6?15 : 7 * c2 + 6)); c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c6 = 7 * c1; c6 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c6++) {
              clf[c6][c5] = Ex[c6][16][c3] - Ax[c6][c3] + Ey[c6][16][c3 + 1] - Ey[c6][16][c3];
              tmp[c6][c5] = cymh[16] / cyph[c5] * Bza[c6][c5][c3] - ch / cyph[c5] * clf[c6][c5];
              Hz[c6][16][c3] = cxmh[c3] / cxph[c3] * Hz[c6][16][c3] + mui * czp[c6] / cxph[c3] * tmp[c6][c5] - mui * czm[c6] / cxph[c3] * Bza[c6][16][c3];
              Bza[c6][16][c3] = tmp[c6][c5];
            }
          }
        }
      }
    }
#pragma omp parallel for private(c2, c5, c6)
    for (c1 = 0; c1 <= 2; c1++) {
      for (c2 = 0; c2 <= 2; c2++) {
        for (c5 = 7 * c2; c5 <= ((15 < 7 * c2 + 6?15 : 7 * c2 + 6)); c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c6 = 7 * c1; c6 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c6++) {
            clf[c6][c5] = Ex[c6][16][16] - Ax[c6][16] + Ry[c6][16] - Ey[c6][16][16];
            tmp[c6][c5] = cymh[16] / cyph[16] * Bza[c6][16][16] - ch / cyph[16] * clf[c6][c5];
            Hz[c6][16][16] = cxmh[16] / cxph[16] * Hz[c6][16][16] + mui * czp[c6] / cxph[16] * tmp[c6][c5] - mui * czm[c6] / cxph[16] * Bza[c6][16][16];
            Bza[c6][16][16] = tmp[c6][c5];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
