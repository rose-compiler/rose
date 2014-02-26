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
    int c1;
    int c2;
    int c0;
    int c3;
    for (c0 = 0; c0 <= 9; c0++) {
      for (c1 = 0; c1 <= 9; c1++) {
        for (c2 = 0; c2 <= 8; c2++) {
          sum[c0][c1][c2] = 0;
          for (c3 = 0; c3 <= c2; c3++) {
            sum[c0][c1][c3] = sum[c0][c1][c3] + A[c0][c1][c2 + -1 * c3] * C4[c2 + -1 * c3][c3];
          }
        }
        sum[c0][c1][9] = 0;
        for (c3 = 0; c3 <= 9; c3++) {
          sum[c0][c1][c3] = sum[c0][c1][c3] + A[c0][c1][-1 * c3 + 9] * C4[-1 * c3 + 9][c3];
        }
        A[c0][c1][0] = sum[c0][c1][0];
        for (c2 = 10; c2 <= 18; c2++) {
          for (c3 = c2 + -9; c3 <= 9; c3++) {
            sum[c0][c1][c3] = sum[c0][c1][c3] + A[c0][c1][c2 + -1 * c3] * C4[c2 + -1 * c3][c3];
          }
          A[c0][c1][c2 + -9] = sum[c0][c1][c2 + -9];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
