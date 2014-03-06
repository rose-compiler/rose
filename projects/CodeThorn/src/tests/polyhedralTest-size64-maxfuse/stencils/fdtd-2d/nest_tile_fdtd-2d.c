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
  int nx = 64;
  int ny = 64;
/* Variable declaration/allocation. */
  double ex[64][64];
  double ey[64][64];
  double hz[64][64];
  double _fict_[2];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c2;
    int c0;
    int c3;
    int c4;
    for (c0 = 0; c0 <= 16; c0++) {
      for (c2 = 1; c2 <= 64; c2++) {
        if (c0 <= 15 && c2 == 1) {
          for (c4 = (1 > 4 * c0?1 : 4 * c0); c4 <= 4 * c0 + 3; c4++) {
            ey[1][c4 + -1] = ey[1][c4 + -1] - 0.5 * (hz[1][c4 + -1] - hz[1 - 1][c4 + -1]);
            ex[0][c4] = ex[0][c4] - 0.5 * (hz[0][c4] - hz[0][c4 - 1]);
            ey[0][c4 + -1] = _fict_[0];
            hz[0][c4 + -1] = hz[0][c4 + -1] - 0.7 * (ex[0][c4 + -1 + 1] - ex[0][c4 + -1] + ey[0 + 1][c4 + -1] - ey[0][c4 + -1]);
          }
        }
        if (c0 == 16 && c2 == 1) {
          ey[1][63] = ey[1][63] - 0.5 * (hz[1][63] - hz[1 - 1][63]);
          ey[0][63] = _fict_[0];
        }
        if (c2 == 1) {
          if (c0 == 0) {
            ey[0][0] = _fict_[1];
          }
          for (c4 = (2 > 4 * c0?2 : 4 * c0); c4 <= ((64 < 4 * c0 + 3?64 : 4 * c0 + 3)); c4++) {
            ex[0][c4 + -1] = ex[0][c4 + -1] - 0.5 * (hz[0][c4 + -1] - hz[0][c4 + -1 - 1]);
            ey[0][c4 + -1] = _fict_[1];
          }
        }
        if (c0 == 16 && c2 >= 2 && c2 <= 63) {
          ey[c2][63] = ey[c2][63] - 0.5 * (hz[c2][63] - hz[c2 - 1][63]);
        }
        if (c0 <= 15 && c2 == 64) {
          for (c4 = (1 > 4 * c0?1 : 4 * c0); c4 <= 4 * c0 + 3; c4++) {
            ex[63][c4] = ex[63][c4] - 0.5 * (hz[63][c4] - hz[63][c4 - 1]);
          }
        }
        if (c2 >= 2) {
          for (c3 = (((0 > 4 * c0 + -63?0 : 4 * c0 + -63)) > c2 + -63?((0 > 4 * c0 + -63?0 : 4 * c0 + -63)) : c2 + -63); c3 <= 1; c3++) {
            if (c0 == 0 && c3 == 1) {
              ey[c2 + -1][0] = ey[c2 + -1][0] - 0.5 * (hz[c2 + -1][0] - hz[c2 + -1 - 1][0]);
            }
            for (c4 = (4 * c0 > c3 + 1?4 * c0 : c3 + 1); c4 <= ((4 * c0 + 3 < c3 + 63?4 * c0 + 3 : c3 + 63)); c4++) {
              ey[c2 + -1 * c3][c4 + -1] = ey[c2 + -1 * c3][c4 + -1] - 0.5 * (hz[c2 + -1 * c3][c4 + -1] - hz[c2 + -1 * c3 - 1][c4 + -1]);
              ex[c2 + -1][-1 * c3 + c4] = ex[c2 + -1][-1 * c3 + c4] - 0.5 * (hz[c2 + -1][-1 * c3 + c4] - hz[c2 + -1][-1 * c3 + c4 - 1]);
              hz[c2 + -1 * c3 + -1][-1 * c3 + c4 + -1] = hz[c2 + -1 * c3 + -1][-1 * c3 + c4 + -1] - 0.7 * (ex[c2 + -1 * c3 + -1][-1 * c3 + c4 + -1 + 1] - ex[c2 + -1 * c3 + -1][-1 * c3 + c4 + -1] + ey[c2 + -1 * c3 + -1 + 1][-1 * c3 + c4 + -1] - ey[c2 + -1 * c3 + -1][-1 * c3 + c4 + -1]);
            }
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
