/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 ****X*********************************************************/

#include "sage3basic.h"
#include "ExprAnalyzer.h"
#include "CodeThornException.h"

using namespace CodeThorn;
using namespace SPRAY;

ExprAnalyzer::ExprAnalyzer():
  _variableIdMapping(0),
  _skipSelectedFunctionCalls(false),
  _skipArrayAccesses(false), 
  _externalFunctionSemantics(false) {
}

void ExprAnalyzer::setSkipSelectedFunctionCalls(bool skip) {
  _skipSelectedFunctionCalls=skip;
}

bool ExprAnalyzer::getSkipSelectedFunctionCalls() {
  return _skipSelectedFunctionCalls;
}

void ExprAnalyzer::setSkipArrayAccesses(bool skip) {
  _skipArrayAccesses=skip;
}

bool ExprAnalyzer::getSkipArrayAccesses() {
  return _skipArrayAccesses;
}

void ExprAnalyzer::setExternalFunctionSemantics(bool flag) {
  _externalFunctionSemantics=flag;
}

bool ExprAnalyzer::getExternalFunctionSemantics() {
  return _externalFunctionSemantics;
}

bool ExprAnalyzer::variable(SgNode* node, string& varName) {
  if(SgVarRefExp* varref=isSgVarRefExp(node)) {
    // found variable
    SgVariableSymbol* varsym=varref->get_symbol();
    varName=varsym->get_name().getString();
    return true;
  } else {
    varName="$";
    return false;
  }
}

bool ExprAnalyzer::variable(SgNode* node, VariableId& varId) {
  assert(node);
  if(SgNodeHelper::isArrayAccess(node)) {
    // 1) array variable id
    // 2) eval array-index expr
    // 3) if const then compute variable id otherwise return non-valid var id (would require set)
#if 0
    VariableId arrayVarId;
    SgExpression* arrayIndexExpr=0;
    int arrayIndexInt=-1;
    //cout<<"DEBUG: ARRAY-ACCESS"<<astTermWithNullValuesToString(node)<<endl;
    if(false) {
      varId=_variableIdMapping->variableIdOfArrayElement(arrayVarId,arrayIndexInt);
    }
#endif
    return false;
  }
  if(SgVarRefExp* varref=isSgVarRefExp(node)) {
    // found variable
    assert(_variableIdMapping);
    varId=_variableIdMapping->variableId(varref);
    return true;
  } else {
    VariableId defaultVarId;
    varId=defaultVarId;
    return false;
  }
}

AbstractValue ExprAnalyzer::constIntLatticeFromSgValueExp(SgValueExp* valueExp) {
  if(isSgFloatVal(valueExp)
     ||isSgDoubleVal(valueExp)
     ||isSgLongDoubleVal(valueExp)
     ||isSgComplexVal(valueExp)
     ||isSgStringVal(valueExp)
     ) {
    return AbstractValue(CodeThorn::Top());
  } else if(SgBoolValExp* exp=isSgBoolValExp(valueExp)) {
    // ROSE uses an integer for a bool
    int val=exp->get_value();
    if(val==0)
      return AbstractValue(false);
    else if(val==1)
      return AbstractValue(true);
    else {
      cerr<<"Error: unknown bool value (not 0 or 1): SgBoolExp::get_value()=="<<val<<endl;
      exit(1);
    }
  } else if(SgShortVal* exp=isSgShortVal(valueExp)) {
    short int val=exp->get_value();
    return AbstractValue((int)val);
  } else if(SgIntVal* exp=isSgIntVal(valueExp)) {
    int val=exp->get_value();
    return AbstractValue(val);
  } else if(SgLongIntVal* exp=isSgLongIntVal(valueExp)) {
    long int val=exp->get_value();
    return AbstractValue(val);
  } else if(SgLongLongIntVal* exp=isSgLongLongIntVal(valueExp)) {
    long long val=exp->get_value();
    return AbstractValue(val);
  } else if(SgUnsignedCharVal* exp=isSgUnsignedCharVal(valueExp)) {
    unsigned char val=exp->get_value();
    return AbstractValue((int)val);
  } else if(SgUnsignedShortVal* exp=isSgUnsignedShortVal(valueExp)) {
    unsigned short val=exp->get_value();
    return AbstractValue((int)val);
  } else if(SgUnsignedIntVal* exp=isSgUnsignedIntVal(valueExp)) {
    unsigned int val=exp->get_value();
    return AbstractValue(val);
  } else if(SgUnsignedLongVal* exp=isSgUnsignedLongVal(valueExp)) {
    unsigned long int val=exp->get_value();
    return AbstractValue(val);
  } else if(SgUnsignedLongVal* exp=isSgUnsignedLongVal(valueExp)) {
    unsigned long int val=exp->get_value();
    return AbstractValue(val);
  } else if(SgWcharVal* exp=isSgWcharVal(valueExp)) {
    long int val=exp->get_value();
    return AbstractValue(val);
  } else if(isSgNullptrValExp(valueExp)) {
    return AbstractValue((int)0);
  } else if(SgEnumVal* exp=isSgEnumVal(valueExp)) {
    int val=exp->get_value();
    return AbstractValue(val);
  } else {
    throw CodeThorn::Exception("Error: constIntLatticeFromSgValueExp::unsupported number type in SgValueExp.");
  }
}

