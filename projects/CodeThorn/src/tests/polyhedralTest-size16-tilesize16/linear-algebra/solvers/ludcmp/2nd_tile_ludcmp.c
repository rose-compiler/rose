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
    int c3;
    int c1;
    int c4;
{
      int c0;
      for (c0 = 1; c0 <= 16; c0++) {
        w = A[c0][0];
        A[c0][0] = w / A[0][0];
      }
    }
    for (c3 = 16; c3 <= 106; c3 = c3 + 6) {
      w = A[0 + 1][(c3 + - 10) / 6];
      w = w - A[0 + 1][0] * A[0][(c3 + - 10) / 6];
      A[0 + 1][(c3 + - 10) / 6] = w;
    }
    for (c1 = 1; c1 <= 15; c1++) {{
        int c5;
        int c2;
        int c0;
        if (c1 <= 15) {
          for (c0 = ((c1 + -14) * 16 < 0?-(-(c1 + -14) / 16) : ((16 < 0?(-(c1 + -14) + - 16 - 1) / - 16 : (c1 + -14 + 16 - 1) / 16))); c0 <= 1; c0++) {
            if (c1 >= 1) {
              for (c2 = (16 * c0 > c1 + 1?16 * c0 : c1 + 1); c2 <= ((16 < 16 * c0 + 15?16 : 16 * c0 + 15)); c2++) {
                w = A[c2][c1];
                for (c5 = 2; c5 <= c1 + 1; c5++) {
                  w = w - A[c2][c5 + -2] * A[c5 + -2][c1];
                }
                A[c2][c1] = w / A[c1][c1];
              }
            }
            if (c1 <= 0) {
              for (c2 = (16 * c0 > c1 + 1?16 * c0 : c1 + 1); c2 <= ((16 < 16 * c0 + 15?16 : 16 * c0 + 15)); c2++) {
                if (c0 == 0 && c2 == 2) {
                  w = A[2][c1];
                  A[2][c1] = w / A[c1][c1];
                }
                if (c2 >= 3) {
                  w = A[c2][c1];
                }
                if (c2 >= 3) {
                  A[c2][c1] = w / A[c1][c1];
                }
                if (c2 <= 1) {
                  A[c2][c1] = w / A[c1][c1];
                }
                if (c2 <= 1) {
                  w = A[c2][c1];
                }
              }
            }
          }
        }
      }
      for (c3 = 6 * c1 + 16; c3 <= 106; c3 = c3 + 6) {
        w = A[c1 + 1][(c3 + - 10) / 6];
        for (c4 = c1; c4 <= 2 * c1; c4++) {
          w = w - A[c1 + 1][- 1 * c1 + c4] * A[- 1 * c1 + c4][(c3 + - 10) / 6];
        }
        A[c1 + 1][(c3 + - 10) / 6] = w;
      }
    }
{
      int c2;
      int c5;
      b[0] = 1.0;
      y[0] = b[0];
      for (c2 = 1; c2 <= 16; c2++) {
        w = b[c2];
        for (c5 = 0; c5 <= c2 + -1; c5++) {
          w = w - A[c2][c5] * y[c5];
        }
        y[c2] = w;
      }
      x[16] = y[16] / A[16][16];
      for (c2 = 0; c2 <= 15; c2++) {
        w = y[16 - 1 - c2];
        for (c5 = -1 * c2 + 16; c5 <= 16; c5++) {
          w = w - A[16 - 1 - c2][c5] * x[c5];
        }
        x[16 - 1 - c2] = w / A[16 - 1 - c2][16 - 1 - c2];
      }
    }
  }
  
#pragma endscop
  return 0;
}
