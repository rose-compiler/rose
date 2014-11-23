#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "AstProcessing.h"
#include "SgNodeHelper.h"
#include <iostream>

using namespace std;

class CppExprEvaluator {
 public:
  CppExprEvaluator(Domain* d, PropertyState* p, VariableIdMapping* vim):domain(d), propertyState(p),variableIdMapping(vim){}
  Lattice evaluate(SgNode* node) {
    if(isBinaryOp(node)) {
      SgNode* lhs=SgNodeHelper::getLhs(node);
      SgNode* rhs=SgNodeHelper::getRhs(node);
      switch(node) {
      case V_SgAddOp:  return domain->add(evaluate(lhs,rhs));
      case V_SgSubtractOp: return domain->sub(lhs,rhs);
      case V_SgMultiplyOp: return domain->mul(lhs,rhs);
      case V_SgDivideOp: return domain->div(lhs,rhs);
      case V_SgModOp: return domain->mod(lhs,rhs);
      default:
	cerr<<"Unknown binary operator."<<endl;
	exit(1);
      }
    }
    switch(node->variantT()) {
    case V_SgIntVal: return domain->intValue(isSgIntVal(node)->get_value());
    case V_SgMinusOp: return domain->neg(SgNodeHelper::getChild());
    case V_SgVarRefExp: {return propertyState->getVarValue(variableIdMapping->variableId(node));}
    case V_SgAssignOp: {cout<<"Eval: Assignment."<<endl;return Lattice();}
    default:
      return Lattice();
    }
    return Lattice();
  }
  void setDomain(Domain* domain) { this->domain=domain; }
  void setPropertyState(PropertyState* pstate) { this->propertyState=pstate; }
  void setVariableIdMapping(VariableIdMapping* variableIdMapping) { this->variableIdMapping=variableIdMapping; }
private:
  Domain* domain;
  PropertyState* propertyState;
  VariableIdMapping* variableIdMapping;
};

template<AbstractLValue, AbstractRValue, PropertyState, Domain>
class AbstractEvaluator {
  void setDomain(Domain* domain) { _domain=domain; }
  virtual AbstractRValue evaluateRValueExpression(SgExpression* node, PropertyState* pstate) { return defaultRValue(); }
  virtual AbstractLValue evaluateLValueExpression(SgExpression* node, PropertyState* pstate) { return defaultLValue(); }
  virtual defautRValue() { return AbstractRValue(); }
  virtual defautLValue() { return AbstractLValue(); }
};

#endif
