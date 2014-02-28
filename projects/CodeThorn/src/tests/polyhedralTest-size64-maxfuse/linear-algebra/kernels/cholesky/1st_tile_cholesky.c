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
  int n = 64;
/* Variable declaration/allocation. */
  double A[64][64];
  double p[64];
  int i;
  int j;
  int k;
  double x;
  
#pragma scop
  for (i = 0; i < 64; ++i) {{
      int c2;
      x = A[i][i];
      for (c2 = 0; c2 <= i + -1; c2++) {
        x = x - A[i][c2] * A[i][c2];
      }
    }
    p[i] = 1.0 / sqrt(x);
{
      int c1;
      int c0;
      int c2;
      if (i <= 62) {
        for (c0 = ((i + -6) * 8 < 0?-(-(i + -6) / 8) : ((8 < 0?(-(i + -6) + - 8 - 1) / - 8 : (i + -6 + 8 - 1) / 8))); c0 <= 7; c0++) {
          if (i >= 1) {
            for (c1 = (8 * c0 > i + 1?8 * c0 : i + 1); c1 <= 8 * c0 + 7; c1++) {
              x = A[i][c1];
              for (c2 = 2; c2 <= i + 1; c2++) {
                x = x - A[c1][c2 + -2] * A[i][c2 + -2];
              }
              A[c1][i] = x * p[i];
            }
          }
          if (i <= 0) {
            for (c1 = (8 * c0 > i + 1?8 * c0 : i + 1); c1 <= 8 * c0 + 7; c1++) {
              if (c0 == 0 && c1 == 2) {
                x = A[i][2];
                A[2][i] = x * p[i];
              }
              if (c1 >= 3) {
                x = A[i][c1];
              }
              if (c1 >= 3) {
                A[c1][i] = x * p[i];
              }
              if (c1 <= 1) {
                A[c1][i] = x * p[i];
              }
              if (c1 <= 1) {
                x = A[i][c1];
              }
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
