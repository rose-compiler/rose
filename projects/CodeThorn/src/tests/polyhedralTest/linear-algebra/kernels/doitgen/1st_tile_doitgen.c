/**
 * doitgen.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "doitgen.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int nr = 10;
  int nq = 10;
  int np = 10;
  double A[10][10][10];
  double sum[10][10][10];
  double C4[10][10];
  int r;
  int q;
  int p;
  int s;
  
#pragma scop
{
    int c3;
    int c2;
    int c1;
    int c7;
    int c5;
#pragma omp parallel for private(c5, c2, c3)
    for (c1 = 0; c1 <= 1; c1++) {
      for (c2 = 0; c2 <= 9; c2++) {
        for (c3 = 0; c3 <= 9; c3++) {
          for (c5 = 8 * c1; c5 <= ((9 < 8 * c1 + 7?9 : 8 * c1 + 7)); c5++) {
            sum[c5][c2][c3] = 0;
          }
        }
      }
    }
#pragma omp parallel for private(c5, c7, c2, c3)
    for (c1 = 0; c1 <= 1; c1++) {
      for (c2 = 0; c2 <= 9; c2++) {
        for (c3 = 0; c3 <= 9; c3++) {
          for (c5 = 8 * c1; c5 <= ((9 < 8 * c1 + 7?9 : 8 * c1 + 7)); c5++) {
            for (c7 = 0; c7 <= 9; c7++) {
              sum[c5][c2][c3] = sum[c5][c2][c3] + A[c5][c2][c7] * C4[c7][c3];
            }
          }
        }
      }
    }
#pragma omp parallel for private(c5, c2, c3)
    for (c1 = 0; c1 <= 1; c1++) {
      for (c2 = 0; c2 <= 9; c2++) {
        for (c3 = 0; c3 <= 9; c3++) {
          for (c5 = 8 * c1; c5 <= ((9 < 8 * c1 + 7?9 : 8 * c1 + 7)); c5++) {
            A[c5][c2][c3] = sum[c5][c2][c3];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
