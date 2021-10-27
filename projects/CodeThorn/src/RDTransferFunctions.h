#ifndef RDTRANSFERFUNCTIONS_H
#define RDTRANSFERFUNCTIONS_H

#include "DFTransferFunctions.h"
#include "RDLattice.h"

namespace CodeThorn
{

class RDTransferFunctions : public DFTransferFunctions {
public:
  RDTransferFunctions();
  void transferExpression(Label label, SgExpression* expr, Lattice& element) override;
  void transferDeclaration(Label label, SgVariableDeclaration* decl, Lattice& element) override;
  void transferReturnStmtExpr(Label label, SgExpression* expr, Lattice& element) override;
  void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element) override;
  void transferFunctionCallReturn(Label lab, VariableId lhsVarId, SgFunctionCallExp* callExp, Lattice& element) override;
  void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element) override;
  void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element) override;
  void initializeExtremalValue(Lattice& element) override;
};

}

//~ using RDTransferFunctions;

#endif
