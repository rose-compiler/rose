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
    int c3;
    int c1;
    int c2;
    for (c0 = 1; c0 <= 15; c0++) {
      for (c1 = 0; c1 <= c0 + 16; c1++) {
#pragma omp parallel for private(c3)
        for (c2 = (((7 * c1 * 8 < 0?-(-(7 * c1) / 8) : ((8 < 0?(-(7 * c1) + - 8 - 1) / - 8 : (7 * c1 + 8 - 1) / 8)))) > c1 + -2?((7 * c1 * 8 < 0?-(-(7 * c1) / 8) : ((8 < 0?(-(7 * c1) + - 8 - 1) / - 8 : (7 * c1 + 8 - 1) / 8)))) : c1 + -2); c2 <= (((((c0 + 7 * c1 + 5) * 8 < 0?((8 < 0?-((-(c0 + 7 * c1 + 5) + 8 + 1) / 8) : -((-(c0 + 7 * c1 + 5) + 8 - 1) / 8))) : (c0 + 7 * c1 + 5) / 8)) < c1?(((c0 + 7 * c1 + 5) * 8 < 0?((8 < 0?-((-(c0 + 7 * c1 + 5) + 8 + 1) / 8) : -((-(c0 + 7 * c1 + 5) + 8 - 1) / 8))) : (c0 + 7 * c1 + 5) / 8)) : c1)); c2++) {
          for (c3 = (7 * c1 + -7 * c2 > -1 * c0 + c2 + 1?7 * c1 + -7 * c2 : -1 * c0 + c2 + 1); c3 <= ((((15 < c2?15 : c2)) < 7 * c1 + -7 * c2 + 6?((15 < c2?15 : c2)) : 7 * c1 + -7 * c2 + 6)); c3++) {
            B[c0][c3] += alpha * A[c0][c2 + -1 * c3] * B[c3][c2 + -1 * c3];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
