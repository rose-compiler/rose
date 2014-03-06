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
      int c1;
      if (i <= 30) {
        for (c0 = (i * 2 < 0?-(-i / 2) : ((2 < 0?(-i + - 2 - 1) / - 2 : (i + 2 - 1) / 2))); c0 <= 15; c0++) {
          if (i >= 1) {
            for (c1 = (2 * c0 > i + 1?2 * c0 : i + 1); c1 <= 2 * c0 + 1; c1++) {
              x = A[i][c1];
              for (c2 = 2; c2 <= i + 1; c2++) {
                x = x - A[c1][c2 + -2] * A[i][c2 + -2];
              }
              A[c1][i] = x * p[i];
            }
          }
          if (i <= 0) {
            for (c1 = (2 * c0 > i + 1?2 * c0 : i + 1); c1 <= 2 * c0 + 1; c1++) {
              if (c0 == 1 && c1 == 2) {
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
