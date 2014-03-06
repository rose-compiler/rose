/**
 * doitgen.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "doitgen.h"


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int nr = NR;
  int nq = NQ;
  int np = NP;


  double A[NR][NQ][NP];
  double sum[NR][NQ][NP];
  double C4[NP][NP];
  int r, q, p, s;

#pragma scop
  for (r = 0; r < _PB_NR; r++)
    for (q = 0; q < _PB_NQ; q++)  {
      for (p = 0; p < _PB_NP; p++)  {
	sum[r][q][p] = 0;
	for (s = 0; s < _PB_NP; s++)
	  sum[r][q][p] = sum[r][q][p] + A[r][q][s] * C4[s][p];
      }
      for (p = 0; p < _PB_NR; p++)
	A[r][q][p] = sum[r][q][p];
    }
#pragma endscop


  return 0;
}
