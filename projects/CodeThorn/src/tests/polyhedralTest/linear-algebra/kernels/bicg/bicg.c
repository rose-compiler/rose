/**
 * bicg.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "bicg.h"


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int nx = NX;
  int ny = NY;

  double A[NX][NY];
  double s[NY];
  double q[NX];
  double p[NY];
  double r[NX];

  int i, j;

#pragma scop
  for (i = 0; i < _PB_NY; i++)
    s[i] = 0;
  for (i = 0; i < _PB_NX; i++)
    {
      q[i] = 0;
      for (j = 0; j < _PB_NY; j++)
	{
	  s[j] = s[j] + r[i] * A[i][j];
	  q[i] = q[i] + A[i][j] * p[j];
	}
    }
#pragma endscop


  return 0;
}
