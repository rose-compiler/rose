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
    int c1;
    int c0;
    int c2;
{
      int c3;
      int c4;
      int c5;
      for (c3 = 1; c3 <= 15; c3++) {
        for (c4 = 0; c4 <= 15; c4++) {
          for (c5 = c4; c5 <= c3 + c4 + -1; c5++) {
            B[c3][c4] += alpha * A[c3][- 1 * c4 + c5] * B[c4][- 1 * c4 + c5];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
