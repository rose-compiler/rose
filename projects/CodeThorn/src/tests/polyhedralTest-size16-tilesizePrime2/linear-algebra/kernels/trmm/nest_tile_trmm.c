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
    int c4;
    int c2;
    int c1;
    for (c0 = 1; c0 <= 15; c0++) {
      for (c1 = 0; c1 <= (((c0 + 27) * 13 < 0?((13 < 0?-((-(c0 + 27) + 13 + 1) / 13) : -((-(c0 + 27) + 13 - 1) / 13))) : (c0 + 27) / 13)); c1++) {
#pragma omp parallel for private(c4, c3)
        for (c2 = (c1 * 2 < 0?-(-c1 / 2) : ((2 < 0?(-c1 + - 2 - 1) / - 2 : (c1 + 2 - 1) / 2))); c2 <= (((((c0 + 13 * c1 + 11) * 26 < 0?((26 < 0?-((-(c0 + 13 * c1 + 11) + 26 + 1) / 26) : -((-(c0 + 13 * c1 + 11) + 26 - 1) / 26))) : (c0 + 13 * c1 + 11) / 26)) < c1?(((c0 + 13 * c1 + 11) * 26 < 0?((26 < 0?-((-(c0 + 13 * c1 + 11) + 26 + 1) / 26) : -((-(c0 + 13 * c1 + 11) + 26 - 1) / 26))) : (c0 + 13 * c1 + 11) / 26)) : c1)); c2++) {
          for (c3 = (13 * c1 + -13 * c2 > -1 * c0 + 13 * c2 + 1?13 * c1 + -13 * c2 : -1 * c0 + 13 * c2 + 1); c3 <= ((15 < 13 * c1 + -13 * c2 + 12?15 : 13 * c1 + -13 * c2 + 12)); c3++) {
            for (c4 = (13 * c2 > c3?13 * c2 : c3); c4 <= ((13 * c2 + 12 < c0 + c3 + -1?13 * c2 + 12 : c0 + c3 + -1)); c4++) {
              B[c0][c3] += alpha * A[c0][-1 * c3 + c4] * B[c3][-1 * c3 + c4];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
