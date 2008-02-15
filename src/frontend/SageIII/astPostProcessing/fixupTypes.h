#ifndef FIXUP_TYPES_H
#define FIXUP_TYPES_H

/*! \brief This traversal uses the Memory Pool traversal to fixup types in declarations that can be shared but are not. (e.g SgclassDeclarations).

    This traversal uses the Memory Pool traversal to fixup types that are not shared across defining vs.
non-defining declarations and additional non-defining declaration can can exist in the AST. 
 */

class ResetTypes : public ROSE_VisitTraversal
   {
     public:
     virtual ~ResetTypes() {};
      //! Required traversal function
          void visit (SgNode* node);
   };

void resetTypesInAST();


// endif for FIXUP_TYPES_H
#endif
