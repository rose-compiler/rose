#ifndef IntervalTRANSFERFUNCTIONS_HPP
#define IntervalTRANSFERFUNCTIONS_HPP

#include "DFTransferFunctions.h"
#include "IntervalPropertyState.h"
#include "CppExprEvaluator.h"

namespace CodeThorn {

class IntervalTransferFunctions : public DFTransferFunctions {
public:
  IntervalTransferFunctions();
  ~IntervalTransferFunctions();
  //  IntervalTransferFunctions(NumberIntervalLattice* domain, PropertyState* p, CodeThorn::Labeler* l, VariableIdMapping* vid);
  NumberIntervalLattice evalExpression(CodeThorn::Label label, SgExpression* expr, Lattice& element);
  void transferCondition(CodeThorn::Edge edge, Lattice& element);
  void transferExpression(CodeThorn::Label label, SgExpression* expr, Lattice& element);
  void transferDeclaration(CodeThorn::Label label, SgVariableDeclaration* decl, Lattice& element);
  void transferSwitchCase(CodeThorn::Label lab,SgStatement* condStmt, SgCaseOptionStmt* caseStmt,Lattice& pstate);
  void transferReturnStmtExpr(CodeThorn::Label label, SgExpression* expr, Lattice& element);
  void transferFunctionCall(CodeThorn::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  void transferFunctionCallReturn(CodeThorn::Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element);
  void transferFunctionEntry(CodeThorn::Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  void transferFunctionExit(CodeThorn::Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
  void setPropertyState(PropertyState* p) { _cppExprEvaluator->setPropertyState(p); }
  //private:
  CppExprEvaluator* getCppExprEvaluator();
  void setCppExprEvaluator(CodeThorn::CppExprEvaluator* expEval);
  void setSkipSelectedFunctionCalls(bool);
  void initializeExtremalValue(Lattice& element);

private:
  CodeThorn::CppExprEvaluator* _cppExprEvaluator;
  NumberIntervalLattice* _domain;
};

} // end of namespace CodeThorn
#endif