//////////////////////////////////////////////////////////////////////
// EVAL CONSTINT
//////////////////////////////////////////////////////////////////////
list<SingleEvalResultConstInt> ExprAnalyzer::listify(SingleEvalResultConstInt res) {
  list<SingleEvalResultConstInt> resList;
  resList.push_back(res);
  return resList;
}

void SingleEvalResultConstInt::init(EState estate, ConstraintSet exprConstraints, AbstractValue result) {
  this->estate=estate;
  this->exprConstraints=exprConstraints;
  this->result=result;
}

#define CASE_EXPR_ANALYZER_EVAL(ROSENODENAME,EVALFUNCTIONNAME) case V_ ## ROSENODENAME: resultList.splice(resultList.end(),EVALFUNCTIONNAME(is ## ROSENODENAME(node),lhsResult,rhsResult,estate,useConstraints));break

#define CASE_EXPR_ANALYZER_EVAL_UNARY_OP(ROSENODENAME,EVALFUNCTIONNAME) case V_ ## ROSENODENAME: resultList.splice(resultList.end(),EVALFUNCTIONNAME(is ## ROSENODENAME(node),operandResult,estate,useConstraints));break

list<SingleEvalResultConstInt> ExprAnalyzer::evalConstInt(SgNode* node,EState estate, bool useConstraints) {
  assert(estate.pstate()); // ensure state exists
  // initialize with default values from argument(s)
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=AbstractValue(CodeThorn::Bot());

  if(SgNodeHelper::isPostfixIncDecOp(node)) {
    cout << "Error: incdec-op not supported in conditions."<<endl;
    exit(1);
  }
  if(SgConditionalExp* condExp=isSgConditionalExp(node)) {
    return evalConditionalExpr(condExp,estate,useConstraints);
  }

  if(dynamic_cast<SgBinaryOp*>(node)) {
    SgNode* lhs=SgNodeHelper::getLhs(node);
    list<SingleEvalResultConstInt> lhsResultList=evalConstInt(lhs,estate,useConstraints);
    SgNode* rhs=SgNodeHelper::getRhs(node);
    list<SingleEvalResultConstInt> rhsResultList=evalConstInt(rhs,estate,useConstraints);
    list<SingleEvalResultConstInt> resultList;
    for(list<SingleEvalResultConstInt>::iterator liter=lhsResultList.begin();
        liter!=lhsResultList.end();
        ++liter) {
      for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
          riter!=rhsResultList.end();
          ++riter) {
        SingleEvalResultConstInt lhsResult=*liter;
        SingleEvalResultConstInt rhsResult=*riter;

        switch(node->variantT()) {
          CASE_EXPR_ANALYZER_EVAL(SgEqualityOp,evalEqualOp);
          CASE_EXPR_ANALYZER_EVAL(SgNotEqualOp,evalNotEqualOp);
          CASE_EXPR_ANALYZER_EVAL(SgAndOp,evalAndOp);
          CASE_EXPR_ANALYZER_EVAL(SgOrOp,evalOrOp);
          CASE_EXPR_ANALYZER_EVAL(SgAddOp,evalAddOp);
          CASE_EXPR_ANALYZER_EVAL(SgSubtractOp,evalSubOp);
          CASE_EXPR_ANALYZER_EVAL(SgMultiplyOp,evalMulOp);
          CASE_EXPR_ANALYZER_EVAL(SgDivideOp,evalDivOp);
          CASE_EXPR_ANALYZER_EVAL(SgModOp,evalModOp);
          CASE_EXPR_ANALYZER_EVAL(SgBitAndOp,evalBitwiseAndOp);
          CASE_EXPR_ANALYZER_EVAL(SgBitOrOp,evalBitwiseOrOp);
          CASE_EXPR_ANALYZER_EVAL(SgBitXorOp,evalBitwiseXorOp);
          CASE_EXPR_ANALYZER_EVAL(SgGreaterOrEqualOp,evalGreaterOrEqualOp);
          CASE_EXPR_ANALYZER_EVAL(SgGreaterThanOp,evalGreaterThanOp);
          CASE_EXPR_ANALYZER_EVAL(SgLessThanOp,evalLessThanOp);
          CASE_EXPR_ANALYZER_EVAL(SgLessOrEqualOp,evalLessOrEqualOp);
          CASE_EXPR_ANALYZER_EVAL(SgPntrArrRefExp,evalArrayReferenceOp);
          CASE_EXPR_ANALYZER_EVAL(SgLshiftOp,evalBitwiseShiftLeftOp);
          CASE_EXPR_ANALYZER_EVAL(SgRshiftOp,evalBitwiseShiftRightOp);

        default:
            cerr << "Binary Op:"<<SgNodeHelper::nodeToString(node)<<"(nodetype:"<<node->class_name()<<")"<<endl;
          throw CodeThorn::Exception("Error: evalConstInt::unkown binary operation.");
        }
      }
    }
    return resultList;
  }
  
  if(dynamic_cast<SgUnaryOp*>(node)) {
    SgNode* child=SgNodeHelper::getFirstChild(node);
    list<SingleEvalResultConstInt> operandResultList=evalConstInt(child,estate,useConstraints);
    //assert(operandResultList.size()==1);
    list<SingleEvalResultConstInt> resultList;
    for(list<SingleEvalResultConstInt>::iterator oiter=operandResultList.begin();
        oiter!=operandResultList.end();
        ++oiter) {
      SingleEvalResultConstInt operandResult=*oiter;
      switch(node->variantT()) {
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgNotOp,evalNotOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgCastExp,evalCastOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgBitComplementOp,evalBitwiseComplementOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgMinusOp,evalUnaryMinusOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgPointerDerefExp,evalDereferenceOp);
      default:
        cerr << "@NODE:"<<node->sage_class_name()<<endl;
        string exceptionInfo=string("Error: evalConstInt::unknown unary operation @")+string(node->sage_class_name());
        throw exceptionInfo; 
      } // end switch
    }
    return  resultList;
  }
  
  ROSE_ASSERT(!dynamic_cast<SgBinaryOp*>(node) && !dynamic_cast<SgUnaryOp*>(node));
  
  // ALL REMAINING CASES DO NOT GENERATE CONSTRAINTS
  // EXPRESSION LEAF NODES
  // this test holds for all subclasses of SgValueExp
  if(SgValueExp* exp=isSgValueExp(node)) {
    return evalValueExp(isSgValueExp(exp),estate,useConstraints);
  }
  switch(node->variantT()) {
  case V_SgVarRefExp:
    return evalRValueVarExp(isSgVarRefExp(node),estate,useConstraints);
  case V_SgFunctionCallExp:
    return evalFunctionCall(isSgFunctionCallExp(node),estate,useConstraints);
  default:
    throw CodeThorn::Exception("Error: evalConstInt::unknown node in expression ("+string(node->sage_class_name())+")");
  } // end of switch
  throw CodeThorn::Exception("Error: evalConstInt failed.");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// EVAL FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////

