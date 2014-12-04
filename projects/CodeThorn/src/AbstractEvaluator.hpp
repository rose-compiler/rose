#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <iostream>
using namespace std;

#include "AstProcessing.h"
#include "SgNodeHelper.h"
#include "Domain.hpp"
#include "VariableIdMapping.h"
#include "PropertyState.h"
#include "NumberIntervalLattice.h"

class CppExprEvaluator {
 public:
  CppExprEvaluator(NumberIntervalLattice* d, PropertyState* p, VariableIdMapping* vim):domain(d), propertyState(p),variableIdMapping(vim){}
  NumberIntervalLattice evaluate(SgNode* node) {
    if(isSgBinaryOp(node)) {
      SgNode* lhs=SgNodeHelper::getLhs(node);
      SgNode* rhs=SgNodeHelper::getRhs(node);
      switch(node->variantT()) {
      case V_SgAddOp:  return domain->arithAdd(evaluate(lhs),evaluate(rhs));
      case V_SgSubtractOp: return domain->arithSub(evaluate(lhs),evaluate(rhs));
      case V_SgMultiplyOp: return domain->arithMul(evaluate(lhs),evaluate(rhs));
      case V_SgDivideOp: return domain->arithDiv(evaluate(lhs),evaluate(rhs));
      case V_SgModOp: return domain->arithMod(evaluate(lhs),evaluate(rhs));
      case V_SgAssignOp: {cout<<"WARNING: Eval: Assignment (not implemented)."<<endl;return evaluate(rhs);}
      default:
	cerr<<"Error: unknown binary operator."<<endl;
	exit(1);
      }
    }
    switch(node->variantT()) {
    case V_SgIntVal: return NumberIntervalLattice(Number(isSgIntVal(node)->get_value()));
    case V_SgMinusOp: return domain->arithSub(NumberIntervalLattice(Number(0)),evaluate(SgNodeHelper::getFirstChild(node)));
    case V_SgVarRefExp: {
      cout<<"WARNING: Eval: VarRefExp (not implemented)."<<endl;
      return NumberIntervalLattice();
    }
    default: // generates bot element
	cerr<<"Error: unknown unary operator."<<endl;
	exit(1);
    }
    cerr<<"Error: Unknown operator."<<endl;
    exit(1);
  }
  void setDomain(NumberIntervalLattice* domain) { this->domain=domain; }
  void setPropertyState(PropertyState* pstate) { this->propertyState=pstate; }
  void setVariableIdMapping(VariableIdMapping* variableIdMapping) { this->variableIdMapping=variableIdMapping; }
private:
  NumberIntervalLattice* domain;
  PropertyState* propertyState;
  VariableIdMapping* variableIdMapping;
};

#if 0
template<AbstractLValue, AbstractRValue, PropertyState, Domain>
class AbstractEvaluator {
  void setDomain(Domain* domain) { _domain=domain; }
  virtual AbstractRValue evaluateRValueExpression(SgExpression* node, PropertyState* pstate) { return defaultRValue(); }
  virtual AbstractLValue evaluateLValueExpression(SgExpression* node, PropertyState* pstate) { return defaultLValue(); }
  virtual defautRValue() { return AbstractRValue(); }
  virtual defautLValue() { return AbstractLValue(); }
};
#endif

#endif
