#include <stdlib.h>

void MPI_Init( int* argc, char*** argv )
{
}

void MPI_Finalize()
{
}

int main( int argc, char **argv )
{
  MPI_Init( &argc, &argv );
    exit(1);
  MPI_Finalize();

  return 0;
} //main()
