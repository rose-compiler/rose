/**
 * ludcmp.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "ludcmp.h"


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;

  /* Variable declaration/allocation. */
  double A[N+1][N+1];
  double b[N+1];
  double x[N+1];
  double y[N+1];
  int i, j, k;

  double w;

#pragma scop
  b[0] = 1.0;
  for (i = 0; i < _PB_N; i++)
    {
      for (j = i+1; j <= _PB_N; j++)
        {
	  w = A[j][i];
	  for (k = 0; k < i; k++)
	    w = w- A[j][k] * A[k][i];
	  A[j][i] = w / A[i][i];
        }
      for (j = i+1; j <= _PB_N; j++)
        {
	  w = A[i+1][j];
	  for (k = 0; k <= i; k++)
	    w = w  - A[i+1][k] * A[k][j];
	  A[i+1][j] = w;
        }
    }
  y[0] = b[0];
  for (i = 1; i <= _PB_N; i++)
    {
      w = b[i];
      for (j = 0; j < i; j++)
	w = w - A[i][j] * y[j];
      y[i] = w;
    }
  x[_PB_N] = y[_PB_N] / A[_PB_N][_PB_N];
  for (i = 0; i <= _PB_N - 1; i++)
    {
      w = y[_PB_N - 1 - (i)];
      for (j = _PB_N - i; j <= _PB_N; j++)
	w = w - A[_PB_N - 1 - i][j] * x[j];
      x[_PB_N - 1 - i] = w / A[_PB_N - 1 - (i)][_PB_N - 1-(i)];
    }
#pragma endscop


  return 0;
}
