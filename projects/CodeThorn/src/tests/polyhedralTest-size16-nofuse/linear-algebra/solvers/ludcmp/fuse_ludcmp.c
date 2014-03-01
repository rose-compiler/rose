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
  int n = 16;
/* Variable declaration/allocation. */
  double A[16 + 1][16 + 1];
  double b[16 + 1];
  double x[16 + 1];
  double y[16 + 1];
  int i;
  int j;
  int k;
  double w;
  
#pragma scop
{
    int c3;
    int c1;
    int c4;
    for (c3 = 1; c3 <= 16; c3++) {
      w = A[c3][0];
      A[c3][0] = w / A[0][0];
    }
    for (c3 = 1; c3 <= 16; c3++) {
      w = A[0 + 1][c3];
      w = w - A[0 + 1][0] * A[0][c3];
      A[0 + 1][c3] = w;
    }
    for (c1 = 1; c1 <= 15; c1++) {
      for (c3 = c1 + 1; c3 <= 16; c3++) {
        w = A[c3][c1];
        for (c4 = 0; c4 <= c1 + -1; c4++) {
          w = w - A[c3][c4] * A[c4][c1];
        }
        A[c3][c1] = w / A[c1][c1];
      }
      for (c3 = c1 + 1; c3 <= 16; c3++) {
        w = A[c1 + 1][c3];
        for (c4 = 0; c4 <= c1; c4++) {
          w = w - A[c1 + 1][c4] * A[c4][c3];
        }
        A[c1 + 1][c3] = w;
      }
    }
    b[0] = 1.0;
    y[0] = b[0];
    for (c1 = 1; c1 <= 16; c1++) {
      w = b[c1];
      for (c3 = 0; c3 <= c1 + -1; c3++) {
        w = w - A[c1][c3] * y[c3];
      }
      y[c1] = w;
    }
    x[16] = y[16] / A[16][16];
    for (c1 = 0; c1 <= 15; c1++) {
      w = y[16 - 1 - c1];
      for (c3 = -1 * c1 + 16; c3 <= 16; c3++) {
        w = w - A[16 - 1 - c1][c3] * x[c3];
      }
      x[16 - 1 - c1] = w / A[16 - 1 - c1][16 - 1 - c1];
    }
  }
  
#pragma endscop
  return 0;
}
