#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#include "instrument.h"

/* Default problem size. */
#ifndef TMAX
# define TMAX 50
#endif
#ifndef NX
# define NX 1024
#endif
#ifndef NY
# define NY 1024
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
DATA_TYPE _fict_[TMAX];
DATA_TYPE ex[NX][NY];
DATA_TYPE ey[NX][NY];
DATA_TYPE hz[NX][NY];
#else
DATA_TYPE* _fict_ = (DATA_TYPE*)malloc(TMAX * sizeof(DATA_TYPE));
DATA_TYPE** ex = (DATA_TYPE**)malloc(NX * sizeof(DATA_TYPE*));
DATA_TYPE** ey = (DATA_TYPE**)malloc(NX * sizeof(DATA_TYPE*));
DATA_TYPE** hz = (DATA_TYPE**)malloc(NX * sizeof(DATA_TYPE*));
{
  int i;
  for (i = 0; i < NX; ++i)
    {
      ex[i] = (DATA_TYPE*)malloc(NY * sizeof(DATA_TYPE));
      ey[i] = (DATA_TYPE*)malloc(NY * sizeof(DATA_TYPE));
      hz[i] = (DATA_TYPE*)malloc(NY * sizeof(DATA_TYPE));
    }
}
#endif

inline
void init_array()
{
  int i, j;

  for (i = 0; i < TMAX; i++)
    _fict_[i] = (DATA_TYPE) i;
  for (i = 0; i < NX; i++)
    for (j = 0; j < NY; j++)
      {
	ex[i][j] = ((DATA_TYPE) i*j + 1) / NX;
	ey[i][j] = ((DATA_TYPE) i*j + 2) / NX;
	hz[i][j] = ((DATA_TYPE) i*j + 3) / NX;
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
      for (i = 0; i < NX; i++)
	for (j = 0; j < NY; j++) {
	  fprintf(stderr, DATA_PRINTF_MODIFIER, ex[i][j]);
	  fprintf(stderr, DATA_PRINTF_MODIFIER, ey[i][j]);
	  fprintf(stderr, DATA_PRINTF_MODIFIER, hz[i][j]);
	  if ((i * NX + j) % 80 == 20) fprintf(stderr, "\n");
	}
      fprintf(stderr, "\n");
    }
}


int main(int argc, char** argv)
{
  int t, i, j;
  int tmax = TMAX;
  int nx = NX;
  int ny = NY;

  /* Initialize array. */
  init_array();

  /* Start timer. */
  polybench_start_instruments;


#pragma scop
#pragma live-out ex, ey, hz

  for(t = 0; t < tmax; t++)
    {
      for (j = 0; j < ny; j++)
	ey[0][j] = _fict_[t];
      for (i = 1; i < nx; i++)
	for (j = 0; j < ny; j++)
	  ey[i][j] = ey[i][j] - 0.5*(hz[i][j]-hz[i-1][j]);
      for (i = 0; i < nx; i++)
	for (j = 1; j < ny; j++)
	  ex[i][j] = ex[i][j] - 0.5*(hz[i][j]-hz[i][j-1]);
      for (i = 0; i < nx - 1; i++)
	for (j = 0; j < ny - 1; j++)
	  hz[i][j] = hz[i][j] - 0.7*  (ex[i][j+1] - ex[i][j] +
				       ey[i+1][j]-ey[i][j]);
    }

#pragma endscop

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  print_array(argc, argv);

  return 0;
}
