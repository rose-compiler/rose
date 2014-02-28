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
  int cz = 64;
  int cym = 64;
  int cxm = 64;
/* Variable declaration/allocation. */
  double mui;
  double ch;
  double Ax[64 + 1][64 + 1];
  double Ry[64 + 1][64 + 1];
  double clf[64 + 1][64 + 1];
  double tmp[64 + 1][64 + 1];
  double Bza[64 + 1][64 + 1][64 + 1];
  double Ex[64 + 1][64 + 1][64 + 1];
  double Ey[64 + 1][64 + 1][64 + 1];
  double Hz[64 + 1][64 + 1][64 + 1];
  double czm[64 + 1];
  double czp[64 + 1];
  double cxmh[64 + 1];
  double cxph[64 + 1];
  double cymh[64 + 1];
  double cyph[64 + 1];
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
    for (c1 = 0; c1 <= 31; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 63; c3++) {
          for (c5 = 4 * c2; c5 <= 4 * c2 + 3; c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c6 = 2 * c1; c6 <= 2 * c1 + 1; c6++) {
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
    for (c1 = 0; c1 <= 31; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        for (c5 = 4 * c2; c5 <= 4 * c2 + 3; c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c6 = 2 * c1; c6 <= 2 * c1 + 1; c6++) {
            clf[c6][c5] = Ex[c6][c5][64] - Ex[c6][c5 + 1][64] + Ry[c6][c5] - Ey[c6][c5][64];
            tmp[c6][c5] = cymh[c5] / cyph[c5] * Bza[c6][c5][64] - ch / cyph[c5] * clf[c6][c5];
            Hz[c6][c5][64] = cxmh[64] / cxph[64] * Hz[c6][c5][64] + mui * czp[c6] / cxph[64] * tmp[c6][c5] - mui * czm[c6] / cxph[64] * Bza[c6][c5][64];
            Bza[c6][c5][64] = tmp[c6][c5];
          }
        }
      }
    }
#pragma omp parallel for private(c2, c3, c5, c6)
    for (c1 = 0; c1 <= 31; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        for (c3 = 0; c3 <= 63; c3++) {
          for (c5 = 4 * c2; c5 <= 4 * c2 + 3; c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c6 = 2 * c1; c6 <= 2 * c1 + 1; c6++) {
              clf[c6][c5] = Ex[c6][64][c3] - Ax[c6][c3] + Ey[c6][64][c3 + 1] - Ey[c6][64][c3];
              tmp[c6][c5] = cymh[64] / cyph[c5] * Bza[c6][c5][c3] - ch / cyph[c5] * clf[c6][c5];
              Hz[c6][64][c3] = cxmh[c3] / cxph[c3] * Hz[c6][64][c3] + mui * czp[c6] / cxph[c3] * tmp[c6][c5] - mui * czm[c6] / cxph[c3] * Bza[c6][64][c3];
              Bza[c6][64][c3] = tmp[c6][c5];
            }
          }
        }
      }
    }
#pragma omp parallel for private(c2, c5, c6)
    for (c1 = 0; c1 <= 31; c1++) {
      for (c2 = 0; c2 <= 15; c2++) {
        for (c5 = 4 * c2; c5 <= 4 * c2 + 3; c5++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c6 = 2 * c1; c6 <= 2 * c1 + 1; c6++) {
            clf[c6][c5] = Ex[c6][64][64] - Ax[c6][64] + Ry[c6][64] - Ey[c6][64][64];
            tmp[c6][c5] = cymh[64] / cyph[64] * Bza[c6][64][64] - ch / cyph[64] * clf[c6][c5];
            Hz[c6][64][64] = cxmh[64] / cxph[64] * Hz[c6][64][64] + mui * czp[c6] / cxph[64] * tmp[c6][c5] - mui * czm[c6] / cxph[64] * Bza[c6][64][64];
            Bza[c6][64][64] = tmp[c6][c5];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
