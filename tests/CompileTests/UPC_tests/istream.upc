/***
 *
 *  UPC Implementation of the HPC Challenge Stream-Triad Benchmark
 *
 *  Benchmark Specification:
 *
 *  The EP-Stream-Triad benchmark implements a simple vector operation
 *  that scales and adds two vectors:
 *  a = b + alpha * c
 *  where
 *  -- a, b, c and m-element double vectors, with the value of m as runtime input
 *  -- b and c are assigned random values
 *  -- The operation should be performed at least 10 
 *
 * Modified to do Integer work and make it closer to the ORNGINAL STREAM benchmark.
 * This version is a little different than the original by John McCalpin, but the
 * results should be the same.
 * Steve Poole (ORNL - 2007)
 *
 */

#include "upc.h"
#include "stdio.h"
#include "stdlib.h"
#include <math.h>
#include <float.h>

#include <sys/time.h>

# ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
# endif

# ifndef MAX
# define MAX(x,y) ((x)>(y)?(x):(y))
# endif

// Added macro to define IBY4 (what is this anyway)
#define IBY4

#if defined (IBY4)
# define MY_MAX_N     40000000
# define NTIMES       10
# define OFFSET       0
# define NUM_TESTS    5
# define MY_DATA_SIZE long
# define MY_DATA_TYPE "long"
# define MY_OP        <<
#elif defined (IBY8)
# define MY_MAX_N     40000000
# define NTIMES       10
# define OFFSET       0
# define NUM_TESTS    5
# define MY_DATA_SIZE long long
# define MY_DATA_TYPE "long long"
# define MY_OP        <<
#endif

# define HLINE "----------------------------------------------------------------\n"

shared MY_DATA_SIZE *a, *b, *c, *d, *e;

static double min_time[5] = {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX},
      	      avg_time[5] = {0.0},
       	      max_time[5] = {0.0};

static double times[5][NTIMES];

static char     *label[5] = {"Copy:      ", "Scale:     ",
    "Add:       ", "Triad:     ", "SScale     "};

static MY_DATA_SIZE  bytes[5] = {
                                  2 * sizeof(MY_DATA_SIZE) * MY_MAX_N,
                                  2 * sizeof(MY_DATA_SIZE) * MY_MAX_N,
                                  2 * sizeof(MY_DATA_SIZE) * MY_MAX_N,
                                  3 * sizeof(MY_DATA_SIZE) * MY_MAX_N,
                                  2 * sizeof(MY_DATA_SIZE) * MY_MAX_N
                                };

double wctime() 
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((double) tv.tv_sec + (double) tv.tv_usec * 1E-6 );
}

/*
** Copy
*/

void copy_kernel(int m_per_thread)
{

  double start, end;
  MY_DATA_SIZE *la = (MY_DATA_SIZE *) &a[MYTHREAD];
  MY_DATA_SIZE *lc = (MY_DATA_SIZE *) &c[MYTHREAD];

  for (int i = 0; i < NTIMES; i++)
  {
    upc_barrier;
    start = wctime();
    for (int j = 0; j < m_per_thread; j++)
    {
      la[j] = lc[j];
    }
    upc_barrier;
    end = wctime();
    if (!MYTHREAD)
    {
	times[0][i] = end - start;
    }
  }
}


/*
** Scale
*/

void scale_kernel(long alpha, int m_per_thread)
{

  double start, end;
  MY_DATA_SIZE *lb = (MY_DATA_SIZE *) &b[MYTHREAD];
  MY_DATA_SIZE *lc = (MY_DATA_SIZE *) &c[MYTHREAD];

  for (int i = 0; i < NTIMES; i++)
  {
    upc_barrier;
    start = wctime();
    for (int j = 0; j < m_per_thread; j++)
    {
      lb[j] = alpha * lc[j];
    }
    upc_barrier;
    end = wctime();
    if (!MYTHREAD)
    {
	times[1][i] = end - start;
    }
  }
}

/*
** Add
*/

