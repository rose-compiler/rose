// Non-template functions of the distributed memory traversal framework.
// Author: Gergo Barany
// $Id: DistributedMemoryAnalysis.C,v 1.1 2008/01/08 02:55:52 dquinlan Exp $

#include "DistributedMemoryAnalysis.h"
#include <mpi.h>

void initializeDistributedMemoryProcessing(int *argc, char ***argv)
{
    MPI_Init(argc, argv);
}
                                                                                                                                                                                                     
void finalizeDistributedMemoryProcessing()
{
    MPI_Finalize();
}

bool
DistributedMemoryAnalysisNamespace::postTraversalEvaluateInheritedAttribute(SgNode *node, bool inFunction)
{
 // Determine from the inherited attribute and the AST node whether this node is inside a defining function declaration.
    if (inFunction)
        return true;

 // DQ (9/28/2007):
 // This is where the load balancing grainularity is defined and it must match that of the
 // other implementation in the DistributedMemoryAnalysisPreTraversal

    SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(node);
    if (funcDecl && funcDecl->get_definingDeclaration() == funcDecl)
        return true;

    return false;
}

