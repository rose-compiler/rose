// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupUseAndUsingDeclarations.h"

void fixupFortranUseDeclarations (SgNode* node)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup Fortran Use Declarations:");

  // printf ("Fixup the Fortran Use Declarations ... \n");

     FixupFortranUseDeclarations astFixupTraversal;

#if 0
  // DQ (4/14/2010): Now I see that this is not used!

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
#endif
   }

void
FixupFortranUseDeclarations::visit ( SgNode* node )
   {
  // DQ (11/24/2007): Output the current IR node for debugging the traversal of the Fortran AST.
  // printf ("node = %s \n",node->class_name().c_str());
   }



