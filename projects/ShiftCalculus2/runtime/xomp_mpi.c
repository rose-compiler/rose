#include "libxomp_mpi.h"
#include <mpi.h> // avoid polluting global headers

void xomp_init_mpi (int* argcp, char** argvp[], int * rank, int * nprocs)
{
  MPI_Init (argcp, argvp);
  MPI_Comm_rank (MPI_COMM_WORLD, rank);
  MPI_Comm_size (MPI_COMM_WORLD, nprocs);
}


