#ifndef RDTRANSFERFUNCTIONS_HPP
#define RDTRANSFERFUNCTIONS_HPP

#include "DFTransferFunctions.hpp"
#include "RDLattice.h"

class RDTransferFunctions : public DFTransferFunctions<RDLattice> {
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
