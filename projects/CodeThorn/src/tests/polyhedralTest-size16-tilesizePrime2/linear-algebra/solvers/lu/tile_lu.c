#   define N ARRAYSIZE
# define _PB_N ARRAYSIZE
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

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 16;
/* Variable declaration/allocation. */
  double A[16][16];
  int i;
  int j;
  int k;
  
#pragma scop
{
    int c0;
    int c2;
    int c1;
{
      int c5;
      int c3;
      int c4;
      for (c3 = 0; c3 <= 14; c3++) {
        for (c4 = c3 + 1; c4 <= 15; c4++) {
          A[c3][c4] = A[c3][c4] / A[c3][c3];
          for (c5 = c3 + 1; c5 <= 15; c5++) {
            A[c5][c4] = A[c5][c4] - A[c5][c3] * A[c3][c4];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
