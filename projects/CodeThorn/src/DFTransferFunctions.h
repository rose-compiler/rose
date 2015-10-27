#ifndef PATRANSFERFUNCTIONS_H
#define PATRANSFERFUNCTIONS_H

#include "Labeler.h"
#include "Lattice.h"
#include "Flow.h"
#include "PointerAnalysisInterface.h"
#include "ProgramAbstractionLayer.h"

namespace SPRAY {

class DFTransferFunctions {
public:
  DFTransferFunctions();
  SPRAY::Labeler* getLabeler() { return _programAbstractionLayer->getLabeler(); }
  VariableIdMapping* getVariableIdMapping() { return _programAbstractionLayer->getVariableIdMapping(); }
  void setProgramAbstractionLayer(SPRAY::ProgramAbstractionLayer* pal) {_programAbstractionLayer=pal; }
  // allow for some pointer analysis to be used directly
  void setPointerAnalysis(SPRAY::PointerAnalysisInterface* pointerAnalysisInterface) { _pointerAnalysisInterface=pointerAnalysisInterface; }
  SPRAY::PointerAnalysisInterface* getPointerAnalysisInterface() { return _pointerAnalysisInterface; }

  virtual void transfer(SPRAY::Edge edge, Lattice& element);
  virtual void transferCondition(Edge edge, Lattice& element);

  virtual void transfer(SPRAY::Label lab, Lattice& element);
  virtual void transferExpression(SPRAY::Label label, SgExpression* expr, Lattice& element);
  virtual void transferEmptyStmt(SPRAY::Label label, SgStatement* stmt, Lattice& element);
  virtual void transferDeclaration(SPRAY::Label label, SgVariableDeclaration* decl, Lattice& element);
  virtual void transferReturnStmtExpr(SPRAY::Label label, SgExpression* expr, Lattice& element);
  virtual void transferSwitchCase(SPRAY::Label lab,SgStatement* condStmt, SgCaseOptionStmt* caseStmt,Lattice& pstate);
  virtual void transferSwitchDefault(SPRAY::Label lab,SgStatement* condStmt, SgDefaultOptionStmt* defaultStmt,Lattice& pstate);
  virtual void transferFunctionCall(SPRAY::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  virtual void transferFunctionCallReturn(SPRAY::Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element);
  virtual void transferFunctionEntry(SPRAY::Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  virtual void transferFunctionExit(SPRAY::Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
  virtual ~DFTransferFunctions() {}
  //protected:
 public:
  SPRAY::PointerAnalysisInterface* _pointerAnalysisInterface;

 public:
  virtual void addParameterPassingVariables();
  VariableId getParameterVariableId(int paramNr);
  VariableId getResultVariableId();
 private:
  SPRAY::ProgramAbstractionLayer* _programAbstractionLayer;
  VariableId parameter0VariableId;
  VariableId resultVariableId;
};

}

#endif
