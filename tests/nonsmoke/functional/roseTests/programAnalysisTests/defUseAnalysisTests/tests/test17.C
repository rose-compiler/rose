typedef unsigned int size_t;
#include "mpi.h"
#define	ARRAYSIZE	60000	
#define MASTER		0	/* taskid of first process */

MPI_Status status;
main(int argc, char **argv) 
{
	int	numtasks, 		/* total number of MPI process in partitiion */
			numworkers,		/* number of worker tasks */
			taskid,			/* task identifier */
			dest,			/* destination task id to send message */
			index, 			/* index into the array */
			i, 			/* loop variable */
			source,			/* origin task id of message */
			chunksize; 		/* for partitioning the array */
	float	data[ARRAYSIZE], 	/* the intial array */
				result[ARRAYSIZE];	/* for holding results of array operations */

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	numworkers = numtasks-1;
	chunksize = (ARRAYSIZE / numworkers);

	if (taskid == MASTER) {

		for(i=0; i<ARRAYSIZE; i++) 
			data[i] =  0.0;
		index = 0;

		for (dest=1; dest<= numworkers; dest++) {
			MPI_Send(&index, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
			MPI_Send(&data[index], chunksize, MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
			index = index + chunksize;
		}

		for (i=1; i<= numworkers; i++) {
			source = i;
			MPI_Recv(&index, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
			MPI_Recv(&result[index], chunksize, MPI_FLOAT, source, 1, MPI_COMM_WORLD, 
					&status);

		}

	}


	if (taskid > MASTER) {
		source = MASTER;
		MPI_Recv(&index, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&result[index], chunksize, MPI_FLOAT, source, 0, 
				MPI_COMM_WORLD, &status);
		for(i=index; i < index + chunksize; i++)
			result[i] = i + 1;


		MPI_Send(&index, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD);
		MPI_Send(&result[index], chunksize, MPI_FLOAT, MASTER, 1, MPI_COMM_WORLD);

	}
	MPI_Finalize();
}
