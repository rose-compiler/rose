#include <mpi.h>
#include <stdlib.h>

int SOURCE_DETERMINISM = 1;
int TAG_DETERMINISM = 1;
int FUNCTION_DETERMINISM = 1;

int foo(void *, int, MPI_Datatype, int, int, MPI_Comm, MPI_Status *);

int main(int argc, char **argv) {
  int a;
  // Test calling functions that are not MPI routines, but have similar arguments in the same positions
  foo(&a, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
  MPI_Recv(&a, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);

  return 0;
}
