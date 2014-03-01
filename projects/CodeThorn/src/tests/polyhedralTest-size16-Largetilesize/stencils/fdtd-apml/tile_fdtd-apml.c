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
{
      int c2;
      int c8;
      int c6;
      for (c2 = 0; c2 <= 15; c2++) {
        for (c6 = 0; c6 <= 15; c6++) {
          for (c8 = 0; c8 <= 15; c8++) {
            clf[c2][c6] = Ex[c2][c6][c8] - Ex[c2][c6 + 1][c8] + Ey[c2][c6][c8 + 1] - Ey[c2][c6][c8];
            tmp[c2][c6] = cymh[c6] / cyph[c6] * Bza[c2][c6][c8] - ch / cyph[c6] * clf[c2][c6];
            Hz[c2][c6][c8] = cxmh[c8] / cxph[c8] * Hz[c2][c6][c8] + mui * czp[c2] / cxph[c8] * tmp[c2][c6] - mui * czm[c2] / cxph[c8] * Bza[c2][c6][c8];
            Bza[c2][c6][c8] = tmp[c2][c6];
          }
          clf[c2][c6] = Ex[c2][c6][16] - Ex[c2][c6 + 1][16] + Ry[c2][c6] - Ey[c2][c6][16];
          tmp[c2][c6] = cymh[c6] / cyph[c6] * Bza[c2][c6][16] - ch / cyph[c6] * clf[c2][c6];
          Hz[c2][c6][16] = cxmh[16] / cxph[16] * Hz[c2][c6][16] + mui * czp[c2] / cxph[16] * tmp[c2][c6] - mui * czm[c2] / cxph[16] * Bza[c2][c6][16];
          Bza[c2][c6][16] = tmp[c2][c6];
          for (c8 = 0; c8 <= 15; c8++) {
            clf[c2][c6] = Ex[c2][16][c8] - Ax[c2][c8] + Ey[c2][16][c8 + 1] - Ey[c2][16][c8];
            tmp[c2][c6] = cymh[16] / cyph[c6] * Bza[c2][c6][c8] - ch / cyph[c6] * clf[c2][c6];
            Hz[c2][16][c8] = cxmh[c8] / cxph[c8] * Hz[c2][16][c8] + mui * czp[c2] / cxph[c8] * tmp[c2][c6] - mui * czm[c2] / cxph[c8] * Bza[c2][16][c8];
            Bza[c2][16][c8] = tmp[c2][c6];
          }
          clf[c2][c6] = Ex[c2][16][16] - Ax[c2][16] + Ry[c2][16] - Ey[c2][16][16];
          tmp[c2][c6] = cymh[16] / cyph[16] * Bza[c2][16][16] - ch / cyph[16] * clf[c2][c6];
          Hz[c2][16][16] = cxmh[16] / cxph[16] * Hz[c2][16][16] + mui * czp[c2] / cxph[16] * tmp[c2][c6] - mui * czm[c2] / cxph[16] * Bza[c2][16][16];
          Bza[c2][16][16] = tmp[c2][c6];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
