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
    int c5;
    int c2;
    int c6;
    for (c5 = 1; c5 <= 16; c5++) {
      w = A[c5][0];
      A[c5][0] = w / A[0][0];
    }
    for (c5 = 16; c5 <= 106; c5 = c5 + 6) {
      w = A[0 + 1][(c5 + -10) / 6];
      w = w - A[0 + 1][0] * A[0][(c5 + -10) / 6];
      A[0 + 1][(c5 + -10) / 6] = w;
    }
    for (c2 = 1; c2 <= 15; c2++) {
      for (c5 = c2 + 1; c5 <= 16; c5++) {
        w = A[c5][c2];
        for (c6 = 0; c6 <= c2 + -1; c6++) {
          w = w - A[c5][c6] * A[c6][c2];
        }
        A[c5][c2] = w / A[c2][c2];
      }
      for (c5 = 6 * c2 + 16; c5 <= 106; c5 = c5 + 6) {
        w = A[c2 + 1][(c5 + -10) / 6];
        for (c6 = c2; c6 <= 2 * c2; c6++) {
          w = w - A[c2 + 1][-1 * c2 + c6] * A[-1 * c2 + c6][(c5 + -10) / 6];
        }
        A[c2 + 1][(c5 + -10) / 6] = w;
      }
    }
    b[0] = 1.0;
    y[0] = b[0];
    for (c2 = 1; c2 <= 16; c2++) {
      w = b[c2];
      for (c5 = 0; c5 <= c2 + -1; c5++) {
        w = w - A[c2][c5] * y[c5];
      }
      y[c2] = w;
    }
    x[16] = y[16] / A[16][16];
    for (c2 = 0; c2 <= 15; c2++) {
      w = y[16 - 1 - c2];
      for (c5 = -1 * c2 + 16; c5 <= 16; c5++) {
        w = w - A[16 - 1 - c2][c5] * x[c5];
      }
      x[16 - 1 - c2] = w / A[16 - 1 - c2][16 - 1 - c2];
    }
  }
  
#pragma endscop
  return 0;
}
