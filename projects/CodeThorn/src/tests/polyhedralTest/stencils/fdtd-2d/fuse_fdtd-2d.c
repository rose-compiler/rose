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
/* Include benchmark-specific header. */
/* Default data type is double, default size is 50x1000x1000. */
#include "fdtd-2d.h"

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int tmax = 2;
  int nx = 32;
  int ny = 32;
/* Variable declaration/allocation. */
  double ex[32][32];
  double ey[32][32];
  double hz[32][32];
  double _fict_[2];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c2;
    int c0;
    int c1;
    for (c0 = 0; c0 <= 1; c0++) {
      if (c0 == 1) {
        ey[0][0] = _fict_[1];
        for (c2 = 2; c2 <= 32; c2++) {
          ey[c2 + -1][0] = ey[c2 + -1][0] - 0.5 * (hz[c2 + -1][0] - hz[c2 + -1 - 1][0]);
        }
      }
      for (c1 = c0 + 1; c1 <= c0 + 31; c1++) {
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
        for (c2 = 2; c2 <= c0 + 31; c2++) {
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
          ex[31][c1] = ex[31][c1] - 0.5 * (hz[31][c1] - hz[31][c1 - 1]);
        }
      }
      if (c0 == 0) {
        ey[1][31] = ey[1][31] - 0.5 * (hz[1][31] - hz[1 - 1][31]);
        ey[0][31] = _fict_[0];
        for (c2 = 2; c2 <= 31; c2++) {
          ey[c2][31] = ey[c2][31] - 0.5 * (hz[c2][31] - hz[c2 - 1][31]);
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
