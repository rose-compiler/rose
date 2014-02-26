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
    int c6;
    int c3;
    int c2;
    int c1;
    
#pragma omp parallel for private(c2, c3, c6)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 31; c2++) {
        for (c3 = 0; c3 <= 31; c3++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c5;
            for (c5 = 8 * c1; c5 <= 8 * c1 + 7; c5++) {
              clf[c5][c2] = Ex[c5][c2][c3] - Ex[c5][c2 + 1][c3] + Ey[c5][c2][c3 + 1] - Ey[c5][c2][c3];
              tmp[c5][c2] = cymh[c2] / cyph[c2] * Bza[c5][c2][c3] - ch / cyph[c2] * clf[c5][c2];
              Hz[c5][c2][c3] = cxmh[c3] / cxph[c3] * Hz[c5][c2][c3] + mui * czp[c5] / cxph[c3] * tmp[c5][c2] - mui * czm[c5] / cxph[c3] * Bza[c5][c2][c3];
              Bza[c5][c2][c3] = tmp[c5][c2];
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c6)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 31; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c4;
          for (c4 = 8 * c1; c4 <= 8 * c1 + 7; c4++) {
            clf[c4][c2] = Ex[c4][c2][32] - Ex[c4][c2 + 1][32] + Ry[c4][c2] - Ey[c4][c2][32];
            tmp[c4][c2] = cymh[c2] / cyph[c2] * Bza[c4][c2][32] - ch / cyph[c2] * clf[c4][c2];
            Hz[c4][c2][32] = cxmh[32] / cxph[32] * Hz[c4][c2][32] + mui * czp[c4] / cxph[32] * tmp[c4][c2] - mui * czm[c4] / cxph[32] * Bza[c4][c2][32];
            Bza[c4][c2][32] = tmp[c4][c2];
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c3, c6)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 31; c2++) {
        for (c3 = 0; c3 <= 31; c3++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c5;
            for (c5 = 8 * c1; c5 <= 8 * c1 + 7; c5++) {
              clf[c5][c2] = Ex[c5][32][c3] - Ax[c5][c3] + Ey[c5][32][c3 + 1] - Ey[c5][32][c3];
              tmp[c5][c2] = cymh[32] / cyph[c2] * Bza[c5][c2][c3] - ch / cyph[c2] * clf[c5][c2];
              Hz[c5][32][c3] = cxmh[c3] / cxph[c3] * Hz[c5][32][c3] + mui * czp[c5] / cxph[c3] * tmp[c5][c2] - mui * czm[c5] / cxph[c3] * Bza[c5][32][c3];
              Bza[c5][32][c3] = tmp[c5][c2];
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c6)
    for (c1 = 0; c1 <= 3; c1++) {
      for (c2 = 0; c2 <= 31; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c4;
          for (c4 = 8 * c1; c4 <= 8 * c1 + 7; c4++) {
            clf[c4][c2] = Ex[c4][32][32] - Ax[c4][32] + Ry[c4][32] - Ey[c4][32][32];
            tmp[c4][c2] = cymh[32] / cyph[32] * Bza[c4][32][32] - ch / cyph[32] * clf[c4][c2];
            Hz[c4][32][32] = cxmh[32] / cxph[32] * Hz[c4][32][32] + mui * czp[c4] / cxph[32] * tmp[c4][c2] - mui * czm[c4] / cxph[32] * Bza[c4][32][32];
            Bza[c4][32][32] = tmp[c4][c2];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
