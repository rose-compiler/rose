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
{
      int c2;
      int c3;
      for (c2 = 0; c2 <= 31; c2++) {
        s[c2] = 0;
        s[c2] = s[c2] + r[0] * A[0][c2];
        s[c2] = s[c2] + r[1] * A[1][c2];
        q[c2] = 0;
        q[c2] = q[c2] + A[c2][0] * p[0];
        q[c2] = q[c2] + A[c2][1] * p[1];
        for (c3 = 2; c3 <= 31; c3++) {
          s[c2] = s[c2] + r[c3] * A[c3][c2];
          q[c2] = q[c2] + A[c2][c3] * p[c3];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
