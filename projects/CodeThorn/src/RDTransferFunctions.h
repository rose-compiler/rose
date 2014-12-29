#ifndef RDTRANSFERFUNCTIONS2_H
#define RDTRANSFERFUNCTIONS2_H

#include "PATransferFunctions.h"
#include "RDLattice.h"

class RDTransferFunctions : public PATransferFunctions {
public:
  RDTransferFunctions();
  void transferExpression(Label label, SgExpression* expr, Lattice& element);
  void transferDeclaration(Label label, SgVariableDeclaration* decl, Lattice& element);
  void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  void transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, Lattice& element);
  void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
};

#endif
