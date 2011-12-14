#include <stdio.h>
#include <math.h>
#include <upc_relaxed.h>

#if WITH_UPC
/* with RTED */
#include "RuntimeSystem.h"

// instrument code
// runtimeCheck heattrans.upc -rose:UPC -rose:upc_threads 8 -DWITH_UPC -I../../../ROSE/config -c -I../../../ROSE/projects/RTED -I. -I../..

// compile instrumented code
// upc -O0 -g -dwarf-2-upc -DWITH_UPC=1 -Wall -Wextra -fupc-threads-8 -I../../../ROSE/projects/RTED -I. -I../.. -c rose_heattrans.upc

// link instrumented code
// upc++link -dwarf-2-upc -o rose_heattrans.bin rose_heattrans.o RuntimeSystemUpc.o ParallelRTS.o -L./CppRuntimeSystem/.libs/ -lUpcRuntimeSystem
#endif


#define N         32
#define Q         (N-1)
#define BLOCKSIZE 1

shared[BLOCKSIZE] double grids[2][N][N][N];
shared            double dTmax_local[THREADS];

void initialize(void)
{
  // boundary values on the edges
  for (int a = 1; a < Q; ++a)
  {
    upc_forall(int b = 1; b < Q; ++b; &grids[0][0][a][b])
    {
      grids[0][0][a][b] = grids[1][0][a][b] = 1.0;
      grids[0][a][b][0] = grids[1][a][b][0] = 1.0;
      grids[0][a][0][b] = grids[1][a][0][b] = 1.0;

      grids[0][Q][a][b] = grids[1][Q][a][b] = 1.0;
      grids[0][a][b][Q] = grids[1][a][b][Q] = 1.0;
      grids[0][a][Q][b] = grids[1][a][Q][b] = 1.0;
    }
  }

  // initial value
  for (int z = 1; z < Q; ++z)
  {
    for (int y = 1; y < Q; ++y)
    {
      upc_forall(int x = 1; x < Q; ++x; &grids[0][z][y][x])
      {
        grids[0][z][y][x] = 2.0;
      }
    }
  }
}

int main()
{
  double dTmax;
  double dT;
  double epsilon;
  int x;
  int y;
  int z;
  double T;
  int nr_iter;
  int sg; // source grid
  int dg; // destination grid

  initialize();

  epsilon = .0001;
  nr_iter = 0;
  sg = 1;
  dg = !sg;

  upc_barrier;

  do
  {
    dg = sg;
    sg = !sg;
    ++nr_iter;

    dTmax = 0.0;
    for (z = 1; z < Q; ++z)
    {
      for (y = 1; y < Q; ++y)
      {
        upc_forall(x = 1; x < Q; ++x; &grids[sg][z][y][x])
        {
          T = ( grids[sg][z+1][y][x]
              + grids[sg][z-1][y][x]
              + grids[sg][z][y+1][x]
              + grids[sg][z][y-1][x]
              + grids[sg][z][y][x+1]
              + grids[sg][z][y][x-1]
              ) / 6.0;

          dT = T - grids[sg][z][y][x];
          grids[dg][z][y][x] = T;

          if (dTmax < fabs(dT)) dTmax = fabs(dT);
        }
      }
    }

    dTmax_local[MYTHREAD] = dTmax;
    upc_barrier;

    dTmax = dTmax_local[0];

    for (int i = 1; i<THREADS; ++i)
    {
      if (dTmax < dTmax_local[i])
        dTmax = dTmax_local[i];
    }

    upc_barrier;
  } while (dTmax > epsilon);

  upc_barrier;

  if (MYTHREAD == 0)
  {
    printf("%d iterations\n", nr_iter);
  }

  return 0;
}
