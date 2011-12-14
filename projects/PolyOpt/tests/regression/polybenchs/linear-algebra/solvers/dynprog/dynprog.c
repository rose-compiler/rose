#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#include "instrument.h"

/* Default problem size. */
#ifndef TSTEPS
# define TSTEPS 10000
#endif
#ifndef LENGTH
# define LENGTH 50
#endif

/* Default data type is int. */
#ifndef DATA_TYPE
# define DATA_TYPE int
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%d "
#endif

/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
DATA_TYPE out;
#ifndef POLYBENCH_TEST_MALLOC
DATA_TYPE sum_c[LENGTH][LENGTH][LENGTH];
DATA_TYPE c[LENGTH][LENGTH];
DATA_TYPE W[LENGTH][LENGTH]; //input
#else
DATA_TYPE** c = (DATA_TYPE**)malloc(LENGTH * sizeof(DATA_TYPE*));
DATA_TYPE** w = (DATA_TYPE**)malloc(LENGTH * sizeof(DATA_TYPE*));
DATA_TYPE*** sum_c = (DATA_TYPE***)malloc(LENGTH * sizeof(DATA_TYPE**));
{
  int i, j;
  for (i = 0; i < LENGTH; ++i)
    {
      c[i] = (DATA_TYPE*)malloc(LENGTH * sizeof(DATA_TYPE));
      W[i] = (DATA_TYPE*)malloc(LENGTH * sizeof(DATA_TYPE));
      sum_c[i] = (DATA_TYPE**)malloc(LENGTH * sizeof(DATA_TYPE*));
      for (j = 0; j < LENGTH; ++j)
	sum_c[i][j] = (DATA_TYPE*)malloc(LENGTH * sizeof(DATA_TYPE));
    }
}
#endif

inline
void init_array()
{
  int i, j;

  for (i = 0; i < LENGTH; i++)
    for (j = 0; j < LENGTH; j++)
      W[i][j] = ((DATA_TYPE) i*j + 1) / LENGTH;
}

/* Define the live-out variables. Code is not executed unless
   POLYBENCH_DUMP_ARRAYS is defined. */
inline
void print_array(int argc, char** argv)
{
  int i, j;
#ifndef POLYBENCH_DUMP_ARRAYS
  if (argc > 42 && ! strcmp(argv[0], ""))
#endif
    {
      fprintf(stderr, DATA_PRINTF_MODIFIER, out);
      fprintf(stderr, "\n");
    }
}


int main(int argc, char** argv)
{
  int iter, i, j, k;
  int length = LENGTH;
  int tsteps = TSTEPS;

  /* Initialize array. */
  init_array();

  /* Start timer. */
  polybench_start_instruments;


#pragma scop
#pragma live-out out

  out = 0;
  for (iter = 0; iter < tsteps; iter++)
    {
      for (i = 0; i <= length - 1; i++)
	for (j = 0; j <= length - 1; j++)
	  c[i][j] = 0;

      for (i = 0; i <= length - 2; i++)
	{
	  for (j = i + 1; j <= length - 1; j++)
	    {
	      sum_c[i][j][i] = 0;
	      for (k = i + 1; k <= j-1; k++)
		sum_c[i][j][k] = sum_c[i][j][k - 1] + c[i][k] + c[k][j];
	      c[i][j] = sum_c[i][j][j-1] + W[i][j];
	    }
	}
      out += c[0][length - 1];
    }

#pragma endscop

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  print_array(argc, argv);

  return 0;
}
