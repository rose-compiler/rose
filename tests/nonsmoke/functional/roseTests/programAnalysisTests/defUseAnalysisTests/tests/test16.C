
#include <stdio.h>
#include "mpi.h"

#define HAND 5

int main() {
  int index,i=0;
  while (i) {
    MPI_Send(&index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    //printf("Hellow world", &index, &HAND);
  }
}
