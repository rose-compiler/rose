#include "mpi.h"

int main(int argc, char *argv[])
{
    int        rank, size, rank2, rank3, blah;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    rank2 = rank + 1;
    rank3 = rank2 - 1;

    if(rank3 == 0) {
        MPI_Bcast(&blah, 1, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        MPI_Status status;
        MPI_Recv(&blah, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    MPI_Finalize( );
    return 0;
}
