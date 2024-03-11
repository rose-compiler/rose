#ifndef ROSE_sageTraversal_H
#define ROSE_sageTraversal_H

#include <rosedll.h>

// DQ (11/26/2005): Support for visitor pattern.
class ROSE_VisitTraversal;
class ROSE_VisitorPattern;
class SgNode;

// DQ (12/23/2005): support for traversal of AST using visitor pattern
ROSE_DLL_API void traverseMemoryPoolNodes          ( ROSE_VisitTraversal & traversal );
ROSE_DLL_API void traverseMemoryPoolVisitorPattern ( ROSE_VisitorPattern & visitor );

// DQ (2/9/2006): Added to support traversal over single representative of each IR node
// This traversal helps support intrnal tools that call static member functions.
ROSE_DLL_API void traverseRepresentativeNodes ( ROSE_VisitTraversal & traversal );

// DQ (12/26/2005): Simple traversal base class for use with ROSE style
// traversals.  Need to move this to a different location later.
class ROSE_VisitTraversal
   {
     public:
          virtual ~ROSE_VisitTraversal() {};
          virtual void visit (SgNode* node) = 0;
          void traverseMemoryPool()
             {
               traverseMemoryPoolNodes(*this);
             }
          void traverseRepresentativeIRnodes()
             {
               traverseRepresentativeNodes(*this);
             }
   };
#endif
