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
    int c5;
    int c3;
    int c1;
    for (c1 = 0; c1 <= 1; c1++) {
      for (c3 = 0; c3 <= 15; c3++) {
        for (c5 = 1; c5 <= 15; c5++) {
          X[c3][c5] = X[c3][c5] - X[c3][c5 - 1] * A[c3][c5] / B[c3][c5 - 1];
          B[c3][c5] = B[c3][c5] - A[c3][c5] * A[c3][c5] / B[c3][c5 - 1];
        }
      }
      for (c3 = 0; c3 <= 15; c3++) {
        X[c3][16 - 1] = X[c3][16 - 1] / B[c3][16 - 1];
      }
      for (c3 = 0; c3 <= 15; c3++) {
        for (c5 = 0; c5 <= 13; c5++) {
          X[c3][16 - c5 - 2] = (X[c3][16 - 2 - c5] - X[c3][16 - 2 - c5 - 1] * A[c3][16 - c5 - 3]) / B[c3][16 - 3 - c5];
        }
      }
      for (c3 = 1; c3 <= 15; c3++) {
        for (c5 = 0; c5 <= 15; c5++) {
          X[c3][c5] = X[c3][c5] - X[c3 - 1][c5] * A[c3][c5] / B[c3 - 1][c5];
          B[c3][c5] = B[c3][c5] - A[c3][c5] * A[c3][c5] / B[c3 - 1][c5];
        }
      }
      for (c3 = 0; c3 <= 15; c3++) {
        X[16 - 1][c3] = X[16 - 1][c3] / B[16 - 1][c3];
      }
      for (c3 = 0; c3 <= 13; c3++) {
        for (c5 = 0; c5 <= 15; c5++) {
          X[16 - 2 - c3][c5] = (X[16 - 2 - c3][c5] - X[16 - c3 - 3][c5] * A[16 - 3 - c3][c5]) / B[16 - 2 - c3][c5];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
