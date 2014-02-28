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
  int n = 64;
/* Variable declaration/allocation. */
  double A[64 + 1][64 + 1];
  double b[64 + 1];
  double x[64 + 1];
  double y[64 + 1];
  int i;
  int j;
  int k;
  double w;
  
#pragma scop
{
    int c7;
    int c1;
    int c8;
{
      int c9;
      int c16;
      int c17;
      b[0] = 1.0;
      y[0] = b[0];
      for (c9 = 1; c9 <= 64; c9++) {
        w = A[c9][0];
        A[c9][0] = w / A[0][0];
      }
      for (c9 = 1; c9 <= 64; c9++) {
        w = A[0 + 1][c9];
        w = w - A[0 + 1][0] * A[0][c9];
        A[0 + 1][c9] = w;
      }
      for (c9 = 1; c9 <= 63; c9++) {
        for (c16 = c9 + 1; c16 <= 64; c16++) {
          w = A[c16][c9];
          for (c17 = 0; c17 <= c9 + -1; c17++) {
            w = w - A[c16][c17] * A[c17][c9];
          }
          A[c16][c9] = w / A[c9][c9];
        }
        for (c16 = c9 + 1; c16 <= 64; c16++) {
          w = A[c9 + 1][c16];
          for (c17 = 0; c17 <= c9; c17++) {
            w = w - A[c9 + 1][c17] * A[c17][c16];
          }
          A[c9 + 1][c16] = w;
        }
      }
      for (c9 = 1; c9 <= 64; c9++) {
        w = b[c9];
        for (c16 = 0; c16 <= c9 + -1; c16++) {
          w = w - A[c9][c16] * y[c16];
        }
        y[c9] = w;
      }
      x[64] = y[64] / A[64][64];
      for (c9 = 0; c9 <= 63; c9++) {
        w = y[64 - 1 - c9];
        for (c16 = -1 * c9 + 64; c16 <= 64; c16++) {
          w = w - A[64 - 1 - c9][c16] * x[c16];
        }
        x[64 - 1 - c9] = w / A[64 - 1 - c9][64 - 1 - c9];
      }
    }
  }
  
#pragma endscop
  return 0;
}
