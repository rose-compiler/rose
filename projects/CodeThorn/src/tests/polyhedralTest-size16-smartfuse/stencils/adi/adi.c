
#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE

# define _PB_TSTEPS STEPSIZE
# define _PB_N ARRAYSIZE
/**
 * adi.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */

  double X[N][N];
  double A[N][N];
  double B[N][N];
  int t, i1, i2;

#pragma scop
  for (t = 0; t < _PB_TSTEPS; t++)
    {
      for (i1 = 0; i1 < _PB_N; i1++)
	for (i2 = 1; i2 < _PB_N; i2++)
	  {
	    X[i1][i2] = X[i1][i2] - X[i1][i2-1] * A[i1][i2] / B[i1][i2-1];
	    B[i1][i2] = B[i1][i2] - A[i1][i2] * A[i1][i2] / B[i1][i2-1];
	  }

      for (i1 = 0; i1 < _PB_N; i1++)
	X[i1][_PB_N-1] = X[i1][_PB_N-1] / B[i1][_PB_N-1];

      for (i1 = 0; i1 < _PB_N; i1++)
	for (i2 = 0; i2 < _PB_N-2; i2++)
	  X[i1][_PB_N-i2-2] = (X[i1][_PB_N-2-i2] - X[i1][_PB_N-2-i2-1] * A[i1][_PB_N-i2-3]) / B[i1][_PB_N-3-i2];

      for (i1 = 1; i1 < _PB_N; i1++)
	for (i2 = 0; i2 < _PB_N; i2++) {
	  X[i1][i2] = X[i1][i2] - X[i1-1][i2] * A[i1][i2] / B[i1-1][i2];
	  B[i1][i2] = B[i1][i2] - A[i1][i2] * A[i1][i2] / B[i1-1][i2];
	}

      for (i2 = 0; i2 < _PB_N; i2++)
	X[_PB_N-1][i2] = X[_PB_N-1][i2] / B[_PB_N-1][i2];

      for (i1 = 0; i1 < _PB_N-2; i1++)
	for (i2 = 0; i2 < _PB_N; i2++)
	  X[_PB_N-2-i1][i2] = (X[_PB_N-2-i1][i2] - X[_PB_N-i1-3][i2] * A[_PB_N-3-i1][i2]) / B[_PB_N-2-i1][i2];
    }
#pragma endscop


  return 0;
}
