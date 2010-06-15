#include "VisitorTraversal.h"
#include <iostream>

VisitorTraversal::VisitorTraversal()
{
    // empty constructor
}

void VisitorTraversal::visit(SgNode *node)
{
    bool fwd_decl = false;
   SgFunctionDeclaration *fndecl = isSgFunctionDeclaration(node);
   if(fndecl != NULL)
       fwd_decl = fndecl->isForward();
   std::cout << fwd_decl << std::endl;
}

void VisitorTraversal::atTraversalEnd()
{
   
}
