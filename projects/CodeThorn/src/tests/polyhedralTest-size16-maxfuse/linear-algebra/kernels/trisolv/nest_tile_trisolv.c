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
    int c2;
    int c3;
    int c4;
    for (c1 = 0; c1 <= 7; c1++) {
      for (c3 = 2 * c1; c3 <= 2 * c1 + 1; c3++) {
        x[c3] = c[c3];
      }
    }
    for (c1 = 0; c1 <= 11; c1++) {
#pragma omp parallel for private(c4, c3)
      for (c2 = (0 > c1 + -7?0 : c1 + -7); c2 <= (((c1 + -3) * 3 < 0?((3 < 0?-((-(c1 + -3) + 3 + 1) / 3) : -((-(c1 + -3) + 3 - 1) / 3))) : (c1 + -3) / 3)); c2++) {
        for (c3 = 2 * c1 + -2 * c2; c3 <= 2 * c1 + -2 * c2 + 1; c3++) {
          for (c4 = (1 > 4 * c2?1 : 4 * c2); c4 <= 4 * c2 + 3; c4++) {
            x[c3] = x[c3] - A[c3][c4 + -1] * x[c4 + -1];
          }
        }
      }
      if (c1 == 11) {
        x[15] = x[15] / A[15][15];
      }
      c2 = ((((c1 + -2) * 3 < 0?-(-(c1 + -2) / 3) : ((3 < 0?(-(c1 + -2) + - 3 - 1) / - 3 : (c1 + -2 + 3 - 1) / 3)))) > c1 + -7?(((c1 + -2) * 3 < 0?-(-(c1 + -2) / 3) : ((3 < 0?(-(c1 + -2) + - 3 - 1) / - 3 : (c1 + -2 + 3 - 1) / 3)))) : c1 + -7);
      if (c2 <= ((c1 * 3 < 0?((3 < 0?-((-c1 + 3 + 1) / 3) : -((-c1 + 3 - 1) / 3))) : c1 / 3))) {
        if (c1 >= 3 && c1 == 3 * c2) {
          if (c1 % 3 == 0) {
            x[4 * c1 / 3] = x[4 * c1 / 3] - A[4 * c1 / 3][(4 * c1 + -3) / 3] * x[(4 * c1 + -3) / 3];
          }
        }
        for (c3 = (2 * c1 + -2 * c2 > 4 * c2 + 1?2 * c1 + -2 * c2 : 4 * c2 + 1); c3 <= ((4 * c2 + 4 < 2 * c1 + -2 * c2 + 1?4 * c2 + 4 : 2 * c1 + -2 * c2 + 1)); c3++) {
          for (c4 = (1 > 4 * c2?1 : 4 * c2); c4 <= c3 + -2; c4++) {
            x[c3] = x[c3] - A[c3][c4 + -1] * x[c4 + -1];
          }
          if (c1 == 0 && c2 == 0 && c3 == 1) {
            x[0] = x[0] / A[0][0];
          }
          if (c3 >= 2) {
            x[c3 + -1] = x[c3 + -1] / A[c3 + -1][c3 + -1];
            x[c3] = x[c3] - A[c3][c3 + -2] * x[c3 + -2];
          }
          if (c2 >= (((c3 + -3) * 4 < 0?-(-(c3 + -3) / 4) : ((4 < 0?(-(c3 + -3) + - 4 - 1) / - 4 : (c3 + -3 + 4 - 1) / 4))))) {
            x[c3] = x[c3] - A[c3][c3 + -1] * x[c3 + -1];
          }
        }
        if (c1 == 3 * c2 + 2) {
          if ((c1 + 1) % 3 == 0) {
            for (c4 = (1 > (((4 * c1 + -8) * 3 < 0?-(-(4 * c1 + -8) / 3) : ((3 < 0?(-(4 * c1 + -8) + - 3 - 1) / - 3 : (4 * c1 + -8 + 3 - 1) / 3))))?1 : (((4 * c1 + -8) * 3 < 0?-(-(4 * c1 + -8) / 3) : ((3 < 0?(-(4 * c1 + -8) + - 3 - 1) / - 3 : (4 * c1 + -8 + 3 - 1) / 3))))); c4 <= (((4 * c1 + 1) * 3 < 0?((3 < 0?-((-(4 * c1 + 1) + 3 + 1) / 3) : -((-(4 * c1 + 1) + 3 - 1) / 3))) : (4 * c1 + 1) / 3)); c4++) {
              x[(4 * c1 + 7) / 3] = x[(4 * c1 + 7) / 3] - A[(4 * c1 + 7) / 3][c4 + -1] * x[c4 + -1];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
