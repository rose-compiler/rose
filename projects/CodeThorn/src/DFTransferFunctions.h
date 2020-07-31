#ifndef PATRANSFERFUNCTIONS_H
#define PATRANSFERFUNCTIONS_H

#include "Labeler.h"
#include "Lattice.h"
#include "Flow.h"
#include "PointerAnalysisInterface.h"
#include "ProgramAbstractionLayer.h"
#include "PropertyState.h"
#include "PropertyStateFactory.h"

namespace CodeThorn {

class DFTransferFunctions {
public:
  DFTransferFunctions();
  CodeThorn::Labeler* getLabeler() { return _programAbstractionLayer->getLabeler(); }
  VariableIdMapping* getVariableIdMapping() { return _programAbstractionLayer->getVariableIdMapping(); }
  virtual void setProgramAbstractionLayer(CodeThorn::ProgramAbstractionLayer* pal) {_programAbstractionLayer=pal; }
  // allow for some pointer analysis to be used directly
  virtual void setPointerAnalysis(CodeThorn::PointerAnalysisInterface* pointerAnalysisInterface) { _pointerAnalysisInterface=pointerAnalysisInterface; }
  CodeThorn::PointerAnalysisInterface* getPointerAnalysisInterface() { return _pointerAnalysisInterface; }

  // this is the main transfer function
  virtual void transfer(CodeThorn::Edge edge, Lattice& element);
  virtual void transferCondition(Edge edge, Lattice& element);

  // called for those nodes that are not conditionals and selection on edge annotations is not necessary
  virtual void transfer(CodeThorn::Label lab, Lattice& element);

  virtual void transferExpression(CodeThorn::Label label, SgExpression* expr, Lattice& element);
  virtual void transferEmptyStmt(CodeThorn::Label label, SgStatement* stmt, Lattice& element);
  virtual void transferDeclaration(CodeThorn::Label label, SgVariableDeclaration* decl, Lattice& element);
  virtual void transferReturnStmtExpr(CodeThorn::Label label, SgExpression* expr, Lattice& element);
  virtual void transferSwitchCase(CodeThorn::Label lab,SgStatement* condStmt, SgCaseOptionStmt* caseStmt,Lattice& pstate);
  virtual void transferSwitchDefault(CodeThorn::Label lab,SgStatement* condStmt, SgDefaultOptionStmt* defaultStmt,Lattice& pstate);
  virtual void transferFunctionCall(CodeThorn::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  virtual void transferConstructorCall(CodeThorn::Label lab, SgConstructorInitializer* callCtor, SgExpressionPtrList& arguments, Lattice& element);
  virtual void transferExternalFunctionCall(CodeThorn::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  //~ virtual void transferFunctionCallReturn(CodeThorn::Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element);
  virtual void transferFunctionCallReturn(CodeThorn::Label lab, CodeThorn::VariableId varId, SgFunctionCallExp* callExp, Lattice& element);
  virtual void transferConstructorCallReturn(CodeThorn::Label lab, CodeThorn::VariableId varId, SgConstructorInitializer* callCtor, Lattice& element);
  virtual void transferFunctionEntry(CodeThorn::Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  virtual void transferFunctionExit(CodeThorn::Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
  virtual ~DFTransferFunctions() {}
  virtual void addParameterPassingVariables();
  VariableId getParameterVariableId(int paramNr);
  VariableId getResultVariableId();
  bool isExternalFunctionCall(Label l);
 public:
  CodeThorn::PointerAnalysisInterface* _pointerAnalysisInterface;
  virtual void setSkipUnknownFunctionCalls(bool flag);
  virtual bool getSkipUnknownFunctionCalls();
  virtual void initializeExtremalValue(Lattice& element);
  virtual Lattice* initializeGlobalVariables(SgProject* root);
  void setInitialElementFactory(PropertyStateFactory*);
  PropertyStateFactory* getInitialElementFactory();

 protected:
  bool _skipSelectedFunctionCalls=false;
  PropertyStateFactory* _initialElementFactory=nullptr;
 private:
  CodeThorn::ProgramAbstractionLayer* _programAbstractionLayer;
  VariableId parameter0VariableId;
  VariableId resultVariableId;
};

}

#endif
