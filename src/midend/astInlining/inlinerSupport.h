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
bool isPotentiallyModified(SgExpression* expr, SgNode* root);

//! Does expression expr potentially have its address taken somewhere in the
//! tree pointed to by root?
bool hasAddressTaken(SgExpression* expr, SgNode* root); 

// From inlinerSupport.C:

//! Flatten nested blocks of code in a program.  This code requires that
//! variable names in the region being processed are already distinct, and
//! it may extend variable lifetimes to the end of the outer block of code.
void flattenBlocks(SgNode* n);

//! Internal routine to check whether a given variable declaration is a
//! member variable.
bool isMemberVariable(const SgNode& in);

//! Rename all variables in a block of code, and ensure that all
//! variable references point to the correct declarations.
void renameVariables(SgNode* n);

//! Rename all labels in a block of code.
void renameLabels(SgNode* n, SgFunctionDefinition* enclosingFunctionDefinition);

//! Change all variable and label references to point to new symbols,
//! created to point to the matching declaration found in the program.  This
//! needs to be moved into somewhere in FixSgTree.
void rebindVariableAndLabelReferences(SgNode* top);

//! Workaround for ROSE bug.  Should be used whenever expressions in a
//! program are modified.
void fixReturnStatements(SgNode* n);

//! Remove unused variables in a scope
void removeUnusedVariables(SgNode* top, const std::set<SgFunctionDeclaration*>& safeFunctions = std::set<SgFunctionDeclaration*>());

//! Find all variables referenced in a region
std::set<SgInitializedName*> findVariablesUsedInRegion(SgNode* e);

//! In code with declarations such as "int foo = bar", where foo and bar are
//! not modified, replace "foo" with "bar" and remove the declaration
void simpleCopyAndConstantPropagation(SgNode* top);

//! Remove all null statements (those with only an integer constant or a null
//! expression)
void removeNullStatements(SgNode* n);

//! Combined procedure for cleaning up code after inlining.  Does not do
//! variable renaming or block flattening, however.
void cleanupInlinedCode(SgNode* n);

//! Change all members in a program to be public.  There should really be a
//! smarter procedure for this that only changes members used by inlined
//! code.
void changeAllMembersToPublic(SgNode* n);

//! Get all variable names declared within a scope, in the correct order for
//! destruction
SgInitializedNamePtrList findInitializedNamesInScope(SgScopeStatement* scope);

//! Remove the declaration of a given variable
void removeVariableDeclaration(SgInitializedName* initname);

#endif // INLINERSUPPORT_H
