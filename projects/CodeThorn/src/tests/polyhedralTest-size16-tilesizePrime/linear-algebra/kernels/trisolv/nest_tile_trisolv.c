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
    int c4;
    int c2;
    int c3;
    for (c1 = 0; c1 <= 3; c1++) {
      for (c3 = 5 * c1; c3 <= ((15 < 5 * c1 + 4?15 : 5 * c1 + 4)); c3++) {
        x[c3] = c[c3];
      }
    }
    for (c1 = 0; c1 <= 5; c1++) {
      c2 = (0 > c1 + -3?0 : c1 + -3);
      if (c2 <= (((5 * c1 + -8) * 12 < 0?((12 < 0?-((-(5 * c1 + -8) + 12 + 1) / 12) : -((-(5 * c1 + -8) + 12 - 1) / 12))) : (5 * c1 + -8) / 12))) {
        for (c3 = 5 * c1 + -5 * c2; c3 <= ((15 < 5 * c1 + -5 * c2 + 4?15 : 5 * c1 + -5 * c2 + 4)); c3++) {
          for (c4 = (1 > 7 * c2?1 : 7 * c2); c4 <= 7 * c2 + 6; c4++) {
            x[c3] = x[c3] - A[c3][c4 + -1] * x[c4 + -1];
          }
        }
      }
      if (c1 == 4) {
        x[14] = x[14] - A[14][13] * x[13];
      }
      if ((5 * c1 + 3) % 12 <= 10) {
        c2 = ((5 * c1 + 3) * 12 < 0?((12 < 0?-((-(5 * c1 + 3) + 12 + 1) / 12) : -((-(5 * c1 + 3) + 12 - 1) / 12))) : (5 * c1 + 3) / 12);
        if (c1 == 2 && c2 == 1) {
          x[7] = x[7] - A[7][6] * x[6];
        }
        for (c3 = (5 * c1 + -5 * c2 > 7 * c2 + 1?5 * c1 + -5 * c2 : 7 * c2 + 1); c3 <= ((((15 < 7 * c2 + 7?15 : 7 * c2 + 7)) < 5 * c1 + -5 * c2 + 4?((15 < 7 * c2 + 7?15 : 7 * c2 + 7)) : 5 * c1 + -5 * c2 + 4)); c3++) {
          for (c4 = (1 > 7 * c2?1 : 7 * c2); c4 <= c3 + -2; c4++) {
            x[c3] = x[c3] - A[c3][c4 + -1] * x[c4 + -1];
          }
          if (c1 == 0 && c2 == 0 && c3 == 1) {
            x[0] = x[0] / A[0][0];
          }
          if (c3 >= 2) {
            x[c3 + -1] = x[c3 + -1] / A[c3 + -1][c3 + -1];
            x[c3] = x[c3] - A[c3][c3 + -2] * x[c3 + -2];
          }
          if (c2 >= (((c3 + -6) * 7 < 0?-(-(c3 + -6) / 7) : ((7 < 0?(-(c3 + -6) + - 7 - 1) / - 7 : (c3 + -6 + 7 - 1) / 7))))) {
            x[c3] = x[c3] - A[c3][c3 + -1] * x[c3 + -1];
          }
        }
        if (c1 == 1 && c2 == 0) {
          for (c3 = 8; c3 <= 9; c3++) {
            for (c4 = 1; c4 <= 6; c4++) {
              x[c3] = x[c3] - A[c3][c4 + -1] * x[c4 + -1];
            }
          }
        }
        if (c1 == 5 && c2 == 2) {
          x[15] = x[15] / A[15][15];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
