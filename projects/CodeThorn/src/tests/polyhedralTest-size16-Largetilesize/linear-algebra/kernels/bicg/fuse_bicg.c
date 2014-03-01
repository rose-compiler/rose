#   define NX ARRAYSIZE
#   define NY ARRAYSIZE
# define _PB_NX ARRAYSIZE
# define _PB_NY ARRAYSIZE
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

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int nx = 16;
  int ny = 16;
  double A[16][16];
  double s[16];
  double q[16];
  double p[16];
  double r[16];
  int i;
  int j;
  
#pragma scop
{
    int c1;
    int c3;
    for (c1 = 0; c1 <= 15; c1++) {
      s[c1] = 0;
    }
    for (c1 = 0; c1 <= 15; c1++) {
      q[c1] = 0;
      for (c3 = 0; c3 <= 15; c3++) {
        s[c3] = s[c3] + r[c1] * A[c1][c3];
        q[c1] = q[c1] + A[c1][c3] * p[c3];
      }
    }
  }
  
#pragma endscop
  return 0;
}
