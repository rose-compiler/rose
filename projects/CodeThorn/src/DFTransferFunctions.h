#ifndef PATRANSFERFUNCTIONS_H
#define PATRANSFERFUNCTIONS_H

#include "Labeler.h"
#include "Lattice.h"
#include "PointerAnalysisInterface.h"

//class Domain;

//#include "Domain.hpp"

class DFTransferFunctions {
public:
  DFTransferFunctions();
  void setLabeler(SPRAY::Labeler* labeler) { _labeler=labeler; }
  SPRAY::Labeler* getLabeler() { return _labeler; }
  //void setDomain(Domain* domain) { _domain=domain; }
  //Domain* getDomain() { return _domain; }
  void setVariableIdMapping(VariableIdMapping* v) { _variableIdMapping=v; }
  VariableIdMapping* getVariableIdMapping() { return _variableIdMapping; }

  // allow for some pointer analysis to be used directly
  void setPointerAnalysis(SPRAY::PointerAnalysisInterface* pointerAnalysisInterface) { _pointerAnalysisInterface=pointerAnalysisInterface; }
  SPRAY::PointerAnalysisInterface* getPointerAnalysisInterface() { return _pointerAnalysisInterface; }

  virtual void transfer(SPRAY::Label lab, Lattice& element);
  virtual void transferExpression(SPRAY::Label label, SgExpression* expr, Lattice& element);
  virtual void transferDeclaration(SPRAY::Label label, SgVariableDeclaration* decl, Lattice& element);
  virtual void transferFunctionCall(SPRAY::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  virtual void transferFunctionCallReturn(SPRAY::Label lab, SgFunctionCallExp* callExp, Lattice& element);
  virtual void transferFunctionEntry(SPRAY::Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  virtual void transferFunctionExit(SPRAY::Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
  virtual ~DFTransferFunctions() {}
  //protected:
 public:
  SPRAY::Labeler* _labeler;
  SPRAY::VariableIdMapping* _variableIdMapping;
  //Domain* _domain;
  SPRAY::PointerAnalysisInterface* _pointerAnalysisInterface;
};

#endif
