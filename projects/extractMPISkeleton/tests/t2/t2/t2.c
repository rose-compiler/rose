// A copy of t5.c but testing preserve and remove.

#include "mpi.h"

int main(int argc, char *argv[])
{
    int        rank, size;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    int x, y;
    x = 5;
    x++;
    y = 6;
    #pragma skel preserve
    y++;

    MPI_Send( &x, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD );
    #pragma skel remove
    MPI_Send( &y, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD );

    MPI_Finalize( );
    return 0;
}
