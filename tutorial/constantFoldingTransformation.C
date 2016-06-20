#include "rose.h"
#include "constantFolding.h"

// IMPORTANT NOTE:
// The constant folding demonstrated here is appropriate for 
// the untransformed AST (it cleans up constant expression trees
// let in the AST by the frontend which can tend to confuse some
// analysis and transformation mechanism (PRE, specifically).
// So it is useful to run before doing forms of analysis where
// your analysis does not want to see unfolded constant expression
// trees (the sort commontly folded by most frontends).  The 
// additional constant folding done here is on the AST directly
// and is useful for cleaning up transformations.  However,
// it is not finished!

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
     ConstantFolding::constantUnFoldingTest(project);
     ConstantFolding::constantUnFoldingTest(project);
     ConstantFolding::constantUnFoldingTest(project);
     ConstantFolding::constantUnFoldingTest(project);

  // Now do the constant folding on the ugly constant expressions that 
  // we just generated!
     ConstantFolding::constantFoldingOptimization(project);

     return backend(project);
   }

