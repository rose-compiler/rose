/**
 * lu.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include benchmark-specific header. */
/* Default data type is double, default size is 1024. */
#include "lu.h"


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */
  double A[N][N];

  int i, j, k;

#pragma scop
  for (k = 0; k < _PB_N; k++)
    {
      for (j = k + 1; j < _PB_N; j++)
	A[k][j] = A[k][j] / A[k][k];
      for(i = k + 1; i < _PB_N; i++)
	for (j = k + 1; j < _PB_N; j++)
	  A[i][j] = A[i][j] - A[i][k] * A[k][j];
    }
#pragma endscop


  return 0;
}
