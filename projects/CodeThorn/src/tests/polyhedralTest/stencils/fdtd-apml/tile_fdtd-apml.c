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
/* Include benchmark-specific header. */
/* Default data type is double, default size is 256x256x256. */
#include "fdtd-apml.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int cz = 32;
  int cym = 32;
  int cxm = 32;
/* Variable declaration/allocation. */
  double mui;
  double ch;
  double Ax[32 + 1][32 + 1];
  double Ry[32 + 1][32 + 1];
  double clf[32 + 1][32 + 1];
  double tmp[32 + 1][32 + 1];
  double Bza[32 + 1][32 + 1][32 + 1];
  double Ex[32 + 1][32 + 1][32 + 1];
  double Ey[32 + 1][32 + 1][32 + 1];
  double Hz[32 + 1][32 + 1][32 + 1];
  double czm[32 + 1];
  double czp[32 + 1];
  double cxmh[32 + 1];
  double cxph[32 + 1];
  double cymh[32 + 1];
  double cyph[32 + 1];
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
      for (c2 = 0; c2 <= 31; c2++) {
        for (c3 = 0; c3 <= 31; c3++) {
          for (c7 = 0; c7 <= 31; c7++) {
            clf[c2][c3] = Ex[c2][c3][c7] - Ex[c2][c3 + 1][c7] + Ey[c2][c3][c7 + 1] - Ey[c2][c3][c7];
            tmp[c2][c3] = cymh[c3] / cyph[c3] * Bza[c2][c3][c7] - ch / cyph[c3] * clf[c2][c3];
            Hz[c2][c3][c7] = cxmh[c7] / cxph[c7] * Hz[c2][c3][c7] + mui * czp[c2] / cxph[c7] * tmp[c2][c3] - mui * czm[c2] / cxph[c7] * Bza[c2][c3][c7];
            Bza[c2][c3][c7] = tmp[c2][c3];
          }
          clf[c2][c3] = Ex[c2][c3][32] - Ex[c2][c3 + 1][32] + Ry[c2][c3] - Ey[c2][c3][32];
          tmp[c2][c3] = cymh[c3] / cyph[c3] * Bza[c2][c3][32] - ch / cyph[c3] * clf[c2][c3];
          Hz[c2][c3][32] = cxmh[32] / cxph[32] * Hz[c2][c3][32] + mui * czp[c2] / cxph[32] * tmp[c2][c3] - mui * czm[c2] / cxph[32] * Bza[c2][c3][32];
          Bza[c2][c3][32] = tmp[c2][c3];
          for (c7 = 0; c7 <= 31; c7++) {
            clf[c2][c3] = Ex[c2][32][c7] - Ax[c2][c7] + Ey[c2][32][c7 + 1] - Ey[c2][32][c7];
            tmp[c2][c3] = cymh[32] / cyph[c3] * Bza[c2][c3][c7] - ch / cyph[c3] * clf[c2][c3];
            Hz[c2][32][c7] = cxmh[c7] / cxph[c7] * Hz[c2][32][c7] + mui * czp[c2] / cxph[c7] * tmp[c2][c3] - mui * czm[c2] / cxph[c7] * Bza[c2][32][c7];
            Bza[c2][32][c7] = tmp[c2][c3];
          }
          clf[c2][c3] = Ex[c2][32][32] - Ax[c2][32] + Ry[c2][32] - Ey[c2][32][32];
          tmp[c2][c3] = cymh[32] / cyph[32] * Bza[c2][32][32] - ch / cyph[32] * clf[c2][c3];
          Hz[c2][32][32] = cxmh[32] / cxph[32] * Hz[c2][32][32] + mui * czp[c2] / cxph[32] * tmp[c2][c3] - mui * czm[c2] / cxph[32] * Bza[c2][32][32];
          Bza[c2][32][32] = tmp[c2][c3];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
