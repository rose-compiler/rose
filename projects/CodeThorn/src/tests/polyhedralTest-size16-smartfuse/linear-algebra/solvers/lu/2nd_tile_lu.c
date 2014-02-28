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
    int c2;
    int c3;
    int c0;
    int c1;
    for (c0 = 1; c0 <= 16; c0++) {
      
#pragma omp parallel for private(c3, c2)
{
        int c6;
        int c7;
        int c5;
        int c4;
        if (c0 >= 1 && c0 <= 16) {
          for (c4 = ((4 * c0 + -4) * 9 < 0?-(-(4 * c0 + -4) / 9) : ((9 < 0?(-(4 * c0 + -4) + - 9 - 1) / - 9 : (4 * c0 + -4 + 9 - 1) / 9))); c4 <= ((7 < ((c0 * 2 < 0?((2 < 0?-((-c0 + 2 + 1) / 2) : -((-c0 + 2 - 1) / 2))) : c0 / 2))?7 : ((c0 * 2 < 0?((2 < 0?-((-c0 + 2 + 1) / 2) : -((-c0 + 2 - 1) / 2))) : c0 / 2)))); c4++) {
#pragma ivdep
#pragma vector always
#pragma simd
            for (c5 = (1 > -16 * c4 + 8 * c0 + -7?1 : -16 * c4 + 8 * c0 + -7); c5 <= (((((8 * c0 + 8) * 9 < 0?((9 < 0?-((-(8 * c0 + 8) + 9 + 1) / 9) : -((-(8 * c0 + 8) + 9 - 1) / 9))) : (8 * c0 + 8) / 9)) < -16 * c4 + 8 * c0 + 8?(((8 * c0 + 8) * 9 < 0?((9 < 0?-((-(8 * c0 + 8) + 9 + 1) / 9) : -((-(8 * c0 + 8) + 9 - 1) / 9))) : (8 * c0 + 8) / 9)) : -16 * c4 + 8 * c0 + 8)); c5++) {
              c7 = ((-1 * c5 + 8 * c0 + 8) * 8 < 0?((8 < 0?-((-(-1 * c5 + 8 * c0 + 8) + 8 + 1) / 8) : -((-(-1 * c5 + 8 * c0 + 8) + 8 - 1) / 8))) : (-1 * c5 + 8 * c0 + 8) / 8);
              A[c5 + - 1][c7] = A[c5 + - 1][c7] / A[c5 + - 1][c5 + - 1];
              A[c5][c7] = A[c5][c7] - A[c5][c5 + - 1] * A[c5 + - 1][c7];
              for (c6 = c5 + 1; c6 <= 15; c6++) {
                c7 = ((-1 * c5 + 8 * c0 + 8) * 8 < 0?((8 < 0?-((-(-1 * c5 + 8 * c0 + 8) + 8 + 1) / 8) : -((-(-1 * c5 + 8 * c0 + 8) + 8 - 1) / 8))) : (-1 * c5 + 8 * c0 + 8) / 8);
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
