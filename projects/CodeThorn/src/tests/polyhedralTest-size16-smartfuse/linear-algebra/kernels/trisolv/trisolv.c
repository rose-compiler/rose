#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
/**
 * trisolv.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */

  double A[N][N];
  double x[N];
  double c[N];
  int i, j;

#pragma scop
  for (i = 0; i < _PB_N; i++)
    {
      x[i] = c[i];
      for (j = 0; j <= i - 1; j++)
        x[i] = x[i] - A[i][j] * x[j];
      x[i] = x[i] / A[i][i];
    }
#pragma endscop


  return 0;
}
