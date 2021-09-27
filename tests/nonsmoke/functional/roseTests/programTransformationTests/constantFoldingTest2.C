#include "rose.h"
#include "constantFolding.h"

int
main ( int argc, char * argv[] )
{
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT(project != NULL);

  ConstantFolding::constantFoldingOptimization(project,false);

  // add types, testing the SageInterface API
  VariantVector vv(V_SgType);
  Rose_STL_Container<SgNode*> tnodes= NodeQuery::queryMemoryPool(vv);
  for (Rose_STL_Container<SgNode*>::const_iterator i = tnodes.begin(); i != tnodes.end(); ++i)
  {
    SageInterface::constantFolding(*i);
  }   

  return backend(project);
}

