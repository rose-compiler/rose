#include <stdlib.h>
#include "mpi.h"

typedef struct _foo {
    int x;
    int *y;
} foo;

int main(int argc, char *argv[])
{
    int        rank, size;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    foo *f;
    f = malloc(sizeof(foo));
    f->x = 1;
    f->y = malloc(2 * sizeof(int));
    f->y[0] = 1;
    f->y[2] = 2;

    MPI_Send(f->y, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);

    free(f);

    MPI_Finalize();
    return 0;
}
