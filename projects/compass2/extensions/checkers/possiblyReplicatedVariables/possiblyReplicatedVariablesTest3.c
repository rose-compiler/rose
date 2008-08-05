#include <mpi.h>
#include <stdio.h>

double sharedArray[100];
double privateArray[100];

int main(int argc, char** argv) {
  int i, myid;
  MPI_Init(&argc, &argv);
  for (i = 0; i < 100; ++i) {
    sharedArray[i] = i * 3.0 + 1;
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  fprintf(stdout, "My value is %lf\n", sharedArray[myid]);
  for (i = 0; i < 100; ++i) {
    privateArray[i] = myid * 1.0 + i;
  }
  fprintf(stdout, "My value2 is %lf\n", privateArray[30]);
  MPI_Finalize();
  return 0;
}
