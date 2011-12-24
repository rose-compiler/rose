#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#include "instrument.h"

/* Default problem size. */
#ifndef LENGTH
# define LENGTH 64
#endif
#ifndef MAXGRID
# define MAXGRID 6
#endif
#ifndef MAXRGC
# define MAXRGC ((MAXGRID - 1) * (MAXGRID - 1) * MAXGRID)
#endif
#ifndef NITER
# define NITER 100000
#endif

/* Default data type is int. */
#ifndef DATA_TYPE
# define DATA_TYPE int
#endif
#ifndef DATA_PRINTF_MODIFIER
# define DATA_PRINTF_MODIFIER "%d "
#endif

/* Array declaration. Enable malloc if POLYBENCH_TEST_MALLOC. */
DATA_TYPE s;
#ifndef POLYBENCH_TEST_MALLOC
DATA_TYPE sum_tang[MAXGRID][MAXGRID];
DATA_TYPE mean[MAXGRID][MAXGRID];
DATA_TYPE diff[MAXGRID][MAXGRID][LENGTH];
DATA_TYPE sum_diff[MAXGRID][MAXGRID][LENGTH];
DATA_TYPE tangent[MAXRGC]; //input
DATA_TYPE path[MAXGRID][MAXGRID]; //output
#else
DATA_TYPE** sum_tang = (DATA_TYPE**)malloc(MAXGRID * sizeof(DATA_TYPE*));
DATA_TYPE** mean = (DATA_TYPE**)malloc(MAXGRID * sizeof(DATA_TYPE*));
DATA_TYPE** path = (DATA_TYPE**)malloc(MAXGRID * sizeof(DATA_TYPE*));
DATA_TYPE*** diff = (DATA_TYPE***)malloc(MAXGRID * sizeof(DATA_TYPE**));
DATA_TYPE*** sum_diff = (DATA_TYPE***)malloc(MAXGRID * sizeof(DATA_TYPE**));
DATA_TYPE* tangent = (DATA_TYPE*)malloc(MAXRGC * sizeof(DATA_TYPE));
{
  int i, j;
  for (i = 0; i < MAXGRID; ++i)
    {
      sum_tang[i] = (DATA_TYPE*)malloc(MAXGRID * sizeof(DATA_TYPE));
      mean[i] = (DATA_TYPE*)malloc(MAXGRID * sizeof(DATA_TYPE));
      path[i] = (DATA_TYPE*)malloc(MAXGRID * sizeof(DATA_TYPE));
      diff[i] = (DATA_TYPE**)malloc(MAXGRID * sizeof(DATA_TYPE*));
      sum_diff[i] = (DATA_TYPE**)malloc(MAXGRID * sizeof(DATA_TYPE*));
      for (j = 0; j < MAXGRID; ++j)
	{
	  diff[i][j] = (DATA_TYPE**)malloc(LENGTH * sizeof(DATA_TYPE));
	  sum_diff[i][j] = (DATA_TYPE**)malloc(LENGTH * sizeof(DATA_TYPE));
	}
    }
}
#endif

inline
void init_array()
{
  int i;

  for (i = 0; i < MAXRGC; i++)
    tangent[i] = ((DATA_TYPE) i + 42);
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
      fprintf(stderr, DATA_PRINTF_MODIFIER, s);
      fprintf(stderr, "\n");
    }
}


int main(int argc, char** argv)
{
  int t, i, j, cnt;
  int length = LENGTH;
  int maxgrid = MAXGRID;
  int niter = NITER;

  /* Initialize array. */
  init_array();

  /* Start timer. */
  polybench_start_instruments;


  s = 0;
  for (j = 0; j <= maxgrid - 1; j++)
    {
      sum_tang[j][j] = tangent[(maxgrid+1)*j];
      for (i = j + 1; i <= maxgrid - 1; i++)
	sum_tang[j][i] = sum_tang[j][i-1] + tangent[i + maxgrid * j];
    }

#pragma scop
#pragma live-out s

  for (t = 0; t < niter; t++)
    {
      for (j = 0; j <= maxgrid - 1; j++)
	for (i = j; i <= maxgrid - 1; i++)
	  for (cnt = 0; cnt <= length - 1; cnt++)
	    diff[j][i][cnt] = sum_tang[j][i];

      for (j = 0; j <= maxgrid - 1; j++)
        {
	  for (i = j; i <= maxgrid - 1; i++)
            {
	      sum_diff[j][i][0] = diff[j][i][0];
	      for (cnt = 1; cnt <= length - 1; cnt++)
		sum_diff[j][i][cnt] = sum_diff[j][i][cnt - 1] + diff[j][i][cnt];
                mean[j][i] = sum_diff[j][i][length - 1];
            }
        }

      for (i = 0; i <= maxgrid - 1; i++)
	path[0][i] = mean[0][i];

      for (j = 1; j <= maxgrid - 1; j++)
	for (i = j; i <= maxgrid - 1; i++)
	  path[j][i] = path[j - 1][i - 1] + mean[j][i];
      s += path[maxgrid - 1][1];
    }

#pragma endscop

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  print_array(argc, argv);

  return 0;
}
