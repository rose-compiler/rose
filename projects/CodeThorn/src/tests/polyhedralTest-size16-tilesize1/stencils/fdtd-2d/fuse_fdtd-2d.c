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
    int c1;
    int c2;
    int c0;
    for (c0 = 0; c0 <= 1; c0++) {
      if (c0 == 1) {
        ey[0][0] = _fict_[1];
        for (c2 = 2; c2 <= 16; c2++) {
          ey[c2 + -1][0] = ey[c2 + -1][0] - 0.5 * (hz[c2 + -1][0] - hz[c2 + -1 - 1][0]);
        }
      }
      for (c1 = c0 + 1; c1 <= c0 + 15; c1++) {
        if (c0 == 0) {
          ex[0][c1] = ex[0][c1] - 0.5 * (hz[0][c1] - hz[0][c1 - 1]);
          ey[1][c1 + -1] = ey[1][c1 + -1] - 0.5 * (hz[1][c1 + -1] - hz[1 - 1][c1 + -1]);
          ey[0][c1 + -1] = _fict_[0];
          hz[0][c1 + -1] = hz[0][c1 + -1] - 0.7 * (ex[0][c1 + -1 + 1] - ex[0][c1 + -1] + ey[0 + 1][c1 + -1] - ey[0][c1 + -1]);
        }
        if (c0 == 1) {
          ex[0][c1 + -1] = ex[0][c1 + -1] - 0.5 * (hz[0][c1 + -1] - hz[0][c1 + -1 - 1]);
          ey[0][c1 + -1] = _fict_[1];
        }
        for (c2 = 2; c2 <= c0 + 15; c2++) {
          if (c0 == 1) {
            ey[c2 + -1][c1 + -1] = ey[c2 + -1][c1 + -1] - 0.5 * (hz[c2 + -1][c1 + -1] - hz[c2 + -1 - 1][c1 + -1]);
            ex[c2 + -1][c1 + -1] = ex[c2 + -1][c1 + -1] - 0.5 * (hz[c2 + -1][c1 + -1] - hz[c2 + -1][c1 + -1 - 1]);
          }
          if (c0 == 0) {
            ex[c2 + -1][c1] = ex[c2 + -1][c1] - 0.5 * (hz[c2 + -1][c1] - hz[c2 + -1][c1 - 1]);
          }
          if (c0 == 0) {
            ey[c2][c1 + -1] = ey[c2][c1 + -1] - 0.5 * (hz[c2][c1 + -1] - hz[c2 - 1][c1 + -1]);
          }
          hz[-1 * c0 + c2 + -1][-1 * c0 + c1 + -1] = hz[-1 * c0 + c2 + -1][-1 * c0 + c1 + -1] - 0.7 * (ex[-1 * c0 + c2 + -1][-1 * c0 + c1 + -1 + 1] - ex[-1 * c0 + c2 + -1][-1 * c0 + c1 + -1] + ey[-1 * c0 + c2 + -1 + 1][-1 * c0 + c1 + -1] - ey[-1 * c0 + c2 + -1][-1 * c0 + c1 + -1]);
        }
        if (c0 == 0) {
          ex[15][c1] = ex[15][c1] - 0.5 * (hz[15][c1] - hz[15][c1 - 1]);
        }
      }
      if (c0 == 0) {
        ey[1][15] = ey[1][15] - 0.5 * (hz[1][15] - hz[1 - 1][15]);
        ey[0][15] = _fict_[0];
        for (c2 = 2; c2 <= 15; c2++) {
          ey[c2][15] = ey[c2][15] - 0.5 * (hz[c2][15] - hz[c2 - 1][15]);
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
