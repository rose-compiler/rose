#include <mpi.h>
#include "Simple.h"

int Simple::doSomething(int y) {
  double buf[10];
  
  int retval = MPI_Send (buf, 10, MPI_DOUBLE, y, 0, MPI_COMM_WORLD);
  return retval;
}
