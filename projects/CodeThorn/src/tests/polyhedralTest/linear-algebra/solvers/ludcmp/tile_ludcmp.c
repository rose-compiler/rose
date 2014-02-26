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
{
      int c9;
      int c12;
      int c13;
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
        for (c12 = c9 + 1; c12 <= 32; c12++) {
          w = A[c12][c9];
          for (c13 = 1; c13 <= c9; c13++) {
            w = w - A[c12][c13 + -1] * A[c13 + -1][c9];
          }
          A[c12][c9] = w / A[c9][c9];
        }
        for (c12 = 12 * c9 + 20; c12 <= 2 * c9 + 330; c12 = c12 + 10) {
          w = A[c9 + 1][(-2 * c9 + c12 + -10) / 10];
          for (c13 = c9; c13 <= 2 * c9; c13++) {
            w = w - A[c9 + 1][-1 * c9 + c13] * A[-1 * c9 + c13][(-2 * c9 + c12 + -10) / 10];
          }
          A[c9 + 1][(-2 * c9 + c12 + -10) / 10] = w;
        }
      }
      for (c9 = 1; c9 <= 32; c9++) {
        w = b[c9];
        for (c12 = 0; c12 <= c9 + -1; c12++) {
          w = w - A[c9][c12] * y[c12];
        }
        y[c9] = w;
      }
      x[32] = y[32] / A[32][32];
      for (c9 = 0; c9 <= 31; c9++) {
        w = y[32 - 1 - c9];
        for (c12 = -1 * c9 + 32; c12 <= 32; c12++) {
          w = w - A[32 - 1 - c9][c12] * x[c12];
        }
        x[32 - 1 - c9] = w / A[32 - 1 - c9][32 - 1 - c9];
      }
    }
  }
  
#pragma endscop
  return 0;
}
