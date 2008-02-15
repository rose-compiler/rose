#ifndef FIXUP_NAMES_H
#define FIXUP_NAMES_H

/*! \brief This traversal uses the Memory Pool traversal to fixup names of declarations that are inconsistant (e.g SgclassDeclarations).

    This traversal uses the Memory Pool traversal to fixup names that are inconsistant across defining vs. 
non-defining declarations and additional non-defining declaration can can exist in the AST. 
 */

class ResetEmptyNames : public ROSE_VisitTraversal
   {
     public:
     virtual ~ResetEmptyNames() {};
      //! Required traversal function
          void visit (SgNode* node);
   };

/*! \brief This traversal uses the Memory Pool traversal to fixup names of declarations that are inconsistant (e.g SgclassDeclarations).

    This traversal uses the Memory Pool traversal to fixup names that are inconsistant across defining vs. 
non-defining declarations and additional non-defining declaration can can exist in the AST. 
 */
class ResetInconsistantNames : public ROSE_VisitTraversal
   {
     public:
     virtual ~ResetInconsistantNames() {};
      //! Required traversal function
          void visit (SgNode* node);
   };

void resetNamesInAST();

// endif for FIXUP_NAMES_H
#endif
