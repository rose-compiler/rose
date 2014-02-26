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
    for (c0 = 0; c0 <= 31; c0++) {
      for (c1 = 0; c1 <= 31; c1++) {
        for (c4 = 0; c4 <= 31; c4++) {
          clf[c0][c1] = Ex[c0][c1][c4] - Ex[c0][c1 + 1][c4] + Ey[c0][c1][c4 + 1] - Ey[c0][c1][c4];
          tmp[c0][c1] = cymh[c1] / cyph[c1] * Bza[c0][c1][c4] - ch / cyph[c1] * clf[c0][c1];
          Hz[c0][c1][c4] = cxmh[c4] / cxph[c4] * Hz[c0][c1][c4] + mui * czp[c0] / cxph[c4] * tmp[c0][c1] - mui * czm[c0] / cxph[c4] * Bza[c0][c1][c4];
          Bza[c0][c1][c4] = tmp[c0][c1];
        }
        clf[c0][c1] = Ex[c0][c1][32] - Ex[c0][c1 + 1][32] + Ry[c0][c1] - Ey[c0][c1][32];
        tmp[c0][c1] = cymh[c1] / cyph[c1] * Bza[c0][c1][32] - ch / cyph[c1] * clf[c0][c1];
        Hz[c0][c1][32] = cxmh[32] / cxph[32] * Hz[c0][c1][32] + mui * czp[c0] / cxph[32] * tmp[c0][c1] - mui * czm[c0] / cxph[32] * Bza[c0][c1][32];
        Bza[c0][c1][32] = tmp[c0][c1];
        for (c4 = 0; c4 <= 31; c4++) {
          clf[c0][c1] = Ex[c0][32][c4] - Ax[c0][c4] + Ey[c0][32][c4 + 1] - Ey[c0][32][c4];
          tmp[c0][c1] = cymh[32] / cyph[c1] * Bza[c0][c1][c4] - ch / cyph[c1] * clf[c0][c1];
          Hz[c0][32][c4] = cxmh[c4] / cxph[c4] * Hz[c0][32][c4] + mui * czp[c0] / cxph[c4] * tmp[c0][c1] - mui * czm[c0] / cxph[c4] * Bza[c0][32][c4];
          Bza[c0][32][c4] = tmp[c0][c1];
        }
        clf[c0][c1] = Ex[c0][32][32] - Ax[c0][32] + Ry[c0][32] - Ey[c0][32][32];
        tmp[c0][c1] = cymh[32] / cyph[32] * Bza[c0][32][32] - ch / cyph[32] * clf[c0][c1];
        Hz[c0][32][32] = cxmh[32] / cxph[32] * Hz[c0][32][32] + mui * czp[c0] / cxph[32] * tmp[c0][c1] - mui * czm[c0] / cxph[32] * Bza[c0][32][32];
        Bza[c0][32][32] = tmp[c0][c1];
      }
    }
  }
  
#pragma endscop
  return 0;
}
