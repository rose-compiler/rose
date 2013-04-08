#ifndef INLINERSUPPORT_H
#define INLINERSUPPORT_H

// DQ (3/12/2006): This is now not required, the file name is 
// changed to rose_config.h and is included directly by rose.h.
// #include "config.h"



// DQ (3/12/2006): This is included within rose.h
// #include <map>

// From isPotentiallyModified.C:

//! Is expression expr potentially modified (assigned to, address taken,
//! passed by non-const reference) somewhere in the tree pointed to by
//! root?
ROSE_DLL_API bool isPotentiallyModified(SgExpression* expr, SgNode* root);

//! Does expression expr potentially have its address taken somewhere in the
//! tree pointed to by root?
ROSE_DLL_API bool hasAddressTaken(SgExpression* expr, SgNode* root); 

// From inlinerSupport.C:

//! Flatten nested blocks of code in a program.  This code requires that
//! variable names in the region being processed are already distinct, and
//! it may extend variable lifetimes to the end of the outer block of code.
ROSE_DLL_API void flattenBlocks(SgNode* n);

//! Internal routine to check whether a given variable declaration is a
//! member variable.
ROSE_DLL_API bool isMemberVariable(const SgNode& in);

//! Rename all variables in a block of code, and ensure that all
//! variable references point to the correct declarations.
ROSE_DLL_API void renameVariables(SgNode* n);

//! Rename all labels in a block of code.
ROSE_DLL_API void renameLabels(SgNode* n, SgFunctionDefinition* enclosingFunctionDefinition);

//! Change all variable and label references to point to new symbols,
//! created to point to the matching declaration found in the program.  This
//! needs to be moved into somewhere in FixSgTree.
ROSE_DLL_API void rebindVariableAndLabelReferences(SgNode* top);

//! Workaround for ROSE bug.  Should be used whenever expressions in a
//! program are modified.
ROSE_DLL_API void fixReturnStatements(SgNode* n);

//! Remove unused variables in a scope
ROSE_DLL_API void removeUnusedVariables(SgNode* top, const std::set<SgFunctionDeclaration*>& safeFunctions = std::set<SgFunctionDeclaration*>());

//! Find all variables referenced in a region
ROSE_DLL_API std::set<SgInitializedName*> findVariablesUsedInRegion(SgNode* e);

//! In code with declarations such as "int foo = bar", where foo and bar are
//! not modified, replace "foo" with "bar" and remove the declaration
ROSE_DLL_API void simpleCopyAndConstantPropagation(SgNode* top);

//! Remove all null statements (those with only an integer constant or a null
//! expression)
ROSE_DLL_API void removeNullStatements(SgNode* n);

//! Combined procedure for cleaning up code after inlining.  Does not do
//! variable renaming or block flattening, however.
ROSE_DLL_API void cleanupInlinedCode(SgNode* n);

//! Change all members in a program to be public.  There should really be a
//! smarter procedure for this that only changes members used by inlined
//! code.
ROSE_DLL_API void changeAllMembersToPublic(SgNode* n);

//! Get all variable names declared within a scope, in the correct order for
//! destruction
ROSE_DLL_API SgInitializedNamePtrList findInitializedNamesInScope(SgScopeStatement* scope);

//! Remove the declaration of a given variable
ROSE_DLL_API void removeVariableDeclaration(SgInitializedName* initname);

#endif // INLINERSUPPORT_H