void add_kernel(int m_per_thread)
{

  double start, end;
  MY_DATA_SIZE *la = (MY_DATA_SIZE *) &a[MYTHREAD];
  MY_DATA_SIZE *lb = (MY_DATA_SIZE *) &b[MYTHREAD];
  MY_DATA_SIZE *lc = (MY_DATA_SIZE *) &c[MYTHREAD];

  for (int i = 0; i < NTIMES; i++)
  {
    upc_barrier;
    start = wctime();
    for (int j = 0; j < m_per_thread; j++)
    {
      la[j] = lb[j] + lc[j];
    }
    upc_barrier;
    end = wctime();
    if (!MYTHREAD)
    {
	times[2][i] = end - start;
    }
  }
}

/*
** Triad
*/

void triad_kernel(long alpha, int m_per_thread)
{

  double start, end;
  MY_DATA_SIZE *la = (MY_DATA_SIZE *) &a[MYTHREAD];
  MY_DATA_SIZE *lb = (MY_DATA_SIZE *) &b[MYTHREAD];
  MY_DATA_SIZE *lc = (MY_DATA_SIZE *) &c[MYTHREAD];

  for (int i = 0; i < NTIMES; i++)
  {
    upc_barrier;
    start = wctime();
    for (int j = 0; j < m_per_thread; j++)
    {
      la[j] = lb[j] + alpha * lc[j];
    }
    upc_barrier;
    end = wctime();
    if (!MYTHREAD)
    {
	times[3][i] = end - start;
    }
  }
}

/*
** SScale
*/

void sscale_kernel(long alpha, int m_per_thread) 
{

  double start, end;
  MY_DATA_SIZE *le = (MY_DATA_SIZE *) &e[MYTHREAD];
  MY_DATA_SIZE *ld = (MY_DATA_SIZE *) &d[MYTHREAD];

  for (int i = 0; i < NTIMES; i++) 
  {
    upc_barrier;
    start = wctime();
    for (int j = 0; j < m_per_thread; j++) 
    {
      le[j] = ld[j] MY_OP alpha;
    }
    upc_barrier;
    end = wctime();
    if (!MYTHREAD) 
    {
	times[4][i] = end - start;
    }
  }
}

/*
** ---------------------------- Main -----------------------
*/

