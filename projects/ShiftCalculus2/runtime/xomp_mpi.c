#include "libxomp_mpi.h"
#include <mpi.h> // avoid polluting global headers
#include <assert.h>

int xomp_init_mpi (int* argcp, char** argvp[], int * rank, int * nprocs)
{
  int rt = 0;
  //TODO add error code handling
  MPI_Init (argcp, argvp);
  MPI_Comm_rank (MPI_COMM_WORLD, rank);
  MPI_Comm_size (MPI_COMM_WORLD, nprocs);

  return rt; 
}


