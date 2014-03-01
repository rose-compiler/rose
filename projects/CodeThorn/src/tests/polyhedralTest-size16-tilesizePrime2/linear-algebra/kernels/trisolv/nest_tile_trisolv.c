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
    int c2;
    int c3;
    int c4;
    int c1;
    for (c1 = 0; c1 <= 1; c1++) {
      for (c3 = 13 * c1; c3 <= ((15 < 13 * c1 + 12?15 : 13 * c1 + 12)); c3++) {
        x[c3] = c[c3];
      }
    }
    for (c1 = 0; c1 <= 2; c1++) {
      c2 = (0 > c1 + -1?0 : c1 + -1);
      if (c2 <= ((c1 * 2 < 0?((2 < 0?-((-c1 + 2 + 1) / 2) : -((-c1 + 2 - 1) / 2))) : c1 / 2))) {
        if (c1 == 2 && c2 == 1) {
          x[13] = x[13] - A[13][12] * x[12];
        }
        for (c3 = (13 * c1 + -13 * c2 > 13 * c2 + 1?13 * c1 + -13 * c2 : 13 * c2 + 1); c3 <= ((((15 < 13 * c2 + 13?15 : 13 * c2 + 13)) < 13 * c1 + -13 * c2 + 12?((15 < 13 * c2 + 13?15 : 13 * c2 + 13)) : 13 * c1 + -13 * c2 + 12)); c3++) {
          for (c4 = (1 > 13 * c2?1 : 13 * c2); c4 <= c3 + -2; c4++) {
            x[c3] = x[c3] - A[c3][c4 + -1] * x[c4 + -1];
          }
          if (c1 == 0 && c2 == 0 && c3 == 1) {
            x[0] = x[0] / A[0][0];
          }
          if (c3 >= 2) {
            x[c3 + -1] = x[c3 + -1] / A[c3 + -1][c3 + -1];
            x[c3] = x[c3] - A[c3][c3 + -2] * x[c3 + -2];
          }
          if (c1 == 2 * c2) {
            if (c1 % 2 == 0) {
              x[c3] = x[c3] - A[c3][c3 + -1] * x[c3 + -1];
            }
          }
        }
        if (c1 == 1 && c2 == 0) {
          for (c3 = 14; c3 <= 15; c3++) {
            for (c4 = 1; c4 <= 12; c4++) {
              x[c3] = x[c3] - A[c3][c4 + -1] * x[c4 + -1];
            }
          }
        }
        if (c1 == 2 && c2 == 1) {
          x[15] = x[15] / A[15][15];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
