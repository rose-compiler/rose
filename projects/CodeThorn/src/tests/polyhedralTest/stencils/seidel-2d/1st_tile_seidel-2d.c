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
    int c3;
    int c0;
    int c2;
    for (c0 = 1; c0 <= 31; c0++) {
      for (c2 = (c0 + 1 > 2 * c0 + -29?c0 + 1 : 2 * c0 + -29); c2 <= ((c0 + 31 < 2 * c0 + 29?c0 + 31 : 2 * c0 + 29)); c2++) {
        for (c3 = (((0 > c0 + -30?0 : c0 + -30)) > -1 * c0 + c2 + -30?((0 > c0 + -30?0 : c0 + -30)) : -1 * c0 + c2 + -30); c3 <= ((((1 < c0 + -1?1 : c0 + -1)) < -1 * c0 + c2 + -1?((1 < c0 + -1?1 : c0 + -1)) : -1 * c0 + c2 + -1)); c3++) {
          A[c0 + -1 * c3][-1 * c0 + c2 + -1 * c3] = (A[c0 + -1 * c3 - 1][-1 * c0 + c2 + -1 * c3 - 1] + A[c0 + -1 * c3 - 1][-1 * c0 + c2 + -1 * c3] + A[c0 + -1 * c3 - 1][-1 * c0 + c2 + -1 * c3 + 1] + A[c0 + -1 * c3][-1 * c0 + c2 + -1 * c3 - 1] + A[c0 + -1 * c3][-1 * c0 + c2 + -1 * c3] + A[c0 + -1 * c3][-1 * c0 + c2 + -1 * c3 + 1] + A[c0 + -1 * c3 + 1][-1 * c0 + c2 + -1 * c3 - 1] + A[c0 + -1 * c3 + 1][-1 * c0 + c2 + -1 * c3] + A[c0 + -1 * c3 + 1][-1 * c0 + c2 + -1 * c3 + 1]) / 9.0;
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
