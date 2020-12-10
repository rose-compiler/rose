#ifndef LVTRANSFERFUNCTIONS2_H
#define LVTRANSFERFUNCTIONS2_H

#include "DFTransferFunctions.h"
#include "LVLattice.h"

namespace CodeThorn {

class LVTransferFunctions : public DFTransferFunctions {
public:
  LVTransferFunctions();
  void transferExpression(Label label, SgExpression* expr, Lattice& element);
  void transferDeclaration(Label label, SgVariableDeclaration* decl, Lattice& element);
  void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  void transferFunctionCallReturn(Label lab, VariableId lhsVarId, SgFunctionCallExp* callExp, Lattice& element);
  //  void transferFunctionCallReturn(Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element);
  void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
  void initializeExtremalValue(Lattice& element);

};

} // end namespace CodeThorn

#endif
