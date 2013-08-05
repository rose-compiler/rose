#include "mpi.h"

int main(int argc, char *argv[])
{
  int x=0;
  int i;

  MPI_Init( &argc, &argv );

  // #pragma skel loop iterate exactly (10)
  #pragma skel preserve
  for (i=0; x < 100 ; i++)
    {
    if (i % 3)
      break;
    x++;
    // MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

  // #pragma skel loop iterate exactly (10)
  for (i=0; x < 101 ; i++) {
    if (i % 3)
      break;
    x++;
    MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  // #pragma skel loop iterate atleast (10)
  for (i=0; x < 102 ; i++) {
    x = i + 1;

    int j;
    for (j=0; x < 103 ; j++) {
      x = j + 1;
      if (j==5)
        break;
    }

    MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    if (x % 3)
      break;
  }

  // #pragma skel loop iterate atmost (10)
  for (i=0; x < 104 ; i++) {
    if (i % 3)
      break;
    x++;
    MPI_Send(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  return 0;
}
