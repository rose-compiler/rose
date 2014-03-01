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
    int c0;
    int c1;
{
      int c2;
      int c3;
      for (c2 = 0; c2 <= 15; c2++) {
        s[c2] = 0;
        s[c2] = s[c2] + r[0] * A[0][c2];
        s[c2] = s[c2] + r[1] * A[1][c2];
        q[c2] = 0;
        q[c2] = q[c2] + A[c2][0] * p[0];
        q[c2] = q[c2] + A[c2][1] * p[1];
        for (c3 = 2; c3 <= 15; c3++) {
          s[c2] = s[c2] + r[c3] * A[c3][c2];
          q[c2] = q[c2] + A[c2][c3] * p[c3];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
