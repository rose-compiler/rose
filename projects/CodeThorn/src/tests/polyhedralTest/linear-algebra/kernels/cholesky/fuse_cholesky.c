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
/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "cholesky.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 32;
/* Variable declaration/allocation. */
  double A[32][32];
  double p[32];
  int i;
  int j;
  int k;
  double x;
  
#pragma scop
  for (i = 0; i < 32; ++i) {{
      int c0;
      x = A[i][i];
      for (c0 = 0; c0 <= i + -1; c0++) {
        x = x - A[i][c0] * A[i][c0];
      }
    }
    p[i] = 1.0 / sqrt(x);
{
      int c1;
      int c0;
      if (i <= 30) {
        if (i >= 1) {
          for (c0 = i + 1; c0 <= 31; c0++) {
            x = A[i][c0];
            for (c1 = 2; c1 <= i + 1; c1++) {
              x = x - A[c0][c1 + -2] * A[i][c1 + -2];
            }
            A[c0][i] = x * p[i];
          }
        }
        if (i <= 0) {
          for (c0 = i + 1; c0 <= 31; c0++) {
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
