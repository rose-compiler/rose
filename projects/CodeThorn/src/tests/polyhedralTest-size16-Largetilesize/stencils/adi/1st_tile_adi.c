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
    int c8;
    int c15;
    for (c0 = 0; c0 <= 1; c0++) {
      for (c8 = 1; c8 <= 15; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          B[c15][c8] = B[c15][c8] - A[c15][c8] * A[c15][c8] / B[c15][c8 - 1];
        }
      }
      for (c8 = 1; c8 <= 15; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          X[c15][c8] = X[c15][c8] - X[c15][c8 - 1] * A[c15][c8] / B[c15][c8 - 1];
        }
      }
      for (c8 = 0; c8 <= 13; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          X[c15][16 - c8 - 2] = (X[c15][16 - 2 - c8] - X[c15][16 - 2 - c8 - 1] * A[c15][16 - c8 - 3]) / B[c15][16 - 3 - c8];
        }
      }
#pragma ivdep
#pragma vector always
#pragma simd
      for (c15 = 0; c15 <= 15; c15++) {
        X[c15][16 - 1] = X[c15][16 - 1] / B[c15][16 - 1];
      }
      for (c8 = 1; c8 <= 15; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          B[c8][c15] = B[c8][c15] - A[c8][c15] * A[c8][c15] / B[c8 - 1][c15];
        }
      }
      for (c8 = 1; c8 <= 15; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          X[c8][c15] = X[c8][c15] - X[c8 - 1][c15] * A[c8][c15] / B[c8 - 1][c15];
        }
      }
      for (c8 = 0; c8 <= 13; c8++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c15 = 0; c15 <= 15; c15++) {
          X[16 - 2 - c8][c15] = (X[16 - 2 - c8][c15] - X[16 - c8 - 3][c15] * A[16 - 3 - c8][c15]) / B[16 - 2 - c8][c15];
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
