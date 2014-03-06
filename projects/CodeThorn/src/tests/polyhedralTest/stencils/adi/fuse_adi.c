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
/* Include benchmark-specific header. */
/* Default data type is double, default size is 10x1024x1024. */
#include "adi.h"

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
    int c3;
    int c10;
    for (c0 = 0; c0 <= 1; c0++) {
      for (c3 = 0; c3 <= 31; c3++) {
        for (c10 = 1; c10 <= 31; c10++) {
          X[c3][c10] = X[c3][c10] - X[c3][c10 - 1] * A[c3][c10] / B[c3][c10 - 1];
          B[c3][c10] = B[c3][c10] - A[c3][c10] * A[c3][c10] / B[c3][c10 - 1];
        }
        for (c10 = 0; c10 <= 29; c10++) {
          X[c3][32 - c10 - 2] = (X[c3][32 - 2 - c10] - X[c3][32 - 2 - c10 - 1] * A[c3][32 - c10 - 3]) / B[c3][32 - 3 - c10];
        }
      }
      for (c3 = 0; c3 <= 31; c3++) {
        X[c3][32 - 1] = X[c3][32 - 1] / B[c3][32 - 1];
      }
      for (c3 = 0; c3 <= 31; c3++) {
        for (c10 = 1; c10 <= 31; c10++) {
          B[c10][c3] = B[c10][c3] - A[c10][c3] * A[c10][c3] / B[c10 - 1][c3];
        }
        for (c10 = 1; c10 <= 31; c10++) {
          X[c10][c3] = X[c10][c3] - X[c10 - 1][c3] * A[c10][c3] / B[c10 - 1][c3];
        }
        for (c10 = 0; c10 <= 29; c10++) {
          X[32 - 2 - c10][c3] = (X[32 - 2 - c10][c3] - X[32 - c10 - 3][c3] * A[32 - 3 - c10][c3]) / B[32 - 2 - c10][c3];
        }
      }
      for (c3 = 0; c3 <= 31; c3++) {
        X[32 - 1][c3] = X[32 - 1][c3] / B[32 - 1][c3];
      }
    }
  }
  
#pragma endscop
  return 0;
}
