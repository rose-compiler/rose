#include <mpi.h>
#include <stdlib.h>

int SOURCE_DETERMINISM = 1;
int TAG_DETERMINISM = 1;
int FUNCTION_DETERMINISM = 1;

int main(int argc, char **argv) {
  int a;
  int zero = 0;
  MPI_Recv(&a, 1, MPI_INT, zero, zero, MPI_COMM_WORLD, NULL);

  return 0;
}
