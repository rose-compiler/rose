#ifndef DFTRANSFERFUNCTIONS_HPP
#define DFTRANSFERFUNCTIONS_HPP

#include "Labeler.h"

template<typename LatticeType>
class DFTransferFunctions {
public:
  DFTransferFunctions();
  void setLabeler(Labeler* labeler) { _labeler=labeler; }
  Labeler* getLabeler() { return _labeler; }
  void setVariableIdMapping(VariableIdMapping* v) { _variableIdMapping=v; }
  VariableIdMapping* getVariableIdMapping() { return _variableIdMapping; }
  virtual LatticeType transfer(Label lab, LatticeType element);
  virtual void transferExpression(Label label, SgExpression* expr, LatticeType& element);
  virtual void transferDeclaration(Label label, SgVariableDeclaration* decl, LatticeType& element);
  virtual void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, LatticeType& element);
  virtual void transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, LatticeType& element);
  virtual void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, LatticeType& element);
  virtual void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, LatticeType& element);
protected:
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
};

#include "DFTransferFunctions.C"

#endif
