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
  void transferExpression(SPRAY::Label label, SgExpression* expr, Lattice& element);
  void transferDeclaration(SPRAY::Label label, SgVariableDeclaration* decl, Lattice& element);
  void transferFunctionCall(SPRAY::Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element);
  void transferFunctionCallReturn(SPRAY::Label lab, SgFunctionCallExp* callExp, Lattice& element);
  void transferFunctionEntry(SPRAY::Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element);
  void transferFunctionExit(SPRAY::Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element);
  CppExprEvaluator* _cppExprEvaluator;
private:
  NumberIntervalLattice* _domain;
  SPRAY::Labeler* _labeler;
  VariableIdMapping* _variableIdMapping;
  void setPropertyState(PropertyState* p) { _cppExprEvaluator->setPropertyState(p); }
};

#endif
