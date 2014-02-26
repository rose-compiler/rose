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
{
      int c5;
      int c6;
      int c4;
      int c7;
      for (c4 = 0; c4 <= 9; c4++) {
        for (c5 = 0; c5 <= 9; c5++) {
          sum[c4][c5][0] = 0;
          for (c7 = 0; c7 <= 8; c7++) {
            sum[c4][c5][0] = sum[c4][c5][0] + A[c4][c5][c7 + - 1 * 0] * C4[c7 + - 1 * 0][0];
          }
          sum[c4][c5][1] = 0;
          for (c7 = 1; c7 <= 7; c7++) {
            sum[c4][c5][1] = sum[c4][c5][1] + A[c4][c5][c7 + - 1 * 1] * C4[c7 + - 1 * 1][1];
          }
          sum[c4][c5][0] = sum[c4][c5][0] + A[c4][c5][- 1 * 0 + 9] * C4[- 1 * 0 + 9][0];
          sum[c4][c5][9] = 0;
          sum[c4][c5][1] = sum[c4][c5][1] + A[c4][c5][8 + - 1 * 1] * C4[8 + - 1 * 1][1];
          for (c6 = 2; c6 <= 8; c6++) {
            if (c6 == 8) {
              sum[c4][c5][7] = sum[c4][c5][7] + A[c4][c5][- 1 * 7 + 9] * C4[- 1 * 7 + 9][7];
              sum[c4][c5][8] = 0;
              sum[c4][c5][8] = sum[c4][c5][8] + A[c4][c5][8 + - 1 * 8] * C4[8 + - 1 * 8][8];
            }
            if (c6 <= 7) {
              sum[c4][c5][c6] = 0;
              sum[c4][c5][c6] = sum[c4][c5][c6] + A[c4][c5][c6 + - 1 * c6] * C4[c6 + - 1 * c6][c6];
            }
            for (c7 = c6 + 1; c7 <= 7; c7++) {
              sum[c4][c5][c6] = sum[c4][c5][c6] + A[c4][c5][c7 + - 1 * c6] * C4[c7 + - 1 * c6][c6];
            }
            if (c6 <= 7) {
              sum[c4][c5][c6 + -1] = sum[c4][c5][c6 + -1] + A[c4][c5][- 1 * (c6 + -1) + 9] * C4[- 1 * (c6 + -1) + 9][c6 + -1];
              sum[c4][c5][c6] = sum[c4][c5][c6] + A[c4][c5][8 + - 1 * c6] * C4[8 + - 1 * c6][c6];
            }
          }
          for (c6 = 9; c6 <= 10; c6++) {
            sum[c4][c5][c6 + -1] = sum[c4][c5][c6 + -1] + A[c4][c5][- 1 * (c6 + -1) + 9] * C4[- 1 * (c6 + -1) + 9][c6 + -1];
          }
          sum[c4][c5][1] = sum[c4][c5][1] + A[c4][c5][10 + - 1 * 1] * C4[10 + - 1 * 1][1];
          A[c4][c5][0] = sum[c4][c5][0];
          for (c7 = 9; c7 <= 16; c7++) {
            sum[c4][c5][c7 + -7] = sum[c4][c5][c7 + -7] + A[c4][c5][10 + - 1 * (c7 + -7)] * C4[10 + - 1 * (c7 + -7)][c7 + -7];
          }
          A[c4][c5][10 + - 9] = sum[c4][c5][10 + - 9];
          for (c6 = 11; c6 <= 18; c6++) {
            for (c7 = c6 + -2; c7 <= 16; c7++) {
              sum[c4][c5][c7 + -7] = sum[c4][c5][c7 + -7] + A[c4][c5][c6 + - 1 * (c7 + -7)] * C4[c6 + - 1 * (c7 + -7)][c7 + -7];
            }
            A[c4][c5][c6 + - 9] = sum[c4][c5][c6 + - 9];
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
