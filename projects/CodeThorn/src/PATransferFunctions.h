#ifndef PATRANSFERFUNCTIONS_H
#define PATRANSFERFUNCTIONS_H

#include "Labeler.h"
#include "Lattice.h"
#include "PointerAnalysisInterface.h"

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

  // allow for some pointer analysis to be used directly
  void setPointerAnalysis(SPRAY::PointerAnalysisInterface* pointerAnalysisInterface) { _pointerAnalysisInterface=pointerAnalysisInterface; }
  SPRAY::PointerAnalysisInterface* getPointerAnalysisInterface() { return _pointerAnalysisInterface; }

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
  SPRAY::PointerAnalysisInterface* _pointerAnalysisInterface;
};

#endif
