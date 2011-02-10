// MPI program that simulates a "Master-Slave" MPI_Bcast with MPI_Sends & MPI_Recv

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {

        int numtasks, rank, dest, source, rc, count, tag=1;
        int buf, sum, i, nr_of_bcasts;
        double starttime, endtime, time_used;

        if(argc > 0) {
                nr_of_bcasts = atoi(argv[1]);
        }
        else { nr_of_bcasts = 100; }

        MPI_Status Stat;

        MPI_Init(&argc,&argv);
        MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        // start time measurement
        if(rank == 0) {starttime = MPI_Wtime(); }

        if(rank == 0) { buf = nr_of_bcasts; }
        sum = 0;

        for(i = 0; i < nr_of_bcasts; ++i ) {

                if (rank == 0) {

                        // as long as the data that will be broadcasted is changed before the actual Broadcast starts
                        //  it doesn't violate the rule to be a "real" Broadcast and it's one of the things to check
                        //  before we can do a Source-Code-Transformation
                        // -> These checks (if it is a Broadcast and if it is safe to transform the code using MPI_Bcast
                        //   will be done by static analysis using ROSEs SystemDependenceGraph)
                        buf--;

                        // simulate the MPI_Bcast:
                        for(dest = 1; dest < numtasks; ++dest) {
                                rc = MPI_Send(&buf, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
                        }

                }

                else if (rank != 0) {

                        source = 0;
                        rc = MPI_Recv(&buf, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &Stat);
                        sum += buf;

                }

                //rc = MPI_Get_count(&Stat, MPI_CHAR, &count);

                // printf("Task %d: Received %d \n",rank, buf);

        }

        if(rank != 0) { fprintf(stderr,"Proc %d: Sum of data %d.\n", rank, sum); }

        if(rank == 0)  {

                endtime=MPI_Wtime(); time_used=endtime-starttime;
                fprintf(stderr,"MASTER: Time for simulating an MPI_BCAST on %d machines with %d enumerations : %lf [sec].\n", numtasks, nr_of_bcasts, time_used);

        }

        MPI_Finalize();

}
