#   define NI ARRAYSIZE
# define _PB_NI ARRAYSIZE
/**
 * trmm.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int ni = 64;
/* Variable declaration/allocation. */
  double alpha;
  double A[64][64];
  double B[64][64];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c0;
    int c1;
    int c2;
    int c3;
    for (c0 = 1; c0 <= 63; c0++) {
      for (c1 = 0; c1 <= c0 + 69; c1++) {
#pragma omp parallel for private(c3)
        for (c2 = (((8 * c1 * 9 < 0?-(-(8 * c1) / 9) : ((9 < 0?(-(8 * c1) + - 9 - 1) / - 9 : (8 * c1 + 9 - 1) / 9)))) > c1 + -7?((8 * c1 * 9 < 0?-(-(8 * c1) / 9) : ((9 < 0?(-(8 * c1) + - 9 - 1) / - 9 : (8 * c1 + 9 - 1) / 9)))) : c1 + -7); c2 <= (((((c0 + 8 * c1 + 6) * 9 < 0?((9 < 0?-((-(c0 + 8 * c1 + 6) + 9 + 1) / 9) : -((-(c0 + 8 * c1 + 6) + 9 - 1) / 9))) : (c0 + 8 * c1 + 6) / 9)) < c1?(((c0 + 8 * c1 + 6) * 9 < 0?((9 < 0?-((-(c0 + 8 * c1 + 6) + 9 + 1) / 9) : -((-(c0 + 8 * c1 + 6) + 9 - 1) / 9))) : (c0 + 8 * c1 + 6) / 9)) : c1)); c2++) {
          for (c3 = (8 * c1 + -8 * c2 > -1 * c0 + c2 + 1?8 * c1 + -8 * c2 : -1 * c0 + c2 + 1); c3 <= ((c2 < 8 * c1 + -8 * c2 + 7?c2 : 8 * c1 + -8 * c2 + 7)); c3++) {
            B[c0][c3] += alpha * A[c0][c2 + -1 * c3] * B[c3][c2 + -1 * c3];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
