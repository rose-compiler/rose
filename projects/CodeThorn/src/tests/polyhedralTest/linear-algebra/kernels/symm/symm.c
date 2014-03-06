/**
 * symm.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Default data type is double, default size is 4000. */
#include "symm.h"



int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int ni = NI;
  int nj = NJ;

  /* Variable declaration/allocation. */
  double alpha;
  double beta;
  double C[NI][NJ];
  double A[NI][NJ];
  double B[NI][NJ];
  int i, j, k;
  double acc;

#pragma scop
  /*  C := alpha*A*B + beta*C, A is symetric */
  for (i = 0; i < _PB_NI; i++)
    for (j = 0; j < _PB_NJ; j++)
      {
	acc = 0;
	for (k = 0; k < j - 1; k++)
	  {
	    C[k][j] += alpha * A[k][i] * B[i][j];
	    acc += B[k][j] * A[k][i];
	  }
	C[i][j] = beta * C[i][j] + alpha * A[i][i] * B[i][j] + alpha * acc;
      }
#pragma endscop


  return 0;
}
