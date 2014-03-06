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
  int n = 16;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double X[16][16];
  double A[16][16];
  double B[16][16];
  int t;
  int i1;
  int i2;
  
#pragma scop
{
    int c0;
    int c9;
    int c15;
    for (c0 = 0; c0 <= 1; c0++) {
      for (c9 = 1; c9 <= 15; c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          B[c15][c9] = B[c15][c9] - A[c15][c9] * A[c15][c9] / B[c15][c9 - 1];
        }
      }
      for (c9 = 1; c9 <= 15; c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          X[c15][c9] = X[c15][c9] - X[c15][c9 - 1] * A[c15][c9] / B[c15][c9 - 1];
        }
      }
      for (c9 = 0; c9 <= 13; c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          X[c15][16 - c9 - 2] = (X[c15][16 - 2 - c9] - X[c15][16 - 2 - c9 - 1] * A[c15][16 - c9 - 3]) / B[c15][16 - 3 - c9];
        }
      }
#pragma ivdep
#pragma vector always
#pragma simd
      for (c15 = 0; c15 <= 15; c15++) {
        X[c15][16 - 1] = X[c15][16 - 1] / B[c15][16 - 1];
      }
      for (c9 = 1; c9 <= 15; c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          B[c9][c15] = B[c9][c15] - A[c9][c15] * A[c9][c15] / B[c9 - 1][c15];
        }
      }
      for (c9 = 1; c9 <= 15; c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          X[c9][c15] = X[c9][c15] - X[c9 - 1][c15] * A[c9][c15] / B[c9 - 1][c15];
        }
      }
      for (c9 = 0; c9 <= 13; c9++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          X[16 - 2 - c9][c15] = (X[16 - 2 - c9][c15] - X[16 - c9 - 3][c15] * A[16 - 3 - c9][c15]) / B[16 - 2 - c9][c15];
        }
      }
#pragma ivdep
#pragma vector always
#pragma simd
      for (c15 = 0; c15 <= 15; c15++) {
        X[16 - 1][c15] = X[16 - 1][c15] / B[16 - 1][c15];
      }
    }
  }
  
#pragma endscop
  return 0;
}
