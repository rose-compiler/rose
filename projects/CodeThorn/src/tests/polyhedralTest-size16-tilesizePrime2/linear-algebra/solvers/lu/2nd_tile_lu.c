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
    int c1;
    int c2;
    int c3;
    for (c0 = 1; c0 <= 16; c0++) {
      
#pragma omp parallel for private(c3, c2)
{
        int c6;
        int c7;
        int c5;
        int c4;
        if (c0 >= 1 && c0 <= 16) {
          c4 = ((c0 + -12) * 14 < 0?-(-(c0 + -12) / 14) : ((14 < 0?(-(c0 + -12) + - 14 - 1) / - 14 : (c0 + -12 + 14 - 1) / 14)));
          if (c4 <= ((c0 * 13 < 0?((13 < 0?-((-c0 + 13 + 1) / 13) : -((-c0 + 13 - 1) / 13))) : c0 / 13))) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c5 = (1 > 13 * c0 + -194?1 : 13 * c0 + -194); c5 <= (((13 * c0 + 13) * 14 < 0?((14 < 0?-((-(13 * c0 + 13) + 14 + 1) / 14) : -((-(13 * c0 + 13) + 14 - 1) / 14))) : (13 * c0 + 13) / 14)); c5++) {
              c7 = ((-1 * c5 + 13 * c0 + 13) * 13 < 0?((13 < 0?-((-(-1 * c5 + 13 * c0 + 13) + 13 + 1) / 13) : -((-(-1 * c5 + 13 * c0 + 13) + 13 - 1) / 13))) : (-1 * c5 + 13 * c0 + 13) / 13);
              A[c5 + - 1][c7] = A[c5 + - 1][c7] / A[c5 + - 1][c5 + - 1];
              A[c5][c7] = A[c5][c7] - A[c5][c5 + - 1] * A[c5 + - 1][c7];
              for (c6 = c5 + 1; c6 <= 15; c6++) {
                c7 = ((-1 * c5 + 13 * c0 + 13) * 13 < 0?((13 < 0?-((-(-1 * c5 + 13 * c0 + 13) + 13 + 1) / 13) : -((-(-1 * c5 + 13 * c0 + 13) + 13 - 1) / 13))) : (-1 * c5 + 13 * c0 + 13) / 13);
                A[c6][c7] = A[c6][c7] - A[c6][c5 + -1] * A[c5 + -1][c7];
              }
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
