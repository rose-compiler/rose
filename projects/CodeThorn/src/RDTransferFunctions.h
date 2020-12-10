#ifndef RDTRANSFERFUNCTIONS_H
#define RDTRANSFERFUNCTIONS_H

#include "DFTransferFunctions.h"
#include "RDLattice.h"

namespace CodeThorn
{

class RDTransferFunctions : public DFTransferFunctions {
public:
  RDTransferFunctions();
  void transferExpression(Label label, SgExpression* expr, Lattice& element) ROSE_OVERRIDE;
  void transferDeclaration(Label label, SgVariableDeclaration* decl, Lattice& element) ROSE_OVERRIDE;
  void transferReturnStmtExpr(Label label, SgExpression* expr, Lattice& element) ROSE_OVERRIDE;
  void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element) ROSE_OVERRIDE;
  void transferFunctionCallReturn(Label lab, VariableId lhsVarId, SgFunctionCallExp* callExp, Lattice& element) ROSE_OVERRIDE;
  void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element) ROSE_OVERRIDE;
  void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element) ROSE_OVERRIDE;
  void initializeExtremalValue(Lattice& element) ROSE_OVERRIDE;
};

}

//~ using RDTransferFunctions;

#endif