// evaluation functions
list<SingleEvalResultConstInt> ExprAnalyzer::evalConditionalExpr(SgConditionalExp* condExp, EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SgExpression* cond=condExp->get_conditional_exp();
  list<SingleEvalResultConstInt> condResultList=evalConstInt(cond,estate,useConstraints);
  if(condResultList.size()==0) {
    cerr<<"Error: evaluating condition of conditional operator inside expressions gives no result."<<endl;
    exit(1);
  }
  if(condResultList.size()==2) {
    list<SingleEvalResultConstInt>::iterator i=condResultList.begin();
    SingleEvalResultConstInt singleResult1=*i;
    ++i;
    SingleEvalResultConstInt singleResult2=*i;
    if((singleResult1.value().operatorEq(singleResult2.value())).isTrue()) {
      cout<<"Info: evaluating condition of conditional operator gives two equal results"<<endl;
    }
  }
  if(condResultList.size()>1) {
    cerr<<"Error: evaluating condition of conditional operator gives more than one result. Not supported yet."<<endl;
    exit(1);
  }
  SingleEvalResultConstInt singleResult=*condResultList.begin();
  if(singleResult.result.isTop()) {
    SgExpression* trueBranch=condExp->get_true_exp();
    list<SingleEvalResultConstInt> trueBranchResultList=evalConstInt(trueBranch,estate,useConstraints);
    SgExpression* falseBranch=condExp->get_false_exp();
    list<SingleEvalResultConstInt> falseBranchResultList=evalConstInt(falseBranch,estate,useConstraints);
    // append falseBranchResultList to trueBranchResultList (moves elements), O(1).
    trueBranchResultList.splice(trueBranchResultList.end(), falseBranchResultList); 
    return trueBranchResultList;
  } else if(singleResult.result.isTrue()) {
    SgExpression* trueBranch=condExp->get_true_exp();
    list<SingleEvalResultConstInt> trueBranchResultList=evalConstInt(trueBranch,estate,useConstraints);
    return trueBranchResultList;
  } else if(singleResult.result.isFalse()) {
    SgExpression* falseBranch=condExp->get_false_exp();
    list<SingleEvalResultConstInt> falseBranchResultList=evalConstInt(falseBranch,estate,useConstraints);
    return falseBranchResultList;
  } else {
    cerr<<"Error: evaluating conditional operator inside expressions - unknown behavior (condition may have evaluated to bot)."<<endl;
    exit(1);
  }
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalEqualOp(SgEqualityOp* node,
                                                         SingleEvalResultConstInt lhsResult, 
                                                         SingleEvalResultConstInt rhsResult,
                                                         EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorEq(rhsResult.result));
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  // record new constraint
  if(useConstraints) {
    VariableId varId;
    SgNode* lhs=SgNodeHelper::getLhs(node);
    SgNode* rhs=SgNodeHelper::getRhs(node);
    if(variable(lhs,varId) && rhsResult.isConstInt()) {
      // if var is top add two states with opposing constraints
      if(!res.estate.pstate()->varIsConst(varId)) {
        SingleEvalResultConstInt tmpres1=res;
        SingleEvalResultConstInt tmpres2=res;
        tmpres1.exprConstraints.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,rhsResult.value()));
        tmpres1.result=true;
        tmpres2.exprConstraints.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,varId,rhsResult.value()));
        tmpres2.result=false;
        resultList.push_back(tmpres1);
        resultList.push_back(tmpres2);
        goto done;
      }
    }
    if(lhsResult.isConstInt() && variable(rhs,varId)) {
      // only add the equality constraint if no constant is bound to the respective variable
      if(!res.estate.pstate()->varIsConst(varId)) {
        SingleEvalResultConstInt tmpres1=res;
        SingleEvalResultConstInt tmpres2=res;
        tmpres1.exprConstraints.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,lhsResult.value()));
        tmpres1.result=true;
        tmpres2.exprConstraints.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,varId,lhsResult.value()));
        tmpres2.result=false;
        resultList.push_back(tmpres1);
        resultList.push_back(tmpres2);
      }
    }
  done:;
  }
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalNotEqualOp(SgNotEqualOp* node,
                                                            SingleEvalResultConstInt lhsResult, 
                                                            SingleEvalResultConstInt rhsResult,
                                                            EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  if(useConstraints) {
    res.result=(lhsResult.result.operatorNotEq(rhsResult.result));
    res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
    // record new constraint
    VariableId varId;
    SgNode* lhs=SgNodeHelper::getLhs(node);
    SgNode* rhs=SgNodeHelper::getRhs(node);
    if(variable(lhs,varId) && rhsResult.isConstInt()) {
      // only add the equality constraint if no constant is bound to the respective variable
      if(!res.estate.pstate()->varIsConst(varId)) {
        SingleEvalResultConstInt tmpres1=res;
        SingleEvalResultConstInt tmpres2=res;
        tmpres1.exprConstraints.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,varId,rhsResult.value()));
        tmpres1.result=true;
        tmpres2.exprConstraints.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,rhsResult.value()));
        tmpres2.result=false;
        resultList.push_back(tmpres1);
        resultList.push_back(tmpres2);
        goto done;
      }
    }
    if(lhsResult.isConstInt() && variable(rhs,varId)) {
      // only add the equality constraint if no constant is bound to the respective variable
      if(!res.estate.pstate()->varIsConst(varId)) {
        SingleEvalResultConstInt tmpres1=res;
        SingleEvalResultConstInt tmpres2=res;
        tmpres1.exprConstraints.addConstraint(Constraint(Constraint::NEQ_VAR_CONST,varId,lhsResult.value()));
        tmpres1.result=true;
        tmpres2.exprConstraints.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,lhsResult.value()));
        tmpres2.result=false;
        resultList.push_back(tmpres1);
        resultList.push_back(tmpres2);
      }
    }
  done:;
  }
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalAndOp(SgAndOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  //cout << "SgAndOp: "<<lhsResult.result.toString()<<"&&"<<rhsResult.result.toString()<<" ==> ";
  res.result=(lhsResult.result.operatorAnd(rhsResult.result));
  //cout << res.result.toString()<<endl;
  if(lhsResult.result.isFalse()) {
    res.exprConstraints=lhsResult.exprConstraints;
    // rhs is not considered due to short-circuit CPP-AND-semantics
  }
  if(lhsResult.result.isTrue() && rhsResult.result.isFalse()) {
    res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
    // nothing to do
  }
  if(lhsResult.result.isTrue() && rhsResult.result.isTrue()) {
    res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  }
  
  // in case of top we do not propagate constraints [imprecision]
  if(!lhsResult.result.isTop()) {
    res.exprConstraints+=lhsResult.exprConstraints;
  }
  if(!rhsResult.result.isTop()) {
    res.exprConstraints+=rhsResult.exprConstraints;
  }
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalOrOp(SgOrOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=lhsResult.result.operatorOr(rhsResult.result);

  // encode short-circuit CPP-OR-semantics
  if(lhsResult.result.isTrue()) {
    res.result=lhsResult.result;
    res.exprConstraints=lhsResult.exprConstraints;
  } 
  if(lhsResult.result.isFalse() && rhsResult.result.isFalse()) {
    res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  } 
  if(lhsResult.result.isFalse() && rhsResult.result.isTrue()) {
    res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  }

  // in case of top do not propagate constraints [imprecision]
  if(!lhsResult.result.isTop()) {
    res.exprConstraints+=lhsResult.exprConstraints;
  }
  if(!rhsResult.result.isTop()) {
    res.exprConstraints+=rhsResult.exprConstraints;
  }

  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalAddOp(SgAddOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result+rhsResult.result);
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}
list<SingleEvalResultConstInt> ExprAnalyzer::evalSubOp(SgSubtractOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result-rhsResult.result);
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}
list<SingleEvalResultConstInt> ExprAnalyzer::evalMulOp(SgMultiplyOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate, bool useConstraints) {

  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result*rhsResult.result);
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}
list<SingleEvalResultConstInt> ExprAnalyzer::evalDivOp(SgDivideOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate, bool useConstraints) {

  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result/rhsResult.result);
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalModOp(SgModOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate, bool useConstraints) {

  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result%rhsResult.result);
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitwiseAndOp(SgBitAndOp* node,
                                                              SingleEvalResultConstInt lhsResult, 
                                                              SingleEvalResultConstInt rhsResult,
                                                              EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorBitwiseAnd(rhsResult.result));
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitwiseOrOp(SgBitOrOp* node,
                                                             SingleEvalResultConstInt lhsResult, 
                                                             SingleEvalResultConstInt rhsResult,
                                                             EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorBitwiseOr(rhsResult.result));
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitwiseXorOp(SgBitXorOp* node,
                                                              SingleEvalResultConstInt lhsResult, 
                                                              SingleEvalResultConstInt rhsResult,
                                                              EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorBitwiseXor(rhsResult.result));
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> 
ExprAnalyzer::evalGreaterOrEqualOp(SgGreaterOrEqualOp* node,
                                   SingleEvalResultConstInt lhsResult, 
                                   SingleEvalResultConstInt rhsResult,
                                   EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorMoreOrEq(rhsResult.result));
  if(boolOptions["relop-constraints"]) {
    if(res.result.isTop())
      throw CodeThorn::Exception("Error: Top found in relational operator (not supported yet).");
  }
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> 
ExprAnalyzer::evalGreaterThanOp(SgGreaterThanOp* node,
                                SingleEvalResultConstInt lhsResult, 
                                SingleEvalResultConstInt rhsResult,
                                EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorMore(rhsResult.result));
  if(boolOptions["relop-constraints"]) {
    if(res.result.isTop())
      throw CodeThorn::Exception("Error: Top found in relational operator (not supported yet).");
  }
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalLessOrEqualOp(SgLessOrEqualOp* node,
                                SingleEvalResultConstInt lhsResult, 
                                SingleEvalResultConstInt rhsResult,
                                EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorLessOrEq(rhsResult.result));
  if(boolOptions["relop-constraints"]) {
    if(res.result.isTop())
      throw CodeThorn::Exception("Error: Top found in relational operator (not supported yet).");
  }
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalLessThanOp(SgLessThanOp* node,
                             SingleEvalResultConstInt lhsResult, 
                             SingleEvalResultConstInt rhsResult,
                             EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorLess(rhsResult.result));
  if(boolOptions["relop-constraints"]) {
    if(res.result.isTop())
      throw CodeThorn::Exception("Error: Top found in relational operator (not supported yet).");
  }
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalBitwiseShiftLeftOp(SgLshiftOp* node,
                             SingleEvalResultConstInt lhsResult, 
                             SingleEvalResultConstInt rhsResult,
                             EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorBitwiseShiftLeft(rhsResult.result));
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalBitwiseShiftRightOp(SgRshiftOp* node,
                             SingleEvalResultConstInt lhsResult, 
                             SingleEvalResultConstInt rhsResult,
                             EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorBitwiseShiftRight(rhsResult.result));
  res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> 
