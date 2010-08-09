#ifndef NULLIFY_AST_H
#define NULLIFY_AST_H

class NullTreeMemoryPoolTraversal 
   : public ROSE_VisitTraversal
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);

          virtual ~NullTreeMemoryPoolTraversal() {};
   };

void NullTree();

#endif // NULLIFY_AST_H

