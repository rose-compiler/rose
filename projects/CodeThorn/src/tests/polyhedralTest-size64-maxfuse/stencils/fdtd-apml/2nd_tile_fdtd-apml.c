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
    int c3;
    int c2;
    int c1;
    
#pragma omp parallel for private(c2, c3, c6)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = 0; c2 <= 63; c2++) {
        for (c3 = 0; c3 <= 63; c3++) {
          
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
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = 0; c2 <= 63; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c4;
          for (c4 = 8 * c1; c4 <= 8 * c1 + 7; c4++) {
            clf[c4][c2] = Ex[c4][c2][64] - Ex[c4][c2 + 1][64] + Ry[c4][c2] - Ey[c4][c2][64];
            tmp[c4][c2] = cymh[c2] / cyph[c2] * Bza[c4][c2][64] - ch / cyph[c2] * clf[c4][c2];
            Hz[c4][c2][64] = cxmh[64] / cxph[64] * Hz[c4][c2][64] + mui * czp[c4] / cxph[64] * tmp[c4][c2] - mui * czm[c4] / cxph[64] * Bza[c4][c2][64];
            Bza[c4][c2][64] = tmp[c4][c2];
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c3, c6)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = 0; c2 <= 63; c2++) {
        for (c3 = 0; c3 <= 63; c3++) {
          
#pragma ivdep
          
#pragma vector always
          
#pragma simd
{
            int c5;
            for (c5 = 8 * c1; c5 <= 8 * c1 + 7; c5++) {
              clf[c5][c2] = Ex[c5][64][c3] - Ax[c5][c3] + Ey[c5][64][c3 + 1] - Ey[c5][64][c3];
              tmp[c5][c2] = cymh[64] / cyph[c2] * Bza[c5][c2][c3] - ch / cyph[c2] * clf[c5][c2];
              Hz[c5][64][c3] = cxmh[c3] / cxph[c3] * Hz[c5][64][c3] + mui * czp[c5] / cxph[c3] * tmp[c5][c2] - mui * czm[c5] / cxph[c3] * Bza[c5][64][c3];
              Bza[c5][64][c3] = tmp[c5][c2];
            }
          }
        }
      }
    }
    
#pragma omp parallel for private(c2, c6)
    for (c1 = 0; c1 <= 7; c1++) {
      for (c2 = 0; c2 <= 63; c2++) {
        
#pragma ivdep
        
#pragma vector always
        
#pragma simd
{
          int c4;
          for (c4 = 8 * c1; c4 <= 8 * c1 + 7; c4++) {
            clf[c4][c2] = Ex[c4][64][64] - Ax[c4][64] + Ry[c4][64] - Ey[c4][64][64];
            tmp[c4][c2] = cymh[64] / cyph[64] * Bza[c4][64][64] - ch / cyph[64] * clf[c4][c2];
            Hz[c4][64][64] = cxmh[64] / cxph[64] * Hz[c4][64][64] + mui * czp[c4] / cxph[64] * tmp[c4][c2] - mui * czm[c4] / cxph[64] * Bza[c4][64][64];
            Bza[c4][64][64] = tmp[c4][c2];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
