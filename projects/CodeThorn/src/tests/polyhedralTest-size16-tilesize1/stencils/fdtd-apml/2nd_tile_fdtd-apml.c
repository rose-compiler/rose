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
    int c3;
    int c2;
    int c1;
    
#pragma omp parallel for private(c2, c3)
{
      int c0;
      int c4;
      int c5;
#pragma omp parallel for private(c5, c4)
      for (c0 = 0; c0 <= 15; c0++) {
        for (c4 = 0; c4 <= 15; c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c5 = 0; c5 <= 15; c5++) {
            clf[c0][c4] = Ex[c0][c4][c5] - Ex[c0][c4 + 1][c5] + Ey[c0][c4][c5 + 1] - Ey[c0][c4][c5];
            tmp[c0][c4] = cymh[c4] / cyph[c4] * Bza[c0][c4][c5] - ch / cyph[c4] * clf[c0][c4];
            Hz[c0][c4][c5] = cxmh[c5] / cxph[c5] * Hz[c0][c4][c5] + mui * czp[c0] / cxph[c5] * tmp[c0][c4] - mui * czm[c0] / cxph[c5] * Bza[c0][c4][c5];
            Bza[c0][c4][c5] = tmp[c0][c4];
          }
        }
      }
    }
    
#pragma omp parallel for private(c2)
{
      int c3;
      int c4;
      for (c3 = 0; c3 <= 15; c3++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 0; c4 <= 15; c4++) {
          clf[c3][c4] = Ex[c3][c4][16] - Ex[c3][c4 + 1][16] + Ry[c3][c4] - Ey[c3][c4][16];
          tmp[c3][c4] = cymh[c4] / cyph[c4] * Bza[c3][c4][16] - ch / cyph[c4] * clf[c3][c4];
          Hz[c3][c4][16] = cxmh[16] / cxph[16] * Hz[c3][c4][16] + mui * czp[c3] / cxph[16] * tmp[c3][c4] - mui * czm[c3] / cxph[16] * Bza[c3][c4][16];
          Bza[c3][c4][16] = tmp[c3][c4];
        }
      }
    }
    
#pragma omp parallel for private(c2, c3)
{
      int c5;
      int c0;
      int c4;
#pragma omp parallel for private(c4, c5)
      for (c0 = 0; c0 <= 15; c0++) {
        for (c4 = 0; c4 <= 15; c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c5 = 0; c5 <= 15; c5++) {
            clf[c0][c4] = Ex[c0][16][c5] - Ax[c0][c5] + Ey[c0][16][c5 + 1] - Ey[c0][16][c5];
            tmp[c0][c4] = cymh[16] / cyph[c4] * Bza[c0][c4][c5] - ch / cyph[c4] * clf[c0][c4];
            Hz[c0][16][c5] = cxmh[c5] / cxph[c5] * Hz[c0][16][c5] + mui * czp[c0] / cxph[c5] * tmp[c0][c4] - mui * czm[c0] / cxph[c5] * Bza[c0][16][c5];
            Bza[c0][16][c5] = tmp[c0][c4];
          }
        }
      }
    }
    
#pragma omp parallel for private(c2)
{
      int c3;
      int c4;
      for (c3 = 0; c3 <= 15; c3++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 0; c4 <= 15; c4++) {
          clf[c3][c4] = Ex[c3][16][16] - Ax[c3][16] + Ry[c3][16] - Ey[c3][16][16];
          tmp[c3][c4] = cymh[16] / cyph[16] * Bza[c3][16][16] - ch / cyph[16] * clf[c3][c4];
          Hz[c3][16][16] = cxmh[16] / cxph[16] * Hz[c3][16][16] + mui * czp[c3] / cxph[16] * tmp[c3][c4] - mui * czm[c3] / cxph[16] * Bza[c3][16][16];
          Bza[c3][16][16] = tmp[c3][c4];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