int main(int argc, char* argv[]) 
{
  double perf;
  int m, m_per_thread;
  int j,k;
  int BytesPerWord;
  MY_DATA_SIZE *la, *lb, *lc, *ld, *le;
  long alpha, beta;

  if (argc < 3) 
  {
    if (!MYTHREAD) 
    {
      fprintf(stderr, "usage:  ./istream-upc <num-elements> <alpha>\n");
      fprintf(stderr, "ERROR, will exit now\n");
    }
    upc_global_exit(-1);
  }
   
  m            = (unsigned) atoi(argv[1]);
  alpha        = strtol(argv[2], NULL, 10);

  if(m > MY_MAX_N)
	m = MY_MAX_N;

  if(!MYTHREAD)
  {
	bytes[0] = 2 * sizeof(MY_DATA_SIZE) * m;
	bytes[1] = 2 * sizeof(MY_DATA_SIZE) * m;
	bytes[2] = 2 * sizeof(MY_DATA_SIZE) * m;
	bytes[3] = 3 * sizeof(MY_DATA_SIZE) * m;
	bytes[4] = 2 * sizeof(MY_DATA_SIZE) * m;
  }

  m_per_thread = m / THREADS + (m % THREADS ? 1 : 0);

/*
**  Give some info
*/

  if(!MYTHREAD)
  {
    BytesPerWord = sizeof(MY_DATA_SIZE);
    printf("\nThe data type is %s\n",MY_DATA_TYPE);
    printf("This system uses %d bytes per Element being tested.\n",
            BytesPerWord);

    printf("Array size = %d, Offset = %d\n" , m, OFFSET);
    printf("Total memory required = %.1f MB.\n",
          (3.0 * BytesPerWord) * ( (float) m / 1048576.0));
    printf("Each test is run %d times, but only\n", NTIMES);
    printf("the *minimum* time for each is used.\n");
    printf(HLINE);
  }

/*
**  This could be done with just three arrays
**  Just in case we ever decide to put in the checker.
*/

  a = (shared MY_DATA_SIZE *) upc_all_alloc(THREADS, m_per_thread * sizeof(MY_DATA_SIZE));
  b = (shared MY_DATA_SIZE *) upc_all_alloc(THREADS, m_per_thread * sizeof(MY_DATA_SIZE));
  c = (shared MY_DATA_SIZE *) upc_all_alloc(THREADS, m_per_thread * sizeof(MY_DATA_SIZE));
  d = (shared MY_DATA_SIZE *) upc_all_alloc(THREADS, m_per_thread * sizeof(MY_DATA_SIZE));
  e = (shared MY_DATA_SIZE *) upc_all_alloc(THREADS, m_per_thread * sizeof(MY_DATA_SIZE));

  if (a == NULL || b == NULL || c == NULL || d == NULL || e == NULL) 
  {
    if (!MYTHREAD) 
    {
      fprintf(stderr, "could not allocate five arrays of %d longs\n", m);
    }
    upc_global_exit(-1);
  }

/*
** initialize arrays
*/

  lb = (MY_DATA_SIZE *) &b[MYTHREAD]; 
  lc = (MY_DATA_SIZE *) &c[MYTHREAD];

  for (int i = 0; i < m_per_thread; i++) 
  {
    lb[i] = random();
    lc[i] = random();
  }

  upc_barrier;

  copy_kernel(m_per_thread);
  scale_kernel(alpha, m_per_thread);
  add_kernel(m_per_thread);

/*
** initialize arrays (TRIAD)
*/

  lb = (MY_DATA_SIZE *) &b[MYTHREAD]; 
  lc = (MY_DATA_SIZE *) &c[MYTHREAD];

  for (int i = 0; i < m_per_thread; i++) 
  {
    lb[i] = random();
    lc[i] = random();
  }

  upc_barrier;

  triad_kernel(alpha, m_per_thread);

/*
** initialize arrays (SSCALE)
*/

  lb = (MY_DATA_SIZE *) &b[MYTHREAD]; 
  lc = (MY_DATA_SIZE *) &c[MYTHREAD];

  for (int i = 0; i < m_per_thread; i++) 
  {
    lb[i] = random();
    lc[i] = random();
  }

  upc_barrier;

  beta = 4;
  sscale_kernel(beta, m_per_thread);

/*
** If we are on the head node, compute the perfomance
** This uses the "ORIGINAL" STREAM method. The average is 
** what is compted, not the min, nor the max.
** In the one on the web, it assumes the minimum.
** Copy, Add, Scale, SScale == 16
** Triad == 24
*/

  if (!MYTHREAD) 
  {

/*
** Copy Performance

    perf = (16 * ((double) m) / min_time[0]) / 1E9;
    printf("Stream-Copy Performance: %d threads, %d element vector, best time = %.3f seconds ( %.3f Gbytes/second)\n", 
	   THREADS, m, min_time[0], perf);
*/

    for (k=1; k<NTIMES; k++) /* note -- skip first iteration */
    {
        for (j=0; j<NUM_TESTS; j++)
        {
            avg_time[j] = avg_time[j] + times[j][k];
            min_time[j] = MIN(min_time[j], times[j][k]);
            max_time[j] = MAX(max_time[j], times[j][k]);
        }
    }

    printf("Function      Rate (MB/s)      Avg time     Min time     Max time\n");
    for (j=0; j<NUM_TESTS; j++)
    {
        avg_time[j] = avg_time[j] / (double)(NTIMES-1);

        printf("%s %12.5f  %12.5f  %12.5f  %12.5f\n",
                label[j],
                   1.0E-06 * (double) bytes[j] / min_time[j],
                           avg_time[j],
                                   min_time[j],
                                           max_time[j]);
    }
    printf(HLINE);
    printf("Run COMPLETE.\n");
  }

}  
