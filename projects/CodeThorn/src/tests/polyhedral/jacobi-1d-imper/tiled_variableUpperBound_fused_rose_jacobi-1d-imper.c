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
/* Include benchmark-specific header. */
/* Default data type is double, default size is 100x10000. */
#include "jacobi-1d-imper.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 10;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[10];
  double B[10];
  int t;
  int i;
  int j;
  int UB;
  
#pragma scop
{
    int c0;
    int c1;
{
      int c2;
      int c4;
      int c5;
      if (UB >= 3) {
#pragma omp parallel for private(c5, c4)
        for (c2 = 0; c2 <= 1; c2++) {
          if (UB >= 4) {
            for (c4 = (c2 * 2 < 0?-(-c2 / 2) : ((2 < 0?(-c2 + - 2 - 1) / - 2 : (c2 + 2 - 1) / 2))); c4 <= (((2 * c2 + UB + -2) * 4 < 0?((4 < 0?-((-(2 * c2 + UB + -2) + 4 + 1) / 4) : -((-(2 * c2 + UB + -2) + 4 - 1) / 4))) : (2 * c2 + UB + -2) / 4)); c4++) {
              for (c5 = (4 * c4 > 2 * c2 + 2?4 * c4 : 2 * c2 + 2); c5 <= ((4 * c4 + 3 < 2 * c2 + UB + -2?4 * c4 + 3 : 2 * c2 + UB + -2)); c5++) {
                B[- 2 * c2 + c5] = 0.33333 * (A[- 2 * c2 + c5 - 1] + A[- 2 * c2 + c5] + A[- 2 * c2 + c5 + 1]);
              }
            }
          }
          B[1] = 0.33333 * (A[1 - 1] + A[1] + A[1 + 1]);
          A[UB + - 2] = B[UB + - 2];
          if (UB >= 4) {
            for (c4 = (c2 * 2 < 0?-(-c2 / 2) : ((2 < 0?(-c2 + - 2 - 1) / - 2 : (c2 + 2 - 1) / 2))); c4 <= (((2 * c2 + UB + -2) * 4 < 0?((4 < 0?-((-(2 * c2 + UB + -2) + 4 + 1) / 4) : -((-(2 * c2 + UB + -2) + 4 - 1) / 4))) : (2 * c2 + UB + -2) / 4)); c4++) {
              for (c5 = (4 * c4 > 2 * c2 + 2?4 * c4 : 2 * c2 + 2); c5 <= ((4 * c4 + 3 < 2 * c2 + UB + -2?4 * c4 + 3 : 2 * c2 + UB + -2)); c5++) {
                A[- 2 * c2 + c5 + - 1] = B[- 2 * c2 + c5 + - 1];
              }
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
