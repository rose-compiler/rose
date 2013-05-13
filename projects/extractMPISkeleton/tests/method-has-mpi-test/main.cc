#include <stdio.h>
#include <mpi.h>
#include "Simple.h"

int main(int ac, char **av) {
  int x,y;
  Simple simple;

  MPI_Init(&ac,&av);

  // y = 42;
  x = simple.doSomething(42);
  printf("%d\n", x);

  MPI_Finalize();
  return(1);
}

