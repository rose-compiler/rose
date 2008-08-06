#include <mpi.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

double sharedArray[100];
double privateArray[100];

int main(int argc, char** argv) {
  int i, myid;
  FILE* f;
  FILE* f_priv;
  char* myfilename;
  MPI_Init(&argc, &argv);
  f = fopen("inputdata", "r");
  assert (f);
  for (i = 0; i < 100; ++i) {
    int count;
    double d;
    count = fscanf(f, "%lf", &d);
    assert (count == 1);
    sharedArray[i] = d;
  }
  fclose(f);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  printf("My value is %lf\n", sharedArray[myid]);
  asprintf(&myfilename, "inputprivate_%d\n", myid);
  f_priv = fopen(myfilename, "r");
  assert (f_priv);
  for (i = 0; i < 100; ++i) {
    int count;
    double d;
    count = fscanf(f_priv, "%lf", &d);
    assert (count == 1);
    privateArray[i] = d * 3. + 7.;
  }
  fclose(f_priv);
  free(myfilename);
  printf("My value2 is %lf\n", privateArray[30]);
  MPI_Finalize();
  return 0;
}
