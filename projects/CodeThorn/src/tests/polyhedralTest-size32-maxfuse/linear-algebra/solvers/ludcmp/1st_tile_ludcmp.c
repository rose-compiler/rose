#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * ludcmp.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Variable declaration/allocation. */
  double A[32 + 1][32 + 1];
  double b[32 + 1];
  double x[32 + 1];
  double y[32 + 1];
  int i;
  int j;
  int k;
  double w;
  
#pragma scop
{
    int c7;
    int c1;
    int c8;
    for (c7 = 1; c7 <= 32; c7++) {
      w = A[c7][0];
      A[c7][0] = w / A[0][0];
    }
    for (c7 = 1; c7 <= 32; c7++) {
      w = A[0 + 1][c7];
      w = w - A[0 + 1][0] * A[0][c7];
      A[0 + 1][c7] = w;
    }
    for (c1 = 1; c1 <= 31; c1++) {
      for (c7 = c1 + 1; c7 <= 32; c7++) {
        w = A[c7][c1];
        for (c8 = 0; c8 <= c1 + -1; c8++) {
          w = w - A[c7][c8] * A[c8][c1];
        }
        A[c7][c1] = w / A[c1][c1];
      }
      for (c7 = c1 + 1; c7 <= 32; c7++) {
        w = A[c1 + 1][c7];
        for (c8 = 0; c8 <= c1; c8++) {
          w = w - A[c1 + 1][c8] * A[c8][c7];
        }
        A[c1 + 1][c7] = w;
      }
    }
    b[0] = 1.0;
    y[0] = b[0];
    for (c1 = 1; c1 <= 32; c1++) {
      w = b[c1];
      for (c7 = 0; c7 <= c1 + -1; c7++) {
        w = w - A[c1][c7] * y[c7];
      }
      y[c1] = w;
    }
    x[32] = y[32] / A[32][32];
    for (c1 = 0; c1 <= 31; c1++) {
      w = y[32 - 1 - c1];
      for (c7 = -1 * c1 + 32; c7 <= 32; c7++) {
        w = w - A[32 - 1 - c1][c7] * x[c7];
      }
      x[32 - 1 - c1] = w / A[32 - 1 - c1][32 - 1 - c1];
    }
  }
  
#pragma endscop
  return 0;
}
