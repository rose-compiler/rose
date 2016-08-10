#ifndef CPPEXPREVALUATOR_H
#define EVALUATOR_H

#include <iostream>

#include "sage3basic.h"
#include "CppExprEvaluator.h"
#include "IntervalPropertyState.h"
#include "SgNodeHelper.h"

using namespace std;

void SPRAY::CppExprEvaluator::setSoundness(bool s) {
  _sound=s;
}

SPRAY::CppExprEvaluator::CppExprEvaluator(SPRAY::NumberIntervalLattice* d, SPRAY::VariableIdMapping* vim) :
  domain(d),
  variableIdMapping(vim),
  propertyState(0),
  _showWarnings(false),
  _pointerAnalysisInterface(0),
  _sound(true)
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
    case V_SgDotExp:
    case V_SgArrowExp:
      evaluate(rhs);
      return NumberIntervalLattice::top();
    case V_SgEqualityOp:       return domain->isEqualInterval(evaluate(lhs),evaluate(rhs));
    case V_SgNotEqualOp:       return domain->isNotEqualInterval(evaluate(lhs),evaluate(rhs));

    case V_SgLessThanOp:       return domain->isSmallerInterval(evaluate(lhs),evaluate(rhs));
    case V_SgLessOrEqualOp:    return domain->isSmallerOrEqualInterval(evaluate(lhs),evaluate(rhs));
    case V_SgGreaterThanOp:    return domain->isGreaterInterval(evaluate(lhs),evaluate(rhs));
    case V_SgGreaterOrEqualOp: return domain->isGreaterOrEqualInterval(evaluate(lhs),evaluate(rhs));

    // schroder3 (2016-08-09): Short circuit logical && and ||
    case V_SgAndOp: {
      NumberIntervalLattice lhsResult = evaluate(lhs);
      // Short circuit evaluation:
      if(lhsResult.isFalse() || lhsResult.isBot()) {
        return lhsResult;
      }
      else {
        NumberIntervalLattice rhsResult = evaluate(rhs);
        if(lhsResult.isTop() && rhsResult.isBot()) {
          // Possible combinations: true && bot (= bot) or false && bot (= false, because rhs is not executed)
          //  Return false because bot joined with false is false:
          return NumberIntervalLattice(false);
        }
        else {
          // All other cases: Both sides are always executed: Use non-short-circuit &&:
          return domain->nonShortCircuitLogicalAndInterval(lhsResult, rhsResult);
        }
      }
    }
    case V_SgOrOp: {
      NumberIntervalLattice lhsResult = evaluate(lhs);
      // Short circuit evaluation:
      if(lhsResult.isTrue() || lhsResult.isBot()) {
        return lhsResult;
      }
      else {
        NumberIntervalLattice rhsResult = evaluate(rhs);
        if(lhsResult.isTop() && rhsResult.isBot()) {
          // Possible combinations: true || bot (= true, because rhs is not executed) or false || bot (= bot)
          //  Return true because bot joined with true is true:
          return NumberIntervalLattice(true);
        }
        else {
          // All other cases: Both sides are always executed: Use non-short-circuit ||:
          return domain->nonShortCircuitLogicalOrInterval(lhsResult, rhsResult);
        }
      }
    }

    case V_SgAddOp:            return domain->arithAdd(evaluate(lhs),evaluate(rhs));
    case V_SgSubtractOp:       return domain->arithSub(evaluate(lhs),evaluate(rhs));
    case V_SgMultiplyOp:       return domain->arithMul(evaluate(lhs),evaluate(rhs));
    case V_SgDivideOp:         return domain->arithDiv(evaluate(lhs),evaluate(rhs));
    case V_SgModOp:            return domain->arithMod(evaluate(lhs),evaluate(rhs));

    case V_SgLshiftOp:         return domain->bitwiseShiftLeft(evaluate(lhs),evaluate(rhs));
    case V_SgRshiftOp:         return domain->bitwiseShiftRight(evaluate(lhs),evaluate(rhs));
    case V_SgPntrArrRefExp:
      return NumberIntervalLattice::top();

    case V_SgPlusAssignOp:
    case V_SgMinusAssignOp:
    case V_SgMultAssignOp:
    case V_SgDivAssignOp:
    case V_SgModAssignOp:
    case V_SgLshiftAssignOp:
    case V_SgRshiftAssignOp:
    case V_SgAssignOp: {
      NumberIntervalLattice lhsResult = NumberIntervalLattice::top();
      VariableId lhsVarId;
      NumberIntervalLattice rhsResult=evaluate(rhs);

      // Determine the lhs result and topify if necessary:
      if(isSgPointerDerefExp(lhs)) {
        VariableIdSet varIdSet=_pointerAnalysisInterface->getModByPointer();
        // TODO: more precise: merge each interval of the lhs memloc-variable(s) with the interval of rhsResult
        ips->topifyVariableSet(varIdSet);
      }
      if(SgVarRefExp* lhsVar=isSgVarRefExp(lhs)) {
        ROSE_ASSERT(variableIdMapping);
        //variableIdMapping->toStream(cout);
        lhsVarId = variableIdMapping->variableId(lhsVar);
        if(variableIdMapping->hasReferenceType(lhsVarId)) {
          // schroder3 (2016-07-05):
          //  We change a reference and we do not know which variable the reference refers to.
          //  ==> Set all variables from which the address was taken (this includes variables from which
          //  an alias/reference was created) to top:
          VariableIdSet varIdSet=_pointerAnalysisInterface->getModByPointer();
          ips->topifyVariableSet(varIdSet);
        }
        lhsResult = ips->getVariable(lhsVarId);
      } else {
        //        if(_showWarnings)
          //          cout<<"Warning: unknown lhs of assignment: "<<lhs->unparseToString()<<"("<<lhs->class_name()<<") ... setting all address-taken variables to unbounded interval and using rhs interval."<<endl;
        VariableIdSet varIdSet=_pointerAnalysisInterface->getModByPointer();
        ips->topifyVariableSet(varIdSet);
      }

      // Compute the operator result:
      NumberIntervalLattice operatorResult;
      switch(node->variantT()) {
        case V_SgAssignOp:
          operatorResult = rhsResult;
          break;
        case V_SgPlusAssignOp:
          operatorResult = domain->arithAdd(lhsResult, rhsResult);
          break;
        case V_SgMinusAssignOp:
          operatorResult = domain->arithSub(lhsResult, rhsResult);
          break;
        case V_SgMultAssignOp:
          operatorResult = domain->arithMul(lhsResult, rhsResult);
          break;
        case V_SgDivAssignOp:
          operatorResult = domain->arithDiv(lhsResult, rhsResult);
          break;
        case V_SgModAssignOp:
          operatorResult = domain->arithMod(lhsResult, rhsResult);
          break;
        case V_SgLshiftAssignOp:
          operatorResult = domain->bitwiseShiftLeft(lhsResult, rhsResult);
          break;
        case V_SgRshiftAssignOp:
          operatorResult = domain->bitwiseShiftRight(lhsResult, rhsResult);
          break;
        default:
          throw SPRAY::Exception("CppExprEvaluator::evaluate: internal error: unsupported case.");
      }

      // Set lhs variable to the operator result if possible:
      if(lhsVarId.isValid()) {
        ips->setVariable(lhsVarId, operatorResult);
      }

      return operatorResult;
    }
    default:
      if(_sound) {
        if(_showWarnings) cout<<"Warning: unknown binary operator: "<<node->sage_class_name()<<" ... setting all variables to unbounded interval and using unbounded result interval."<<endl;
        ips->topifyAllVariables();
      }
      else if(_showWarnings) {
        cout<<"Warning: unknown binary operator: "<<node->sage_class_name()<<"("<<node->unparseToString()<<") ... using unbounded result interval."<<endl;
      }
      return NumberIntervalLattice::top();
    }
  }
  if(isSgUnaryOp(node)) {
    SgNode* operand=SgNodeHelper::getFirstChild(node);
    switch(node->variantT()) {
    case V_SgMinusOp: {
      return domain->arithSub(NumberIntervalLattice(Number(0)),evaluate(operand));
    }
    case V_SgNotOp: {
      return domain->logicalNotInterval(evaluate(operand));
    }
    case V_SgAddressOfOp:
    case V_SgPointerDerefExp:
      // discard result as pointer value intervals are not represented in this domain, but evaluate to ensure all side-effects are represented in the state
      evaluate(operand);
      return NumberIntervalLattice::top();
    case V_SgCastExp: return evaluate(operand);
    case V_SgMinusMinusOp:
    case V_SgPlusPlusOp: {
      SgVarRefExp* varRefExp=isSgVarRefExp(SgNodeHelper::getFirstChild(node));
      if(varRefExp) {
        VariableId varId=variableIdMapping->variableId(varRefExp);
        IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(propertyState);
        Number plusOrMinusOne = (isSgMinusMinusOp(node) ? -1 : (isSgPlusPlusOp(node) ? 1 : (ROSE_ASSERT(false), 0)));
        NumberIntervalLattice res=domain->arithAdd(evaluate(operand), plusOrMinusOne);
        if(variableIdMapping->hasReferenceType(varId)) {
          // schroder3 (2016-07-05):
          //  We change a reference and we do not know which variable the reference refers to.
          //  ==> Set all variables from which the address was taken (this includes variables from which
          //  an alias/reference was created) to top:
          VariableIdSet varIdSet=_pointerAnalysisInterface->getModByPointer();
          ips->topifyVariableSet(varIdSet);
        }
        else {
          if(SgNodeHelper::isPrefixIncDecOp(node)) {
            ips->setVariable(varId,res);
          }
          if(SgNodeHelper::isPostfixIncDecOp(node)) {
            if(isExprRootNode(node)) {
              ips->setVariable(varId,res);
            } else {
              SgNode* exprRootNode=findExprRootNode(node);
              cerr<<"Error: CppExprEvaluator: post-fix operator ++ not supported in sub-expressions yet: expression: "<<"\""<<(exprRootNode?exprRootNode->unparseToString():0)<<"\""<<endl;
              exit(1);
            }
          }
        }
        return res;
      } else {
        cerr<<"Error: CppExprEvaluator: ++/-- operation on lhs-expression not supported yet."<<endl;
        exit(1);
      }
    }
    default: // generates top element
      if(_sound) {
        if(_showWarnings) cout<<"Warning: unknown unary operator: "<<node->sage_class_name()<<" ... setting all variables to unbounded interval and using unbounded result interval."<<endl;
        ips->topifyAllVariables();
      }
      else if(_showWarnings) {
        cout<<"Warning: unknown unary operator: "<<node->sage_class_name()<<"("<<node->unparseToString()<<") ... using unbounded result interval."<<endl;
      }
      return NumberIntervalLattice::top();
    }
  }
  // ternary operator
  if(isSgConditionalExp(node)) {
    SgNode* cond=SgNodeHelper::getCond(node);
    SgNode* trueBranch=SgNodeHelper::getTrueBranch(node);
    SgNode* falseBranch=SgNodeHelper::getFalseBranch(node);
    NumberIntervalLattice condVal=evaluate(cond);
    // schroder3 (2016-08-09): Changed behavior: Previously every non-const interval
    //  was treated the same as top. However non-const intervals that do not contain
    //  zero are always true.
    if(condVal.isBot()) {
      return NumberIntervalLattice::bot();
    }
    else if(condVal.isTrue()) {
      return evaluate(trueBranch);
    }
    else if(condVal.isFalse()) {
      return evaluate(falseBranch);
    }
    else if(condVal.isTop()) {
      // analyse both true-branch and false-branch and join.
      NumberIntervalLattice trueBranchResult=evaluate(trueBranch);
      NumberIntervalLattice falseBranchResult=evaluate(falseBranch);
      return NumberIntervalLattice::join(trueBranchResult,falseBranchResult);
    }
    else {
      ROSE_ASSERT(false);
    }
  }

  switch(node->variantT()) {
  case V_SgIntVal: return NumberIntervalLattice(Number(isSgIntVal(node)->get_value()));

  case V_SgUnsignedCharVal: return NumberIntervalLattice(Number(isSgUnsignedCharVal(node)->get_value()));
  case V_SgUnsignedShortVal: return NumberIntervalLattice(Number(isSgUnsignedShortVal(node)->get_value()));
  case V_SgUnsignedIntVal: return NumberIntervalLattice(Number(isSgUnsignedIntVal(node)->get_value()));
  case V_SgUnsignedLongVal: return NumberIntervalLattice(Number(isSgUnsignedLongVal(node)->get_value()));
  case V_SgUnsignedLongLongIntVal: return NumberIntervalLattice(Number(isSgUnsignedLongLongIntVal(node)->get_value()));

  case V_SgStringVal: return NumberIntervalLattice::top();

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
  case V_SgFunctionCallExp: {
    if(SgNodeHelper::getFunctionName(node)=="__assert_fail") {
      return NumberIntervalLattice::bot();
    } else {
      if(_showWarnings) cout<<"Warning: unknown function call: "<<node->unparseToString()<<" ... using unbounded result interval."<<endl;
      return NumberIntervalLattice::top();
    }
  }
  default: // generates top element
    if(_showWarnings) cout<<"Warning: unknown leaf node: "<<node->sage_class_name()<<"("<<node->unparseToString()<<") ... using unbounded result interval."<<endl;
    return NumberIntervalLattice::top();
  }
  // an unknown operator may have an arbitrary effect, to err on the safe side we topify all variables
  if(_sound) {
    if(_showWarnings) cout<<"Warning: Unknown operator."<<node->sage_class_name()<<" ... setting all variables to unbounded interval and using unbounded result interval."<<endl;
    ips->topifyAllVariables();
  }
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
