#include <stdio.h>
#include "mpi.h"

/* This example handles a 12 x 12 mesh, on 4 processors only. */
#define maxn 12

int main( argc, argv )
int argc;
char **argv;
{
    int rank, size, errcnt, toterr, i, j;
    MPI_Status status;
    double xlocal[(12/4)+2][12];

    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    if (size != 4) MPI_Abort( MPI_COMM_WORLD, 1 );

    /* xlocal[][0] is lower ghostpoints, xlocal[][maxn+2] is upper */

    /* Fill the data as specified */
    for (i=1; i<=maxn/size; i++) 
	for (j=0; j<maxn; j++) 
	    xlocal[i][j] = rank;
    for (j=0; j<maxn; j++) {
	xlocal[0][j] = -1;
	xlocal[maxn/size+1][j] = -1;
    }

    /* Send up unless I'm at the top, then receive from below */
    /* Note the use of xlocal[i] for &xlocal[i][0] */
    if (rank < size - 1) 
	MPI_Send( xlocal[maxn/size], maxn, MPI_DOUBLE, rank + 1, 0, 
		  MPI_COMM_WORLD );
    if (rank > 0)
	MPI_Recv( xlocal[0], maxn, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, 
		  &status );
    /* Send down unless I'm at the bottom */
    if (rank > 0) 
	MPI_Send( xlocal[1], maxn, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD );
    if (rank < size - 1) 
	MPI_Recv( xlocal[maxn/size+1], maxn, MPI_DOUBLE, rank + 1, 1, 
		  MPI_COMM_WORLD, &status );

    /* Check that we have the correct results */
    errcnt = 0;
    for (i=1; i<=maxn/size; i++) 
	for (j=0; j<maxn; j++) 
	    if (xlocal[i][j] != rank) errcnt++;
    for (j=0; j<maxn; j++) {
	if (xlocal[0][j] != rank - 1) errcnt++;
	if (rank < size-1 && xlocal[maxn/size+1][j] != rank + 1) errcnt++;
    }

    MPI_Reduce( &errcnt, &toterr, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );
    if (rank == 0) {
	if (toterr)
	    printf( "! found %d errors\n", toterr );
	else
	    printf( "No errors\n" );
    }

    MPI_Finalize( );
    return 0;
}
