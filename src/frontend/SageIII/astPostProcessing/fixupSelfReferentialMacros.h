// This AST postprocessing fixes some known self referential macros 
// in common header files.  These are header files that are new to 
// being seen by ROSE and a part of new applications being processed.

#ifndef FIXUP_SELF_REFERENTIAL_MACROS_H
#define FIXUP_SELF_REFERENTIAL_MACROS_H

// DQ (10/5/2012):
/*! \brief Fixup known macros that reference themselves and cause recursive macro expansion in the generated (unparsed) code.

    \implementation This is a temporary fix until macros are properly re-wrapped in the unparsed code.
 */
void 
fixupSelfReferentialMacrosInAST (SgNode* node);


// DQ (3/11/2006):
/*! \brief Fixup known macros that reference themselves and cause recursive macro expansion in the generated (unparsed) code.

    This is lower level support for the fixupSelfReferentialMacrosInAST(SgNode*) function.

 */
class FixupSelfReferentialMacrosInAST : public AstSimpleProcessing
   {
  // This class uses a traversal to test the values of the definingDeclaration and
  // firstNondefiningDeclaration pointers in each SgDeclarationStatement.  See code for
  // details, since both of these pointers are not always set.

     public:
          void visit ( SgNode* node );
   };

// endif for FIXUP_SELF_REFERENTIAL_MACROS_H
#endif


