#include <stdio.h>
#include <mpi.h>
#include "Simple.h"

int main(int ac, char **av) {
  int x,y;
  Simple simple;

  MPI_Init(&ac,&av);

  while (1) {
    y = 42;
    x = simple.doSomething(y);

    printf("%d\n",x);
    if (x == 14) { break; }
  }  

  MPI_Finalize();
  return(1);
}
  
    
#include <mpi.h>
#include "Simple.h"

int Simple::doSomething(int y) {
  double buf[10];
  
  int retval = MPI_Send (buf, 10, MPI_DOUBLE, y, 0, MPI_COMM_WORLD);
  return retval;
}
