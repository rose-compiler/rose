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
{
      int c1;
      int c5;
      int c8;
      for (c1 = 0; c1 <= 1; c1++) {
        for (c5 = 0; c5 <= 31; c5++) {
          for (c8 = 1; c8 <= 31; c8++) {
            X[c5][c8] = X[c5][c8] - X[c5][c8 - 1] * A[c5][c8] / B[c5][c8 - 1];
            B[c5][c8] = B[c5][c8] - A[c5][c8] * A[c5][c8] / B[c5][c8 - 1];
          }
        }
        for (c5 = 0; c5 <= 31; c5++) {
          X[c5][32 - 1] = X[c5][32 - 1] / B[c5][32 - 1];
        }
        for (c8 = 0; c8 <= 29; c8++) {
          X[0][32 - c8 - 2] = (X[0][32 - 2 - c8] - X[0][32 - 2 - c8 - 1] * A[0][32 - c8 - 3]) / B[0][32 - 3 - c8];
        }
        for (c5 = 1; c5 <= 31; c5++) {
          for (c8 = 0; c8 <= 29; c8++) {
            X[c5][32 - c8 - 2] = (X[c5][32 - 2 - c8] - X[c5][32 - 2 - c8 - 1] * A[c5][32 - c8 - 3]) / B[c5][32 - 3 - c8];
          }
          for (c8 = 0; c8 <= 31; c8++) {
            B[c5][c8] = B[c5][c8] - A[c5][c8] * A[c5][c8] / B[c5 - 1][c8];
            X[c5][c8] = X[c5][c8] - X[c5 - 1][c8] * A[c5][c8] / B[c5 - 1][c8];
          }
        }
        for (c5 = 0; c5 <= 31; c5++) {
          X[32 - 1][c5] = X[32 - 1][c5] / B[32 - 1][c5];
        }
        for (c5 = 0; c5 <= 31; c5++) {
          for (c8 = 0; c8 <= 29; c8++) {
            X[32 - 2 - c8][c5] = (X[32 - 2 - c8][c5] - X[32 - c8 - 3][c5] * A[32 - 3 - c8][c5]) / B[32 - 2 - c8][c5];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
