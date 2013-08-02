#include "mpi.h"

int main(int argc, char *argv[])
{
  int x=0;
  int i,k;

  MPI_Init( &argc, &argv );

  #pragma skel preserve
  for (i=0; x < 100 ; i++) {
    x = i + 1;
    if (x % 2)
      x += 5;

    int j;
    #pragma skel remove
    for (j=0; x < 100 ; j++) {
      x = j + 1;
    }
  }

  #pragma skel remove
  for (k=0; x < 500 ; k++) {
    x = k + 1;
    if (x % 2)
      x += 5;

    int j;
    #pragma  skel  preserve
    for (j=0; x < 500 ; j++) {
      x = j + 1;
    }
  }
  return x;
}
