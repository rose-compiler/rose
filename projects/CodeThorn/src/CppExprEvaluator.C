#ifndef CPPEXPREVALUATOR_H
#define EVALUATOR_H

#include <iostream>

#include "sage3basic.h"
#include "CppExprEvaluator.h"
#include "IntervalPropertyState.h"
#include "SgNodeHelper.h"

using namespace std;

SPRAY::CppExprEvaluator::CppExprEvaluator(SPRAY::NumberIntervalLattice* d, SPRAY::VariableIdMapping* vim) :
  domain(d),
  variableIdMapping(vim),
  propertyState(0),
  _showWarnings(false),
  _pointerAnalysisInterface(0)
{
}

void SPRAY::CppExprEvaluator::setPointerAnalysis(SPRAY::PointerAnalysisInterface* pointerAnalysisInterface) {
  _pointerAnalysisInterface=pointerAnalysisInterface;
}

SPRAY::NumberIntervalLattice SPRAY::CppExprEvaluator::evaluate(SgNode* node, PropertyState* pstate) {
  setPropertyState(pstate);
  return evaluate(node);
}
  
SPRAY::NumberIntervalLattice SPRAY::CppExprEvaluator::evaluate(SgNode* node) {
  ROSE_ASSERT(domain);
  ROSE_ASSERT(propertyState);
  ROSE_ASSERT(variableIdMapping);
  IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(propertyState);
  if(!ips) {
    cerr<<"Error: CppExprEvaluator:: Unsupported type of property state."<<endl;
    exit(1);
  }
  
  if(isSgBinaryOp(node)) {
    SgNode* lhs=SgNodeHelper::getLhs(node);
    SgNode* rhs=SgNodeHelper::getRhs(node);
    switch(node->variantT()) {
    case V_SgEqualityOp:
      return domain->isEqualInterval(evaluate(lhs),evaluate(rhs));
    case V_SgAddOp:  return domain->arithAdd(evaluate(lhs),evaluate(rhs));
    case V_SgSubtractOp: return domain->arithSub(evaluate(lhs),evaluate(rhs));
    case V_SgMultiplyOp: return domain->arithMul(evaluate(lhs),evaluate(rhs));
    case V_SgDivideOp: return domain->arithDiv(evaluate(lhs),evaluate(rhs));
    case V_SgModOp: return domain->arithMod(evaluate(lhs),evaluate(rhs));
    case V_SgPntrArrRefExp:
      return NumberIntervalLattice::top();
    case V_SgAssignOp: {
      if(isSgPointerDerefExp(lhs)) {
        VariableIdSet varIdSet=_pointerAnalysisInterface->getModByPointer();
        NumberIntervalLattice rhsResult=evaluate(rhs);
        // TODO: more precise: merge each interval of the lhs memloc-variable(s) with the interval of rhsResult
        ips->topifyVariableSet(varIdSet);
        return rhsResult;
      }
      if(SgVarRefExp* lhsVar=isSgVarRefExp(lhs)) {
        ROSE_ASSERT(variableIdMapping);
        //variableIdMapping->toStream(cout);
        VariableId varId=variableIdMapping->variableId(lhsVar);
        NumberIntervalLattice rhsResult=evaluate(rhs);
        ips->setVariable(varId,rhsResult);
        return rhsResult;
      } else {
        // TODO: handle *var and update modByPointer() ...
        if(_showWarnings)
          cout<<"Warning: unknown lhs of assignment: "<<lhs->unparseToString()<<" ... setting all variables to unbounded interval and using unbounded result interval."<<endl;
        ips->topifyAllVariables();
        return NumberIntervalLattice::top();
      }
    }
    default:
      if(_showWarnings) cout<<"Warning: unknown binary operator: "<<node->sage_class_name()<<" ... setting all variables to unbounded interval and using unbounded result interval."<<endl;
      ips->topifyAllVariables();
      return NumberIntervalLattice::top();
    }
  }
  if(isSgUnaryOp(node)) {
    SgNode* operand=SgNodeHelper::getFirstChild(node);
    switch(node->variantT()) {
    case V_SgMinusOp: {
      return domain->arithSub(NumberIntervalLattice(Number(0)),evaluate(operand));
    }
    case V_SgAddressOfOp:
    case V_SgPointerDerefExp:
      // discard result as pointer value intervals are not represented in this domain, but evaluate to ensure all side-effects are represented in the state
      evaluate(operand);
      return NumberIntervalLattice::top();
    case V_SgMinusMinusOp:
    case V_SgPlusPlusOp: {
      int incdecVal=0;
      if(isSgPlusPlusOp(node)) {
        incdecVal=1;
      } else if(isSgMinusMinusOp(node)) {
          incdecVal=-1;
      } else {
        cerr<<"Error: CppExprEvaluator: unknown operator in ++/-- computation:"<<node->sage_class_name()<<endl;
        exit(1);
      }
      SgVarRefExp* varRefExp=isSgVarRefExp(SgNodeHelper::getFirstChild(node));
      if(varRefExp) {
        VariableId varId=variableIdMapping->variableId(varRefExp);
        if(SgNodeHelper::isPrefixIncDecOp(node)) {
          IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(propertyState);
          NumberIntervalLattice res=domain->arithAdd(NumberIntervalLattice(Number(1)),evaluate(SgNodeHelper::getFirstChild(node)));
          ips->setVariable(varId,res);
          return res;
        }
        if(SgNodeHelper::isPostfixIncDecOp(node)) {
          IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(propertyState);
          NumberIntervalLattice res=domain->arithAdd(NumberIntervalLattice(Number(1)),evaluate(SgNodeHelper::getFirstChild(node)));
          if(isExprRootNode(node)) {
            ips->setVariable(varId,res);
            return res;
          } else {
            SgNode* exprRootNode=findExprRootNode(node);
            cerr<<"Error: CppExprEvaluator: post-fix operator ++ not supported in sub-expressions yet: expression: "<<"\""<<(exprRootNode?exprRootNode->unparseToString():0)<<"\""<<endl;
            exit(1);
          }
        }
      } else {
        cerr<<"Error: CppExprEvaluator: ++/-- operation on lhs-expression not supported yet."<<endl;
        exit(1);
      }
    }
    default: // generates top element
      if(_showWarnings) cout<<"Warning: unknown unary operator: "<<node->sage_class_name()<<" ... setting all variables to unbounded interval and using unbounded result interval."<<endl;
      ips->topifyAllVariables();
      return NumberIntervalLattice::top();
    }
  }
  switch(node->variantT()) {
  case V_SgIntVal: return NumberIntervalLattice(Number(isSgIntVal(node)->get_value()));
  case V_SgVarRefExp: {
    SgVarRefExp* varRefExp=isSgVarRefExp(node);
    ROSE_ASSERT(varRefExp);
    VariableId varId=variableIdMapping->variableId(varRefExp);
    
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
    if(_showWarnings) cout<<"Warning: unknown leaf node: "<<node->sage_class_name()<<" ... using unbounded result interval."<<endl;
    return NumberIntervalLattice::top();
  }
  if(_showWarnings) cout<<"Warning: Unknown operator."<<node->sage_class_name()<<" ... setting all variables to unbounded interval and using unbounded result interval."<<endl;
  // an unknown operator may have an arbitrary effect, to err on the safe side we topify all variables
  ips->topifyAllVariables();
  return NumberIntervalLattice::top();
}

void SPRAY::CppExprEvaluator::setDomain(NumberIntervalLattice* domain) {
  this->domain=domain;
}

void SPRAY::CppExprEvaluator::setPropertyState(PropertyState* pstate) {
  this->propertyState=pstate;
}

void SPRAY::CppExprEvaluator::setVariableIdMapping(VariableIdMapping* variableIdMapping) { 
  this->variableIdMapping=variableIdMapping;
}

bool SPRAY::CppExprEvaluator::isValid() {
  return domain!=0 && propertyState!=0 && variableIdMapping!=0;
}

void SPRAY::CppExprEvaluator::setShowWarnings(bool warnings) {
  _showWarnings=warnings;
}


bool SPRAY::CppExprEvaluator::isExprRootNode(SgNode* node) {
  return (isSgExpression(node) && !isSgExpression(node->get_parent()));
}

SgNode* SPRAY::CppExprEvaluator::findExprRootNode(SgNode* node) {
  if(isSgExpression(node)&&!isExprRootNode(node)) {
    return findExprRootNode(node->get_parent());
  } else if(isExprRootNode(node)) {
    return node;
  } else {
    return 0;
  }
}
#endif
