#include <mpi.h>
#include <stdlib.h>

int SOURCE_DETERMINISM = 0;
int TAG_DETERMINISM = 1;
int FUNCTION_DETERMINISM = 1;

int main(int argc, char **argv) {
  int a;
  MPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, NULL);

  return 0;
}
