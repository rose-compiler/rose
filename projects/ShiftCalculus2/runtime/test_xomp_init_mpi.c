#include <stdio.h>
#include <mpi.h> 
#include "libxomp_mpi.h"

void main (int argc, char *argv[]) 
{
  int myrank, size;

#if 0
  MPI_Init(&argc, &argv);                 /* Initialize MPI       */
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank); /* Get my rank          */
  MPI_Comm_size(MPI_COMM_WORLD, &size);   /* Get the total number of processors */
#else
  xomp_init_mpi (&argc, &argv,&myrank, &size); 
#endif 
  printf("Processor %d of %d: Hello World!\n", myrank, size);

  MPI_Finalize();                         /* Terminate MPI        */

}
