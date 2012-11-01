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






// This AST postprocessing fixes a bug in EDG where some variable declaration are dropped 
// from the source sequence list (or appear to be).  This is demonstrated by test2012_144.c
// and is fixed (temporarily by forcing the convert_field_use() function to add the declarations
// to the class definition.  Under some rare cases (demonstrated in test2012_144.c (the second 
// union definition) this is required as the only way to generate the varabile declarations into
// the AST.  Later in processing the variable will be added and in most cases this will be
// redundant with the addition of the variabel from the convert_field_use() function.  However
// the addition of these variables from the convert_field_use() function is certainly wrong,
// so we record all such variable declarations added via the convert_field_use() function
// and detect where they are duplicates in the finall class definitions as a post processing step.
// Only duplicates addes via the convert_field_use() function can be removed.  The STL set
// nodesAddedWithinFieldUseSet is used to record the list of possible (and likely) duplicate
// variable declarations.  Detected duplicate members are removed from the class definition,
// but presently left in the AST.  They could be removed, but I would rather not have to
// do this fix.  So we will have to see if this is a probem in future versions of EDG.

#ifndef FIXUP_EDG_BUG_DUPLICATE_VARIABLES_H
#define FIXUP_EDG_BUG_DUPLICATE_VARIABLES_H

// DQ (10/5/2012):
/*! \brief Fixup known bug in EDG (see comments in header file.

    \implementation This is a temporary fix until EDG is fixed.
 */
void 
fixupEdgBugDuplicateVariablesInAST ();

// endif for FIXUP_EDG_BUG_DUPLICATE_VARIABLES_H
#endif


