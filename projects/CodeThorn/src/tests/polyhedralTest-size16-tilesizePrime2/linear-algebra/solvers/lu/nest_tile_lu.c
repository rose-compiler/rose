#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * lu.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c0;
    int c3;
    int c1;
    int c4;
    int c2;
    for (c0 = 0; c0 <= 2; c0++) {
      c1 = (c0 * 2 < 0?-(-c0 / 2) : ((2 < 0?(-c0 + - 2 - 1) / - 2 : (c0 + 2 - 1) / 2)));
      if (c1 <= ((1 < c0?1 : c0))) {
        for (c2 = 13 * c0 + -13 * c1 + 1; c2 <= 15; c2++) {
          for (c3 = 13 * c0 + -13 * c1; c3 <= ((((13 * c1 + 11 < c2 + -2?13 * c1 + 11 : c2 + -2)) < 13 * c0 + -13 * c1 + 12?((13 * c1 + 11 < c2 + -2?13 * c1 + 11 : c2 + -2)) : 13 * c0 + -13 * c1 + 12)); c3++) {
            for (c4 = (13 * c1 > c3 + 1?13 * c1 : c3 + 1); c4 <= ((15 < 13 * c1 + 12?15 : 13 * c1 + 12)); c4++) {
              A[c2][c4] = A[c2][c4] - A[c2][c3] * A[c3][c4];
            }
          }
          if (c0 >= (((13 * c1 + c2 + -13) * 13 < 0?-(-(13 * c1 + c2 + -13) / 13) : ((13 < 0?(-(13 * c1 + c2 + -13) + - 13 - 1) / - 13 : (13 * c1 + c2 + -13 + 13 - 1) / 13)))) && c1 >= (((c2 + -12) * 13 < 0?-(-(c2 + -12) / 13) : ((13 < 0?(-(c2 + -12) + - 13 - 1) / - 13 : (c2 + -12 + 13 - 1) / 13))))) {
            for (c4 = (13 * c1 > c2?13 * c1 : c2); c4 <= ((15 < 13 * c1 + 12?15 : 13 * c1 + 12)); c4++) {
              A[c2 + -1][c4] = A[c2 + -1][c4] / A[c2 + -1][c2 + -1];
              A[c2][c4] = A[c2][c4] - A[c2][c2 + -1] * A[c2 + -1][c4];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
