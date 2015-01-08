#ifndef RDTRANSFERFUNCTIONS_H
#define RDTRANSFERFUNCTIONS_H

#include "DFTransferFunctions.h"
#include "RDLattice.h"

class RDTransferFunctions : public DFTransferFunctions {
public:
  RDTransferFunctions();
  void transferExpression(SPRAY::Label label, SgExpression* expr, Lattice& element);
  void transferDeclaration(SPRAY::Label label, SgVariableDeclaration* decl, Lattice& element);
  void transferFunctionCall(SPRAY::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  void transferFunctionCallReturn(SPRAY::Label lab, SgFunctionCallExp* callExp, Lattice& element);
  void transferFunctionEntry(SPRAY::Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  void transferFunctionExit(SPRAY::Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
};

#endif
