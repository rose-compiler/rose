#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
/**
 * syrk.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int nj = 16;
/* Variable declaration/allocation. */
  double alpha;
  double beta;
  double C[16][16];
  double A[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c1;
    int c0;
    int c2;
    for (c0 = 0; c0 <= 15; c0++) {
      for (c1 = 0; c1 <= 15; c1++) {
        C[c0][c1] *= beta;
        for (c2 = 0; c2 <= 15; c2++) {
          C[c0][c1] += alpha * A[c0][c2] * A[c1][c2];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
