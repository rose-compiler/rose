#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * gesummv.c: This file is part of the PolyBench/C 3.2 test suite.
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
  double alpha;
  double beta;
  double A[16][16];
  double B[16][16];
  double tmp[16];
  double x[16];
  double y[16];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c3;
{
      int c2;
      int c5;
      for (c2 = 0; c2 <= 15; c2++) {
        tmp[c2] = 0;
        y[c2] = 0;
        for (c5 = 0; c5 <= 15; c5++) {
          tmp[c2] = A[c2][c5] * x[c5] + tmp[c2];
          y[c2] = B[c2][c5] * x[c5] + y[c2];
        }
        y[c2] = alpha * tmp[c2] + beta * y[c2];
      }
    }
  }
  
#pragma endscop
  return 0;
}
