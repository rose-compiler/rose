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
    cout<<"DEBUG: eval @:"<<node->unparseToString()<<endl;
    if(isSgBinaryOp(node)) {
      SgNode* lhs=SgNodeHelper::getLhs(node);
      SgNode* rhs=SgNodeHelper::getRhs(node);
      switch(node->variantT()) {
      case V_SgAddOp:  return domain->arithAdd(evaluate(lhs),evaluate(rhs));
      case V_SgSubtractOp: return domain->arithSub(evaluate(lhs),evaluate(rhs));
      case V_SgMultiplyOp: return domain->arithMul(evaluate(lhs),evaluate(rhs));
      case V_SgDivideOp: return domain->arithDiv(evaluate(lhs),evaluate(rhs));
      case V_SgModOp: return domain->arithMod(evaluate(lhs),evaluate(rhs));
      case V_SgAssignOp: {
        if(SgVarRefExp* lhsVar=isSgVarRefExp(lhs)) {
          cout<<"DEBUG: EVAL: LHS-VAR: VID:"<<variableIdMapping<<endl;
#if 1
          ROSE_ASSERT(variableIdMapping);
          variableIdMapping->toStream(cout);
          VariableId varId=variableIdMapping->variableId(lhsVar);
          if(IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(propertyState)) {
            cout<<"DEBUG: eval: rhs:"<<rhs->unparseToString()<<endl;
            NumberIntervalLattice rhsResult=evaluate(rhs);
            cout<<"DEBUG: eval: rhs-result:"<<rhsResult.toString()<<endl;
            cout<<"DEBUG: eval: updating:"<<varId.toString()<<endl;

            cout<<"DEBUG: eval: before-update:"<<ips->intervals[varId].toString()<<endl;
            ips->intervals[varId]=rhsResult;
            cout<<"DEBUG: eval: after-update :"<<ips->intervals[varId].toString()<<endl;
            return rhsResult;
          } else {
            cerr<<"Error: CppExprEvaluator:: Unknown type of property state."<<endl;
            exit(1);
          }
#else
          cout<<"WARNING: not handling assignment.[vid:"<<variableIdMapping<<","<<"ps:"<<propertyState<<"]"<<endl;
#endif
        } else {
          cout<<"Warning: unknown lhs of assignment: "<<lhs->unparseToString()<<endl;
          return NumberIntervalLattice::top();
        }
      }
      default:
        cout<<"Warning: unknown binary operator."<<endl;
        return NumberIntervalLattice::top();
      }
    }
    switch(node->variantT()) {
    case V_SgIntVal: return NumberIntervalLattice(Number(isSgIntVal(node)->get_value()));
    case V_SgMinusOp: return domain->arithSub(NumberIntervalLattice(Number(0)),evaluate(SgNodeHelper::getFirstChild(node)));
    case V_SgVarRefExp: {
      SgVarRefExp* varRefExp=isSgVarRefExp(node);
      ROSE_ASSERT(varRefExp);
      VariableId varId=variableIdMapping->variableId(varRefExp);
      IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(propertyState);
      ROSE_ASSERT(ips);
      NumberIntervalLattice evalResult=ips->intervals[varId];
      return evalResult;
    }
    default: // generates top element
      cout<<"Warning: unknown unary operator."<<endl;
      return NumberIntervalLattice::top();
    }
    cerr<<"Warning: Unknown operator."<<endl;
    return NumberIntervalLattice::top();
  }
  void setDomain(NumberIntervalLattice* domain) { this->domain=domain; }
  void setPropertyState(PropertyState* pstate) { this->propertyState=pstate; }
  void setVariableIdMapping(VariableIdMapping* variableIdMapping) { 
    this->variableIdMapping=variableIdMapping;
  }
private:
  NumberIntervalLattice* domain;
  PropertyState* propertyState;

  //temporarily public
 public:
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