ExprAnalyzer::evalArrayReferenceOp(SgPntrArrRefExp* node,
                                 SingleEvalResultConstInt arrayExprResult, 
                                 SingleEvalResultConstInt indexExprResult,
                                 EState estate, bool useConstraints) {
  //cout<<"DEBUG: evalArrayReferenceOp: "<<node->unparseToString()<<endl;
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  SgNode* arrayExpr=SgNodeHelper::getLhs(node);
  
  if(indexExprResult.value().isTop()||getSkipArrayAccesses()==true) {
    // set result to top when index is top [imprecision]
    // assume top for array elements if skipped
    res.result=CodeThorn::Top();
    res.exprConstraints=arrayExprResult.exprConstraints+indexExprResult.exprConstraints;
    resultList.push_back(res);
    return resultList;
  } else {
    if(SgVarRefExp* varRefExp=isSgVarRefExp(arrayExpr)) {
      AbstractValue arrayPtrValue=arrayExprResult.result;
      const PState* pstate=estate.pstate();
      PState pstate2=*pstate; // also removes constness
      VariableId arrayVarId=_variableIdMapping->variableId(varRefExp);
      // two cases
      if(_variableIdMapping->hasArrayType(arrayVarId)) {
        arrayPtrValue=AbstractValue::createAddressOfArray(arrayVarId);
      } else if(_variableIdMapping->hasPointerType(arrayVarId)) {
        // in case it is a pointer retrieve pointer value
        //cout<<"DEBUG: pointer-array access!"<<endl;
        if(pstate->varExists(arrayVarId)) {
          arrayPtrValue=pstate2[arrayVarId]; // pointer value (without index)
          ROSE_ASSERT(arrayPtrValue.isPtr());
        } else {
          cerr<<"Error: pointer variable does not exist in PState."<<endl;
          exit(1);
        }
      } else {
        cerr<<"Error: unkown type of array or pointer."<<endl;
        exit(1);
      }
      AbstractValue indexExprResultValue=indexExprResult.value();
      AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrayPtrValue,indexExprResultValue);
#if 0
      VariableId arrayVarId2=arrayPtrPlusIndexValue.getVariableId();
      int index2=arrayPtrPlusIndexValue.getIntValue();
      if(!checkArrayBounds(arrayVarId2,index2)) {
        cerr<<"Read access: "<<node->unparseToString()<<endl;
      }
      VariableId arrayElementId=_variableIdMapping->variableIdOfArrayElement(arrayVarId2,index2);
      ROSE_ASSERT(arrayElementId.isValid());
#endif
      if(pstate->varExists(arrayPtrPlusIndexValue)) {
        res.result=pstate2[arrayPtrPlusIndexValue];
        //cout<<"DEBUG: retrieved array element value:"<<res.result<<endl;
        if(res.result.isTop() && useConstraints) {
          AbstractValue val=res.estate.constraints()->varAbstractValue(arrayPtrPlusIndexValue);
          res.result=val;
        }
        return listify(res);
      } else {
        // array variable NOT in state. Special space optimization case for constant array.
        if(_variableIdMapping->isConstantArray(arrayVarId)) {
          SgExpressionPtrList& initList=_variableIdMapping->getInitializerListOfArrayVariable(arrayVarId);
          int elemIndex=0;
          // TODO: slow linear lookup (TODO: pre-compute all values and provide access function)
          for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
            //VariableId arrayElemId=_variableIdMapping->variableIdOfArrayElement(initDeclVarId,elemIndex);
            SgExpression* exp=*i;
            SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
            if(assignInit) {
              SgExpression* initExp=assignInit->get_operand_i();
              ROSE_ASSERT(initExp);
              if(SgIntVal* intValNode=isSgIntVal(initExp)) {
                int intVal=intValNode->get_value();
                //cout<<"DEBUG:initializing array element:"<<arrayElemId.toString()<<"="<<intVal<<endl;
                //newPState.setVariableToValue(arrayElemId,CodeThorn::AValue(AbstractValue(intVal)));
                int index2=arrayPtrPlusIndexValue.getIndexIntValue();
                if(elemIndex==index2) {
                  AbstractValue val=AbstractValue(intVal);
                  res.result=val;
                  return listify(res);
                }
              } else {
                cerr<<"Error: unsupported array initializer value:"<<exp->unparseToString()<<" AST:"<<SPRAY::AstTerm::astTermWithNullValuesToString(exp)<<endl;
                exit(1);
              }
            } else {
              cerr<<"Error: no assign initialize:"<<exp->unparseToString()<<" AST:"<<SPRAY::AstTerm::astTermWithNullValuesToString(exp)<<endl;
            }
            elemIndex++;
          }
          cerr<<"Error: access to element of constant array (not in state). Not supported yet."<<endl;
          exit(1);
        } else {
          cerr<<"Error: Array Element does not exist (out of array access?)"<<endl;
          cerr<<"array-element: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
          cerr<<"PState: "<<pstate->toString(_variableIdMapping)<<endl;
          cerr<<"AST: "<<node->unparseToString()<<endl;
          exit(1);
        }
      }
    } else {
      cerr<<"Error: array-access uses expr for denoting the array. Not supported yet."<<endl;
      cerr<<"expr: "<<arrayExpr->unparseToString()<<endl;
      cerr<<"arraySkip: "<<getSkipArrayAccesses()<<endl;
      exit(1);
    }
    return resultList;
  }
  ROSE_ASSERT(false); // not reachable
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalNotOp(SgNotOp* node, 
                                                       SingleEvalResultConstInt operandResult, 
                                                       EState estate, bool useConstraints) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=operandResult.result.operatorNot();
  // do *not* invert the constraints, instead negate the operand result (TODO: investigate)
  res.exprConstraints=operandResult.exprConstraints;
  return listify(res);
}
list<SingleEvalResultConstInt> ExprAnalyzer::evalUnaryMinusOp(SgMinusOp* node, 
                                                              SingleEvalResultConstInt operandResult, 
                                                              EState estate, bool useConstraints) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=operandResult.result.operatorUnaryMinus();
  res.exprConstraints=operandResult.exprConstraints;
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalCastOp(SgCastExp* node, 
                                                        SingleEvalResultConstInt operandResult, 
                                                        EState estate, bool useConstraints) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  // TODO: model effect of cast when sub language is extended
  //SgCastExp* castExp=isSgCastExp(node);
  res.result=operandResult.result;
  res.exprConstraints=operandResult.exprConstraints;
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitwiseComplementOp(SgBitComplementOp* node, 
                                                                     SingleEvalResultConstInt operandResult, 
                                                                     EState estate, bool useConstraints) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=operandResult.result.operatorBitwiseComplement();
  res.exprConstraints=operandResult.exprConstraints;
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalDereferenceOp(SgPointerDerefExp* node, 
                                                              SingleEvalResultConstInt operandResult, 
                                                              EState estate, bool useConstraints) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  AbstractValue derefOperandValue=operandResult.result;
  //cout<<"DEBUG: derefOperandValue: "<<derefOperandValue.toRhsString(_variableIdMapping);
  // (varid,idx) => varid'; return estate.pstate()[varid'] || pstate(AValue)
  //res.result=readFromMemoryLocation(estate.pstate(), derefOperandValue);
  res.result=derefOperandValue;
  res.exprConstraints=operandResult.exprConstraints;
  return listify(res);
}


