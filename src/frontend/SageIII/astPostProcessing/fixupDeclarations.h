#ifndef FIXUP_DECLARATIONS_H
#define FIXUP_DECLARATIONS_H

// DQ (4/19/2005): 
/*! \brief Fixup definingDeclaration and firstNondefiningDeclaration pointers in SgDeclarationStatement.
 */
void fixupDeclarations( SgNode* node );

/*! \brief Initialize the definingDeclaration and firstNondefiningDeclaration pointers in SgDeclarationStatement
           consistantly for all declarations.  Also fixup special case of multiple forward declaration where only the
           first one (the firstNondefiningDeclaration) get a valid pointer to the defining declaration if it exists.

    \internal This is a mechanism that would be complex to replace with a query (I think).  It would be at least expensive since
           the defining and non-defining declarations need not even appear in the same scope.
 */
class FixupDeclarations : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };

// endif for FIXUP_DECLARATIONS_H
#endif
