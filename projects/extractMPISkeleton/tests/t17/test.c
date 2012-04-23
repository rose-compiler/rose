#include "mpi.h"

int main(int argc, char *argv[])
{
  int x=0;
  int i,k;

  #pragma skel remove
  #pragma skel initializer repeat ( 10 )
  int foobar[10];

  MPI_Init( &argc, &argv );

  // #pragma skel loop iterate exactly (10)
  for (i=0; x < 100 ; i++)
    x++;

  /*
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

  //  #pragma skel loop iterate exactly (99)
  // #pragma skel preserve
  for (k=0; x < 500 ; k++) {
    x = k + 1;
    if (x % 2)
      x += 5;
  }
  */
  return x;
}
