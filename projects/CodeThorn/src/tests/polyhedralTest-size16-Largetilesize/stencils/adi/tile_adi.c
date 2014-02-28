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
    int c3;
    int c10;
{
      int c1;
      int c5;
      int c6;
      for (c1 = 0; c1 <= 1; c1++) {
        for (c5 = 0; c5 <= 15; c5++) {
          for (c6 = 1; c6 <= 15; c6++) {
            X[c5][c6] = X[c5][c6] - X[c5][c6 - 1] * A[c5][c6] / B[c5][c6 - 1];
            B[c5][c6] = B[c5][c6] - A[c5][c6] * A[c5][c6] / B[c5][c6 - 1];
          }
        }
        for (c5 = 0; c5 <= 15; c5++) {
          X[c5][16 - 1] = X[c5][16 - 1] / B[c5][16 - 1];
        }
        for (c6 = 0; c6 <= 13; c6++) {
          X[0][16 - c6 - 2] = (X[0][16 - 2 - c6] - X[0][16 - 2 - c6 - 1] * A[0][16 - c6 - 3]) / B[0][16 - 3 - c6];
        }
        for (c5 = 1; c5 <= 15; c5++) {
          for (c6 = 0; c6 <= 13; c6++) {
            X[c5][16 - c6 - 2] = (X[c5][16 - 2 - c6] - X[c5][16 - 2 - c6 - 1] * A[c5][16 - c6 - 3]) / B[c5][16 - 3 - c6];
          }
          for (c6 = 3 * c5 + 10; c6 <= 3 * c5 + 25; c6++) {
            B[c5][-3 * c5 + c6 + -10] = B[c5][-3 * c5 + c6 + -10] - A[c5][-3 * c5 + c6 + -10] * A[c5][-3 * c5 + c6 + -10] / B[c5 - 1][-3 * c5 + c6 + -10];
            X[c5][-3 * c5 + c6 + -10] = X[c5][-3 * c5 + c6 + -10] - X[c5 - 1][-3 * c5 + c6 + -10] * A[c5][-3 * c5 + c6 + -10] / B[c5 - 1][-3 * c5 + c6 + -10];
          }
        }
        for (c5 = 0; c5 <= 15; c5++) {
          X[16 - 1][c5] = X[16 - 1][c5] / B[16 - 1][c5];
        }
        for (c5 = 0; c5 <= 15; c5++) {
          for (c6 = 0; c6 <= 13; c6++) {
            X[16 - 2 - c6][c5] = (X[16 - 2 - c6][c5] - X[16 - c6 - 3][c5] * A[16 - 3 - c6][c5]) / B[16 - 2 - c6][c5];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
