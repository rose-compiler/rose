/**
 * correlation.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#   define N ARRAYSIZE
#   define M ARRAYSIZE
# define _PB_N ARRAYSIZE
# define _PB_M ARRAYSIZE

int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int m = M;

  double data[M][N];
  double symmat[M][N];
  double mean[M];
  double stddev[M];
  int i, j, j1, j2;

  double float_n = 1.2;
  double eps = 0.1f;

#define sqrt_of_array_cell(x,j) sqrt(x[j])

#pragma scop
  /* Determine mean of column vectors of input data matrix */
  for (j = 0; j < _PB_M; j++)
    {
      mean[j] = 0.0;
      for (i = 0; i < _PB_N; i++)
	mean[j] += data[i][j];
      mean[j] /= float_n;
    }

  /* Determine standard deviations of column vectors of data matrix. */
  for (j = 0; j < _PB_M; j++)
    {
      stddev[j] = 0.0;
      for (i = 0; i < _PB_N; i++)
	stddev[j] += (data[i][j] - mean[j]) * (data[i][j] - mean[j]);
      stddev[j] /= float_n;
      stddev[j] = sqrt_of_array_cell(stddev, j);
      /* The following in an inelegant but usual way to handle
	 near-zero std. dev. values, which below would cause a zero-
	 divide. */
      stddev[j] = stddev[j] <= eps ? 1.0 : stddev[j];
    }

  /* Center and reduce the column vectors. */
  for (i = 0; i < _PB_N; i++)
    for (j = 0; j < _PB_M; j++)
      {
	data[i][j] -= mean[j];
	data[i][j] /= sqrt(float_n) * stddev[j];
      }

  /* Calculate the m * m correlation matrix. */
  for (j1 = 0; j1 < _PB_M-1; j1++)
    {
      symmat[j1][j1] = 1.0;
      for (j2 = j1+1; j2 < _PB_M; j2++)
	{
	  symmat[j1][j2] = 0.0;
	  for (i = 0; i < _PB_N; i++)
	    symmat[j1][j2] += (data[i][j1] * data[i][j2]);
	  symmat[j2][j1] = symmat[j1][j2];
	}
    }
  symmat[_PB_M-1][_PB_M-1] = 1.0;
#pragma endscop


  return 0;
}
