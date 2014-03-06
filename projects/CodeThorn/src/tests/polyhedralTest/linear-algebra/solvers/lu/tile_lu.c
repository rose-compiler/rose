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
{
      int c3;
      int c4;
      int c5;
      for (c3 = 0; c3 <= 30; c3++) {
        for (c4 = c3 + 1; c4 <= 31; c4++) {
          A[c3][c4] = A[c3][c4] / A[c3][c3];
          for (c5 = c3 + 1; c5 <= 31; c5++) {
            A[c5][c4] = A[c5][c4] - A[c5][c3] * A[c3][c4];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
