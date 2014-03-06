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
    int c4;
    int c3;
    int c1;
    for (c1 = 0; c1 <= 2; c1++) {
      for (c3 = 7 * c1; c3 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c3++) {
        x[c3] = c[c3];
      }
    }
    for (c1 = 0; c1 <= 4; c1++) {
      if (c1 == 2) {
        for (c3 = 14; c3 <= 15; c3++) {
          for (c4 = 1; c4 <= 6; c4++) {
            x[c3] = x[c3] - A[c3][c4 + -1] * x[c4 + -1];
          }
        }
      }
      c2 = (c1 * 2 < 0?((2 < 0?-((-c1 + 2 + 1) / 2) : -((-c1 + 2 - 1) / 2))) : c1 / 2);
      if (c1 >= 2 && c1 == 2 * c2) {
        if (c1 % 2 == 0) {
          x[7 * c1 / 2] = x[7 * c1 / 2] - A[7 * c1 / 2][(7 * c1 + -2) / 2] * x[(7 * c1 + -2) / 2];
        }
      }
      for (c3 = (7 * c1 + -7 * c2 > 7 * c2 + 1?7 * c1 + -7 * c2 : 7 * c2 + 1); c3 <= ((((15 < 7 * c2 + 7?15 : 7 * c2 + 7)) < 7 * c1 + -7 * c2 + 6?((15 < 7 * c2 + 7?15 : 7 * c2 + 7)) : 7 * c1 + -7 * c2 + 6)); c3++) {
        for (c4 = (1 > 7 * c2?1 : 7 * c2); c4 <= c3 + -2; c4++) {
          x[c3] = x[c3] - A[c3][c4 + -1] * x[c4 + -1];
        }
        if (c1 == 0 && c2 == 0 && c3 == 1) {
          x[0] = x[0] / A[0][0];
        }
        if (c3 >= 2) {
          x[c3 + -1] = x[c3 + -1] / A[c3 + -1][c3 + -1];
        }
        if (c3 >= 2) {
          x[c3] = x[c3] - A[c3][c3 + -2] * x[c3 + -2];
        }
        if (c1 == 2 * c2) {
          if (c1 % 2 == 0) {
            x[c3] = x[c3] - A[c3][c3 + -1] * x[c3 + -1];
          }
        }
      }
      if (c1 == 2 * c2 + 1) {
        for (c3 = ((7 * c1 + 9) * 2 < 0?-(-(7 * c1 + 9) / 2) : ((2 < 0?(-(7 * c1 + 9) + - 2 - 1) / - 2 : (7 * c1 + 9 + 2 - 1) / 2))); c3 <= ((15 < (((7 * c1 + 19) * 2 < 0?((2 < 0?-((-(7 * c1 + 19) + 2 + 1) / 2) : -((-(7 * c1 + 19) + 2 - 1) / 2))) : (7 * c1 + 19) / 2))?15 : (((7 * c1 + 19) * 2 < 0?((2 < 0?-((-(7 * c1 + 19) + 2 + 1) / 2) : -((-(7 * c1 + 19) + 2 - 1) / 2))) : (7 * c1 + 19) / 2)))); c3++) {
          for (c4 = (1 > (((7 * c1 + -7) * 2 < 0?-(-(7 * c1 + -7) / 2) : ((2 < 0?(-(7 * c1 + -7) + - 2 - 1) / - 2 : (7 * c1 + -7 + 2 - 1) / 2))))?1 : (((7 * c1 + -7) * 2 < 0?-(-(7 * c1 + -7) / 2) : ((2 < 0?(-(7 * c1 + -7) + - 2 - 1) / - 2 : (7 * c1 + -7 + 2 - 1) / 2))))); c4 <= (((7 * c1 + 5) * 2 < 0?((2 < 0?-((-(7 * c1 + 5) + 2 + 1) / 2) : -((-(7 * c1 + 5) + 2 - 1) / 2))) : (7 * c1 + 5) / 2)); c4++) {
            if ((c1 + 1) % 2 == 0) {
              x[c3] = x[c3] - A[c3][c4 + -1] * x[c4 + -1];
            }
          }
        }
      }
      if (c1 == 4 && c2 == 2) {
        x[15] = x[15] / A[15][15];
      }
    }
  }
  
#pragma endscop
  return 0;
}
