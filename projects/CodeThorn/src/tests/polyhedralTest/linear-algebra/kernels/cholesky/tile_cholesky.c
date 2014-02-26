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
{
        int c1;
        x = A[i][i];
        for (c1 = 0; c1 <= i + -1; c1++) {
          x = x - A[i][c1] * A[i][c1];
        }
      }
    }
    p[i] = 1.0 / sqrt(x);
{
      int c1;
      int c0;
{
        int c2;
        int c3;
        if (i <= 30) {
          for (c2 = i + 1; c2 <= 1; c2++) {
            A[c2][i] = x * p[i];
            x = A[i][c2];
          }
          if (i <= 0) {
            x = A[i][2];
            A[2][i] = x * p[i];
          }
          if (i <= 0) {
            for (c2 = 3; c2 <= 31; c2++) {
              x = A[i][c2];
              A[c2][i] = x * p[i];
            }
          }
          if (i >= 1) {
            for (c2 = i + 1; c2 <= 31; c2++) {
              x = A[i][c2];
              for (c3 = 2; c3 <= i + 1; c3++) {
                x = x - A[c2][c3 + - 2] * A[i][c3 + - 2];
              }
              A[c2][i] = x * p[i];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
