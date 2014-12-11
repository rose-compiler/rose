#ifndef LVTRANSFERFUNCTIONS2_H
#define LVTRANSFERFUNCTIONS2_H

#include "PATransferFunctions.h"
#include "LVLattice.h"

namespace SPRAY {

class LVTransferFunctions : public PATransferFunctions {
public:
  LVTransferFunctions();
  void transferExpression(Label label, SgExpression* expr, Lattice& element);
  void transferDeclaration(Label label, SgVariableDeclaration* decl, Lattice& element);
  void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  void transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, Lattice& element);
  void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
};

} // end namespace SPRAY

#endif
