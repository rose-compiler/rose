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
{
      int c6;
      int c8;
      int c2;
      for (c2 = 0; c2 <= 1; c2++) {
        for (c6 = 0; c6 <= 15; c6++) {
          ey[0][c6] = _fict_[c2];
        }
        for (c6 = 1; c6 <= 15; c6++) {
          for (c8 = 0; c8 <= 15; c8++) {
            ey[c6][c8] = ey[c6][c8] - 0.5 * (hz[c6][c8] - hz[c6 - 1][c8]);
          }
        }
        for (c6 = 0; c6 <= 15; c6++) {
          for (c8 = 1; c8 <= 15; c8++) {
            ex[c6][c8] = ex[c6][c8] - 0.5 * (hz[c6][c8] - hz[c6][c8 - 1]);
          }
        }
        for (c6 = 0; c6 <= 14; c6++) {
          for (c8 = 0; c8 <= 14; c8++) {
            hz[c6][c8] = hz[c6][c8] - 0.7 * (ex[c6][c8 + 1] - ex[c6][c8] + ey[c6 + 1][c8] - ey[c6][c8]);
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
