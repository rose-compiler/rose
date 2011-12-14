#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#include "instrument.h"


/* Default problem size. */
#ifndef N
# define N 4000
#endif

/* Default data type is double. */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif

/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
#ifndef POLYBENCH_TEST_MALLOC
DATA_TYPE A[N][N];
DATA_TYPE x1[N];
DATA_TYPE y_1[N];
DATA_TYPE x2[N];
DATA_TYPE y_2[N];
#else
DATA_TYPE** A = (DATA_TYPE**)malloc(N * sizeof(DATA_TYPE*));
DATA_TYPE* x1 = (DATA_TYPE*)malloc(N * sizeof(DATA_TYPE));
DATA_TYPE* y_1 = (DATA_TYPE*)malloc(N * sizeof(DATA_TYPE));
DATA_TYPE* x2 = (DATA_TYPE*)malloc(N * sizeof(DATA_TYPE));
DATA_TYPE* y_2 = (DATA_TYPE*)malloc(N * sizeof(DATA_TYPE));
{
  int i;
  for (i = 0; i < N; ++i)
    A[i] = (DATA_TYPE*)malloc(N * sizeof(DATA_TYPE));
}
#endif

inline
void init_array()
{
  int i, j;

  for (i = 0; i < N; i++)
    {
      x1[i] = ((DATA_TYPE) i) / N;
      x2[i] = ((DATA_TYPE) i + 1) / N;
      y_1[i] = ((DATA_TYPE) i + 3) / N;
      y_2[i] = ((DATA_TYPE) i + 4) / N;
      for (j = 0; j < N; j++)
	A[i][j] = ((DATA_TYPE) i*j) / N;
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
	fprintf(stderr, "%0.2lf ", x1[i]);
	fprintf(stderr, "%0.2lf ", x2[i]);
	if ((2 * i) % 80 == 20) fprintf(stderr, "\n");
      }
      fprintf(stderr, "\n");
    }
}


int main(int argc, char** argv)
{
  int i, j;
  int n = N;

  /* Initialize array. */
  init_array();

  /* Start timer. */
  polybench_start_instruments;

#pragma scop
#pragma live-out x1, x2

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      x1[i] = x1[i] + A[i][j] * y_1[j];
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      x2[i] = x2[i] + A[j][i] * y_2[j];

#pragma endscop

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  print_array(argc, argv);

  return 0;
}
