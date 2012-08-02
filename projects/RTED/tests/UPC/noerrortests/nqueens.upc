#include <stdio.h>
#include <upc_relaxed.h>
#include <upc.h>

#if WITH_UPC
/* with RTED */
#include "RuntimeSystem.h"

// instrument code
// .libs/lt-runtimeCheck nqueens.upc -rose:UPC -DWITH_UPC -I../../../ROSE/config -c -I../../../ROSE/projects/RTED -I. -I../..

// compile instrumented code
// upc -O0 -g -dwarf-2-upc -DWITH_UPC=1 -Wall -Wextra -g -I../../../ROSE/projects/RTED -I. -I../.. -c rose_nqueens.upc

// link instrumented code
// upc++link -dwarf-2-upc -o rose_nqueens.bin  rose_nqueens.o  RuntimeSystemUpc.o ParallelRTS.o -L./CppRuntimeSystem/.libs/ -lUpcRuntimeSystem
#endif


static const int ROUND_ROBIN = 0;

typedef long /*todo*/ mask_t;

int N;
int level;
int no_solutions = 0;
int method = 0;

mask_t basemask;
mask_t leftdiagmask;
mask_t centermask;
mask_t rightdiagmask;

shared int sh_solutions[THREADS];

void NQueens(int cur_row, mask_t unsafe_cells)
{
  int col;
  mask_t vrlnbl_cells;
  mask_t next_row_unsafe;

  if (cur_row == N)
  {
    ++no_solutions;
  }
  else
  {
  for (col = 0; col < N; ++col)
  {
    vrlnbl_cells = basemask << col;

      if (!(unsafe_cells & vrlnbl_cells))
      {
    next_row_unsafe = unsafe_cells | vrlnbl_cells;
    next_row_unsafe = ( ((next_row_unsafe & leftdiagmask) >> 1)
                      | (next_row_unsafe & centermask)
                      | ((next_row_unsafe & rightdiagmask) << 1)
                      );

    NQueens(cur_row+1, next_row_unsafe);
      }
    }
  }
}

void do_job(int job, int no_jobs)
{
  int j;
  int row;
  int col;
  int conflict;

  mask_t vrlnbl_cells;
  mask_t unsafe_cells;

  j = no_jobs/N;
  unsafe_cells = 0;
  row = 0;
  conflict = 0;

  while (j>=1 && !conflict)
  {
    col = job % (N*j) / j;
    vrlnbl_cells = basemask << col;

    conflict = (unsafe_cells & vrlnbl_cells);

    if (!conflict)
    {
    unsafe_cells |= vrlnbl_cells;
    unsafe_cells = ( ((unsafe_cells & leftdiagmask) >> 1)
                   | (unsafe_cells & centermask)
                   | ((unsafe_cells & rightdiagmask) << 1)
                   );

    if (j == 1)
    {
      NQueens(row+1, unsafe_cells);
    }

      ++row; j/=N;
    }
  }
}

void distribute_work()
{
  int i;
  int job;
  int no_jobs;

  for (i = 0, no_jobs = N; i < level; ++i)
  {
    no_jobs *= N;
  }

  if (method == ROUND_ROBIN)
  {
    upc_forall(job = 0; job < no_jobs; ++job; job)
    {
      do_job(job, no_jobs);
    }
  }
  else
  {
    upc_forall(job = 0; job < no_jobs; ++job; (job*THREADS)/no_jobs)
    {
      do_job(job, no_jobs);
    }
  }
}

int main(int argc, char**argv)
{
  int i;

  if ((argc != 3) && (argc != 4))
  {
    if (MYTHREAD == 0) printf("Usage: %s [N] [1v1] [CHUNK_FLAG]\n", argv[0]);

    return -1;
  }

  N = atoi(argv[1]);
  level = atoi(argv[2]);

  if (argc == 3) method = 1; // 3 args, CHUNK method enabled

  if (level >= N)
  {
    if (MYTHREAD == 0) printf("lvl should be < N\n");

    return -1;
  }

  if (N*3 > 64)
  {
    if (MYTHREAD == 0) printf("64 bit word not enough\n");

    return -1;
  }

  basemask = 1 | (1<<N) | (1<<(2*N));
  leftdiagmask = (1<<N) - 1;
  centermask = leftdiagmask << N;
  rightdiagmask = centermask << N;

  distribute_work();

  printf("# %d: Total number of solutions for N=%d: %d\n", MYTHREAD, N, no_solutions);
  sh_solutions[MYTHREAD] = no_solutions;

  upc_barrier;
  if (MYTHREAD == 0)
  {
    for (i = 1; i<THREADS; ++i)
    {
      no_solutions += sh_solutions[i];
    }

    printf("Total number of solutions: %d\n", no_solutions);
  }

  return 0;
}
