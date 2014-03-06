/**
 * bicg.c: This file is part of the PolyBench/C 3.2 test suite.
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
#include "bicg.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int nx = 32;
  int ny = 32;
  double A[32][32];
  double s[32];
  double q[32];
  double p[32];
  double r[32];
  int i;
  int j;
  
#pragma scop
{
    int c0;
    int c1;
    for (c0 = 0; c0 <= 31; c0++) {
      q[c0] = 0;
      q[c0] = q[c0] + A[c0][0] * p[0];
      s[c0] = 0;
      s[c0] = s[c0] + r[0] * A[0][c0];
      for (c1 = 1; c1 <= 31; c1++) {
        q[c0] = q[c0] + A[c0][c1] * p[c1];
        s[c0] = s[c0] + r[c1] * A[c1][c0];
      }
    }
  }
  
#pragma endscop
  return 0;
}
