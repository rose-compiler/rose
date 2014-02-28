#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * cholesky.c: This file is part of the PolyBench/C 3.2 test suite.
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
  double A[16][16];
  double p[16];
  int i;
  int j;
  int k;
  double x;
  
#pragma scop
  for (i = 0; i < 16; ++i) {{
      int c2;
      x = A[i][i];
      for (c2 = 0; c2 <= i + -1; c2++) {
        x = x - A[i][c2] * A[i][c2];
      }
    }
    p[i] = 1.0 / sqrt(x);
{
      int c2;
      int c0;
      if (i <= 14) {
        for (c0 = i + 1; c0 <= 15; c0++) {
          if (i >= 1) {
            x = A[i][c0];
            for (c2 = 2; c2 <= i + 1; c2++) {
              x = x - A[c0][c2 + -2] * A[i][c2 + -2];
            }
            A[c0][i] = x * p[i];
          }
          if (i <= 0) {
            if (c0 == 2) {
              x = A[i][2];
              A[2][i] = x * p[i];
            }
            if (c0 >= 3) {
              x = A[i][c0];
            }
            if (c0 >= 3) {
              A[c0][i] = x * p[i];
            }
            if (c0 <= 1) {
              A[c0][i] = x * p[i];
            }
            if (c0 <= 1) {
              x = A[i][c0];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
