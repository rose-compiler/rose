#   define NI ARRAYSIZE
# define _PB_NI ARRAYSIZE
/**
 * trmm.c: This file is part of the PolyBench/C 3.2 test suite.
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

  /* Variable declaration/allocation. */
  double alpha;

  double A[NI][NI];
  double B[NI][NI];
  int i, j, k;

#pragma scop
  /*  B := alpha*A'*B, A triangular */
  for (i = 1; i < _PB_NI; i++)
    for (j = 0; j < _PB_NI; j++)
      for (k = 0; k < i; k++)
        B[i][j] += alpha * A[i][k] * B[j][k];
#pragma endscop

  return 0;
}
