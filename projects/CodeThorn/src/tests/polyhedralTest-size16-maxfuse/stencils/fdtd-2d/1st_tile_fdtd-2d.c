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
    int c0;
    int c3;
    int c2;
    for (c0 = 1; c0 <= 16; c0++) {
      for (c2 = 1; c2 <= 16; c2++) {
        if (c0 <= 15 && c2 == 1) {
          ex[0][c0] = ex[0][c0] - 0.5 * (hz[0][c0] - hz[0][c0 - 1]);
          ey[1][c0 + -1] = ey[1][c0 + -1] - 0.5 * (hz[1][c0 + -1] - hz[1 - 1][c0 + -1]);
          ey[0][c0 + -1] = _fict_[0];
          hz[0][c0 + -1] = hz[0][c0 + -1] - 0.7 * (ex[0][c0 + -1 + 1] - ex[0][c0 + -1] + ey[0 + 1][c0 + -1] - ey[0][c0 + -1]);
        }
        if (c0 == 16 && c2 == 1) {
          ey[1][15] = ey[1][15] - 0.5 * (hz[1][15] - hz[1 - 1][15]);
          ey[0][15] = _fict_[0];
        }
        if (c0 >= 2 && c2 == 1) {
          ex[0][c0 + -1] = ex[0][c0 + -1] - 0.5 * (hz[0][c0 + -1] - hz[0][c0 + -1 - 1]);
          ey[0][c0 + -1] = _fict_[1];
        }
        if (c0 == 1 && c2 == 1) {
          ey[0][0] = _fict_[1];
        }
        if (c0 == 16 && c2 >= 2 && c2 <= 15) {
          ey[c2][15] = ey[c2][15] - 0.5 * (hz[c2][15] - hz[c2 - 1][15]);
        }
        if (c0 <= 15 && c2 == 16) {
          ex[15][c0] = ex[15][c0] - 0.5 * (hz[15][c0] - hz[15][c0 - 1]);
        }
        if (c2 >= 2) {
          for (c3 = (((0 > c0 + -15?0 : c0 + -15)) > c2 + -15?((0 > c0 + -15?0 : c0 + -15)) : c2 + -15); c3 <= ((1 < c0 + -1?1 : c0 + -1)); c3++) {
            ey[c2 + -1 * c3][c0 + -1] = ey[c2 + -1 * c3][c0 + -1] - 0.5 * (hz[c2 + -1 * c3][c0 + -1] - hz[c2 + -1 * c3 - 1][c0 + -1]);
            ex[c2 + -1][c0 + -1 * c3] = ex[c2 + -1][c0 + -1 * c3] - 0.5 * (hz[c2 + -1][c0 + -1 * c3] - hz[c2 + -1][c0 + -1 * c3 - 1]);
            hz[c2 + -1 * c3 + -1][c0 + -1 * c3 + -1] = hz[c2 + -1 * c3 + -1][c0 + -1 * c3 + -1] - 0.7 * (ex[c2 + -1 * c3 + -1][c0 + -1 * c3 + -1 + 1] - ex[c2 + -1 * c3 + -1][c0 + -1 * c3 + -1] + ey[c2 + -1 * c3 + -1 + 1][c0 + -1 * c3 + -1] - ey[c2 + -1 * c3 + -1][c0 + -1 * c3 + -1]);
          }
        }
        if (c0 == 1 && c2 >= 2) {
          ey[c2 + -1][0] = ey[c2 + -1][0] - 0.5 * (hz[c2 + -1][0] - hz[c2 + -1 - 1][0]);
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
