#include "rose.h"
#include "constantFolding.h"

int
main ( int argc, char * argv[] )
{
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT(project != NULL);

  ConstantFolding::constantFoldingOptimization(project,false);

  return backend(project);
}

