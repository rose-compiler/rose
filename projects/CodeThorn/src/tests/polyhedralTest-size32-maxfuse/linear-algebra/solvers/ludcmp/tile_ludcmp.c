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
  int n = 16;
/* Variable declaration/allocation. */
  double A[16 + 1][16 + 1];
  double b[16 + 1];
  double x[16 + 1];
  double y[16 + 1];
  int i;
  int j;
  int k;
  double w;
  
#pragma scop
{
    int c5;
    int c2;
    int c6;
{
      int c0;
      for (c0 = 1; c0 <= 16; c0++) {
        w = A[c0][0];
        A[c0][0] = w / A[0][0];
      }
    }
    for (c5 = 16; c5 <= 106; c5 = c5 + 6) {
      w = A[0 + 1][(c5 + - 10) / 6];
      w = w - A[0 + 1][0] * A[0][(c5 + - 10) / 6];
      A[0 + 1][(c5 + - 10) / 6] = w;
    }
    for (c2 = 1; c2 <= 15; c2++) {{
        int c1;
        int c0;
        if (c2 <= 15) {
          if (c2 >= 1) {
            for (c0 = c2 + 1; c0 <= 16; c0++) {
              w = A[c0][c2];
              for (c1 = 2; c1 <= c2 + 1; c1++) {
                w = w - A[c0][c1 + -2] * A[c1 + -2][c2];
              }
              A[c0][c2] = w / A[c2][c2];
            }
          }
          if (c2 <= 0) {
            for (c0 = c2 + 1; c0 <= 16; c0++) {
              if (c0 == 2) {
                w = A[2][c2];
                A[2][c2] = w / A[c2][c2];
              }
              if (c0 >= 3) {
                w = A[c0][c2];
              }
              if (c0 >= 3) {
                A[c0][c2] = w / A[c2][c2];
              }
              if (c0 <= 1) {
                A[c0][c2] = w / A[c2][c2];
              }
              if (c0 <= 1) {
                w = A[c0][c2];
              }
            }
          }
        }
      }
      for (c5 = 6 * c2 + 16; c5 <= 106; c5 = c5 + 6) {
        w = A[c2 + 1][(c5 + - 10) / 6];
        for (c6 = c2; c6 <= 2 * c2; c6++) {
          w = w - A[c2 + 1][- 1 * c2 + c6] * A[- 1 * c2 + c6][(c5 + - 10) / 6];
        }
        A[c2 + 1][(c5 + - 10) / 6] = w;
      }
    }
{
      int c3;
      int c7;
      b[0] = 1.0;
      y[0] = b[0];
      for (c3 = 1; c3 <= 16; c3++) {
        w = b[c3];
        for (c7 = 0; c7 <= c3 + -1; c7++) {
          w = w - A[c3][c7] * y[c7];
        }
        y[c3] = w;
      }
      x[16] = y[16] / A[16][16];
      for (c3 = 0; c3 <= 15; c3++) {
        w = y[16 - 1 - c3];
        for (c7 = -1 * c3 + 16; c7 <= 16; c7++) {
          w = w - A[16 - 1 - c3][c7] * x[c7];
        }
        x[16 - 1 - c3] = w / A[16 - 1 - c3][16 - 1 - c3];
      }
    }
  }
  
#pragma endscop
  return 0;
}
