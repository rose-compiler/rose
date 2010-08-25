typedef unsigned int size_t;
#include <stdio.h>
#include "mpi.h"

MPI_Status status;
main(int argc, char **argv) 
{
        int     numtasks,               /* total number of MPI process in partitiion */
                numworkers,             /* number of worker tasks */
                taskid;                 /* task identifier */
                #pragma SliceTarget
MPI_Init(&argc, &argv);
                #pragma SliceTarget
MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
                #pragma SliceTarget
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
                #pragma SliceTarget
MPI_Finalize();
}
