#ifndef IntervalTRANSFERFUNCTIONS_HPP
#define IntervalTRANSFERFUNCTIONS_HPP

#include "DFTransferFunctions.hpp"
#include "IntervalPropertyState.h"

class IntervalTransferFunctions : public DFTransferFunctions<IntervalPropertyState> {
public:
  IntervalTransferFunctions();
  ~IntervalTransferFunctions();
  IntervalTransferFunctions(Domain* domain, PropertyState* p, Labeler* l, VariableIdMapping vid);
  void transferExpression(Label label, SgExpression* expr, IntervalPropertyState& element);
  void transferDeclaration(Label label, SgVariableDeclaration* decl, IntervalPropertyState& element);
  void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, IntervalPropertyState& element);
  void transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, IntervalPropertyState& element);
  void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, IntervalPropertyState& element);
  void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, IntervalPropertyState& element);
};

#endif
