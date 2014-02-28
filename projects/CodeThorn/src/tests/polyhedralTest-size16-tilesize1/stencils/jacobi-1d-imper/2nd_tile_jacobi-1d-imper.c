/**
 * jacobi-1d-imper.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE
# define _PB_N ARRAYSIZE

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 16;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[16];
  double B[16];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c0;
    int c1;
    for (c0 = 1; c0 <= 18; c0++) {
      
#pragma omp parallel for
{
        int c4;
        if (c0 >= 1 && c0 <= 18) {
          if (c0 <= 4) {
            if ((2 * c0 + 1) % 3 == 0) {
              B[- 2 * c0 + 3 * ((2 * c0 + 1) / 3)] = 0.33333 * (A[- 2 * c0 + 3 * ((2 * c0 + 1) / 3) - 1] + A[- 2 * c0 + 3 * ((2 * c0 + 1) / 3)] + A[- 2 * c0 + 3 * ((2 * c0 + 1) / 3) + 1]);
            }
          }
          for (c4 = ((((2 * c0 + 2) * 3 < 0?-(-(2 * c0 + 2) / 3) : ((3 < 0?(-(2 * c0 + 2) + - 3 - 1) / - 3 : (2 * c0 + 2 + 3 - 1) / 3)))) > c0 + -1?(((2 * c0 + 2) * 3 < 0?-(-(2 * c0 + 2) / 3) : ((3 < 0?(-(2 * c0 + 2) + - 3 - 1) / - 3 : (2 * c0 + 2 + 3 - 1) / 3)))) : c0 + -1); c4 <= (((((2 * c0 + 14) * 3 < 0?((3 < 0?-((-(2 * c0 + 14) + 3 + 1) / 3) : -((-(2 * c0 + 14) + 3 - 1) / 3))) : (2 * c0 + 14) / 3)) < c0?(((2 * c0 + 14) * 3 < 0?((3 < 0?-((-(2 * c0 + 14) + 3 + 1) / 3) : -((-(2 * c0 + 14) + 3 - 1) / 3))) : (2 * c0 + 14) / 3)) : c0)); c4++) {
            B[- 2 * c0 + 3 * c4] = 0.33333 * (A[- 2 * c0 + 3 * c4 - 1] + A[- 2 * c0 + 3 * c4] + A[- 2 * c0 + 3 * c4 + 1]);
            A[- 2 * c0 + 3 * c4 + - 1] = B[- 2 * c0 + 3 * c4 + - 1];
          }
          if (c0 >= 15) {
            if (2 * c0 % 3 == 0) {
              A[- 2 * c0 + 3 * ((2 * c0 + 15) / 3) + - 1] = B[- 2 * c0 + 3 * ((2 * c0 + 15) / 3) + - 1];
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  POLYBENCH_FREE_ARRAY(B);
  return 0;
}
