#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE

# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE

/**
 * gramschmidt.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int ni = NI;
  int nj = NJ;

  /* Variable declaration/allocation. */

  double A[NI][NJ];
  double R[NI][NJ];
  double Q[NI][NJ];
  int i, j, k;

  double nrm;

#pragma scop
  for (k = 0; k < _PB_NJ; k++)
    {
      nrm = 0;
      for (i = 0; i < _PB_NI; i++)
        nrm += A[i][k] * A[i][k];
      R[k][k] = sqrt(nrm);
      for (i = 0; i < _PB_NI; i++)
        Q[i][k] = A[i][k] / R[k][k];
      for (j = k + 1; j < _PB_NJ; j++)
	{
	  R[k][j] = 0;
	  for (i = 0; i < _PB_NI; i++)
	    R[k][j] += Q[i][k] * A[i][j];
	  for (i = 0; i < _PB_NI; i++)
	    A[i][j] = A[i][j] - Q[i][k] * R[k][j];
	}
    }
#pragma endscop

  return 0;
}
