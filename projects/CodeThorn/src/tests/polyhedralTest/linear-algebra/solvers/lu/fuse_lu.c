/**
 * lu.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "lu.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 32;
/* Variable declaration/allocation. */
  double A[32][32];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c1;
    int c0;
    int c2;
    for (c0 = 0; c0 <= 30; c0++) {
      for (c1 = c0 + 1; c1 <= 31; c1++) {
        A[c0][c1] = A[c0][c1] / A[c0][c0];
        for (c2 = c0 + 1; c2 <= 31; c2++) {
          A[c2][c1] = A[c2][c1] - A[c2][c0] * A[c0][c1];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
