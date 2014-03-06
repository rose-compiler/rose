#   define NX ARRAYSIZE
#   define NY ARRAYSIZE
# define _PB_NX ARRAYSIZE
# define _PB_NY ARRAYSIZE
/**
 * atax.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int nx = 16;
  int ny = 16;
  double A[16][16];
  double x[16];
  double y[16];
  double tmp[16];
  int i;
  int j;
  
#pragma scop
{
    int c3;
    int c1;
    for (c1 = 0; c1 <= 15; c1++) {
      y[c1] = 0;
    }
    for (c1 = 0; c1 <= 15; c1++) {
      tmp[c1] = 0;
      for (c3 = 0; c3 <= 15; c3++) {
        tmp[c1] = tmp[c1] + A[c1][c3] * x[c3];
      }
      for (c3 = 0; c3 <= 15; c3++) {
        y[c3] = y[c3] + A[c1][c3] * tmp[c1];
      }
    }
  }
  
#pragma endscop
  return 0;
}
