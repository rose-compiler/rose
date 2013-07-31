#include <mpi.h>

int main(int argc, char** argv){
  int x,y;
#if 1
  MPI_Init(&argc, &argv);
#endif
  x = 3;
  y = 4;
  return 0;
}
