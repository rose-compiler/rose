#ifndef IntervalTRANSFERFUNCTIONS_HPP
#define IntervalTRANSFERFUNCTIONS_HPP

#include "PATransferFunctions.h"
#include "IntervalPropertyState.h"
#include "AbstractEvaluator.h"

class IntervalTransferFunctions : public PATransferFunctions {
public:
  IntervalTransferFunctions();
  ~IntervalTransferFunctions();
  IntervalTransferFunctions(NumberIntervalLattice* domain, PropertyState* p, Labeler* l, VariableIdMapping* vid);
  void transferExpression(Label label, SgExpression* expr, IntervalPropertyState& element);
  void transferDeclaration(Label label, SgVariableDeclaration* decl, IntervalPropertyState& element);
  void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, IntervalPropertyState& element);
  void transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, IntervalPropertyState& element);
  void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, IntervalPropertyState& element);
  void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, IntervalPropertyState& element);
private:
  CppExprEvaluator* _cppExprEvaluator;
  NumberIntervalLattice* _domain;
  Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
};

#endif
