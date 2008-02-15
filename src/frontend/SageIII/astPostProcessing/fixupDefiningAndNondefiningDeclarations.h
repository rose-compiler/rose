#ifndef FIXUP_DEFINING_AND_NONDEFINING_DECLARATIONS_H
#define FIXUP_DEFINING_AND_NONDEFINING_DECLARATIONS_H

// DQ (6/27/2005):
/*! \brief Fixup all SgDeclarationStatement references to defining and non-defining declarations.

    \implementation Some declarations are there own defining declarations.
 */
void fixupAstDefiningAndNondefiningDeclarations ( SgNode* node );

class FixupAstDefiningAndNondefiningDeclarations
// : public AstSimpleProcessing
   : public ROSE_VisitTraversal
   {
  // This class uses a traversal to test the values of the definingDeclaration and
  // firstNondefiningDeclaration pointers in each SgDeclarationStatement.  See code for
  // details, since both of these pointers are not always set.

     public:
     virtual ~FixupAstDefiningAndNondefiningDeclarations() {};
          void visit ( SgNode* node );
   };

// endif for FIXUP_DEFINING_AND_NONDEFINING_DECLARATIONS_H
#endif
