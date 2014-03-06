/**
 * dynprog.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
/* Default data type is int, default size is 50. */
#include "dynprog.h"


/* Array initialization. */
static
void init_array(int length,
		DATA_TYPE POLYBENCH_2D(c,LENGTH,LENGTH,length,length),
		DATA_TYPE POLYBENCH_2D(W,LENGTH,LENGTH,length,length))
{
  int i, j;
  for (i = 0; i < length; i++)
    for (j = 0; j < length; j++) {
      c[i][j] = i*j % 2;
      W[i][j] = ((DATA_TYPE) i-j) / length;
    }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(DATA_TYPE out)
{
  fprintf (stderr, DATA_PRINTF_MODIFIER, out);
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_dynprog(int tsteps, int length,
		    DATA_TYPE POLYBENCH_2D(c,LENGTH,LENGTH,length,length),
		    DATA_TYPE POLYBENCH_2D(W,LENGTH,LENGTH,length,length),
		    DATA_TYPE POLYBENCH_3D(sum_c,LENGTH,LENGTH,LENGTH,length,length,length),
		    DATA_TYPE *out)
{
  int iter, i, j, k;

  DATA_TYPE out_l = 0;

#pragma scop
  for (iter = 0; iter < _PB_TSTEPS; iter++)
    {
      for (i = 0; i <= _PB_LENGTH - 1; i++)
	for (j = 0; j <= _PB_LENGTH - 1; j++)
	  c[i][j] = 0;

      for (i = 0; i <= _PB_LENGTH - 2; i++)
	{
	  for (j = i + 1; j <= _PB_LENGTH - 1; j++)
	    {
	      sum_c[i][j][i] = 0;
	      for (k = i + 1; k <= j-1; k++)
		sum_c[i][j][k] = sum_c[i][j][k - 1] + c[i][k] + c[k][j];
	      c[i][j] = sum_c[i][j][j-1] + W[i][j];
	    }
	}
      out_l += c[0][_PB_LENGTH - 1];
    }
#pragma endscop

  *out = out_l;
}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int length = LENGTH;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */
  DATA_TYPE out;
  POLYBENCH_3D_ARRAY_DECL(sum_c,DATA_TYPE,LENGTH,LENGTH,LENGTH,length,length,length);
  POLYBENCH_2D_ARRAY_DECL(c,DATA_TYPE,LENGTH,LENGTH,length,length);
  POLYBENCH_2D_ARRAY_DECL(W,DATA_TYPE,LENGTH,LENGTH,length,length);

  /* Initialize array(s). */
  init_array (length, POLYBENCH_ARRAY(c), POLYBENCH_ARRAY(W));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_dynprog (tsteps, length,
		  POLYBENCH_ARRAY(c),
		  POLYBENCH_ARRAY(W),
		  POLYBENCH_ARRAY(sum_c),
		  &out);

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(out));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(sum_c);
  POLYBENCH_FREE_ARRAY(c);
  POLYBENCH_FREE_ARRAY(W);

  return 0;
}
