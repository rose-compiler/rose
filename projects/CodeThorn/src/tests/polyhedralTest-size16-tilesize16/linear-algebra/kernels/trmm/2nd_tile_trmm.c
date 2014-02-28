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
  int ni = 16;
/* Variable declaration/allocation. */
  double alpha;
  double A[16][16];
  double B[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c0;
    int c1;
    int c3;
{
      int c5;
      int c4;
      int c2;
      int c6;
#pragma omp parallel for private(c6, c4, c5)
      for (c2 = 1; c2 <= 15; c2++) {
#pragma ivdep
#pragma vector always
#pragma simd
        for (c4 = 0; c4 <= (((c2 + 254) * 16 < 0?((16 < 0?-((-(c2 + 254) + 16 + 1) / 16) : -((-(c2 + 254) + 16 - 1) / 16))) : (c2 + 254) / 16)); c4++) {
          for (c5 = ((-1 * c2 + 16 * c4 + 1) * 17 < 0?-(-(-1 * c2 + 16 * c4 + 1) / 17) : ((17 < 0?(-(-1 * c2 + 16 * c4 + 1) + - 17 - 1) / - 17 : (-1 * c2 + 16 * c4 + 1 + 17 - 1) / 17))); c5 <= ((15 < c4?15 : c4)); c5++) {
            for (c6 = (c5 > 16 * c4 + -16 * c5?c5 : 16 * c4 + -16 * c5); c6 <= ((c2 + c5 + -1 < 16 * c4 + -16 * c5 + 15?c2 + c5 + -1 : 16 * c4 + -16 * c5 + 15)); c6++) {
              B[c2][c5] += alpha * A[c2][c6 + - 1 * c5] * B[c5][c6 + - 1 * c5];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
