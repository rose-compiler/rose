#include "mpi.h"

int main(int argc, char *argv[]) {
  int i = 0;
  int x = 0;

  MPI_Init( &argc, &argv );

  #pragma skel remove
  for (i=0; i<5; i++) {
    MPI_Send(&i, sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  #pragma skel remove
  for (i=0; i<5; i++) {
    MPI_Send(&i, sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  for (i=0; i<5; i++) {
    x += i;
  }
  /* comment */
  for (i=0; i<5; i++) {
    x += i;
  }

  return 0;
}
