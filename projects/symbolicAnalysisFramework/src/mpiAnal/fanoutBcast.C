//#include <mpi.h>

#define MPI_COMM_WORLD 1
typedef struct
{
	int count;
} MPI_Status;
#define MPI_INTEGER 0

int MPI_Comm_rank(int, int*);//{return 0;}
int MPI_Comm_size(int, int*);//{return 0;}
int MPI_Init(int*, char***);//{return 0;}
int MPI_Send(void*, int, int, int, int, int);//{return 0;}
int MPI_Recv(void*, int, int, int, int, int, MPI_Status*);//{return 0;}

int main(int argc, char** argv)
{
	int rank, nprocs;
	MPI_Init(&argc, &argv);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	
	if(nprocs>=2)
	{
		int buf[100];
		if(rank<=0)
		{
			for(int i=1; i<nprocs; i++)
			{
				MPI_Send(buf, 100, MPI_INTEGER, i, 1, MPI_COMM_WORLD);
			}
		}
		else
		{
			MPI_Status status;
			MPI_Recv(buf, 100, MPI_INTEGER, 0, 1, MPI_COMM_WORLD, &status);
		}
	}
/*	int mult;
	mult = rank * nprocs;
	int greg=mult, raga=rank, kali;*/
	return 0;
}
