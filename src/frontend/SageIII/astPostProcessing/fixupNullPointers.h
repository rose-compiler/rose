#ifndef FIXUP_NULL_POINTERS_H
#define FIXUP_NULL_POINTERS_H

// DQ (3/11/2006):
/*! \brief Fixup specific pointers in the AST that we would like to not have be NULL.

    Even though the AST's generated from the EDG/SageIII translation have few NULL 
    pointers, AST's built by users often don't follow as many rules.  This function fixup any NULL 
    pointers left unset by the user (most often with SgNullStatement and SgNullExpression IR nodes).

    As an example, the SgForStatement need not have an increment expression (it is still valid C or C++ code).
    In this case the generated AST will build a SgNullExpression so that it is clear that there was intended to
    be an empty expression specified.  The user when building a SgForStatement might decide to just not specify
    a SgNullExpression and for simplicity use a NULL pointer.  This fixup function replaces the NULL pointer
    with a SgNullExpression so that uniform error checking can determine that there was intended to be an 
    empty value and that is not some other sort of error.  Where ever possible a NULL pointer in Sage III 
    is used to indicate an error or not yet initialized pointer, instead of a valid pointer.  In this way
    Null pointers can be treated as an error and the robustness of the AST is improved.  The goal has
    been to eliminate any semantics of a pointer being NULL.

    Although we have tried to eliminate all NULL pointers in the AST, some are required, still, and
    some pointers have semantics associated with them being NULL (much as I don't like the design concept).
    An example is a SgFunctionDeclaration, which has a NULL pointer to a definition if it is not a defining
    declaration (not that if it is a forward declaration then it is marked explicit as a forward declaration).

    \implementation Since we test for valid pointers in numerous within ROSE, this fixup could be 
    considered to partial defeat those tests.
 */
void 
fixupNullPointersInAST (SgNode* node);


// DQ (3/11/2006):
/*! \brief Fixup pointers in the AST that user's may have left as NULL, but should not be NULL.

    This is lower level support for the fixupNullPointersInAST(SgNode*) function.

 */
class FixupNullPointersInAST : public AstSimpleProcessing
   {
  // This class uses a traversal to test the values of the definingDeclaration and
  // firstNondefiningDeclaration pointers in each SgDeclarationStatement.  See code for
  // details, since both of these pointers are not always set.

     public:
          void visit ( SgNode* node );
   };

// endif for FIXUP_NULL_POINTERS_H
#endif
