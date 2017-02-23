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
using namespace AType;

ExprAnalyzer::ExprAnalyzer():_variableIdMapping(0),_skipSelectedFunctionCalls(false),_skipArrayAccesses(false){
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
#if 0
    SgSymbol* sym=varref->get_symbol();
    assert(sym);
    varId=_variableIdMapping->variableId(sym);
#else
    // MS: to investigate: even with the new var-sym-only case this does not work
    // MS: investigage getSymbolOfVariable
    // MS: 9/6/2014: this seems to work now
    varId=_variableIdMapping->variableId(varref);
#endif
    return true;
  } else {
    VariableId defaultVarId;
    varId=defaultVarId;
    return false;
  }
}

AType::ConstIntLattice ExprAnalyzer::constIntLatticeFromSgValueExp(SgValueExp* valueExp) {
  if(isSgFloatVal(valueExp)
     ||isSgDoubleVal(valueExp)
     ||isSgLongDoubleVal(valueExp)
     ||isSgComplexVal(valueExp)
     ||isSgStringVal(valueExp)
     ) {
    return ConstIntLattice(AType::Top());
  } else if(SgBoolValExp* exp=isSgBoolValExp(valueExp)) {
    // ROSE uses an integer for a bool
    int val=exp->get_value();
    if(val==0)
      return ConstIntLattice(false);
    else if(val==1)
      return ConstIntLattice(true);
    else {
      cerr<<"Error: unknown bool value (not 0 or 1): SgBoolExp::get_value()=="<<val<<endl;
      exit(1);
    }
  } else if(SgShortVal* exp=isSgShortVal(valueExp)) {
    short int val=exp->get_value();
    return ConstIntLattice((int)val);
  } else if(SgIntVal* exp=isSgIntVal(valueExp)) {
    int val=exp->get_value();
    return ConstIntLattice(val);
  } else if(SgLongIntVal* exp=isSgLongIntVal(valueExp)) {
    long int val=exp->get_value();
    return ConstIntLattice(val);
  } else if(SgLongLongIntVal* exp=isSgLongLongIntVal(valueExp)) {
    long long val=exp->get_value();
    return ConstIntLattice(val);
  } else if(SgUnsignedCharVal* exp=isSgUnsignedCharVal(valueExp)) {
    unsigned char val=exp->get_value();
    return ConstIntLattice((int)val);
  } else if(SgUnsignedShortVal* exp=isSgUnsignedShortVal(valueExp)) {
    unsigned short val=exp->get_value();
    return ConstIntLattice((int)val);
  } else if(SgUnsignedIntVal* exp=isSgUnsignedIntVal(valueExp)) {
    unsigned int val=exp->get_value();
    return ConstIntLattice(val);
  } else if(SgUnsignedLongVal* exp=isSgUnsignedLongVal(valueExp)) {
    unsigned long int val=exp->get_value();
    return ConstIntLattice(val);
  } else if(SgUnsignedLongVal* exp=isSgUnsignedLongVal(valueExp)) {
    unsigned long int val=exp->get_value();
    return ConstIntLattice(val);
  } else if(SgWcharVal* exp=isSgWcharVal(valueExp)) {
    long int val=exp->get_value();
    return ConstIntLattice(val);
  } else if(isSgNullptrValExp(valueExp)) {
    return ConstIntLattice((int)0);
  } else if(SgEnumVal* exp=isSgEnumVal(valueExp)) {
    int val=exp->get_value();
    return ConstIntLattice(val);
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

void SingleEvalResultConstInt::init(EState estate, ConstraintSet exprConstraints, AType::ConstIntLattice result) {
  this->estate=estate;
  this->exprConstraints=exprConstraints;
  this->result=result;
}

#define CASE_EXPR_ANALYZER_EVAL(ROSENODENAME,EVALFUNCTIONNAME) case V_ ## ROSENODENAME: resultList.splice(resultList.end(),EVALFUNCTIONNAME(is ## ROSENODENAME(node),lhsResult,rhsResult,estate,useConstraints));break

list<SingleEvalResultConstInt> ExprAnalyzer::evalConstInt(SgNode* node,EState estate, bool useConstraints) {
  assert(estate.pstate()); // ensure state exists
  // initialize with default values from argument(s)
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=AType::ConstIntLattice(AType::Bot());

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
          CASE_EXPR_ANALYZER_EVAL(SgBitAndOp,evalBitAndOp);
          CASE_EXPR_ANALYZER_EVAL(SgBitOrOp,evalBitOrOp);
          CASE_EXPR_ANALYZER_EVAL(SgBitXorOp,evalBitXorOp);
          CASE_EXPR_ANALYZER_EVAL(SgGreaterOrEqualOp,evalGreaterOrEqualOp);
          CASE_EXPR_ANALYZER_EVAL(SgGreaterThanOp,evalGreaterThanOp);
          CASE_EXPR_ANALYZER_EVAL(SgLessThanOp,evalLessThanOp);
          CASE_EXPR_ANALYZER_EVAL(SgLessOrEqualOp,evalLessOrEqualOp);
          CASE_EXPR_ANALYZER_EVAL(SgPntrArrRefExp,evalArrayReferenceOp);
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
      case V_SgNotOp:
        res.result=operandResult.result.operatorNot();
        // we do NOT invert the constraints, instead we negate the operand result (TODO: investigate)
        res.exprConstraints=operandResult.exprConstraints;
        resultList.push_back(res);
      break;
      case V_SgCastExp: {
        // TODO: model effect of cast when sub language is extended
        //SgCastExp* castExp=isSgCastExp(node);
        res.result=operandResult.result;
        res.exprConstraints=operandResult.exprConstraints;
        resultList.push_back(res);
        break;
      }
        // unary minus
      case V_SgMinusOp: {
        res.result=operandResult.result.operatorUnaryMinus();
        res.exprConstraints=operandResult.exprConstraints;
        resultList.push_back(res);
        break;
      }
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
  if(SgValueExp* exp=isSgValueExp(node)) {
    res.result=constIntLatticeFromSgValueExp(exp);
    return listify(res);
  }
  switch(node->variantT()) {
  case V_SgVarRefExp: {
    VariableId varId;
    bool isVar=ExprAnalyzer::variable(node,varId);
      //cout<<"DEBUG: EvalConstInt: V_SgVarRefExp: isVar:"<<isVar<<", varIdcode:"<<varId.getIdCode()<<"Source:"<<node->unparseToString()<<endl;
    assert(isVar);
    const PState* pstate=estate.pstate();
    if(pstate->varExists(varId)) {
      PState pstate2=*pstate; // also removes constness

      if(_variableIdMapping->hasArrayType(varId)) {
        // CODE-POINT-1
        // for arrays (by default the address is used) return its pointer value (the var-id-code)
        res.result=AType::ConstIntLattice(varId.getIdCode());
      } else {
        res.result=pstate2[varId]; // this include assignment of pointer values
      }
      if(res.result.isTop() && useConstraints) {
        // in case of TOP we try to extract a possibly more precise value from the constraints
        AType::ConstIntLattice val=res.estate.constraints()->varConstIntLatticeValue(varId);
        // TODO: TOPIFY-MODE: most efficient here
        res.result=val;
      }
      return listify(res);
    } else {
      if(_variableIdMapping->isConstantArray(varId) && boolOptions["rersmode"]) {
        res.result=AType::ConstIntLattice(varId.getIdCode());
        return listify(res);
      } else {
        res.result=AType::Top();
        //cerr << "WARNING: variable not in PState (var="<<_variableIdMapping->uniqueLongVariableName(varId)<<"). Initialized with top."<<endl;
        return listify(res);
      }
    }
    break;
  }
  case V_SgFunctionCallExp: {
    if(getSkipSelectedFunctionCalls()) {
      // return default value
      return listify(res);
    } else {
      throw CodeThorn::Exception("Error: evalConstInt::function call inside expression.");
    }

  }
  default:
    cerr << "@NODE:"<<node->sage_class_name()<<endl;
    throw CodeThorn::Exception("Error: evalConstInt::unknown operation failed.");
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
        //return resultList; MS: removed 3/11/2014
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
  
  // in case of top we do not propagate constraints [inprecision]
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

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitAndOp(SgBitAndOp* node,
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

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitOrOp(SgBitOrOp* node,
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

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitXorOp(SgBitXorOp* node,
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
ExprAnalyzer::evalArrayReferenceOp(SgPntrArrRefExp* node,
                                 SingleEvalResultConstInt lhsResult, 
                                 SingleEvalResultConstInt rhsResult,
                                 EState estate, bool useConstraints) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  SgNode* lhs=SgNodeHelper::getLhs(node);
  
  // assume top for array elements (array elements are not stored in state)
  //cout<<"DEBUG: ARRAY-ACCESS2: ARR"<<node->unparseToString()<<"Index:"<<rhsResult.value()<<"skip:"<<getSkipArrayAccesses()<<endl;
  if(rhsResult.value().isTop()||getSkipArrayAccesses()==true) {
    // set result to top when index is top [imprecision]
    //cerr<<"DEBUG: arr-ref-exp: top!"<<endl;
    res.result=AType::Top();
    res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
    resultList.push_back(res);
    return resultList;
  } else {
    if(SgVarRefExp* varRefExp=isSgVarRefExp(lhs)) {
      const PState* pstate=estate.pstate();
      PState pstate2=*pstate; // also removes constness
      VariableId arrayVarId=_variableIdMapping->variableId(varRefExp);
      // two cases
      if(_variableIdMapping->hasArrayType(arrayVarId)) {
        // has already correct id
        // nothing to do
      } else if(_variableIdMapping->hasPointerType(arrayVarId)) {
        // in case it is a pointer retrieve pointer value
        //cout<<"DEBUG: pointer-array access!"<<endl;
        if(pstate->varExists(arrayVarId)) {
          AValue aValuePtr=pstate2[arrayVarId];
          // convert integer to VariableId
          // TODO (topify mode: does read this as integer)
          if(!aValuePtr.isConstInt()) {
            res.result=AType::Top();
            res.exprConstraints=lhsResult.exprConstraints+rhsResult.exprConstraints;
            resultList.push_back(res);
            return resultList;
          }
          int aValueInt=aValuePtr.getIntValue();
          // change arrayVarId to refered array!
          //cout<<"DEBUG: defering pointer-to-array: ptr:"<<_variableIdMapping->variableName(arrayVarId);
          arrayVarId=_variableIdMapping->variableIdFromCode(aValueInt);
          //cout<<" to "<<_variableIdMapping->variableName(arrayVarId)<<endl;//DEBUG
        } else {
          cerr<<"Error: pointer variable does not exist in PState."<<endl;
          exit(1);
        }
      } else {
        cerr<<"Error: unkown type of array or pointer."<<endl;
        exit(1);
      }
      VariableId arrayElementId;
      AValue aValue=rhsResult.value();
      int index=-1;
      if(aValue.isConstInt()) {
        index=aValue.getIntValue();
        arrayElementId=_variableIdMapping->variableIdOfArrayElement(arrayVarId,index);
        //cout<<"DEBUG: arrayElementVarId:"<<arrayElementId.toString()<<":"<<_variableIdMapping->variableName(arrayVarId)<<" Index:"<<index<<endl;
      } else {
        cerr<<"Error: array index cannot be evaluated to a constant. Not supported yet."<<endl;
        cerr<<"expr: "<<varRefExp->unparseToString()<<endl;
        exit(1);
      }
      ROSE_ASSERT(arrayElementId.isValid());
      // read value of variable var id (same as for VarRefExp - TODO: reuse)
      // TODO: check whether arrayElementId (or array) is a constant array (arrayVarId)
      if(pstate->varExists(arrayElementId)) {
        res.result=pstate2[arrayElementId];
        //cout<<"DEBUG: retrieved array element value:"<<res.result<<endl;
        if(res.result.isTop() && useConstraints) {
          AType::ConstIntLattice val=res.estate.constraints()->varConstIntLatticeValue(arrayElementId);
          res.result=val;
        }
        return listify(res);
      } else {
        // check that array is constant array (it is therefore ok that it is not in the state)
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
                //newPState.setVariableToValue(arrayElemId,CodeThorn::AValue(AType::ConstIntLattice(intVal)));
                if(elemIndex==index) {
                  AType::ConstIntLattice val=AType::ConstIntLattice(intVal);
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
          cerr<<"array-element-id: "<<arrayElementId.toString()<<" name:"<<_variableIdMapping->variableName(arrayElementId)<<endl;
          cerr<<"PState: "<<pstate->toString(_variableIdMapping)<<endl;
          cerr<<"AST: "<<node->unparseToString()<<endl;
          exit(1);
        }
      }
    } else {
      cerr<<"Error: array-access uses expr for denoting the array. Not supported yet."<<endl;
      cerr<<"expr: "<<lhs->unparseToString()<<endl;
      cerr<<"arraySkip: "<<getSkipArrayAccesses()<<endl;
      exit(1);
    }
    return resultList;
  }
  ROSE_ASSERT(false); // not reachable
}

list<SingleEvalResultConstInt> evalNotOp(SgNode* node) {
  list<SingleEvalResultConstInt> resultList;
  return resultList;
}
list<SingleEvalResultConstInt> evalUnaryMinusOp(SgNode* node) {
  list<SingleEvalResultConstInt> resultList;
  return resultList;
}
list<SingleEvalResultConstInt> evalBitNotOp(SgNode* node) {
  list<SingleEvalResultConstInt> resultList;
  return resultList;
}
list<SingleEvalResultConstInt> evalCastOp(SgNode* node) {
  list<SingleEvalResultConstInt> resultList;
  return resultList;
}

