#ifndef IntervalTRANSFERFUNCTIONS_HPP
#define IntervalTRANSFERFUNCTIONS_HPP

#include "DFTransferFunctions.h"
#include "IntervalPropertyState.h"
#include "AbstractEvaluator.h"

using namespace std;

class IntervalTransferFunctions : public DFTransferFunctions {
public:
  IntervalTransferFunctions();
  ~IntervalTransferFunctions();
  IntervalTransferFunctions(NumberIntervalLattice* domain, PropertyState* p, SPRAY::Labeler* l, VariableIdMapping* vid);
  void transferExpression(SPRAY::Label label, SgExpression* expr, IntervalPropertyState& element);
  void transferDeclaration(SPRAY::Label label, SgVariableDeclaration* decl, IntervalPropertyState& element);
  void transferFunctionCall(SPRAY::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, IntervalPropertyState& element);
  void transferFunctionCallReturn(SPRAY::Label lab, SgFunctionCallExp* callExp, IntervalPropertyState& element);
  void transferFunctionEntry(SPRAY::Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, IntervalPropertyState& element);
  void transferFunctionExit(SPRAY::Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, IntervalPropertyState& element);
private:
  CppExprEvaluator* _cppExprEvaluator;
  NumberIntervalLattice* _domain;
  SPRAY::Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
};

#endif
