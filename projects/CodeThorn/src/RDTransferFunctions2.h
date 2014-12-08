#ifndef RDTRANSFERFUNCTIONS2_H
#define RDTRANSFERFUNCTIONS2_H

#include "PATransferFunctions.h"
#include "RDLattice.h"

class RDTransferFunctions : public PATransferFunctions {
public:
  RDTransferFunctions();
  void transferExpression(Label label, SgExpression* expr, RDLattice& element);
  void transferDeclaration(Label label, SgVariableDeclaration* decl, RDLattice& element);
  void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, RDLattice& element);
  void transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, RDLattice& element);
  void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, RDLattice& element);
  void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, RDLattice& element);
};

#endif
