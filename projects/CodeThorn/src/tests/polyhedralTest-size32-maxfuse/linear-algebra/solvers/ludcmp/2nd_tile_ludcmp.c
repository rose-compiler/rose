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
{
      int c12;
      int c11;
      int c9;
      b[0] = 1.0;
      y[0] = b[0];
      for (c9 = 1; c9 <= 32; c9++) {
        w = A[c9][0];
        A[c9][0] = w / A[0][0];
      }
      for (c9 = 1; c9 <= 32; c9++) {
        w = A[0 + 1][c9];
        w = w - A[0 + 1][0] * A[0][c9];
        A[0 + 1][c9] = w;
      }
      for (c9 = 1; c9 <= 31; c9++) {
        for (c11 = c9 + 1; c11 <= 32; c11++) {
          w = A[c11][c9];
          for (c12 = 1; c12 <= c9; c12++) {
            w = w - A[c11][c12 + -1] * A[c12 + -1][c9];
          }
          A[c11][c9] = w / A[c9][c9];
        }
        for (c11 = 12 * c9 + 20; c11 <= 2 * c9 + 330; c11 = c11 + 10) {
          w = A[c9 + 1][(-2 * c9 + c11 + -10) / 10];
          for (c12 = c9; c12 <= 2 * c9; c12++) {
            w = w - A[c9 + 1][-1 * c9 + c12] * A[-1 * c9 + c12][(-2 * c9 + c11 + -10) / 10];
          }
          A[c9 + 1][(-2 * c9 + c11 + -10) / 10] = w;
        }
      }
      for (c9 = 1; c9 <= 32; c9++) {
        w = b[c9];
        for (c11 = 0; c11 <= c9 + -1; c11++) {
          w = w - A[c9][c11] * y[c11];
        }
        y[c9] = w;
      }
      x[32] = y[32] / A[32][32];
      for (c9 = 0; c9 <= 31; c9++) {
        w = y[32 - 1 - c9];
        for (c11 = -1 * c9 + 32; c11 <= 32; c11++) {
          w = w - A[32 - 1 - c9][c11] * x[c11];
        }
        x[32 - 1 - c9] = w / A[32 - 1 - c9][32 - 1 - c9];
      }
    }
  }
  
#pragma endscop
  return 0;
}
