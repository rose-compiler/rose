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
    int c5;
    int c3;
    int c1;
    for (c1 = 0; c1 <= 31; c1++) {
      for (c3 = 0; c3 <= 31; c3++) {
        for (c5 = 0; c5 <= 31; c5++) {
          clf[c1][c3] = Ex[c1][c3][c5] - Ex[c1][c3 + 1][c5] + Ey[c1][c3][c5 + 1] - Ey[c1][c3][c5];
          tmp[c1][c3] = cymh[c3] / cyph[c3] * Bza[c1][c3][c5] - ch / cyph[c3] * clf[c1][c3];
          Hz[c1][c3][c5] = cxmh[c5] / cxph[c5] * Hz[c1][c3][c5] + mui * czp[c1] / cxph[c5] * tmp[c1][c3] - mui * czm[c1] / cxph[c5] * Bza[c1][c3][c5];
          Bza[c1][c3][c5] = tmp[c1][c3];
        }
        clf[c1][c3] = Ex[c1][c3][32] - Ex[c1][c3 + 1][32] + Ry[c1][c3] - Ey[c1][c3][32];
        tmp[c1][c3] = cymh[c3] / cyph[c3] * Bza[c1][c3][32] - ch / cyph[c3] * clf[c1][c3];
        Hz[c1][c3][32] = cxmh[32] / cxph[32] * Hz[c1][c3][32] + mui * czp[c1] / cxph[32] * tmp[c1][c3] - mui * czm[c1] / cxph[32] * Bza[c1][c3][32];
        Bza[c1][c3][32] = tmp[c1][c3];
        for (c5 = 0; c5 <= 31; c5++) {
          clf[c1][c3] = Ex[c1][32][c5] - Ax[c1][c5] + Ey[c1][32][c5 + 1] - Ey[c1][32][c5];
          tmp[c1][c3] = cymh[32] / cyph[c3] * Bza[c1][c3][c5] - ch / cyph[c3] * clf[c1][c3];
          Hz[c1][32][c5] = cxmh[c5] / cxph[c5] * Hz[c1][32][c5] + mui * czp[c1] / cxph[c5] * tmp[c1][c3] - mui * czm[c1] / cxph[c5] * Bza[c1][32][c5];
          Bza[c1][32][c5] = tmp[c1][c3];
        }
        clf[c1][c3] = Ex[c1][32][32] - Ax[c1][32] + Ry[c1][32] - Ey[c1][32][32];
        tmp[c1][c3] = cymh[32] / cyph[32] * Bza[c1][32][32] - ch / cyph[32] * clf[c1][c3];
        Hz[c1][32][32] = cxmh[32] / cxph[32] * Hz[c1][32][32] + mui * czp[c1] / cxph[32] * tmp[c1][c3] - mui * czm[c1] / cxph[32] * Bza[c1][32][32];
        Bza[c1][32][32] = tmp[c1][c3];
      }
    }
  }
  
#pragma endscop
  return 0;
}
