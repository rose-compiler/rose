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
    int c1;
    int c2;
{
      int c6;
      int c5;
      for (c5 = 2; c5 <= 15; c5++) {
        ey[c5][15] = ey[c5][15] - 0.5 * (hz[c5][15] - hz[c5 - 1][15]);
      }
      ey[0][15] = _fict_[0];
      ey[1][15] = ey[1][15] - 0.5 * (hz[1][15] - hz[1 - 1][15]);
      for (c5 = 1; c5 <= 15; c5++) {
        ex[15][c5] = ex[15][c5] - 0.5 * (hz[15][c5] - hz[15][c5 - 1]);
      }
      for (c5 = 1; c5 <= 15; c5++) {
        ey[0][c5 + - 1] = _fict_[0];
      }
      for (c5 = 1; c5 <= 15; c5++) {
        ey[1][c5 + - 1] = ey[1][c5 + - 1] - 0.5 * (hz[1][c5 + - 1] - hz[1 - 1][c5 + - 1]);
      }
      for (c5 = 1; c5 <= 15; c5++) {
        ex[0][c5] = ex[0][c5] - 0.5 * (hz[0][c5] - hz[0][c5 - 1]);
      }
      for (c5 = 1; c5 <= 15; c5++) {
        hz[0][c5 + - 1] = hz[0][c5 + - 1] - 0.7 * (ex[0][c5 + - 1 + 1] - ex[0][c5 + - 1] + ey[0 + 1][c5 + - 1] - ey[0][c5 + - 1]);
      }
      for (c5 = 2; c5 <= 16; c5++) {
        ey[0][c5 + - 1] = _fict_[1];
      }
      for (c5 = 2; c5 <= 16; c5++) {
        ex[0][c5 + - 1] = ex[0][c5 + - 1] - 0.5 * (hz[0][c5 + - 1] - hz[0][c5 + - 1 - 1]);
      }
      ey[0][0] = _fict_[1];
      for (c5 = 2; c5 <= 15; c5++) {
        for (c6 = 1; c6 <= 15; c6++) {
          ey[- 1 * 0 + c5][c6 + - 1] = ey[- 1 * 0 + c5][c6 + - 1] - 0.5 * (hz[- 1 * 0 + c5][c6 + - 1] - hz[- 1 * 0 + c5 - 1][c6 + - 1]);
          ex[c5 + - 1][- 1 * 0 + c6] = ex[c5 + - 1][- 1 * 0 + c6] - 0.5 * (hz[c5 + - 1][- 1 * 0 + c6] - hz[c5 + - 1][- 1 * 0 + c6 - 1]);
          hz[- 1 * 0 + c5 + - 1][- 1 * 0 + c6 + - 1] = hz[- 1 * 0 + c5 + - 1][- 1 * 0 + c6 + - 1] - 0.7 * (ex[- 1 * 0 + c5 + - 1][- 1 * 0 + c6 + - 1 + 1] - ex[- 1 * 0 + c5 + - 1][- 1 * 0 + c6 + - 1] + ey[- 1 * 0 + c5 + - 1 + 1][- 1 * 0 + c6 + - 1] - ey[- 1 * 0 + c5 + - 1][- 1 * 0 + c6 + - 1]);
        }
      }
      for (c5 = 2; c5 <= 16; c5++) {
        ey[c5 + - 1][0] = ey[c5 + - 1][0] - 0.5 * (hz[c5 + - 1][0] - hz[c5 + - 1 - 1][0]);
        for (c6 = 2; c6 <= 16; c6++) {
          ey[- 1 * 1 + c5][c6 + - 1] = ey[- 1 * 1 + c5][c6 + - 1] - 0.5 * (hz[- 1 * 1 + c5][c6 + - 1] - hz[- 1 * 1 + c5 - 1][c6 + - 1]);
          ex[c5 + - 1][- 1 * 1 + c6] = ex[c5 + - 1][- 1 * 1 + c6] - 0.5 * (hz[c5 + - 1][- 1 * 1 + c6] - hz[c5 + - 1][- 1 * 1 + c6 - 1]);
          hz[- 1 * 1 + c5 + - 1][- 1 * 1 + c6 + - 1] = hz[- 1 * 1 + c5 + - 1][- 1 * 1 + c6 + - 1] - 0.7 * (ex[- 1 * 1 + c5 + - 1][- 1 * 1 + c6 + - 1 + 1] - ex[- 1 * 1 + c5 + - 1][- 1 * 1 + c6 + - 1] + ey[- 1 * 1 + c5 + - 1 + 1][- 1 * 1 + c6 + - 1] - ey[- 1 * 1 + c5 + - 1][- 1 * 1 + c6 + - 1]);
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
