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
    int c5;
    int c3;
{
      int c8;
      int c6;
      int c2;
      for (c2 = 1; c2 <= 15; c2++) {
        for (c6 = 0; c6 <= 15; c6++) {
          for (c8 = 0; c8 <= c2 + -1; c8++) {
            B[c2][c6] += alpha * A[c2][c8] * B[c6][c8];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
