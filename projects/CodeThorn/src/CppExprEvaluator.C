#ifndef CPPEXPREVALUATOR_H
#define EVALUATOR_H

#include <iostream>
using namespace std;

#include "sage3basic.h"
#include "CppExprEvaluator.h"
#include "IntervalPropertyState.h"

CppExprEvaluator::CppExprEvaluator(NumberIntervalLattice* d, SPRAY::VariableIdMapping* vim):domain(d),variableIdMapping(vim),propertyState(0){
}
  
NumberIntervalLattice CppExprEvaluator::evaluate(SgNode* node) {
  
  ROSE_ASSERT(domain);
  ROSE_ASSERT(propertyState);
  ROSE_ASSERT(variableIdMapping);
  
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
        ROSE_ASSERT(variableIdMapping);
        //variableIdMapping->toStream(cout);
        VariableId varId=variableIdMapping->variableId(lhsVar);
        if(IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(propertyState)) {
          NumberIntervalLattice rhsResult=evaluate(rhs);
          ips->setVariable(varId,rhsResult);
          return rhsResult;
        } else {
          cerr<<"Error: CppExprEvaluator:: Unknown type of property state."<<endl;
          exit(1);
        }
      } else {
        cout<<"Warning: unknown lhs of assignment: "<<lhs->unparseToString()<<endl;
        return NumberIntervalLattice::top();
      }
    }
    default:
      cout<<"Warning: unknown binary operator: "<<node->sage_class_name()<<" ... using unbounded interval."<<endl;
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
    NumberIntervalLattice evalResult=ips->getVariable(varId);
    return evalResult;
  }
  case V_SgBoolValExp: {
    SgBoolValExp* boolValExp=isSgBoolValExp(node);
    ROSE_ASSERT(boolValExp);
    int boolVal= boolValExp->get_value();
    if(boolVal==0) {
      return NumberIntervalLattice(0,0);
    }
    if(boolVal==1) {
      return NumberIntervalLattice(1,1);
    }
  }
  default: // generates top element
    cout<<"Warning: unknown unary operator: "<<node->sage_class_name()<<" ... using unbounded interval."<<endl;
    return NumberIntervalLattice::top();
  }
  cout<<"Warning: Unknown operator."<<node->sage_class_name()<<" ... using unbounded interval."<<endl;
  return NumberIntervalLattice::top();
}

void CppExprEvaluator::setDomain(NumberIntervalLattice* domain) {
  this->domain=domain;
}

void CppExprEvaluator::setPropertyState(PropertyState* pstate) {
  this->propertyState=pstate;
}

void CppExprEvaluator::setVariableIdMapping(VariableIdMapping* variableIdMapping) { 
  this->variableIdMapping=variableIdMapping;
}

bool CppExprEvaluator::isValid() {
  return domain!=0 && propertyState!=0 && variableIdMapping!=0;
}

#endif
