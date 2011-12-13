#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#include "instrument.h"


/* Default problem size. */
#ifndef N
# define N 1024
#endif
#ifndef M
# define M 1024
#endif

/* Default data type is double (dsyr2k). */
#ifndef DATA_TYPE
# define DATA_TYPE double
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%0.2lf "
#endif

/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
DATA_TYPE alpha;
DATA_TYPE beta;
#ifndef POLYBENCH_TEST_MALLOC
DATA_TYPE A[N][M];
DATA_TYPE B[N][M];
DATA_TYPE C[N][N];
#else
DATA_TYPE** A = (DATA_TYPE**)malloc(N * sizeof(DATA_TYPE*));
DATA_TYPE** B = (DATA_TYPE**)malloc(N * sizeof(DATA_TYPE*));
DATA_TYPE** C = (DATA_TYPE**)malloc(N * sizeof(DATA_TYPE*));
{
  int i;
  for (i = 0; i < N; ++i)
    {
      A[i] = (DATA_TYPE*)malloc(M * sizeof(DATA_TYPE));
      B[i] = (DATA_TYPE*)malloc(M * sizeof(DATA_TYPE));
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
    {
      for (j = 0; j < N; j++)
	C[i][j] = ((DATA_TYPE) i*j + 2) / N;
      for (j = 0; j < M; j++)
	{
	  A[i][j] = ((DATA_TYPE) i*j) / N;
	  B[i][j] = ((DATA_TYPE) i*j + 1) / N;
	}
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
      for (i = 0; i < N; i++)
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

  /*    C := alpha*A*B' + alpha*B*A' + beta*C */
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      C[i][j] *= beta;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      for (k = 0; k < m; k++)
	{
	  C[i][j] += alpha * A[i][k] * B[j][k];
	  C[i][j] += alpha * B[i][k] * A[j][k];
	}

#pragma endscop

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  print_array(argc, argv);

  return 0;
}
