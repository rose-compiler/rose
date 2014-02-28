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
    int c3;
    int c0;
    int c2;
    int c1;
    for (c0 = 1; c0 <= 17; c0++) {
      
#pragma omp parallel for private(c2, c3)
{
        int c6;
        int c7;
        int c5;
        int c4;
        if (c0 >= 1 && c0 <= 17) {
          for (c4 = ((7 * c0 + -31) * 40 < 0?-(-(7 * c0 + -31) / 40) : ((40 < 0?(-(7 * c0 + -31) + - 40 - 1) / - 40 : (7 * c0 + -31 + 40 - 1) / 40))); c4 <= ((c0 * 5 < 0?((5 < 0?-((-c0 + 5 + 1) / 5) : -((-c0 + 5 - 1) / 5))) : c0 / 5)); c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c5 = (((1 > 7 * c0 + -104?1 : 7 * c0 + -104)) > -35 * c4 + 7 * c0 + -27?((1 > 7 * c0 + -104?1 : 7 * c0 + -104)) : -35 * c4 + 7 * c0 + -27); c5 <= (((((7 * c0 + 7) * 8 < 0?((8 < 0?-((-(7 * c0 + 7) + 8 + 1) / 8) : -((-(7 * c0 + 7) + 8 - 1) / 8))) : (7 * c0 + 7) / 8)) < -35 * c4 + 7 * c0 + 7?(((7 * c0 + 7) * 8 < 0?((8 < 0?-((-(7 * c0 + 7) + 8 + 1) / 8) : -((-(7 * c0 + 7) + 8 - 1) / 8))) : (7 * c0 + 7) / 8)) : -35 * c4 + 7 * c0 + 7)); c5++) {
              c7 = ((-1 * c5 + 7 * c0 + 7) * 7 < 0?((7 < 0?-((-(-1 * c5 + 7 * c0 + 7) + 7 + 1) / 7) : -((-(-1 * c5 + 7 * c0 + 7) + 7 - 1) / 7))) : (-1 * c5 + 7 * c0 + 7) / 7);
              A[c5 + - 1][c7] = A[c5 + - 1][c7] / A[c5 + - 1][c5 + - 1];
              A[c5][c7] = A[c5][c7] - A[c5][c5 + - 1] * A[c5 + - 1][c7];
              for (c6 = c5 + 1; c6 <= 15; c6++) {
                c7 = ((-1 * c5 + 7 * c0 + 7) * 7 < 0?((7 < 0?-((-(-1 * c5 + 7 * c0 + 7) + 7 + 1) / 7) : -((-(-1 * c5 + 7 * c0 + 7) + 7 - 1) / 7))) : (-1 * c5 + 7 * c0 + 7) / 7);
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
