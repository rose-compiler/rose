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
{
      int c8;
      int c6;
      int c2;
      for (c2 = 0; c2 <= 1; c2++) {
        for (c6 = 0; c6 <= 15; c6++) {
          for (c8 = 1; c8 <= 15; c8++) {
            X[c6][c8] = X[c6][c8] - X[c6][c8 - 1] * A[c6][c8] / B[c6][c8 - 1];
            B[c6][c8] = B[c6][c8] - A[c6][c8] * A[c6][c8] / B[c6][c8 - 1];
          }
        }
        for (c6 = 0; c6 <= 15; c6++) {
          X[c6][16 - 1] = X[c6][16 - 1] / B[c6][16 - 1];
        }
        for (c6 = 0; c6 <= 15; c6++) {
          for (c8 = 0; c8 <= 13; c8++) {
            X[c6][16 - c8 - 2] = (X[c6][16 - 2 - c8] - X[c6][16 - 2 - c8 - 1] * A[c6][16 - c8 - 3]) / B[c6][16 - 3 - c8];
          }
        }
        for (c6 = 1; c6 <= 15; c6++) {
          for (c8 = 0; c8 <= 15; c8++) {
            X[c6][c8] = X[c6][c8] - X[c6 - 1][c8] * A[c6][c8] / B[c6 - 1][c8];
            B[c6][c8] = B[c6][c8] - A[c6][c8] * A[c6][c8] / B[c6 - 1][c8];
          }
        }
        for (c6 = 0; c6 <= 15; c6++) {
          X[16 - 1][c6] = X[16 - 1][c6] / B[16 - 1][c6];
        }
        for (c6 = 0; c6 <= 13; c6++) {
          for (c8 = 0; c8 <= 15; c8++) {
            X[16 - 2 - c6][c8] = (X[16 - 2 - c6][c8] - X[16 - c6 - 3][c8] * A[16 - 3 - c6][c8]) / B[16 - 2 - c6][c8];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
