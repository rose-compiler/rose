#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#include "instrument.h"


/* Default problem size. */
#ifndef N
# define N 512
#endif
#ifndef M
# define M 512
#endif

/* Default data type is double (dsymm). */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif

/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
DATA_TYPE alpha;
DATA_TYPE beta;
DATA_TYPE acc;
#ifndef POLYBENCH_TEST_MALLOC
DATA_TYPE A[N][N];
DATA_TYPE B[M][N];
DATA_TYPE C[M][N];
#else
DATA_TYPE** A = (DATA_TYPE**)malloc(N * sizeof(DATA_TYPE*));
DATA_TYPE** B = (DATA_TYPE**)malloc(M * sizeof(DATA_TYPE*));
DATA_TYPE** C = (DATA_TYPE**)malloc(M * sizeof(DATA_TYPE*));
{
  int i;
  for (i = 0; i < N; ++i)
    A[i] = (DATA_TYPE*)malloc(N * sizeof(DATA_TYPE));
  for (i = 0; i < M; ++i)
    {
      B[i] = (DATA_TYPE*)malloc(N * sizeof(DATA_TYPE));
      C[i] = (DATA_TYPE*)malloc(N * sizeof(DATA_TYPE));
    }
}
#endif

inline
void init_array()
{
  int i, j;

  alpha = 12435;
  beta = 4546;
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      A[i][j] = ((DATA_TYPE) i*j) / N;
  for (i = 0; i < M; i++)
    for (j = 0; j < N; j++)
      {
	B[i][j] = ((DATA_TYPE) i*j + 1) / N;
	C[i][j] = ((DATA_TYPE) i*j + 2) / N;
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
      for (i = 0; i < M; i++)
	for (j = 0; j < N; j++) {
	  fprintf(stderr, DATA_PRINTF_MODIFIER, C[i][j]);
	if ((i * N + j) % 80 == 20) fprintf(stderr, "\n");
      }
      fprintf(stderr, "\n");
    }
}


int main(int argc, char** argv)
{
  int i, j, k;
  int n = N;
  int m = M;

  /* Initialize array. */
  init_array();

  /* Start timer. */
  polybench_start_instruments;

#pragma scop
#pragma live-out C

  /*  C := alpha*A*B + beta*C, A is symetric */
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
      {
	acc = 0;
	for (k = 0; k < j - 1; k++)
	  {
	    C[k][j] += alpha * A[k][i] * B[i][j];
	    acc += B[k][j] * A[k][i];
	  }
	C[i][j] = beta * C[i][j] + alpha * A[i][i] * B[i][j] + alpha * acc;
      }
#pragma endscop

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  print_array(argc, argv);

  return 0;
}
