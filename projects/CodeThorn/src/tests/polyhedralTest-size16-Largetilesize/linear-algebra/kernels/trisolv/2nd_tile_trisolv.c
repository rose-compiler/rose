#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * trisolv.c: This file is part of the PolyBench/C 3.2 test suite.
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
  double x[16];
  double c[16];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c3;
{
      int c5;
      int c2;
      x[0] = c[0];
      x[0] = x[0] / A[0][0];
      for (c5 = 1; c5 <= 15; c5++) {
        x[c5] = c[c5];
      }
#pragma omp parallel for private(c5)
      for (c2 = 3; c2 <= 17; c2++) {
        if (c2 <= 15) {
          if (c2 <= 4) {
            x[c2 + -2] = x[c2 + -2] - A[c2 + -2][c2 + -2 + - 1] * x[c2 + -2 + - 1];
            x[c2 + -2] = x[c2 + -2] / A[c2 + -2][c2 + -2];
          }
          if (c2 == 3) {
            x[1 + 1] = x[1 + 1] - A[1 + 1][1 + - 1] * x[1 + - 1];
          }
          for (c5 = 3; c5 <= c2 + -3; c5++) {
            x[c2] = x[c2] - A[c2][c5 + -2 + - 1] * x[c5 + -2 + - 1];
          }
          if (c2 >= 5) {
            x[c2] = x[c2] - A[c2][c2 + -4 + - 1] * x[c2 + -4 + - 1];
            x[c2 + -2] = x[c2 + -2] - A[c2 + -2][c2 + -2 + - 1] * x[c2 + -2 + - 1];
            x[c2 + -2] = x[c2 + -2] / A[c2 + -2][c2 + -2];
          }
          if (c2 >= 4) {
            x[c2] = x[c2] - A[c2][c2 + -3 + - 1] * x[c2 + -3 + - 1];
            x[c2 + -2 + 1] = x[c2 + -2 + 1] - A[c2 + -2 + 1][c2 + -2 + - 1] * x[c2 + -2 + - 1];
          }
          x[c2] = x[c2] - A[c2][c2 + -2 + - 1] * x[c2 + -2 + - 1];
        }
        if (c2 == 16) {
          x[14] = x[14] - A[14][14 + - 1] * x[14 + - 1];
          x[14] = x[14] / A[14][14];
          x[14 + 1] = x[14 + 1] - A[14 + 1][14 + - 1] * x[14 + - 1];
        }
        if (c2 == 17) {
          x[15] = x[15] - A[15][15 + - 1] * x[15 + - 1];
        }
      }
      x[15] = x[15] / A[15][15];
    }
  }
  
#pragma endscop
  return 0;
}
