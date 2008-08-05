#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char** argv) {
  int i, myid;
  double *sharedArray, *privateArray;
  sharedArray = (double*)malloc(100 * sizeof(double));
  assert (sharedArray);
  privateArray = (double*)malloc(100 * sizeof(double));
  assert (privateArray);
  MPI_Init(&argc, &argv);
  for (i = 0; i < 100; ++i) {
    sharedArray[i] = i * 3.0 + 1;
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  printf("My value is %lf\n", sharedArray[myid]);
  for (i = 0; i < 100; ++i) {
    privateArray[i] = myid * 1.0 + i;
  }
  printf("My value2 is %lf\n", privateArray[30]);
  MPI_Finalize();
  free(sharedArray);
  free(privateArray);
  return 0;
}
