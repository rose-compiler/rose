#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int        rank, size;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    int *a1;
    a1 = malloc(2 * sizeof(int));
    a1[0] = 1;
    a1[1] = 2;
    int *a2;
    a2 = a1;

    MPI_Send(a2, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}
