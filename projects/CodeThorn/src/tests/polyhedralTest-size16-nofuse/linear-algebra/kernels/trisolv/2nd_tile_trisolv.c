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
{
      int c4;
      int c2;
      for (c2 = 0; c2 <= 15; c2++) {
        c4 = (c2 * 8 < 0?((8 < 0?-((-c2 + 8 + 1) / 8) : -((-c2 + 8 - 1) / 8))) : c2 / 8);
        x[c2] = c[c2];
      }
    }
    for (c1 = 0; c1 <= 17; c1++) {{
        int c4;
        if (c1 >= 0 && c1 <= 17) {
          if (c1 == 0) {
            x[0] = x[0] / A[0][0];
          }
          if (c1 == 17) {
            x[15] = x[15] / A[15][15];
          }
          if (c1 >= 2 && c1 <= 7) {
            for (c4 = 8; c4 <= 15; c4++) {
              x[c4] = x[c4] - A[c4][c1 + - 2] * x[c1 + - 2];
            }
          }
        }
      }
      if ((8 * c1 + 7) % 9 == 0) {
        x[(8 * c1 + 7) / 9] = x[(8 * c1 + 7) / 9] - A[(8 * c1 + 7) / 9][(8 * c1 + - 2) / 9] * x[(8 * c1 + - 2) / 9];
      }
{
        c2 = (((1 > (((8 * c1 + - 1) * 9 < 0?-(-(8 * c1 + - 1) / 9) : ((9 < 0?(-(8 * c1 + - 1) + - 9 - 1) / - 9 : (8 * c1 + - 1 + 9 - 1) / 9))))?1 : (((8 * c1 + - 1) * 9 < 0?-(-(8 * c1 + - 1) / 9) : ((9 < 0?(-(8 * c1 + - 1) + - 9 - 1) / - 9 : (8 * c1 + - 1 + 9 - 1) / 9)))))) > c1 + - 1?((1 > (((8 * c1 + - 1) * 9 < 0?-(-(8 * c1 + - 1) / 9) : ((9 < 0?(-(8 * c1 + - 1) + - 9 - 1) / - 9 : (8 * c1 + - 1 + 9 - 1) / 9))))?1 : (((8 * c1 + - 1) * 9 < 0?-(-(8 * c1 + - 1) / 9) : ((9 < 0?(-(8 * c1 + - 1) + - 9 - 1) / - 9 : (8 * c1 + - 1 + 9 - 1) / 9)))))) : c1 + - 1);
      }
{
        int c5;
        if (8 * c1 >= 9 * c2 + -6) {
          if (c1 >= (((9 * c2 + 1) * 8 < 0?-(-(9 * c2 + 1) / 8) : ((8 < 0?(-(9 * c2 + 1) + - 8 - 1) / - 8 : (9 * c2 + 1 + 8 - 1) / 8)))) && c2 >= -1) {
            x[c2 + 1] = x[c2 + 1] - A[c2 + 1][c2 + - 1] * x[c2 + - 1];
            x[c2] = x[c2] / A[c2][c2];
          }
          for (c5 = c2 + 2; c5 <= ((-1 < 8 * c1 + -8 * c2 + 7?-1 : 8 * c1 + -8 * c2 + 7)); c5++) {
            x[c5] = x[c5] - A[c5][c2 + - 1] * x[c2 + - 1];
          }
          if (c1 <= ((9 * c2 * 8 < 0?((8 < 0?-((-(9 * c2) + 8 + 1) / 8) : -((-(9 * c2) + 8 - 1) / 8))) : 9 * c2 / 8))) {
            x[c2] = x[c2] - A[c2][c2 + - 1] * x[c2 + - 1];
            x[c2 + 1] = x[c2 + 1] - A[c2 + 1][c2 + - 1] * x[c2 + - 1];
            x[c2] = x[c2] / A[c2][c2];
          }
          if (c1 >= c2 && c2 <= -2) {
            x[c2 + 1] = x[c2 + 1] - A[c2 + 1][c2 + - 1] * x[c2 + - 1];
            x[0] = x[0] - A[0][c2 + - 1] * x[c2 + - 1];
            x[c2] = x[c2] / A[c2][c2];
          }
          if (c1 >= (((9 * c2 + 1) * 8 < 0?-(-(9 * c2 + 1) / 8) : ((8 < 0?(-(9 * c2 + 1) + - 8 - 1) / - 8 : (9 * c2 + 1 + 8 - 1) / 8)))) && c1 <= c2 + -1) {
            x[c2 + 1] = x[c2 + 1] - A[c2 + 1][c2 + - 1] * x[c2 + - 1];
            x[c2] = x[c2] / A[c2][c2];
          }
          for (c5 = (1 > c2 + 2?1 : c2 + 2); c5 <= 8 * c1 + -8 * c2 + 7; c5++) {
            x[c5] = x[c5] - A[c5][c2 + - 1] * x[c2 + - 1];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
