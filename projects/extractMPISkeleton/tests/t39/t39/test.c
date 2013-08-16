#include "mpi.h"

int main(int argc, char *argv[])
{
  int x=0;
  int i;

  MPI_Init( &argc, &argv );

  #pragma skel remove
  for (i=0; x < 100 ; i++)
  {
    x++;
    if (i % 3)
      break;
    MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

  return 0;
}
