#   define TSTEPS STEPSIZE
#   define N ARRAYSIZE
# define _PB_TSTEPS STEPSIZE
# define _PB_N ARRAYSIZE
/**
 * seidel-2d.c: This file is part of the PolyBench/C 3.2 test suite.
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
  int n = 16;
  int tsteps = 2;
/* Variable declaration/allocation. */
  double A[16][16];
  int t;
  int i;
  int j;
  
#pragma scop
{
    int c0;
    int c4;
    int c3;
    int c2;
    for (c0 = 0; c0 <= 2; c0++) {
      for (c2 = (2 > 7 * c0 + 1?2 : 7 * c0 + 1); c2 <= ((30 < 7 * c0 + 21?30 : 7 * c0 + 21)); c2++) {
        for (c3 = (((0 > (((c2 + -28) * 2 < 0?-(-(c2 + -28) / 2) : ((2 < 0?(-(c2 + -28) + - 2 - 1) / - 2 : (c2 + -28 + 2 - 1) / 2))))?0 : (((c2 + -28) * 2 < 0?-(-(c2 + -28) / 2) : ((2 < 0?(-(c2 + -28) + - 2 - 1) / - 2 : (c2 + -28 + 2 - 1) / 2)))))) > -7 * c0 + c2 + -20?((0 > (((c2 + -28) * 2 < 0?-(-(c2 + -28) / 2) : ((2 < 0?(-(c2 + -28) + - 2 - 1) / - 2 : (c2 + -28 + 2 - 1) / 2))))?0 : (((c2 + -28) * 2 < 0?-(-(c2 + -28) / 2) : ((2 < 0?(-(c2 + -28) + - 2 - 1) / - 2 : (c2 + -28 + 2 - 1) / 2)))))) : -7 * c0 + c2 + -20); c3 <= ((((1 < (((c2 + -2) * 2 < 0?((2 < 0?-((-(c2 + -2) + 2 + 1) / 2) : -((-(c2 + -2) + 2 - 1) / 2))) : (c2 + -2) / 2))?1 : (((c2 + -2) * 2 < 0?((2 < 0?-((-(c2 + -2) + 2 + 1) / 2) : -((-(c2 + -2) + 2 - 1) / 2))) : (c2 + -2) / 2)))) < -7 * c0 + c2 + -1?((1 < (((c2 + -2) * 2 < 0?((2 < 0?-((-(c2 + -2) + 2 + 1) / 2) : -((-(c2 + -2) + 2 - 1) / 2))) : (c2 + -2) / 2))?1 : (((c2 + -2) * 2 < 0?((2 < 0?-((-(c2 + -2) + 2 + 1) / 2) : -((-(c2 + -2) + 2 - 1) / 2))) : (c2 + -2) / 2)))) : -7 * c0 + c2 + -1)); c3++) {
          for (c4 = (((7 * c0 > c3 + 1?7 * c0 : c3 + 1)) > c2 + -1 * c3 + -14?((7 * c0 > c3 + 1?7 * c0 : c3 + 1)) : c2 + -1 * c3 + -14); c4 <= ((((7 * c0 + 6 < c3 + 14?7 * c0 + 6 : c3 + 14)) < c2 + -1 * c3 + -1?((7 * c0 + 6 < c3 + 14?7 * c0 + 6 : c3 + 14)) : c2 + -1 * c3 + -1)); c4++) {
            A[-1 * c3 + c4][c2 + -1 * c3 + -1 * c4] = (A[-1 * c3 + c4 - 1][c2 + -1 * c3 + -1 * c4 - 1] + A[-1 * c3 + c4 - 1][c2 + -1 * c3 + -1 * c4] + A[-1 * c3 + c4 - 1][c2 + -1 * c3 + -1 * c4 + 1] + A[-1 * c3 + c4][c2 + -1 * c3 + -1 * c4 - 1] + A[-1 * c3 + c4][c2 + -1 * c3 + -1 * c4] + A[-1 * c3 + c4][c2 + -1 * c3 + -1 * c4 + 1] + A[-1 * c3 + c4 + 1][c2 + -1 * c3 + -1 * c4 - 1] + A[-1 * c3 + c4 + 1][c2 + -1 * c3 + -1 * c4] + A[-1 * c3 + c4 + 1][c2 + -1 * c3 + -1 * c4 + 1]) / 9.0;
          }
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
