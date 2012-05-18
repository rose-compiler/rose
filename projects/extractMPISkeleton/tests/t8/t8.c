#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int        rank, size;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    int *a;
    a = malloc(2 * sizeof(int));
    a[0] = 1;
    a[1] = 2;

    MPI_Send(a, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);

    free(a);

    MPI_Finalize();
    return 0;
}