list<SingleEvalResultConstInt> ExprAnalyzer::evalRValueVarExp(SgVarRefExp* node, EState estate, bool useConstraints) {
  //cout<<"DEBUG: evalRValueVarExp: "<<node->unparseToString()<<endl;
  SingleEvalResultConstInt res;
  res.init(estate,*estate.constraints(),AbstractValue(CodeThorn::Bot()));
  const PState* pstate=estate.pstate();
  VariableId varId=_variableIdMapping->variableId(node);
  if(pstate->varExists(varId)) {
    //PState pstate2=*pstate; // also removes constness
    if(_variableIdMapping->hasArrayType(varId)) {
      // CODE-POINT-1
      res.result=AbstractValue::createAddressOfArray(varId);
      // for arrays (by default the address is used) return its pointer value (the var-id-code)
      // with a unified pointer representation this case is now equal
      //res.result=AbstractValue(varId.getIdCode());
      //res.result=const_cast<PState*>(pstate)->operator[](varId); // this includes assignment of pointer values
    } else {
      //res.result=AbstractValue::createAddressOfArray(varId,AbstractValue(0));
      res.result=const_cast<PState*>(pstate)->operator[](varId); // this includes assignment of pointer values
    }
    if(res.result.isTop() && useConstraints) {
      // in case of TOP we try to extract a possibly more precise value from the constraints
      AbstractValue val=res.estate.constraints()->varAbstractValue(varId);
      // TODO: TOPIFY-MODE: most efficient here
      res.result=val;
    }
    return listify(res);
  } else {
    // special modes to represent information not stored in the state
    // i) unmodified arrays: data can be stored outside the state
    // ii) undefined variables mapped to 'top' (abstraction by removing variables from state)
    if(_variableIdMapping->isConstantArray(varId) && boolOptions["rersmode"]) {
      // currently only used in rersmode
      res.result=AbstractValue(varId.getIdCode());
      return listify(res);
    } else {
      res.result=CodeThorn::Top();
      //cerr << "WARNING: variable not in PState (var="<<_variableIdMapping->uniqueLongVariableName(varId)<<"). Initialized with top."<<endl;
      return listify(res);
    }
  }
  // unreachable
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalValueExp(SgValueExp* node, EState estate, bool useConstraints) {
  SingleEvalResultConstInt res;
  res.init(estate,*estate.constraints(),AbstractValue(CodeThorn::Bot()));
  res.result=constIntLatticeFromSgValueExp(node);
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalFunctionCall(SgFunctionCallExp* funCall, EState estate, bool useConstraints) {
  SingleEvalResultConstInt res;
  res.init(estate,*estate.constraints(),AbstractValue(CodeThorn::Top()));
  if(getSkipSelectedFunctionCalls()) {
    // return default value
    return listify(res);
  } else if(getExternalFunctionSemantics()) {
    //cout<<"DEBUG: FOUND function call inside expression (external): "<<funCall->unparseToString()<<endl;
    string funName=SgNodeHelper::getFunctionName(funCall);
    if(funName=="malloc") {
      return evalFunctionCallMalloc(funCall,estate,useConstraints);
    } else if(funName=="memcpy") {
      return evalFunctionCallMemCpy(funCall,estate,useConstraints);
    } else {
      cout<<"WARNING: unknown external function ("<<funName<<") inside expression detected. Assuming it is side-effect free."<<endl;
      return listify(res);
    }
  } else {
    string s=funCall->unparseToString();
    throw CodeThorn::Exception("Unknown semantics of function call inside expression: "+s);
  }
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalFunctionCallMalloc(SgFunctionCallExp* funCall, EState estate, bool useConstraints) {
  SingleEvalResultConstInt res;
  static int memorylocid=0; // to be integrated in VariableIdMapping
  memorylocid++;
  stringstream ss;
  ss<<"$MEM"<<memorylocid;
  ROSE_ASSERT(_variableIdMapping);
  SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  if(argsList.size()==1) {
    SgExpression* arg1=*argsList.begin();
    list<SingleEvalResultConstInt> resList=evalConstInt(arg1,estate,useConstraints);
    if(resList.size()!=1) {
      cerr<<"Error: conditional control-flow in function argument expression not supported. Expression normalization required."<<endl;
      exit(1);
    }
    SingleEvalResultConstInt sres=*resList.begin();
    AbstractValue arg1val=sres.result;
    VariableId memLocVarId;
    int memoryRegionSize;
    if(arg1val.isConstInt()) {
      memoryRegionSize=arg1val.getIntValue();
    } else {
      // unknown size
      memoryRegionSize=0;
    }
    memLocVarId=_variableIdMapping->createAndRegisterNewMemoryRegion(ss.str(),memoryRegionSize);
    AbstractValue allocatedMemoryPtr=AbstractValue::createAddressOfArray(memLocVarId);
    res.init(estate,*estate.constraints(),allocatedMemoryPtr);
    //cout<<"DEBUG: evaluating function call malloc:"<<funCall->unparseToString()<<endl;
    ROSE_ASSERT(allocatedMemoryPtr.isPtr());
    //cout<<"Generated malloc-allocated mem-chunk pointer is OK."<<endl;
    return listify(res);
  } else {
    // this will become an error in future
    cerr<<"WARNING: unknown malloc function "<<funCall->unparseToString()<<endl;
  }
  return listify(res);
}

int ExprAnalyzer::getMemoryRegionSize(CodeThorn::AbstractValue ptrToRegion) {
  ROSE_ASSERT(ptrToRegion.isPtr());
  VariableId ptrVariableId=ptrToRegion.getVariableId();
  //cout<<"DEBUG: ptrVariableId:"<<ptrVariableId<<" "<<_variableIdMapping->variableName(ptrVariableId)<<endl;
  int size=_variableIdMapping->getSize(ptrVariableId);
  return size;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalFunctionCallMemCpy(SgFunctionCallExp* funCall, EState estate, bool useConstraints) {
  //cout<<"DETECTED: memcpy: "<<funCall->unparseToString()<<endl;
  SingleEvalResultConstInt res;
  // memcpy is a void function, no return value
  res.init(estate,*estate.constraints(),AbstractValue(CodeThorn::Top())); 
  SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  if(argsList.size()==3) {
    AbstractValue memcpyArgs[3];
    int i=0;
    for(SgExpressionPtrList::iterator argIter=argsList.begin();argIter!=argsList.end();++argIter) {
      SgExpression* arg=*argIter;
      list<SingleEvalResultConstInt> resList=evalConstInt(arg,estate,useConstraints);
      if(resList.size()!=1) {
        cerr<<"Error: conditional control-flow in function argument expression. Expression normalization required."<<endl;
        exit(1);
      }
      SingleEvalResultConstInt sres=*resList.begin();
      AbstractValue argVal=sres.result;
      memcpyArgs[i++]=argVal;
    }
    // determine sizes of memory regions (refered to by pointer)
    for(int i=0;i<3;i++) {
      //cout<<"memcpy argument "<<i<<": "<<memcpyArgs[i].toString(_variableIdMapping)<<endl;
    }
    int memRegionSizeTarget=getMemoryRegionSize(memcpyArgs[0]);
    int memRegionSizeSource=getMemoryRegionSize(memcpyArgs[1]);
    
    //cout<<"DEBUG: memRegionSize target:"<<memRegionSizeTarget<<endl;
    //cout<<"DEBUG: memRegionSize source:"<<memRegionSizeSource<<endl;
    if(memcpyArgs[2].isTop()) {
      cout<<"Program error detected at line "<<SgNodeHelper::sourceLineColumnToString(funCall)<<funCall->unparseToString()<<" : potential out of bounds access (source and target)."<<endl;
      return listify(res);
    }
    bool errorDetected=false;
    int copyRegionSize=memcpyArgs[2].getIntValue();
    //cout<<"DEBUG: copyRegionSize:"<<copyRegionSize<<endl;
    if(memRegionSizeSource<copyRegionSize) {
      if(memRegionSizeSource==0) {
        cout<<"Program error detected at line "<<SgNodeHelper::sourceLineColumnToString(funCall)<<": "<<funCall->unparseToString()<<" : potential out of bounds access at copy source."<<endl;
        errorDetected=true;
      } else {
        cout<<"Program error detected at line "<<SgNodeHelper::sourceLineColumnToString(funCall)<<": "<<funCall->unparseToString()<<" : definitive out of bounds access at copy source - memcpy(["<<(memRegionSizeTarget>0?std::to_string(memRegionSizeTarget):"-")<<"],["<<memRegionSizeSource<<"],"<<copyRegionSize<<")"<<endl;
        errorDetected=true;
      }
    }
    if(memRegionSizeTarget<copyRegionSize) {
      if(memRegionSizeTarget==0) {
        cout<<"Program error detected at line "<<SgNodeHelper::sourceLineColumnToString(funCall)<<": "<<funCall->unparseToString()<<" : potential out of bounds access at copy target."<<endl;
        errorDetected=true;
      } else {
        cout<<"Program error detected at line "<<SgNodeHelper::sourceLineColumnToString(funCall)<<": "<<funCall->unparseToString()<<" : definitive out of bounds access at copy target - memcpy(["<<(memRegionSizeTarget>0?std::to_string(memRegionSizeTarget):"-")<<"],["<<memRegionSizeSource<<"],"<<copyRegionSize<<")"<<endl;
        errorDetected=true;
      }
    }
    if(!errorDetected) {
      // no error occured. Copy region.
      cout<<"DEBUG: copy region now. "<<endl;
      for(int i=0;i<copyRegionSize;i++) {
        AbstractValue index(i);
        AbstractValue targetPtr=memcpyArgs[0]+index;
        AbstractValue sourcePtr=memcpyArgs[1]+index;
        cout<<"DEBUG: copying "<<targetPtr.toString(_variableIdMapping)<<" from "<<sourcePtr.toString(_variableIdMapping)<<endl;
      }
    }
    return listify(res);
  } else {
    // this will become an error in future
    cerr<<"WARNING: unknown memcpy function (number of arguments != 3)"<<funCall->unparseToString()<<endl;
  }
  return listify(res);
}


bool ExprAnalyzer::checkArrayBounds(VariableId arrayVarId,int accessIndex) {
  // check array bounds
  int arraySize=_variableIdMapping->getSize(arrayVarId);
  if(accessIndex<0||accessIndex>=arraySize) {  
    // this will throw a specific exception that will be caught by the analyzer to report verification results
    cerr<<"Detected out of bounds array access in application: ";
    cerr<<"array size: "<<arraySize<<", array index: "<<accessIndex<<" :: ";
    return false; // fail
  }
  return true; // pass
}

// compute absolute variableId as encoded in the VariableIdMapping.
// obsolete with new domain
SPRAY::VariableId ExprAnalyzer::resolveToAbsoluteVariableId(AbstractValue abstrValue) const {
  VariableId arrayVarId2=abstrValue.getVariableId();
  int index2=abstrValue.getIntValue();
  return _variableIdMapping->variableIdOfArrayElement(arrayVarId2,index2);
}

AbstractValue ExprAnalyzer::readFromMemoryLocation(const PState* pState, AbstractValue abstrValue) const {
  if(abstrValue.isTop()) {
    // report memory violation
    cout<<"WARNING: reading from unknown memory location (top)."<<endl;
    return abstrValue;
  }
#if 0
  return pState->varValue(resolveToAbsoluteVariableId(abstrValue));
#else
  return pState->varValue(abstrValue);
#endif
}

void ExprAnalyzer::writeToMemoryLocation(PState& pState,
                                         AbstractValue abstractMemLoc,
                                         AbstractValue abstractValue) {
#if 0
  VariableId absoluteMemLoc=resolveToAbsoluteVariableId(abstractMemLoc);
  pState.setVariableToValue(absoluteMemLoc,abstractValue);
#else
  if(abstractValue.isBot()) {
    //cout<<"INFO: conversion: bot(uninitialized)->top(any)."<<endl;
    abstractValue=AbstractValue(CodeThorn::Top());
  }
  pState.setVariableToValue(abstractMemLoc,abstractValue);
#endif
}
