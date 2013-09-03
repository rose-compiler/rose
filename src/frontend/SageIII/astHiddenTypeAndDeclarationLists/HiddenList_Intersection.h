#if 0
// DQ (5/21/2013): I think that we can and should remove this file.
// for the moment I will comment out the whole file.

// Header-File for the Hidden List Intersection Procedure
// Robert Preissl
// Last modified : April 16, 2007

#ifndef DEBUG_HIDDENLIST_INTERSECTION_H_
#define DEBUG_HIDDENLIST_INTERSECTION_H_

// DQ (5/8/2007): This header file is included in the HiddenList.h header file
#include "HiddenList.h"

#ifndef USE_ROSE
  // DQ (2/11/2013): This fails for ROSE compilign "rose.h" header file (ROSE compiling ROSE).

namespace Hidden_List_Computation {

// Robert Preissl, June 1 2007, call Vector_Of_SymbolInformation per reference
//ScopeStackEntry Intersection(std::vector<ScopeStackEntry>* p_VSHM, int depth, Vector_Of_SymbolInformation &X);
void Intersection(

    std::vector<ScopeStackEntry>* p_VSHM,
    int depth,
    Vector_Of_SymbolInformation &X,
    SetSgSymbolPointers &AvailableHidden_Functions_Variables,
    SetSgSymbolPointers &AvailableHidden_Types,
    SetSgSymbolPointers &ElaboratingForcingHidden_Types,
    UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations,
    UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations,
    UsingDirectiveStatement_SetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations_2,
    UsingDeclarationStatement_SetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations_2,
    SetSgUsingDirectiveStatementsWithSgScopeStatement& UsingDirectivesSet,
    SetSgUsingDeclarationWithScopeWithSgScopeStatement& UsingDeclarationsSet

);

// Functions are treated separately, because they are not elements of the symbol-table in ROSE
void Intersection_with_FunctionDeclaration(

    std::vector<ScopeStackEntry>* p_VSHM,
    int depth,
    SgFunctionDeclaration* f_decl,
    SetSgSymbolPointers &AvailableHidden_Functions_Variables,
    SetSgSymbolPointers &AvailableHidden_Types,
    SetSgSymbolPointers &ElaboratingForcingHidden_Types,
    UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations,
    UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations,
    UsingDirectiveStatement_SetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations_2,
    UsingDeclarationStatement_SetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations_2,
    SetSgUsingDirectiveStatementsWithSgScopeStatement& UsingDirectivesSet,
    SetSgUsingDeclarationWithScopeWithSgScopeStatement& UsingDeclarationsSet);

} // end of namespace: Hidden_List_Computation

#endif

#endif /*DEBUG_HIDDENLIST_INTERSECTION_H_*/

// DQ (5/21/2013): I think that we can and should remove this file.
// for the moment I will comment out the whole file.
#endif

