/**
 * fdtd-2d.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#   define TMAX STEPSIZE
#   define NX ARRAYSIZE
#   define NY ARRAYSIZE
# define _PB_TMAX STEPSIZE 
# define _PB_NX ARRAYSIZE
# define _PB_NY ARRAYSIZE

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int tmax = 2;
  int nx = 16;
  int ny = 16;
/* Variable declaration/allocation. */
  double ex[16][16];
  double ey[16][16];
  double hz[16][16];
  double _fict_[2];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c3;
    int c1;
    int c5;
    for (c1 = 0; c1 <= 1; c1++) {
      for (c3 = 0; c3 <= 15; c3++) {
        ey[0][c3] = _fict_[c1];
      }
      for (c3 = 1; c3 <= 15; c3++) {
        for (c5 = 0; c5 <= 15; c5++) {
          ey[c3][c5] = ey[c3][c5] - 0.5 * (hz[c3][c5] - hz[c3 - 1][c5]);
        }
      }
      for (c3 = 0; c3 <= 15; c3++) {
        for (c5 = 1; c5 <= 15; c5++) {
          ex[c3][c5] = ex[c3][c5] - 0.5 * (hz[c3][c5] - hz[c3][c5 - 1]);
        }
      }
      for (c3 = 0; c3 <= 14; c3++) {
        for (c5 = 0; c5 <= 14; c5++) {
          hz[c3][c5] = hz[c3][c5] - 0.7 * (ex[c3][c5 + 1] - ex[c3][c5] + ey[c3 + 1][c5] - ey[c3][c5]);
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
