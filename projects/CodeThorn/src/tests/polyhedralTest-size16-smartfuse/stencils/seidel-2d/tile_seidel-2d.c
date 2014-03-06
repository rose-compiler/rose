#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE
# define _PB_N ARRAYSIZE
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

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 16;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[16][16];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c5;
    int c3;
{
      int c8;
      int c6;
      int c2;
      for (c2 = 0; c2 <= 1; c2++) {
        for (c6 = 1; c6 <= 14; c6++) {
          for (c8 = 1; c8 <= 14; c8++) {
            A[c6][c8] = (A[c6 - 1][c8 - 1] + A[c6 - 1][c8] + A[c6 - 1][c8 + 1] + A[c6][c8 - 1] + A[c6][c8] + A[c6][c8 + 1] + A[c6 + 1][c8 - 1] + A[c6 + 1][c8] + A[c6 + 1][c8 + 1]) / 9.0;
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
