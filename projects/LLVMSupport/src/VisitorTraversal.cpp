#include "VisitorTraversal.h"

VisitorTraversal::VisitorTraversal()
{
    // empty constructor
}

void VisitorTraversal::visit(SgNode *node)
{
    SgFunctionDeclaration *fndecl = isSgFunctionDeclaration(node);
    
}

void VisitorTraversal::atTraversalEnd()
{
   
}
