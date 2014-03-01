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
    int c2;
    int c3;
    int c1;
    int c4;
    for (c0 = 0; c0 <= 4; c0++) {
#pragma omp parallel for private(c4, c3, c2)
      for (c1 = (c0 * 2 < 0?-(-c0 / 2) : ((2 < 0?(-c0 + - 2 - 1) / - 2 : (c0 + 2 - 1) / 2))); c1 <= ((2 < c0?2 : c0)); c1++) {
        for (c2 = 7 * c0 + -7 * c1 + 1; c2 <= 15; c2++) {
          for (c3 = 7 * c0 + -7 * c1; c3 <= ((((7 * c1 + 5 < c2 + -2?7 * c1 + 5 : c2 + -2)) < 7 * c0 + -7 * c1 + 6?((7 * c1 + 5 < c2 + -2?7 * c1 + 5 : c2 + -2)) : 7 * c0 + -7 * c1 + 6)); c3++) {
            for (c4 = (7 * c1 > c3 + 1?7 * c1 : c3 + 1); c4 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c4++) {
              A[c2][c4] = A[c2][c4] - A[c2][c3] * A[c3][c4];
            }
          }
          if (c0 >= (((7 * c1 + c2 + -7) * 7 < 0?-(-(7 * c1 + c2 + -7) / 7) : ((7 < 0?(-(7 * c1 + c2 + -7) + - 7 - 1) / - 7 : (7 * c1 + c2 + -7 + 7 - 1) / 7)))) && c1 >= (((c2 + -6) * 7 < 0?-(-(c2 + -6) / 7) : ((7 < 0?(-(c2 + -6) + - 7 - 1) / - 7 : (c2 + -6 + 7 - 1) / 7))))) {
            for (c4 = (7 * c1 > c2?7 * c1 : c2); c4 <= ((15 < 7 * c1 + 6?15 : 7 * c1 + 6)); c4++) {
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
