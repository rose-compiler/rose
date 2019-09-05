#ifndef RDTRANSFERFUNCTIONS_H
#define RDTRANSFERFUNCTIONS_H

#include "DFTransferFunctions.h"
#include "RDLattice.h"

namespace CodeThorn
{

class RDTransferFunctions : public CodeThorn::DFTransferFunctions {
public:
  RDTransferFunctions();
  void transferExpression(CodeThorn::Label label, SgExpression* expr, CodeThorn::Lattice& element);
  void transferDeclaration(CodeThorn::Label label, SgVariableDeclaration* decl, CodeThorn::Lattice& element);
  void transferReturnStmtExpr(CodeThorn::Label label, SgExpression* expr, CodeThorn::Lattice& element);
  void transferFunctionCall(CodeThorn::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, CodeThorn::Lattice& element);
  //void transferFunctionCallReturn(CodeThorn::Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, CodeThorn::Lattice& element);
  void transferFunctionCallReturn(CodeThorn::Label lab, CodeThorn::VariableId lhsVarId, SgFunctionCallExp* callExp, CodeThorn::Lattice& element);
  void transferFunctionEntry(CodeThorn::Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, CodeThorn::Lattice& element);
  void transferFunctionExit(CodeThorn::Label lab, SgFunctionDefinition* funDef, CodeThorn::VariableIdSet& localVariablesInFunction, CodeThorn::Lattice& element);
  void initializeExtremalValue(CodeThorn::Lattice& element);
};

}

//~ using CodeThorn::RDTransferFunctions;

#endif
