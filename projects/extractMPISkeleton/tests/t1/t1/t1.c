#include "mpi.h"

int main(int argc, char *argv[])
{
  int        rank, size;
  int n = 10;
  int i;

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );

  #pragma skel initializer repeat(1/3)
  double f[n];

  for (i = 0; i < n; i++)
    f[i] = i*i*2.0;

  MPI_Send(f, n, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD );

  MPI_Finalize( );
  return 0;
}
