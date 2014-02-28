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
    int c1;
    int c0;
    int c2;
    int c4;
    int c3;
    for (c0 = 0; c0 <= 10; c0++) {
#pragma omp parallel for private(c3, c4, c2)
      for (c1 = ((c0 + -1) * 3 < 0?-(-(c0 + -1) / 3) : ((3 < 0?(-(c0 + -1) + - 3 - 1) / - 3 : (c0 + -1 + 3 - 1) / 3))); c1 <= ((3 < c0?3 : c0)); c1++) {
        for (c2 = 2 * c0 + -2 * c1 + 1; c2 <= 15; c2++) {
          for (c3 = 2 * c0 + -2 * c1; c3 <= ((((4 * c1 + 2 < c2 + -2?4 * c1 + 2 : c2 + -2)) < 2 * c0 + -2 * c1 + 1?((4 * c1 + 2 < c2 + -2?4 * c1 + 2 : c2 + -2)) : 2 * c0 + -2 * c1 + 1)); c3++) {
            for (c4 = (4 * c1 > c3 + 1?4 * c1 : c3 + 1); c4 <= 4 * c1 + 3; c4++) {
              A[c2][c4] = A[c2][c4] - A[c2][c3] * A[c3][c4];
            }
          }
          if (c0 >= (((2 * c1 + c2 + -2) * 2 < 0?-(-(2 * c1 + c2 + -2) / 2) : ((2 < 0?(-(2 * c1 + c2 + -2) + - 2 - 1) / - 2 : (2 * c1 + c2 + -2 + 2 - 1) / 2)))) && c1 >= (((c2 + -3) * 4 < 0?-(-(c2 + -3) / 4) : ((4 < 0?(-(c2 + -3) + - 4 - 1) / - 4 : (c2 + -3 + 4 - 1) / 4))))) {
            for (c4 = (4 * c1 > c2?4 * c1 : c2); c4 <= 4 * c1 + 3; c4++) {
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
