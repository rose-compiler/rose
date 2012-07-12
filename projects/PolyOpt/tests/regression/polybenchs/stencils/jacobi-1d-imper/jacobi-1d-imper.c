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
#ifndef N
# define N 4096
#endif

/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif

/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
#ifndef POLYBENCH_TEST_MALLOC
DATA_TYPE A[N];
DATA_TYPE B[N];
#else
DATA_TYPE* A = (DATA_TYPE**)malloc(N * sizeof(DATA_TYPE));
DATA_TYPE* B = (DATA_TYPE**)malloc(N * sizeof(DATA_TYPE));
#endif

inline
void init_array()
{
  int i, j;

  for (i = 0; i < N; i++)
    {
      A[i] = ((DATA_TYPE) i + 10) / N;
      B[i] = ((DATA_TYPE) i + 11) / N;
    }
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
      for (i = 0; i < N; i++) {
	fprintf(stderr, DATA_PRINTF_MODIFIER, A[i]);
	if (i % 80 == 20) fprintf(stderr, "\n");
      }
      fprintf(stderr, "\n");
    }
}


int main(int argc, char** argv)
{
  int t, i, j;
  int tsteps = TSTEPS;
  int n = N;

  /* Initialize array. */
  init_array();

  /* Start timer. */
  polybench_start_instruments;

#pragma scop
#pragma live-out A

  for (t = 0; t < tsteps; t++)
    {
      for (i = 2; i < n - 1; i++)
	B[i] = 0.33333 * (A[i-1] + A[i] + A[i + 1]);

      for (j = 2; j < n - 1; j++)
	A[j] = B[j];
    }

#pragma endscop

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  print_array(argc, argv);

  return 0;
}
