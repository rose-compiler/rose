#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE
# define _PB_N ARRAYSIZE
/**
 * adi.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 32;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double X[32][32];
  double A[32][32];
  double B[32][32];
  int t;
  int i1;
  int i2;
  
#pragma scop
{
    int c0;
    int c2;
    int c8;
    int c9;
    int c15;
    for (c0 = 0; c0 <= 1; c0++) {
#pragma omp parallel for private(c15, c9, c8)
      for (c2 = 0; c2 <= 15; c2++) {
        for (c8 = 0; c8 <= 7; c8++) {
          for (c9 = (1 > 4 * c8?1 : 4 * c8); c9 <= 4 * c8 + 3; c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c15 = 2 * c2; c15 <= 2 * c2 + 1; c15++) {
              B[c15][c9] = B[c15][c9] - A[c15][c9] * A[c15][c9] / B[c15][c9 - 1];
            }
          }
        }
        for (c8 = 0; c8 <= 7; c8++) {
          for (c9 = (1 > 4 * c8?1 : 4 * c8); c9 <= 4 * c8 + 3; c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c15 = 2 * c2; c15 <= 2 * c2 + 1; c15++) {
              X[c15][c9] = X[c15][c9] - X[c15][c9 - 1] * A[c15][c9] / B[c15][c9 - 1];
            }
          }
        }
        for (c8 = 0; c8 <= 7; c8++) {
          for (c9 = 4 * c8; c9 <= ((29 < 4 * c8 + 3?29 : 4 * c8 + 3)); c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c15 = 2 * c2; c15 <= 2 * c2 + 1; c15++) {
              X[c15][32 - c9 - 2] = (X[c15][32 - 2 - c9] - X[c15][32 - 2 - c9 - 1] * A[c15][32 - c9 - 3]) / B[c15][32 - 3 - c9];
            }
          }
        }
      }
#pragma omp parallel for private(c15)
      for (c2 = 0; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 2 * c2; c15 <= 2 * c2 + 1; c15++) {
          X[c15][32 - 1] = X[c15][32 - 1] / B[c15][32 - 1];
        }
      }
#pragma omp parallel for private(c15, c9, c8)
      for (c2 = 0; c2 <= 15; c2++) {
        for (c8 = 0; c8 <= 7; c8++) {
          for (c9 = (1 > 4 * c8?1 : 4 * c8); c9 <= 4 * c8 + 3; c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c15 = 2 * c2; c15 <= 2 * c2 + 1; c15++) {
              B[c9][c15] = B[c9][c15] - A[c9][c15] * A[c9][c15] / B[c9 - 1][c15];
            }
          }
        }
        for (c8 = 0; c8 <= 7; c8++) {
          for (c9 = (1 > 4 * c8?1 : 4 * c8); c9 <= 4 * c8 + 3; c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c15 = 2 * c2; c15 <= 2 * c2 + 1; c15++) {
              X[c9][c15] = X[c9][c15] - X[c9 - 1][c15] * A[c9][c15] / B[c9 - 1][c15];
            }
          }
        }
        for (c8 = 0; c8 <= 7; c8++) {
          for (c9 = 4 * c8; c9 <= ((29 < 4 * c8 + 3?29 : 4 * c8 + 3)); c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c15 = 2 * c2; c15 <= 2 * c2 + 1; c15++) {
              X[32 - 2 - c9][c15] = (X[32 - 2 - c9][c15] - X[32 - c9 - 3][c15] * A[32 - 3 - c9][c15]) / B[32 - 2 - c9][c15];
            }
          }
        }
      }
#pragma omp parallel for private(c15)
      for (c2 = 0; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 2 * c2; c15 <= 2 * c2 + 1; c15++) {
          X[32 - 1][c15] = X[32 - 1][c15] / B[32 - 1][c15];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
