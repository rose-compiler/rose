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
/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "trmm.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int ni = 32;
/* Variable declaration/allocation. */
  double alpha;
  double A[32][32];
  double B[32][32];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c1;
    int c0;
    int c2;
    for (c0 = 1; c0 <= 31; c0++) {
      for (c1 = 0; c1 <= 31; c1++) {
        for (c2 = c1; c2 <= c0 + c1 + -1; c2++) {
          B[c0][c1] += alpha * A[c0][-1 * c1 + c2] * B[c1][-1 * c1 + c2];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
