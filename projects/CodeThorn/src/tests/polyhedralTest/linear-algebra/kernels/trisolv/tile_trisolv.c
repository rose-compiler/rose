/**
 * trisolv.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "trisolv.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 32;
/* Variable declaration/allocation. */
  double A[32][32];
  double x[32];
  double c[32];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c0;
{
      int c2;
      int c3;
      x[0] = c[0];
      x[0] = x[0] / A[0][0];
      for (c2 = 1; c2 <= 31; c2++) {
        x[c2] = c[c2];
        for (c3 = 1; c3 <= c2; c3++) {
          x[c2] = x[c2] - A[c2][c3 + -1] * x[c3 + -1];
        }
        x[c2] = x[c2] / A[c2][c2];
      }
    }
  }
  
#pragma endscop
  return 0;
}
