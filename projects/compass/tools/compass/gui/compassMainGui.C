#ifdef ROSE_MPI
  #include <mpi.h>
#endif

#include "rose.h"
#include <vector>
#include <qrose.h>

#include "compassGui.h"
#include "compassInterface.h"

using namespace qrs;

int main( int argc, char **argv )
{
#ifdef ROSE_MPI
  std::cerr << "MPI enbaled .. initializing. " << std::endl;
     // Initialize MPI if needed...
     // need to do this to make test cases pass with MPI. 
     /* setup MPI */
     MPI_Init(&argc, &argv);
     MPI_Comm_rank(MPI_COMM_WORLD, &Compass::my_rank);
     MPI_Comm_size(MPI_COMM_WORLD, &Compass::processes);
#endif

  QROSE::init(argc,argv);

  CompassInterface compassInterface;
  compassInterface.init(argc, argv);

  CompassGui compassGui(compassInterface);

  QROSE::exec();
#ifdef ROSE_MPI
  MPI_Finalize();
#endif

  return 0;
}
