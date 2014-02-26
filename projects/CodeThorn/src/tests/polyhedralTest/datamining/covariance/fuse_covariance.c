/**
 * covariance.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "covariance.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int n = 32;
  int m = 32;
  double float_n = 1.2;
  double data[m][n];
  double symmat[m][n];
  double mean[m];
  int i;
  int j;
  int j1;
  int j2;
  
#pragma scop
{
    int c0;
    int c3;
    int c4;
    for (c0 = 0; c0 <= 31; c0++) {
      for (c3 = c0; c3 <= 31; c3++) {
        symmat[c0][c3] = 0.0;
      }
      mean[c0] = 0.0;
      for (c3 = 0; c3 <= 31; c3++) {
        mean[c0] += data[c3][c0];
      }
      mean[c0] /= float_n;
      for (c3 = 0; c3 <= 31; c3++) {
        data[c3][c0] -= mean[c0];
      }
      for (c3 = 0; c3 <= c0; c3++) {
        for (c4 = 0; c4 <= 31; c4++) {
          symmat[c3][c0] += data[c4][c3] * data[c4][c0];
        }
      }
      for (c3 = 0; c3 <= c0; c3++) {
        symmat[c0][c3] = symmat[c3][c0];
      }
    }
  }
  
#pragma endscop
  return 0;
}
