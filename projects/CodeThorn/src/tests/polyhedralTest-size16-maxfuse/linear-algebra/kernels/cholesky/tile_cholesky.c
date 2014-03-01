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
        if (i <= 14) {
          for (c2 = i + 1; c2 <= 1; c2++) {
            A[c2][i] = x * p[i];
            x = A[i][c2];
          }
          if (i <= 0) {
            x = A[i][2];
            A[2][i] = x * p[i];
          }
          if (i <= 0) {
            for (c2 = 3; c2 <= 15; c2++) {
              x = A[i][c2];
              A[c2][i] = x * p[i];
            }
          }
          if (i >= 1) {
            for (c2 = i + 1; c2 <= 15; c2++) {
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
