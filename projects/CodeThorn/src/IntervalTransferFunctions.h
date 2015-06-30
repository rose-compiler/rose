#ifndef IntervalTRANSFERFUNCTIONS_HPP
#define IntervalTRANSFERFUNCTIONS_HPP

#include "DFTransferFunctions.h"
#include "IntervalPropertyState.h"
#include "CppExprEvaluator.h"

namespace SPRAY {

class IntervalTransferFunctions : public DFTransferFunctions {
public:
  IntervalTransferFunctions();
  ~IntervalTransferFunctions();
  //  IntervalTransferFunctions(NumberIntervalLattice* domain, PropertyState* p, SPRAY::Labeler* l, VariableIdMapping* vid);
  void transferExpression(SPRAY::Label label, SgExpression* expr, Lattice& element);
  void transferDeclaration(SPRAY::Label label, SgVariableDeclaration* decl, Lattice& element);
  void transferSwitchCase(SPRAY::Label lab,SgStatement* condStmt, SgCaseOptionStmt* caseStmt,Lattice& pstate);
  void transferFunctionCall(SPRAY::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  void transferFunctionCallReturn(SPRAY::Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element);
  void transferFunctionEntry(SPRAY::Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  void transferFunctionExit(SPRAY::Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
  void setPropertyState(PropertyState* p) { _cppExprEvaluator->setPropertyState(p); }
  //private:
  CppExprEvaluator* getCppExprEvaluator();
  void setCppExprEvaluator(SPRAY::CppExprEvaluator* expEval);
private:
  SPRAY::CppExprEvaluator* _cppExprEvaluator;
  NumberIntervalLattice* _domain;
};

} // end of namespace SPRAY
#endif
