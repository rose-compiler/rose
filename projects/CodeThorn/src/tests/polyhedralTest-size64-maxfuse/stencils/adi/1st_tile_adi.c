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
  int n = 64;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double X[64][64];
  double A[64][64];
  double B[64][64];
  int t;
  int i1;
  int i2;
  
#pragma scop
{
    int c0;
    int c2;
    int c8;
    int c15;
    for (c0 = 0; c0 <= 1; c0++) {
#pragma omp parallel for private(c15, c8)
      for (c2 = 0; c2 <= 7; c2++) {
        for (c8 = 1; c8 <= 63; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c15 = 8 * c2; c15 <= 8 * c2 + 7; c15++) {
            B[c15][c8] = B[c15][c8] - A[c15][c8] * A[c15][c8] / B[c15][c8 - 1];
          }
        }
        for (c8 = 1; c8 <= 63; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c15 = 8 * c2; c15 <= 8 * c2 + 7; c15++) {
            X[c15][c8] = X[c15][c8] - X[c15][c8 - 1] * A[c15][c8] / B[c15][c8 - 1];
          }
        }
        for (c8 = 0; c8 <= 61; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c15 = 8 * c2; c15 <= 8 * c2 + 7; c15++) {
            X[c15][64 - c8 - 2] = (X[c15][64 - 2 - c8] - X[c15][64 - 2 - c8 - 1] * A[c15][64 - c8 - 3]) / B[c15][64 - 3 - c8];
          }
        }
      }
#pragma omp parallel for private(c15)
      for (c2 = 0; c2 <= 7; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 8 * c2; c15 <= 8 * c2 + 7; c15++) {
          X[c15][64 - 1] = X[c15][64 - 1] / B[c15][64 - 1];
        }
      }
#pragma omp parallel for private(c15, c8)
      for (c2 = 0; c2 <= 7; c2++) {
        for (c8 = 1; c8 <= 63; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c15 = 8 * c2; c15 <= 8 * c2 + 7; c15++) {
            B[c8][c15] = B[c8][c15] - A[c8][c15] * A[c8][c15] / B[c8 - 1][c15];
          }
        }
        for (c8 = 1; c8 <= 63; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c15 = 8 * c2; c15 <= 8 * c2 + 7; c15++) {
            X[c8][c15] = X[c8][c15] - X[c8 - 1][c15] * A[c8][c15] / B[c8 - 1][c15];
          }
        }
        for (c8 = 0; c8 <= 61; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
          for (c15 = 8 * c2; c15 <= 8 * c2 + 7; c15++) {
            X[64 - 2 - c8][c15] = (X[64 - 2 - c8][c15] - X[64 - c8 - 3][c15] * A[64 - 3 - c8][c15]) / B[64 - 2 - c8][c15];
          }
        }
      }
#pragma omp parallel for private(c15)
      for (c2 = 0; c2 <= 7; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 8 * c2; c15 <= 8 * c2 + 7; c15++) {
          X[64 - 1][c15] = X[64 - 1][c15] / B[64 - 1][c15];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
