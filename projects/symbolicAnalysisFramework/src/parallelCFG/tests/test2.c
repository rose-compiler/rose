#include "mpi.h"

int main(int argc, char* argv[])
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;

    int msg = 10;
    
    if(rank == 0) {
#pragma pcfg_match (alpha, 1) (beta, 1)
        MPI_Recv(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
    }
    else if(rank == 1) { 
#pragma pcfg_match (beta, 1) (alpha, 1)
        MPI_Send(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }   
    #pragma pcfg_merge
    MPI_Finalize();

    return 0;
}
