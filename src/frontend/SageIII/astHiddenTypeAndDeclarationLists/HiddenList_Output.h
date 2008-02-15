// Header-File for the Hidden List Output (& Debug prints)
// Robert Preissl
// Last modified : April 16, 2007

#ifndef DEBUG_HIDDENLIST_OUTPUT_H_
#define DEBUG_HIDDENLIST_OUTPUT_H_

// DQ (5/8/2007): This header file is included in the HiddenList.h header file
 #include "HiddenList.h"

namespace Hidden_List_Computation {

void VectorOutput(Vector_Of_SymbolInformation X);

void HiddenListOutput(ScopeStackEntry SSE);

void OutputVectorStringSetHashMap(std::vector<ScopeStackEntry> VSSE);

void StringVectorHashMapOutput(StringVectorHashMap &SVHM);

void String_VectorOfNamespaceInformation_HashMapOutput(String_VectorOfNamespaceInformation_HashMap &Out);

void OutputVectorSetSgDeclarationStatements (std::vector<SetSgDeclarationStatements>* VSS);

void OutputSubsetsOfHiddenList(SetSgSymbolPointers AvailableHidden_Functions_Variables, SetSgSymbolPointers AvailableHidden_Types, SetSgSymbolPointers ElaboratingForcingHidden_Types) ;

void UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap__Output (UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& X);

void StackSetSgDeclarationStatements__Output(const StackSetSgDeclarationStatements& X, SgNamespaceDeclarationStatement* namesp_decl_stat );

void StackSetSgDeclarationStatements__Output(const StackSetSgDeclarationStatements& X );

void OutputSetSgDeclarationStatements(SetSgDeclarationStatements& X);

void UsingDirectiveStatement_SetSgDeclarationStatements_HashMap__Output(UsingDirectiveStatement_SetSgDeclarationStatements_HashMap& X);

void UsingDeclarationStatement_SetSgDeclarationStatements_HashMap__Output(UsingDeclarationStatement_SetSgDeclarationStatements_HashMap& X);

void StackOfSymbolTableOutput(std::vector<ScopeStackEntry>& VectorScopeStack, int depth);

} // end of namespace: Hidden_List_Computation

#endif /*DEBUG_HIDDENLIST_OUTPUT_H_*/
