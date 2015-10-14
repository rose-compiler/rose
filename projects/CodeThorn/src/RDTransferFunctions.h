#ifndef RDTRANSFERFUNCTIONS_H
#define RDTRANSFERFUNCTIONS_H

#include "DFTransferFunctions.h"
#include "RDLattice.h"

class RDTransferFunctions : public SPRAY::DFTransferFunctions {
public:
  RDTransferFunctions();
  void transferExpression(SPRAY::Label label, SgExpression* expr, SPRAY::Lattice& element);
  void transferDeclaration(SPRAY::Label label, SgVariableDeclaration* decl, SPRAY::Lattice& element);
  void transferReturnStmtExpr(SPRAY::Label label, SgExpression* expr, SPRAY::Lattice& element);
  void transferFunctionCall(SPRAY::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, SPRAY::Lattice& element);
  void transferFunctionCallReturn(SPRAY::Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, SPRAY::Lattice& element);
  void transferFunctionEntry(SPRAY::Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, SPRAY::Lattice& element);
  void transferFunctionExit(SPRAY::Label lab, SgFunctionDefinition* funDef, SPRAY::VariableIdSet& localVariablesInFunction, SPRAY::Lattice& element);
};

#endif
