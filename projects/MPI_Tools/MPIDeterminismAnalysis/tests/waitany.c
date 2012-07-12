#include <mpi.h>
#include <stdlib.h>

int SOURCE_DETERMINISM = 1;
int TAG_DETERMINISM = 1;
int FUNCTION_DETERMINISM = 0;

int main(int argc, char **argv) {
  int a;

  MPI_Request reqs[2];

  MPI_Waitany(2, reqs, &a, MPI_STATUS_IGNORE);

  return 0;
}
