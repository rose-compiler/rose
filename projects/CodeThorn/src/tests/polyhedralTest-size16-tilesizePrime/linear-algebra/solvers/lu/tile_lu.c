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
    int c1;
    int c5;
    int c3;
{
      int c2;
      int c6;
      int c8;
      for (c2 = 0; c2 <= 14; c2++) {
        for (c6 = c2 + 1; c6 <= 15; c6++) {
          A[c2][c6] = A[c2][c6] / A[c2][c2];
        }
        for (c6 = c2 + 1; c6 <= 15; c6++) {
          for (c8 = c2 + 1; c8 <= 15; c8++) {
            A[c6][c8] = A[c6][c8] - A[c6][c2] * A[c2][c8];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
