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
{
        int c3;
        x = A[i][i];
        for (c3 = 0; c3 <= i + -1; c3++) {
          x = x - A[i][c3] * A[i][c3];
        }
      }
    }
    p[i] = 1.0 / sqrt(x);
{
      int c2;
      int c0;
{
        int c4;
        int c5;
        if (i <= 14) {
          for (c4 = i + 1; c4 <= 1; c4++) {
            A[c4][i] = x * p[i];
            x = A[i][c4];
          }
          if (i <= 0) {
            x = A[i][2];
            A[2][i] = x * p[i];
          }
          if (i <= 0) {
            for (c4 = 3; c4 <= 15; c4++) {
              x = A[i][c4];
              A[c4][i] = x * p[i];
            }
          }
          if (i >= 1) {
            for (c4 = i + 1; c4 <= 15; c4++) {
              x = A[i][c4];
              for (c5 = 2; c5 <= i + 1; c5++) {
                x = x - A[c4][c5 + - 2] * A[i][c5 + - 2];
              }
              A[c4][i] = x * p[i];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
