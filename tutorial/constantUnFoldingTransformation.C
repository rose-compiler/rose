#include "rose.h"
#include "constantFolding.h"

int
main ( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

  // Test constant folding on expression trees already folded by EDG
  // (nothing to do since the frontend already did constant folding,
  // but this does remove the constant expression trees which are 
  // redundent with previously constant folded values).
     ConstantFolding::constantFoldingOptimization(project);

  // Generate from constant expressions from simple constants, as some
  // transformations might be likely to do, this will clean them up.
  // Generate the intermediate example used to demonstrate the constant folding.
     ConstantFolding::constantUnFoldingTest(project);
     ConstantFolding::constantUnFoldingTest(project);
     ConstantFolding::constantUnFoldingTest(project);
     ConstantFolding::constantUnFoldingTest(project);

     return backend(project);
   }

