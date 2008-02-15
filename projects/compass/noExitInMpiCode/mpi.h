#ifndef MPI_H
#define MPI_H

// This is a header for those without MPI; it's assumed that the system will
// find an actual mpi.h header before this local file is included

int MPI_Init( int *argc, char ***argv );
int MPI_Finalize();

namespace MPI
{
  void Init();
  void Init(  int& argc, char**& argv );
  void Finalize();
}

#endif
