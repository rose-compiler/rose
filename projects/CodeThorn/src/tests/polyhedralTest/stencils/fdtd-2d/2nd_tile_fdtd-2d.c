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
    int c3;
    int c0;
    int c2;
{
      int c6;
      int c7;
      int c4;
      for (c4 = 1; c4 <= 17; c4++) {
        if (c4 == 17) {
          ey[1][31] = ey[1][31] - 0.5 * (hz[1][31] - hz[1 - 1][31]);
          ey[0][31] = _fict_[0];
        }
        if (c4 >= 2 && c4 <= 16) {
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
        if (c4 == 17) {
          ex[0][32 + - 1] = ex[0][32 + - 1] - 0.5 * (hz[0][32 + - 1] - hz[0][32 + - 1 - 1]);
          ey[0][32 + - 1] = _fict_[1];
        }
        if (c4 == 1) {
          ey[0][0] = _fict_[1];
        }
      }
      for (c4 = 18; c4 <= 47; c4++) {
        ey[c4 + -16][31] = ey[c4 + -16][31] - 0.5 * (hz[c4 + -16][31] - hz[c4 + -16 - 1][31]);
        if (c4 >= 32) {
          for (c7 = (1 > 2 * c4 + -64?1 : 2 * c4 + -64); c7 <= 2 * c4 + -63; c7++) {
            ex[31][c7] = ex[31][c7] - 0.5 * (hz[31][c7] - hz[31][c7 - 1]);
          }
        }
      }
      for (c4 = 2; c4 <= 32; c4++) {
        if (c4 <= 31) {
          ey[c4 + - 1 * 0][1 + - 1] = ey[c4 + - 1 * 0][1 + - 1] - 0.5 * (hz[c4 + - 1 * 0][1 + - 1] - hz[c4 + - 1 * 0 - 1][1 + - 1]);
          ex[c4 + - 1][1 + - 1 * 0] = ex[c4 + - 1][1 + - 1 * 0] - 0.5 * (hz[c4 + - 1][1 + - 1 * 0] - hz[c4 + - 1][1 + - 1 * 0 - 1]);
          hz[c4 + - 1 * 0 + - 1][1 + - 1 * 0 + - 1] = hz[c4 + - 1 * 0 + - 1][1 + - 1 * 0 + - 1] - 0.7 * (ex[c4 + - 1 * 0 + - 1][1 + - 1 * 0 + - 1 + 1] - ex[c4 + - 1 * 0 + - 1][1 + - 1 * 0 + - 1] + ey[c4 + - 1 * 0 + - 1 + 1][1 + - 1 * 0 + - 1] - ey[c4 + - 1 * 0 + - 1][1 + - 1 * 0 + - 1]);
          ey[c4 + - 1][0] = ey[c4 + - 1][0] - 0.5 * (hz[c4 + - 1][0] - hz[c4 + - 1 - 1][0]);
        }
        if (c4 == 32) {
          ey[32 + - 1][0] = ey[32 + - 1][0] - 0.5 * (hz[32 + - 1][0] - hz[32 + - 1 - 1][0]);
        }
        for (c6 = 2; c6 <= 32; c6++) {
          for (c7 = (((0 > c4 + -31?0 : c4 + -31)) > c6 + -31?((0 > c4 + -31?0 : c4 + -31)) : c6 + -31); c7 <= 1; c7++) {
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
