/**
 * 3mm.c: This file is part of the PolyBench/C 3.2 test suite.
 * three steps of matrix multiplication to multiply four matrices. 
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 * License: /LICENSE.OSU.txt
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
/* Include polybench common header. */
#include <polybench.h>
/* Include benchmark-specific header. */
/* Default data type is double, default size is 4000. */
#include "3mm.h"
/* Array initialization. */

static void init_array(int ni,int nj,int nk,int nl,int nm,double A[128 + 0][128 + 0],double B[128 + 0][128 + 0],double C[128 + 0][128 + 0],double D[128 + 0][128 + 0])
{
  //int i;
  //int j;
{
    int c2;
    int c1;
    if (nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = 0; c1 <= ((((((ni + -1 < nj + -1?ni + -1 : nj + -1)) < nk + -1?((ni + -1 < nj + -1?ni + -1 : nj + -1)) : nk + -1)) < nm + -1?((((ni + -1 < nj + -1?ni + -1 : nj + -1)) < nk + -1?((ni + -1 < nj + -1?ni + -1 : nj + -1)) : nk + -1)) : nm + -1)); c1++) {
        for (c2 = 0; c2 <= ((((((nj + -1 < nk + -1?nj + -1 : nk + -1)) < nl + -1?((nj + -1 < nk + -1?nj + -1 : nk + -1)) : nl + -1)) < nm + -1?((((nj + -1 < nk + -1?nj + -1 : nk + -1)) < nl + -1?((nj + -1 < nk + -1?nj + -1 : nk + -1)) : nl + -1)) : nm + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = nl; c2 <= ((((nj + -1 < nk + -1?nj + -1 : nk + -1)) < nm + -1?((nj + -1 < nk + -1?nj + -1 : nk + -1)) : nm + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nm; c2 <= ((((nj + -1 < nk + -1?nj + -1 : nk + -1)) < nl + -1?((nj + -1 < nk + -1?nj + -1 : nk + -1)) : nl + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = (nl > nm?nl : nm); c2 <= ((nj + -1 < nk + -1?nj + -1 : nk + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = nj; c2 <= ((((nk + -1 < nl + -1?nk + -1 : nl + -1)) < nm + -1?((nk + -1 < nl + -1?nk + -1 : nl + -1)) : nm + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = (nj > nl?nj : nl); c2 <= ((nk + -1 < nm + -1?nk + -1 : nm + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = (nj > nm?nj : nm); c2 <= ((nk + -1 < nl + -1?nk + -1 : nl + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = (((nj > nl?nj : nl)) > nm?((nj > nl?nj : nl)) : nm); c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
        for (c2 = nk; c2 <= ((((nj + -1 < nl + -1?nj + -1 : nl + -1)) < nm + -1?((nj + -1 < nl + -1?nj + -1 : nl + -1)) : nm + -1)); c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = (nk > nl?nk : nl); c2 <= ((nj + -1 < nm + -1?nj + -1 : nm + -1)); c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = (nk > nm?nk : nm); c2 <= ((nj + -1 < nl + -1?nj + -1 : nl + -1)); c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = (((nk > nl?nk : nl)) > nm?((nk > nl?nk : nl)) : nm); c2 <= nj + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = (nj > nk?nj : nk); c2 <= ((nl + -1 < nm + -1?nl + -1 : nm + -1)); c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = (((nj > nk?nj : nk)) > nl?((nj > nk?nj : nk)) : nl); c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = (((nj > nk?nj : nk)) > nm?((nj > nk?nj : nk)) : nm); c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
    if (nl <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = 0; c1 <= ((((((ni + -1 < nj + -1?ni + -1 : nj + -1)) < nk + -1?((ni + -1 < nj + -1?ni + -1 : nj + -1)) : nk + -1)) < nm + -1?((((ni + -1 < nj + -1?ni + -1 : nj + -1)) < nk + -1?((ni + -1 < nj + -1?ni + -1 : nj + -1)) : nk + -1)) : nm + -1)); c1++) {
        for (c2 = 0; c2 <= ((((nj + -1 < nk + -1?nj + -1 : nk + -1)) < nm + -1?((nj + -1 < nk + -1?nj + -1 : nk + -1)) : nm + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nm; c2 <= ((nj + -1 < nk + -1?nj + -1 : nk + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = nj; c2 <= ((nk + -1 < nm + -1?nk + -1 : nm + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = (nj > nm?nj : nm); c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
        for (c2 = nk; c2 <= ((nj + -1 < nm + -1?nj + -1 : nm + -1)); c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = (nk > nm?nk : nm); c2 <= nj + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = (nj > nk?nj : nk); c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
      }
    }
    if (nm >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = nm; c1 <= ((((ni + -1 < nj + -1?ni + -1 : nj + -1)) < nk + -1?((ni + -1 < nj + -1?ni + -1 : nj + -1)) : nk + -1)); c1++) {
        for (c2 = 0; c2 <= nm + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nm; c2 <= ((nj + -1 < nk + -1?nj + -1 : nk + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = nj; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
        for (c2 = nk; c2 <= nj + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
      }
    }
    if (nm <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = 0; c1 <= ((((ni + -1 < nj + -1?ni + -1 : nj + -1)) < nk + -1?((ni + -1 < nj + -1?ni + -1 : nj + -1)) : nk + -1)); c1++) {
        for (c2 = 0; c2 <= ((nj + -1 < nk + -1?nj + -1 : nk + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = nj; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
        for (c2 = nk; c2 <= nj + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
      }
    }
    if (nj >= 1 && nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = nj; c1 <= ((((ni + -1 < nk + -1?ni + -1 : nk + -1)) < nm + -1?((ni + -1 < nk + -1?ni + -1 : nk + -1)) : nm + -1)); c1++) {
        for (c2 = 0; c2 <= ((nj + -1 < nl + -1?nj + -1 : nl + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = nl; c2 <= nj + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = nj; c2 <= ((nk + -1 < nl + -1?nk + -1 : nl + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = (nj > nl?nj : nl); c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
        for (c2 = nk; c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
    if (nj >= 1 && nl <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = nj; c1 <= ((((ni + -1 < nk + -1?ni + -1 : nk + -1)) < nm + -1?((ni + -1 < nk + -1?ni + -1 : nk + -1)) : nm + -1)); c1++) {
        for (c2 = 0; c2 <= nj + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = nj; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
      }
    }
    if (nj >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (nj > nm?nj : nm); c1 <= ((ni + -1 < nk + -1?ni + -1 : nk + -1)); c1++) {
        for (c2 = 0; c2 <= nj + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = nj; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
      }
    }
    if (nj <= 0 && nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = 0; c1 <= ((((ni + -1 < nk + -1?ni + -1 : nk + -1)) < nm + -1?((ni + -1 < nk + -1?ni + -1 : nk + -1)) : nm + -1)); c1++) {
        for (c2 = 0; c2 <= ((nk + -1 < nl + -1?nk + -1 : nl + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = nl; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
        for (c2 = nk; c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
    if (nj <= 0 && nl <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = 0; c1 <= ((((ni + -1 < nk + -1?ni + -1 : nk + -1)) < nm + -1?((ni + -1 < nk + -1?ni + -1 : nk + -1)) : nm + -1)); c1++) {
        for (c2 = 0; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
      }
    }
    if (nj <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = (0 > nm?0 : nm); c1 <= ((ni + -1 < nk + -1?ni + -1 : nk + -1)); c1++) {
        for (c2 = 0; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
      }
    }
    if (nk >= 1 && nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = nk; c1 <= ((((ni + -1 < nj + -1?ni + -1 : nj + -1)) < nm + -1?((ni + -1 < nj + -1?ni + -1 : nj + -1)) : nm + -1)); c1++) {
        for (c2 = 0; c2 <= ((nk + -1 < nl + -1?nk + -1 : nl + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = nl; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nk; c2 <= ((nl + -1 < nm + -1?nl + -1 : nm + -1)); c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = (nk > nl?nk : nl); c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nm; c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
    if (nk >= 1 && nl <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = nk; c1 <= ((((ni + -1 < nj + -1?ni + -1 : nj + -1)) < nm + -1?((ni + -1 < nj + -1?ni + -1 : nj + -1)) : nm + -1)); c1++) {
        for (c2 = 0; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nk; c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
      }
    }
    if (nk >= 1 && nm >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (nk > nm?nk : nm); c1 <= ((ni + -1 < nj + -1?ni + -1 : nj + -1)); c1++) {
        for (c2 = 0; c2 <= ((nk + -1 < nm + -1?nk + -1 : nm + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nm; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
        for (c2 = nk; c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
      }
    }
    if (nk >= 1 && nm <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = nk; c1 <= ((ni + -1 < nj + -1?ni + -1 : nj + -1)); c1++) {
        for (c2 = 0; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
      }
    }
    if (nk >= 1 && nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (nj > nk?nj : nk); c1 <= ((ni + -1 < nm + -1?ni + -1 : nm + -1)); c1++) {
        for (c2 = 0; c2 <= ((nk + -1 < nl + -1?nk + -1 : nl + -1)); c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = nl; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
        for (c2 = nk; c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
    if (nk >= 1 && nl <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = (nj > nk?nj : nk); c1 <= ((ni + -1 < nm + -1?ni + -1 : nm + -1)); c1++) {
        for (c2 = 0; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
      }
    }
    if (nk >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (((nj > nk?nj : nk)) > nm?((nj > nk?nj : nk)) : nm); c1 <= ni + -1; c1++) {
        for (c2 = 0; c2 <= nk + -1; c2++) {
          A[c1][c2] = ((double )c1) * c2 / ni;
        }
      }
    }
    if (nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (0 > ni?0 : ni); c1 <= ((((nj + -1 < nk + -1?nj + -1 : nk + -1)) < nm + -1?((nj + -1 < nk + -1?nj + -1 : nk + -1)) : nm + -1)); c1++) {
        for (c2 = 0; c2 <= ((((nj + -1 < nl + -1?nj + -1 : nl + -1)) < nm + -1?((nj + -1 < nl + -1?nj + -1 : nl + -1)) : nm + -1)); c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = nl; c2 <= ((nj + -1 < nm + -1?nj + -1 : nm + -1)); c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nm; c2 <= ((nj + -1 < nl + -1?nj + -1 : nl + -1)); c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = (nl > nm?nl : nm); c2 <= nj + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = nj; c2 <= ((nl + -1 < nm + -1?nl + -1 : nm + -1)); c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = (nj > nl?nj : nl); c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = (nj > nm?nj : nm); c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
    if (nl <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = (0 > ni?0 : ni); c1 <= ((((nj + -1 < nk + -1?nj + -1 : nk + -1)) < nm + -1?((nj + -1 < nk + -1?nj + -1 : nk + -1)) : nm + -1)); c1++) {
        for (c2 = 0; c2 <= ((nj + -1 < nm + -1?nj + -1 : nm + -1)); c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nm; c2 <= nj + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = nj; c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
      }
    }
    if (nm >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (ni > nm?ni : nm); c1 <= ((nj + -1 < nk + -1?nj + -1 : nk + -1)); c1++) {
        for (c2 = 0; c2 <= nm + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nm; c2 <= nj + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
      }
    }
    if (nm <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = (0 > ni?0 : ni); c1 <= ((nj + -1 < nk + -1?nj + -1 : nk + -1)); c1++) {
        for (c2 = 0; c2 <= nj + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
      }
    }
    if (nj >= 1 && nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (ni > nj?ni : nj); c1 <= ((nk + -1 < nm + -1?nk + -1 : nm + -1)); c1++) {
        for (c2 = 0; c2 <= ((nj + -1 < nl + -1?nj + -1 : nl + -1)); c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = nl; c2 <= nj + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
        for (c2 = nj; c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
    if (nj >= 1 && nl <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = (ni > nj?ni : nj); c1 <= ((nk + -1 < nm + -1?nk + -1 : nm + -1)); c1++) {
        for (c2 = 0; c2 <= nj + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
      }
    }
    if (nj >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (((ni > nj?ni : nj)) > nm?((ni > nj?ni : nj)) : nm); c1 <= nk + -1; c1++) {
        for (c2 = 0; c2 <= nj + -1; c2++) {
          B[c1][c2] = ((double )c1) * (c2 + 1) / nj;
        }
      }
    }
    if (nk >= 1 && nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (ni > nk?ni : nk); c1 <= ((nj + -1 < nm + -1?nj + -1 : nm + -1)); c1++) {
        for (c2 = 0; c2 <= ((nl + -1 < nm + -1?nl + -1 : nm + -1)); c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = nl; c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nm; c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
    if (nk >= 1 && nl <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = (ni > nk?ni : nk); c1 <= ((nj + -1 < nm + -1?nj + -1 : nm + -1)); c1++) {
        for (c2 = 0; c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
      }
    }
    if (nk >= 1 && nm >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (((ni > nk?ni : nk)) > nm?((ni > nk?ni : nk)) : nm); c1 <= nj + -1; c1++) {
        for (c2 = 0; c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
      }
    }
    if (nk <= 0 && nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = 0; c1 <= ((nj + -1 < nm + -1?nj + -1 : nm + -1)); c1++) {
        for (c2 = 0; c2 <= ((nl + -1 < nm + -1?nl + -1 : nm + -1)); c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
        for (c2 = nl; c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
        for (c2 = nm; c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
    if (nk <= 0 && nl <= 0) {
#pragma omp parallel for private(c2)
      for (c1 = 0; c1 <= ((nj + -1 < nm + -1?nj + -1 : nm + -1)); c1++) {
        for (c2 = 0; c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
      }
    }
    if (nk <= 0 && nm >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = nm; c1 <= nj + -1; c1++) {
        for (c2 = 0; c2 <= nm + -1; c2++) {
          C[c1][c2] = ((double )c1) * (c2 + 3) / nl;
        }
      }
    }
    if (nj <= 0 && nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (0 > ni?0 : ni); c1 <= ((nk + -1 < nm + -1?nk + -1 : nm + -1)); c1++) {
        for (c2 = 0; c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
    if (nk >= 1 && nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (((ni > nj?ni : nj)) > nk?((ni > nj?ni : nj)) : nk); c1 <= nm + -1; c1++) {
        for (c2 = 0; c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
    if (nk <= 0 && nl >= 1) {
#pragma omp parallel for private(c2)
      for (c1 = (0 > nj?0 : nj); c1 <= nm + -1; c1++) {
        for (c2 = 0; c2 <= nl + -1; c2++) {
          D[c1][c2] = ((double )c1) * (c2 + 2) / nk;
        }
      }
    }
  }
}
/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */

static void print_array(int ni,int nl,double G[128 + 0][128 + 0])
{
  int i;
  int j;
  for (i = 0; i < ni; i++) 
    for (j = 0; j < nl; j++) {
      fprintf(stderr,"%0.2lf ",G[i][j]);
      if ((i * ni + j) % 20 == 0) 
        fprintf(stderr,"\n");
    }
  fprintf(stderr,"\n");
}
/* Main computational kernel. The whole function will be timed,
   including the call and return. */

static void kernel_3mm(int ni,int nj,int nk,int nl,int nm,double E[128 + 0][128 + 0],double A[128 + 0][128 + 0],double B[128 + 0][128 + 0],double F[128 + 0][128 + 0],double C[128 + 0][128 + 0],double D[128 + 0][128 + 0],double G[128 + 0][128 + 0])
{
  //int i;
  //int j;
  //int k;
  
  //#pragma scop
{
    int c1;
    int c2;
    int c5;
#pragma omp parallel for private(c2)
    for (c1 = 0; c1 <= 127; c1++) {
      for (c2 = 0; c2 <= 127; c2++) {
        G[c1][c2] = 0;
        F[c1][c2] = 0;
      }
    }
#pragma omp parallel for private(c5, c2)
    for (c1 = 0; c1 <= 127; c1++) {
      for (c2 = 0; c2 <= 127; c2++) {
        for (c5 = 0; c5 <= 127; c5++) {
          F[c1][c2] += C[c1][c5] * D[c5][c2];
        }
      }
    }
#pragma omp parallel for private(c2)
    for (c1 = 0; c1 <= 127; c1++) {
      for (c2 = 0; c2 <= 127; c2++) {
        E[c1][c2] = 0;
      }
    }
#pragma omp parallel for private(c5, c2)
    for (c1 = 0; c1 <= 127; c1++) {
      for (c2 = 0; c2 <= 127; c2++) {
        for (c5 = 0; c5 <= 127; c5++) {
          E[c1][c2] += A[c1][c5] * B[c5][c2];
        }
        for (c5 = 0; c5 <= 127; c5++) {
          G[c1][c5] += E[c1][c2] * F[c2][c5];
        }
      }
    }
  }
  
//#pragma endscop
}

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int ni = 128;
  int nj = 128;
  int nk = 128;
  int nl = 128;
  int nm = 128;
/* Variable declaration/allocation. */
  double (*E)[128 + 0][128 + 0];
  E = ((double (*)[128 + 0][128 + 0])(polybench_alloc_data(((128 + 0) * (128 + 0)),(sizeof(double )))));
  ;
  double (*A)[128 + 0][128 + 0];
  A = ((double (*)[128 + 0][128 + 0])(polybench_alloc_data(((128 + 0) * (128 + 0)),(sizeof(double )))));
  ;
  double (*B)[128 + 0][128 + 0];
  B = ((double (*)[128 + 0][128 + 0])(polybench_alloc_data(((128 + 0) * (128 + 0)),(sizeof(double )))));
  ;
  double (*F)[128 + 0][128 + 0];
  F = ((double (*)[128 + 0][128 + 0])(polybench_alloc_data(((128 + 0) * (128 + 0)),(sizeof(double )))));
  ;
  double (*C)[128 + 0][128 + 0];
  C = ((double (*)[128 + 0][128 + 0])(polybench_alloc_data(((128 + 0) * (128 + 0)),(sizeof(double )))));
  ;
  double (*D)[128 + 0][128 + 0];
  D = ((double (*)[128 + 0][128 + 0])(polybench_alloc_data(((128 + 0) * (128 + 0)),(sizeof(double )))));
  ;
  double (*G)[128 + 0][128 + 0];
  G = ((double (*)[128 + 0][128 + 0])(polybench_alloc_data(((128 + 0) * (128 + 0)),(sizeof(double )))));
  ;
/* Initialize array(s). */
  init_array(ni,nj,nk,nl,nm, *A, *B, *C, *D);
/* Start timer. */
  polybench_timer_start();
  ;
/* Run kernel. */
  kernel_3mm(ni,nj,nk,nl,nm, *E, *A, *B, *F, *C, *D, *G);
/* Stop and print timer. */
  polybench_timer_stop();
  ;
  polybench_timer_print();
  ;
/* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  if (argc > 42 && !strcmp(argv[0],"")) 
    print_array(ni,nl, *G);
/* Be clean. */
  free(((void *)E));
  ;
  free(((void *)A));
  ;
  free(((void *)B));
  ;
  free(((void *)F));
  ;
  free(((void *)C));
  ;
  free(((void *)D));
  ;
  free(((void *)G));
  ;
  return 0;
}
