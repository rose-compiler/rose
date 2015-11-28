/**
 * jacobi-1d-imper.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define ARRAYSIZE 10
#define STEPSIZE 2

#define TSTEPS STEPSIZE
#define N ARRAYSIZE
#define _PB_TSTEPS STEPSIZE
#define _PB_N ARRAYSIZE

int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */

 double A[N];
 double B[N];
 int t, i, j;
#pragma scop
 t=0;
 for (; t < _PB_TSTEPS; t++)
    {
      i=1;
      for (; i < _PB_N - 1; i++)
	B[i] = 0.33333 * (A[i-1] + A[i] + A[i + 1]);
      j=1;
      for (; j < _PB_N - 1; j++)
	A[j] = B[j];
    }
#pragma endscop

  return 0;
}
