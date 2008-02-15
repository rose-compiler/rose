#include <stdlib.h>
#include "mpi.h"

int main( int argc, char **argv )
{
  MPI_Init( &argc, &argv );
    exit(1);
  MPI_Finalize();

  return 0;
} //main()
