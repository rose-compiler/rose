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
/* Include benchmark-specific header. */
/* Default data type is double, default size is 1024. */
#include "ludcmp.h"

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
    int c2;
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
    for (c2 = 1; c2 <= 31; c2++) {
      for (c7 = c2 + 1; c7 <= 32; c7++) {
        w = A[c7][c2];
        for (c8 = 0; c8 <= c2 + -1; c8++) {
          w = w - A[c7][c8] * A[c8][c2];
        }
        A[c7][c2] = w / A[c2][c2];
      }
      for (c7 = c2 + 1; c7 <= 32; c7++) {
        w = A[c2 + 1][c7];
        for (c8 = 0; c8 <= c2; c8++) {
          w = w - A[c2 + 1][c8] * A[c8][c7];
        }
        A[c2 + 1][c7] = w;
      }
    }
    b[0] = 1.0;
    y[0] = b[0];
    for (c2 = 1; c2 <= 32; c2++) {
      w = b[c2];
      for (c7 = 0; c7 <= c2 + -1; c7++) {
        w = w - A[c2][c7] * y[c7];
      }
      y[c2] = w;
    }
    x[32] = y[32] / A[32][32];
    for (c2 = 0; c2 <= 31; c2++) {
      w = y[32 - 1 - c2];
      for (c7 = -1 * c2 + 32; c7 <= 32; c7++) {
        w = w - A[32 - 1 - c2][c7] * x[c7];
      }
      x[32 - 1 - c2] = w / A[32 - 1 - c2][32 - 1 - c2];
    }
  }
  
#pragma endscop
  return 0;
}
