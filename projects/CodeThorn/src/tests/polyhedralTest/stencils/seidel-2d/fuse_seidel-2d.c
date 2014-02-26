/**
 * seidel-2d.c: This file is part of the PolyBench/C 3.2 test suite.
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
/* Default data type is double, default size is 20x1000. */
#include "seidel-2d.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 32;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[32][32];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c2;
    int c0;
    int c1;
    for (c0 = 0; c0 <= 1; c0++) {
      for (c1 = c0 + 1; c1 <= c0 + 30; c1++) {
        for (c2 = c0 + c1 + 1; c2 <= c0 + c1 + 30; c2++) {
          A[-1 * c0 + c1][-1 * c0 + -1 * c1 + c2] = (A[-1 * c0 + c1 - 1][-1 * c0 + -1 * c1 + c2 - 1] + A[-1 * c0 + c1 - 1][-1 * c0 + -1 * c1 + c2] + A[-1 * c0 + c1 - 1][-1 * c0 + -1 * c1 + c2 + 1] + A[-1 * c0 + c1][-1 * c0 + -1 * c1 + c2 - 1] + A[-1 * c0 + c1][-1 * c0 + -1 * c1 + c2] + A[-1 * c0 + c1][-1 * c0 + -1 * c1 + c2 + 1] + A[-1 * c0 + c1 + 1][-1 * c0 + -1 * c1 + c2 - 1] + A[-1 * c0 + c1 + 1][-1 * c0 + -1 * c1 + c2] + A[-1 * c0 + c1 + 1][-1 * c0 + -1 * c1 + c2 + 1]) / 9.0;
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
