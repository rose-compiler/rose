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
    int c2;
    int c8;
    for (c0 = 0; c0 <= 1; c0++) {
#pragma omp parallel for private(c8)
      for (c2 = 0; c2 <= 15; c2++) {
        for (c8 = 1; c8 <= 15; c8++) {
          B[c2][c8] = B[c2][c8] - A[c2][c8] * A[c2][c8] / B[c2][c8 - 1];
        }
        for (c8 = 1; c8 <= 15; c8++) {
          X[c2][c8] = X[c2][c8] - X[c2][c8 - 1] * A[c2][c8] / B[c2][c8 - 1];
        }
        for (c8 = 0; c8 <= 13; c8++) {
          X[c2][16 - c8 - 2] = (X[c2][16 - 2 - c8] - X[c2][16 - 2 - c8 - 1] * A[c2][16 - c8 - 3]) / B[c2][16 - 3 - c8];
        }
      }
#pragma omp parallel for
      for (c2 = 0; c2 <= 15; c2++) {
        X[c2][16 - 1] = X[c2][16 - 1] / B[c2][16 - 1];
      }
#pragma omp parallel for private(c8)
      for (c2 = 0; c2 <= 15; c2++) {
        for (c8 = 1; c8 <= 15; c8++) {
          B[c8][c2] = B[c8][c2] - A[c8][c2] * A[c8][c2] / B[c8 - 1][c2];
        }
        for (c8 = 1; c8 <= 15; c8++) {
          X[c8][c2] = X[c8][c2] - X[c8 - 1][c2] * A[c8][c2] / B[c8 - 1][c2];
        }
        for (c8 = 0; c8 <= 13; c8++) {
          X[16 - 2 - c8][c2] = (X[16 - 2 - c8][c2] - X[16 - c8 - 3][c2] * A[16 - 3 - c8][c2]) / B[16 - 2 - c8][c2];
        }
      }
#pragma omp parallel for
      for (c2 = 0; c2 <= 15; c2++) {
        X[16 - 1][c2] = X[16 - 1][c2] / B[16 - 1][c2];
      }
    }
  }
  
#pragma endscop
  return 0;
}
