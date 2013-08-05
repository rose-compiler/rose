#include "mpi.h"

int main(int argc, char *argv[])
{
  int x=0;
  int i,k;

  MPI_Init( &argc, &argv );

  #pragma skel loop iterate exactly (10)
  for (i=0; x < 100 ; i++)
    x++;

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

  #pragma skel condition prob(9/10)
  if (k == 0) {
    x += 5;
  }
  return x;
}
