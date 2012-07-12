#include <mpi.h>
#include <stdlib.h>

int SOURCE_DETERMINISM = 1;
int TAG_DETERMINISM = 1;
int FUNCTION_DETERMINISM = 1;

int main(int argc, char **argv) {
  int a;
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Recv(&a, 1, MPI_INT, rank ^ 1, 0, MPI_COMM_WORLD, NULL);

  return 0;
}
