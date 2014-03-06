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
    int c0;
    int c2;
    int c3;
{
      int c6;
      int c7;
      int c4;
      for (c4 = 1; c4 <= 33; c4++) {
        if (c4 == 33) {
          ey[1][63] = ey[1][63] - 0.5 * (hz[1][63] - hz[1 - 1][63]);
          ey[0][63] = _fict_[0];
        }
        if (c4 >= 2 && c4 <= 32) {
          for (c7 = 2 * c4 + -2; c7 <= 2 * c4 + -1; c7++) {
            ex[0][c7] = ex[0][c7] - 0.5 * (hz[0][c7] - hz[0][c7 - 1]);
            ey[1][c7 + - 1] = ey[1][c7 + - 1] - 0.5 * (hz[1][c7 + - 1] - hz[1 - 1][c7 + - 1]);
            ey[0][c7 + - 1] = _fict_[0];
            hz[0][c7 + - 1] = hz[0][c7 + - 1] - 0.7 * (ex[0][c7 + - 1 + 1] - ex[0][c7 + - 1] + ey[0 + 1][c7 + - 1] - ey[0][c7 + - 1]);
            ex[0][c7 + - 1] = ex[0][c7 + - 1] - 0.5 * (hz[0][c7 + - 1] - hz[0][c7 + - 1 - 1]);
            ey[0][c7 + - 1] = _fict_[1];
          }
        }
        if (c4 == 1) {
          ex[0][1] = ex[0][1] - 0.5 * (hz[0][1] - hz[0][1 - 1]);
          ey[1][1 + - 1] = ey[1][1 + - 1] - 0.5 * (hz[1][1 + - 1] - hz[1 - 1][1 + - 1]);
          ey[0][1 + - 1] = _fict_[0];
          hz[0][1 + - 1] = hz[0][1 + - 1] - 0.7 * (ex[0][1 + - 1 + 1] - ex[0][1 + - 1] + ey[0 + 1][1 + - 1] - ey[0][1 + - 1]);
        }
        if (c4 == 33) {
          ex[0][64 + - 1] = ex[0][64 + - 1] - 0.5 * (hz[0][64 + - 1] - hz[0][64 + - 1 - 1]);
          ey[0][64 + - 1] = _fict_[1];
        }
        if (c4 == 1) {
          ey[0][0] = _fict_[1];
        }
      }
      for (c4 = 34; c4 <= 95; c4++) {
        ey[c4 + -32][63] = ey[c4 + -32][63] - 0.5 * (hz[c4 + -32][63] - hz[c4 + -32 - 1][63]);
        if (c4 >= 64) {
          for (c7 = (1 > 2 * c4 + -128?1 : 2 * c4 + -128); c7 <= 2 * c4 + -127; c7++) {
            ex[63][c7] = ex[63][c7] - 0.5 * (hz[63][c7] - hz[63][c7 - 1]);
          }
        }
      }
      for (c4 = 2; c4 <= 64; c4++) {
        if (c4 <= 63) {
          ey[c4 + - 1 * 0][1 + - 1] = ey[c4 + - 1 * 0][1 + - 1] - 0.5 * (hz[c4 + - 1 * 0][1 + - 1] - hz[c4 + - 1 * 0 - 1][1 + - 1]);
          ex[c4 + - 1][1 + - 1 * 0] = ex[c4 + - 1][1 + - 1 * 0] - 0.5 * (hz[c4 + - 1][1 + - 1 * 0] - hz[c4 + - 1][1 + - 1 * 0 - 1]);
          hz[c4 + - 1 * 0 + - 1][1 + - 1 * 0 + - 1] = hz[c4 + - 1 * 0 + - 1][1 + - 1 * 0 + - 1] - 0.7 * (ex[c4 + - 1 * 0 + - 1][1 + - 1 * 0 + - 1 + 1] - ex[c4 + - 1 * 0 + - 1][1 + - 1 * 0 + - 1] + ey[c4 + - 1 * 0 + - 1 + 1][1 + - 1 * 0 + - 1] - ey[c4 + - 1 * 0 + - 1][1 + - 1 * 0 + - 1]);
          ey[c4 + - 1][0] = ey[c4 + - 1][0] - 0.5 * (hz[c4 + - 1][0] - hz[c4 + - 1 - 1][0]);
        }
        if (c4 == 64) {
          ey[64 + - 1][0] = ey[64 + - 1][0] - 0.5 * (hz[64 + - 1][0] - hz[64 + - 1 - 1][0]);
        }
        for (c6 = 2; c6 <= 64; c6++) {
          for (c7 = (((0 > c4 + -63?0 : c4 + -63)) > c6 + -63?((0 > c4 + -63?0 : c4 + -63)) : c6 + -63); c7 <= 1; c7++) {
            ey[c4 + - 1 * c7][c6 + - 1] = ey[c4 + - 1 * c7][c6 + - 1] - 0.5 * (hz[c4 + - 1 * c7][c6 + - 1] - hz[c4 + - 1 * c7 - 1][c6 + - 1]);
            ex[c4 + - 1][c6 + - 1 * c7] = ex[c4 + - 1][c6 + - 1 * c7] - 0.5 * (hz[c4 + - 1][c6 + - 1 * c7] - hz[c4 + - 1][c6 + - 1 * c7 - 1]);
            hz[c4 + - 1 * c7 + - 1][c6 + - 1 * c7 + - 1] = hz[c4 + - 1 * c7 + - 1][c6 + - 1 * c7 + - 1] - 0.7 * (ex[c4 + - 1 * c7 + - 1][c6 + - 1 * c7 + - 1 + 1] - ex[c4 + - 1 * c7 + - 1][c6 + - 1 * c7 + - 1] + ey[c4 + - 1 * c7 + - 1 + 1][c6 + - 1 * c7 + - 1] - ey[c4 + - 1 * c7 + - 1][c6 + - 1 * c7 + - 1]);
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
