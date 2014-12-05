#ifndef DFTRANSFERFUNCTIONS_HPP
#define DFTRANSFERFUNCTIONS_HPP

#include "Labeler.h"

class Domain;

class PATransferFunctions {
public:
  PATransferFunctions();
  void setLabeler(Labeler* labeler) { _labeler=labeler; }
  Labeler* getLabeler() { return _labeler; }
  void setDomain(Domain* domain) { _domain=domain; }
  Domain* getDomain() { return _domain; }
  void setVariableIdMapping(VariableIdMapping* v) { _variableIdMapping=v; }
  VariableIdMapping* getVariableIdMapping() { return _variableIdMapping; }
  virtual void transfer(Label lab, Lattice& element);
  virtual void transferExpression(Label label, SgExpression* expr, Lattice& element);
  virtual void transferDeclaration(Label label, SgVariableDeclaration* decl, Lattice& element);
  virtual void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  virtual void transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, Lattice& element);
  virtual void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  virtual void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
protected:
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
  Domain* _domain;
};

#include "DFTransferFunctions.C"

#endif
