#include "rose.h"
#include "MPINonBlockingCodeMotion.h"

using namespace MPIOptimization;
using namespace std;
using namespace VirtualCFG;

int main(int argc, char *argv[])
{
  SgProject* project = frontend(argc, argv);
	cout << "Han: frontend finished" << endl;

  MPINonBlockingCodeMotion* nonBlockingCodeMotion = new MPINonBlockingCodeMotion(project);

  if(nonBlockingCodeMotion->run()==0)
  {
    std::cerr << "MPI Non Blocking Code Motion failed!" << endl;
  }

	cout << "Han: code motion is done. Starting backend" << endl;
	
	backend(project);

  return 0;
}
