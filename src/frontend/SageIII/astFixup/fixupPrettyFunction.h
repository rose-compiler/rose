#ifndef FIXUP_PRETTY_FUNCTION_H
#define FIXUP_PRETTY_FUNCTION_H

class FixupPrettyFunctionVariablesInheritedAttribute 
   {
     public:
          SgFunctionDeclaration* functionDeclaration;
          FixupPrettyFunctionVariablesInheritedAttribute();

       // NOTE: This copy constructor is required to propagate the value of "functionDeclaration" member data across copies.
          FixupPrettyFunctionVariablesInheritedAttribute(const FixupPrettyFunctionVariablesInheritedAttribute &);

          FixupPrettyFunctionVariablesInheritedAttribute&
             operator=(const FixupPrettyFunctionVariablesInheritedAttribute&) = default;
   };

/*! \brief This traversal uses inherited attributes to fixup the variables put into the AST that match the function name (this is EDG trickery to signal that the variable name is "__PRETTY_FUNCTION__").

    \implementation This is required because EDG has this odd behavior of exchanging the name of variables that one would expect to be called "__PRETTY_FUNCTION__"
    with the name of the function (when it occurs in a function).  This happens frequently because "__PRETTY_FUNCTION__" is the last variable (actually pointer to 
    a function, I think) used in the translation of the "assert" macro (at least for GNU compilers).  Since ROSE_ASSERT is frequently a macro for "assert" this
    is a common problem for the compilation of the ROSE source code using ROSE translators.  Note that this subject of replacement of "__PRETTY_FUNCTION__" with 
    the name of the function where it occurs is documented in the EDG_3.3/Changes file (and now in the source code for the translation of the EDG AST to the ROSE AST.
 */
class FixupPrettyFunctionVariables : public SgTopDownProcessing<FixupPrettyFunctionVariablesInheritedAttribute>
   {
     public:
          FixupPrettyFunctionVariablesInheritedAttribute evaluateInheritedAttribute ( SgNode* node, FixupPrettyFunctionVariablesInheritedAttribute ia );
   };

void fixupPrettyFunctionVariables( SgNode* node );

// endif for FIXUP_PRETTY_FUNCTION_H
#endif
