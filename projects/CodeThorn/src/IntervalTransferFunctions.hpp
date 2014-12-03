#ifndef IntervalTRANSFERFUNCTIONS_HPP
#define IntervalTRANSFERFUNCTIONS_HPP

#include "DFTransferFunctions.hpp"
#include "IntervalLattice.h"
#include "AbstractEvaluator.hpp"

class IntervalTransferFunctions : public DFTransferFunctions<IntervalLattice<int> > {
public:
  IntervalTransferFunctions();
  ~IntervalTransferFunctions();
  IntervalTransferFunctions(Domain* domain, PropertyState* p, Labeler* l, VariableIdMapping vid);
  void transferExpression(Label label, SgExpression* expr, IntervalLattice& element);
  void transferDeclaration(Label label, SgVariableDeclaration* decl, IntervalLattice& element);
  void transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, IntervalLattice& element);
  void transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, IntervalLattice& element);
  void transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, IntervalLattice& element);
  void transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, IntervalLattice& element);
};

#endif
