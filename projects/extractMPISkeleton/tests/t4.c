#include "mpi.h"

int main(int argc, char *argv[])
{
    int        rank, size;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    int abortarg;
    abortarg = 1;
    if (size != 4)
      MPI_Abort( MPI_COMM_WORLD, abortarg );
    MPI_Finalize( );
    return 0;
}
