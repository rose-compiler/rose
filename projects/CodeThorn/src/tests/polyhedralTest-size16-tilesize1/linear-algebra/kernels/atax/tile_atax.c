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
    int c1;
    int c3;
{
      int c5;
      int c2;
      for (c2 = 0; c2 <= 15; c2++) {
        y[c2] = 0;
      }
      for (c2 = 0; c2 <= 15; c2++) {
        tmp[c2] = 0;
        for (c5 = 0; c5 <= 15; c5++) {
          tmp[c2] = tmp[c2] + A[c2][c5] * x[c5];
        }
        for (c5 = 0; c5 <= 15; c5++) {
          y[c5] = y[c5] + A[c2][c5] * tmp[c2];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
