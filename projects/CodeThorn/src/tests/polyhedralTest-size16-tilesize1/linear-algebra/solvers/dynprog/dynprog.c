/**
 * dynprog.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Default to STANDARD_DATASET. */
#   define TSTEPS STEPSIZE
#   define LENGTH ARRAYSIZE

# define _PB_TSTEPS STEPSIZE
# define _PB_LENGTH ARRAYSIZE



int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int length = LENGTH;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */
  double out;

  double sum_c[LENGTH][LENGTH][LENGTH];
  double c[LENGTH][LENGTH];
  double W[LENGTH][LENGTH];
  int iter, i, j, k;

  double out_l = 0;

#pragma scop
  for (iter = 0; iter < _PB_TSTEPS; iter++)
    {
      for (i = 0; i <= _PB_LENGTH - 1; i++)
	for (j = 0; j <= _PB_LENGTH - 1; j++)
	  c[i][j] = 0;

      for (i = 0; i <= _PB_LENGTH - 2; i++)
	{
	  for (j = i + 1; j <= _PB_LENGTH - 1; j++)
	    {
	      sum_c[i][j][i] = 0;
	      for (k = i + 1; k <= j-1; k++)
		sum_c[i][j][k] = sum_c[i][j][k - 1] + c[i][k] + c[k][j];
	      c[i][j] = sum_c[i][j][j-1] + W[i][j];
	    }
	}
      out_l += c[0][_PB_LENGTH - 1];
    }
#pragma endscop

  out = out_l;

  return 0;
}
