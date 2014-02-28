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
    for (c1 = 0; c1 <= 1; c1++) {
      for (c3 = 8 * c1; c3 <= 8 * c1 + 7; c3++) {
        x[c3] = c[c3];
      }
    }
    for (c1 = 0; c1 <= 17; c1++) {
      if (c1 >= 2 && c1 <= 7) {
        for (c3 = 8; c3 <= 15; c3++) {
          x[c3] = x[c3] - A[c3][c1 + -2] * x[c1 + -2];
        }
      }
      if (c1 == 17) {
        x[15] = x[15] / A[15][15];
      }
      if (c1 == 0) {
        x[0] = x[0] / A[0][0];
      }
      if ((8 * c1 + 7) % 9 == 0) {
        x[(8 * c1 + 7) / 9] = x[(8 * c1 + 7) / 9] - A[(8 * c1 + 7) / 9][(8 * c1 + -2) / 9] * x[(8 * c1 + -2) / 9];
      }
      c2 = (((1 > (((8 * c1 + -1) * 9 < 0?-(-(8 * c1 + -1) / 9) : ((9 < 0?(-(8 * c1 + -1) + - 9 - 1) / - 9 : (8 * c1 + -1 + 9 - 1) / 9))))?1 : (((8 * c1 + -1) * 9 < 0?-(-(8 * c1 + -1) / 9) : ((9 < 0?(-(8 * c1 + -1) + - 9 - 1) / - 9 : (8 * c1 + -1 + 9 - 1) / 9)))))) > c1 + -1?((1 > (((8 * c1 + -1) * 9 < 0?-(-(8 * c1 + -1) / 9) : ((9 < 0?(-(8 * c1 + -1) + - 9 - 1) / - 9 : (8 * c1 + -1 + 9 - 1) / 9))))?1 : (((8 * c1 + -1) * 9 < 0?-(-(8 * c1 + -1) / 9) : ((9 < 0?(-(8 * c1 + -1) + - 9 - 1) / - 9 : (8 * c1 + -1 + 9 - 1) / 9)))))) : c1 + -1);
      if (c2 <= (((8 * c1 + 6) * 9 < 0?((9 < 0?-((-(8 * c1 + 6) + 9 + 1) / 9) : -((-(8 * c1 + 6) + 9 - 1) / 9))) : (8 * c1 + 6) / 9))) {
        if (c1 <= ((9 * c2 * 8 < 0?((8 < 0?-((-(9 * c2) + 8 + 1) / 8) : -((-(9 * c2) + 8 - 1) / 8))) : 9 * c2 / 8))) {
          x[c2] = x[c2] - A[c2][c2 + -1] * x[c2 + -1];
        }
        x[c2] = x[c2] / A[c2][c2];
        x[c2 + 1] = x[c2 + 1] - A[c2 + 1][c2 + -1] * x[c2 + -1];
        for (c3 = c2 + 2; c3 <= 8 * c1 + -8 * c2 + 7; c3++) {
          x[c3] = x[c3] - A[c3][c2 + -1] * x[c2 + -1];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
