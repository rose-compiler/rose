/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"
#include "ExprAnalyzer.h"
#include "CodeThornException.h"
#include "Analyzer.h" // dependency on process-functions

using namespace CodeThorn;
using namespace SPRAY;
using namespace Sawyer::Message;

Sawyer::Message::Facility ExprAnalyzer::logger;

ExprAnalyzer::ExprAnalyzer() {
  initDiagnostics();
}

void ExprAnalyzer::setVariableIdMapping(VariableIdMapping* variableIdMapping) {
  _variableIdMapping=variableIdMapping; 
}


void ExprAnalyzer::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::ExprAnalyzer", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
    }
}

void ExprAnalyzer::initializeStructureAccessLookup(SgProject* node) {
  ROSE_ASSERT(node);
  ROSE_ASSERT(_variableIdMapping);
  structureAccessLookup.initializeOffsets(_variableIdMapping,node);
  cout<<"INFO: Structure access lookup num of members: "<<structureAccessLookup.numOfStoredMembers()<<endl;
}

void ExprAnalyzer::setAnalyzer(Analyzer* analyzer) {
  ROSE_ASSERT(analyzer);
  _analyzer=analyzer;
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

void ExprAnalyzer::setSVCompFunctionSemantics(bool flag) {
  _svCompFunctionSemantics=flag;
}

bool ExprAnalyzer::getSVCompFunctionSemantics() {
  return _svCompFunctionSemantics;
}

bool ExprAnalyzer::stdFunctionSemantics() {
  return _stdFunctionSemantics;
}

bool ExprAnalyzer::getStdFunctionSemantics() {
  return _stdFunctionSemantics;
}

void ExprAnalyzer::setStdFunctionSemantics(bool flag) {
  _stdFunctionSemantics=flag;
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
    ROSE_ASSERT(_variableIdMapping);
    varId=_variableIdMapping->variableId(varref);
    return true;
  } else {
    VariableId defaultVarId;
    varId=defaultVarId;
    return false;
  }
}

AbstractValue ExprAnalyzer::constIntLatticeFromSgValueExp(SgValueExp* valueExp) {
  ROSE_ASSERT(valueExp);
  if(isSgFloatVal(valueExp)
     ||isSgDoubleVal(valueExp)
     ||isSgLongDoubleVal(valueExp)
     ||isSgComplexVal(valueExp)) {
    return AbstractValue(CodeThorn::Top());
  } else if(SgStringVal* stringVal=isSgStringVal(valueExp)) {
    // handle string literals
    std::string s=stringVal->get_value();
    VariableId stringValVarId=_variableIdMapping->getStringLiteralVariableId(stringVal);
    AbstractValue val=AbstractValue::createAddressOfVariable(stringValVarId);
    //cout<<"DEBUG: Found StringValue: "<<s<<": abstract value: "<<val.toString(_variableIdMapping)<<endl;
    return val;
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
  } else if(SgCharVal* exp=isSgCharVal(valueExp)) {
    unsigned char val=(unsigned char)(signed char)exp->get_value();
    return AbstractValue((int)val);
  } else if(SgWcharVal* exp=isSgWcharVal(valueExp)) {
    long int val=exp->get_value();
    return AbstractValue(val);
  } else if(isSgNullptrValExp(valueExp)) {
    return AbstractValue((int)0);
  } else if(SgEnumVal* exp=isSgEnumVal(valueExp)) {
    int val=exp->get_value();
    return AbstractValue(val);
  } else {
    string s;
    if(valueExp)
      s=valueExp->class_name();
    else
      s="nullptr";
    throw CodeThorn::Exception("Error: constIntLatticeFromSgValueExp::unsupported number type in SgValueExp ("+s+")");
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

void SingleEvalResultConstInt::init(EState estate, AbstractValue result) {
  this->estate=estate;
  this->result=result;
}

#define CASE_EXPR_ANALYZER_EVAL(ROSENODENAME,EVALFUNCTIONNAME) case V_ ## ROSENODENAME: resultList.splice(resultList.end(),EVALFUNCTIONNAME(is ## ROSENODENAME(node),lhsResult,rhsResult,estate));break

#define CASE_EXPR_ANALYZER_EVAL_UNARY_OP(ROSENODENAME,EVALFUNCTIONNAME) case V_ ## ROSENODENAME: resultList.splice(resultList.end(),EVALFUNCTIONNAME(is ## ROSENODENAME(node),operandResult,estate));break

list<SingleEvalResultConstInt> ExprAnalyzer::evaluateLExpression(SgNode* node,EState estate) {
  list<SingleEvalResultConstInt> resList;
  AbstractValue result;
  if(SgVarRefExp* varExp=isSgVarRefExp(node)) {
#if 0
    result=computeAbstractAddress(varExp);
#else
    return evalLValueVarRefExp(varExp,estate);
#endif
  } else {
    cerr<<"Error: unsupported lvalue expression: "<<node->unparseToString()<<endl;
    cerr<<"     : "<<SgNodeHelper::sourceLineColumnToString(node)<<" : "<<AstTerm::astTermWithNullValuesToString(node)<<endl;
    exit(1);
  }
  SingleEvalResultConstInt res;
  res.init(estate,result);
  resList.push_back(res);
  return resList;
}

bool ExprAnalyzer::isLValueOp(SgNode* node) {
  // assign operators not included yet
  return isSgAddressOfOp(node)
    || SgNodeHelper::isPrefixIncDecOp(node)
    || SgNodeHelper::isPostfixIncDecOp(node)
    ;
}

//#define NEW_SEMANTICS_MODELLING

list<SingleEvalResultConstInt> ExprAnalyzer::evaluateExpression(SgNode* node,EState estate) {
  ROSE_ASSERT(estate.pstate()); // ensure state exists
  // initialize with default values from argument(s)
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=AbstractValue(CodeThorn::Bot());
#if 0
  if(SgNodeHelper::isPostfixIncDecOp(node)) {
    cout << "Error: incdec-op not supported in conditions."<<endl;
    exit(1);
  }
#endif

  if(SgConditionalExp* condExp=isSgConditionalExp(node)) {
    return evalConditionalExpr(condExp,estate);
  }

  if(dynamic_cast<SgBinaryOp*>(node)) {
    SgNode* lhs=SgNodeHelper::getLhs(node);
    list<SingleEvalResultConstInt> lhsResultList=evaluateExpression(lhs,estate);
    SgNode* rhs=SgNodeHelper::getRhs(node);
    list<SingleEvalResultConstInt> rhsResultList=evaluateExpression(rhs,estate);
    list<SingleEvalResultConstInt> resultList;
    for(list<SingleEvalResultConstInt>::iterator liter=lhsResultList.begin();
        liter!=lhsResultList.end();
        ++liter) {
      for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
          riter!=rhsResultList.end();
          ++riter) {
        SingleEvalResultConstInt lhsResult=*liter;
        SingleEvalResultConstInt rhsResult=*riter;

        // handle binary pointer operators
        switch(node->variantT()) {
          CASE_EXPR_ANALYZER_EVAL(SgArrowExp,evalArrowOp);
          CASE_EXPR_ANALYZER_EVAL(SgDotExp,evalDotOp);
        default:
          // fall through;
          ;
        }
        if(node->variantT()==V_SgArrowExp||node->variantT()==V_SgDotExp) {
          return resultList;
        }
        
        // for all other binary operators (which are not pointer operators)
        // prepare for binary non-pointer operators
#ifdef NEW_SEMANTICS_MODELLING
        // read if an address otherwise use as value
        if(!lhsResult.result.isConstInt()&&!lhsResult.result.isTop()) {
          cout<<"DEBUG: reading from "<<lhsResult.result.toString(_variableIdMapping)<<endl;
          lhsResult.result=estate.pstate()->readFromMemoryLocation(lhsResult.result);
          cout<<"DEBUG: read value: "<<lhsResult.result.toString(_variableIdMapping)<<endl;
        }
        if(!rhsResult.result.isConstInt()&&!rhsResult.result.isTop()) {
          rhsResult.result=estate.pstate()->readFromMemoryLocation(rhsResult.result);
        }
#else
        // nothing todo
#endif
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
          throw CodeThorn::Exception("Error: evaluateExpression::unknown binary operation.");
        }
      }
    }
    return resultList;
  }
  
  if(isLValueOp(node)) {
    SgNode* child=SgNodeHelper::getFirstChild(node);
    list<SingleEvalResultConstInt> operandResultList=evaluateLExpression(child,estate);
    //assert(operandResultList.size()==1);
    list<SingleEvalResultConstInt> resultList;
    for(auto oiter:operandResultList) {
      SingleEvalResultConstInt operandResult=oiter;
      switch(node->variantT()) {
        // covers same operators as isLValueOp
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgAddressOfOp,evalAddressOfOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgPlusPlusOp,evalPlusPlusOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgMinusMinusOp,evalMinusMinusOp);
        // SgPointerDerefExp??
      default:
        ; // nothing to do, fall through to next loop on unary ops
      }
    }
    return resultList;
  }

  if(dynamic_cast<SgUnaryOp*>(node)) {
    SgNode* child=SgNodeHelper::getFirstChild(node);
    list<SingleEvalResultConstInt> operandResultList=evaluateExpression(child,estate);
    list<SingleEvalResultConstInt> resultList;
    for(auto oiter:operandResultList) {
      SingleEvalResultConstInt operandResult=oiter;
      switch(node->variantT()) {
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgNotOp,evalNotOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgCastExp,evalCastOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgBitComplementOp,evalBitwiseComplementOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgMinusOp,evalUnaryMinusOp);
        CASE_EXPR_ANALYZER_EVAL_UNARY_OP(SgPointerDerefExp,evalDereferenceOp);
      default:
        cerr << "@NODE:"<<node->sage_class_name()<<endl;
        string exceptionInfo=string("Error: evaluateExpression::unknown unary operation @")+string(node->sage_class_name());
        throw exceptionInfo; 
      } // end switch
    }
    return  resultList;
  }
  
  ROSE_ASSERT(!dynamic_cast<SgBinaryOp*>(node) && !dynamic_cast<SgUnaryOp*>(node));
  
  // ALL REMAINING CASES DO NOT GENERATE CONSTRAINTS
  // EXPRESSION LEAF NODES
  // this test holds for all subclasses of SgValueExp

  // special case sizeof operator (operates on types and types of expressions)
  if(SgSizeOfOp* sizeOfOp=isSgSizeOfOp(node)) {
    return evalSizeofOp(sizeOfOp,estate);
  }
  if(SgValueExp* exp=isSgValueExp(node)) {
    ROSE_ASSERT(exp!=nullptr);
    return evalValueExp(exp,estate);
  }
  switch(node->variantT()) {
  case V_SgVarRefExp:
    return evalRValueVarRefExp(isSgVarRefExp(node),estate);
  case V_SgFunctionCallExp: {
    //cout<<"DEBUG: SgFunctionCall detected in subexpression."<<endl;
    return evalFunctionCall(isSgFunctionCallExp(node),estate);
  }
  default:
    throw CodeThorn::Exception("Error: evaluateExpression::unknown node in expression ("+string(node->sage_class_name())+")");
  } // end of switch
  throw CodeThorn::Exception("Error: evaluateExpression failed.");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// EVAL FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////

// evaluation functions
list<SingleEvalResultConstInt> ExprAnalyzer::evalConditionalExpr(SgConditionalExp* condExp, EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SgExpression* cond=condExp->get_conditional_exp();
  list<SingleEvalResultConstInt> condResultList=evaluateExpression(cond,estate);
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
      //cout<<"Info: evaluating condition of conditional operator gives two equal results"<<endl;
    }
  }
  if(condResultList.size()>1) {
    cerr<<"Error: evaluating condition of conditional operator gives more than one result. Not supported yet."<<endl;
    exit(1);
  }
  SingleEvalResultConstInt singleResult=*condResultList.begin();
  if(singleResult.result.isTop()) {
    SgExpression* trueBranch=condExp->get_true_exp();
    list<SingleEvalResultConstInt> trueBranchResultList=evaluateExpression(trueBranch,estate);
    SgExpression* falseBranch=condExp->get_false_exp();
    list<SingleEvalResultConstInt> falseBranchResultList=evaluateExpression(falseBranch,estate);
    // append falseBranchResultList to trueBranchResultList (moves elements), O(1).
    trueBranchResultList.splice(trueBranchResultList.end(), falseBranchResultList); 
    return trueBranchResultList;
  } else if(singleResult.result.isTrue()) {
    SgExpression* trueBranch=condExp->get_true_exp();
    list<SingleEvalResultConstInt> trueBranchResultList=evaluateExpression(trueBranch,estate);
    return trueBranchResultList;
  } else if(singleResult.result.isFalse()) {
    SgExpression* falseBranch=condExp->get_false_exp();
    list<SingleEvalResultConstInt> falseBranchResultList=evaluateExpression(falseBranch,estate);
    return falseBranchResultList;
  } else {
    cerr<<"Error: evaluating conditional operator inside expressions - unknown behavior (condition may have evaluated to bot)."<<endl;
    exit(1);
  }
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalEqualOp(SgEqualityOp* node,
                                                         SingleEvalResultConstInt lhsResult, 
                                                         SingleEvalResultConstInt rhsResult,
                                                         EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorEq(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalNotEqualOp(SgNotEqualOp* node,
                                                            SingleEvalResultConstInt lhsResult, 
                                                            SingleEvalResultConstInt rhsResult,
                                                            EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorNotEq(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalAndOp(SgAndOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  //cout << "SgAndOp: "<<lhsResult.result.toString()<<"&&"<<rhsResult.result.toString()<<" ==> ";
  res.result=(lhsResult.result.operatorAnd(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalOrOp(SgOrOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=lhsResult.result.operatorOr(rhsResult.result);

  // encode short-circuit CPP-OR-semantics
  if(lhsResult.result.isTrue()) {
    res.result=lhsResult.result;
  } 
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalAddOp(SgAddOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result+rhsResult.result);
  resultList.push_back(res);
  return resultList;
}
list<SingleEvalResultConstInt> ExprAnalyzer::evalSubOp(SgSubtractOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result-rhsResult.result);
  resultList.push_back(res);
  return resultList;
}
list<SingleEvalResultConstInt> ExprAnalyzer::evalMulOp(SgMultiplyOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate) {

  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result*rhsResult.result);
  resultList.push_back(res);
  return resultList;
}
list<SingleEvalResultConstInt> ExprAnalyzer::evalDivOp(SgDivideOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate) {

  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result/rhsResult.result);
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalModOp(SgModOp* node,
                                                      SingleEvalResultConstInt lhsResult, 
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate) {

  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result%rhsResult.result);
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitwiseAndOp(SgBitAndOp* node,
                                                              SingleEvalResultConstInt lhsResult, 
                                                              SingleEvalResultConstInt rhsResult,
                                                              EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorBitwiseAnd(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitwiseOrOp(SgBitOrOp* node,
                                                             SingleEvalResultConstInt lhsResult, 
                                                             SingleEvalResultConstInt rhsResult,
                                                             EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorBitwiseOr(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitwiseXorOp(SgBitXorOp* node,
                                                              SingleEvalResultConstInt lhsResult, 
                                                              SingleEvalResultConstInt rhsResult,
                                                              EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorBitwiseXor(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> 
ExprAnalyzer::evalGreaterOrEqualOp(SgGreaterOrEqualOp* node,
                                   SingleEvalResultConstInt lhsResult, 
                                   SingleEvalResultConstInt rhsResult,
                                   EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorMoreOrEq(rhsResult.result));
  if(args.getBool("relop-constraints")) {
    if(res.result.isTop())
      throw CodeThorn::Exception("Error: Top found in relational operator (not supported yet).");
  }
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> 
ExprAnalyzer::evalGreaterThanOp(SgGreaterThanOp* node,
                                SingleEvalResultConstInt lhsResult, 
                                SingleEvalResultConstInt rhsResult,
                                EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorMore(rhsResult.result));
  if(args.getBool("relop-constraints")) {
    if(res.result.isTop())
      throw CodeThorn::Exception("Error: Top found in relational operator (not supported yet).");
  }
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalLessOrEqualOp(SgLessOrEqualOp* node,
                                SingleEvalResultConstInt lhsResult, 
                                SingleEvalResultConstInt rhsResult,
                                EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorLessOrEq(rhsResult.result));
  if(args.getBool("relop-constraints")) {
    if(res.result.isTop())
      throw CodeThorn::Exception("Error: Top found in relational operator (not supported yet).");
  }
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalLessThanOp(SgLessThanOp* node,
                             SingleEvalResultConstInt lhsResult, 
                             SingleEvalResultConstInt rhsResult,
                             EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorLess(rhsResult.result));
  if(args.getBool("relop-constraints")) {
    if(res.result.isTop())
      throw CodeThorn::Exception("Error: Top found in relational operator (not supported yet).");
  }
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalBitwiseShiftLeftOp(SgLshiftOp* node,
                             SingleEvalResultConstInt lhsResult, 
                             SingleEvalResultConstInt rhsResult,
                             EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorBitwiseShiftLeft(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalBitwiseShiftRightOp(SgRshiftOp* node,
                             SingleEvalResultConstInt lhsResult, 
                             SingleEvalResultConstInt rhsResult,
                             EState estate) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorBitwiseShiftRight(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> 
ExprAnalyzer::evalArrayReferenceOp(SgPntrArrRefExp* node,
                                 SingleEvalResultConstInt arrayExprResult, 
                                 SingleEvalResultConstInt indexExprResult,
                                 EState estate) {
  //cout<<"DEBUG: evalArrayReferenceOp: "<<node->unparseToString()<<endl;
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  SgNode* arrayExpr=SgNodeHelper::getLhs(node);
 
  if(indexExprResult.value().isTop()||getSkipArrayAccesses()==true) {
    // set result to top when index is top [imprecision]
    // assume top for array elements if skipped
    res.result=CodeThorn::Top();
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
        //cout<<"DEBUG: pointer-array access."<<endl;
        if(pstate->varExists(arrayVarId)) {
          //cout<<"DEBUG: arrayPtrValue read from memory."<<endl;
          arrayPtrValue=pstate2.readFromMemoryLocation(arrayVarId); // pointer value (without index)
          if(!(arrayPtrValue.isTop()||arrayPtrValue.isBot()||arrayPtrValue.isPtr()||arrayPtrValue.isNullPtr())) {
            cout<<"Error: value not a pointer value: "<<arrayPtrValue.toString()<<endl;
            cout<<estate.toString(_variableIdMapping)<<endl;
            exit(1);
          }
        } else {
          cerr<<"Error: pointer variable does not exist in PState."<<endl;
          exit(1);
        }
      } else {
        cerr<<"Error: unknown type of array or pointer."<<endl;
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
      if(pstate->varExists(arrayPtrValue)) {
        //cout<<"DEBUG: ARRAY PTR VALUE IN STATE (OK!)."<<endl;
      }
      if(pstate->varExists(arrayPtrPlusIndexValue)) {
#ifdef NEW_SEMANTICS_MODELLING
        //res.result=pstate2.readFromMemoryLocation(arrayPtrPlusIndexValue);
        res.result=arrayPtrPlusIndexValue;
#else
        // return address of denoted memory location
        res.result=pstate2.readFromMemoryLocation(arrayPtrPlusIndexValue);
#endif
        //cout<<"DEBUG: retrieved array element value:"<<res.result<<endl;
        return listify(res);
      } else {
        // array variable NOT in state. Special space optimization case for constant array.
        if(_variableIdMapping->hasArrayType(arrayVarId) && args.getBool("explicit-arrays")==false) {
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
                //newPState.writeToMemoryLocation(arrayElemId,CodeThorn::AbstractValue(AbstractValue(intVal)));
                int index2=arrayPtrPlusIndexValue.getIndexIntValue();
                if(elemIndex==index2) {
                  AbstractValue val=AbstractValue(intVal);
                  res.result=val;
                  return listify(res);
                }
              } else {
                cerr<<"Error: unsupported array initializer value:"<<exp->unparseToString()<<" AST:"<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
                exit(1);
              }
            } else {
              cerr<<"Error: no assign initialize:"<<exp->unparseToString()<<" AST:"<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
              exit(1);
            }
            elemIndex++;
          }
          cerr<<"Error: access to element of constant array (not in state). Not supported."<<endl;
          exit(1);
        } else if(_variableIdMapping->isStringLiteralAddress(arrayVarId)) {
          cout<<"TODO: Found string literal address. Data not present in state yet. skipping for now, returning top"<<endl;
          res.result=CodeThorn::Top();
          return listify(res);
        } else {
          //cout<<estate.toString(_variableIdMapping)<<endl;
          //cout<<"DEBUG: Program error detected: potential out of bounds access 1 : array: "<<arrayPtrValue.toString(_variableIdMapping)<<", access: address: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
          //cout<<"DEBUG: array-element: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
          //cerr<<"PState: "<<pstate->toString(_variableIdMapping)<<endl;
          //cerr<<"AST: "<<node->unparseToString()<<endl;
          //cerr<<"explicit arrays flag: "<<args.getBool("explicit-arrays")<<endl;
#ifdef NEW_SEMANTICS_MODELLING
          // null pointer dereference is check in the read function
#else
          _nullPointerDereferenceLocations.recordPotentialDereference(estate.label());
#endif
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
                                                       EState estate) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=operandResult.result.operatorNot();
  // do *not* invert the constraints, instead negate the operand result (TODO: investigate)
  return listify(res);
}
list<SingleEvalResultConstInt> ExprAnalyzer::evalUnaryMinusOp(SgMinusOp* node, 
                                                              SingleEvalResultConstInt operandResult, 
                                                              EState estate) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=operandResult.result.operatorUnaryMinus();
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalSizeofOp(SgSizeOfOp* node, 
                                                              EState estate) {
  
  SgType* operandType=node->get_operand_type();
  ROSE_ASSERT(operandType);
  SPRAY::TypeSize typeSize=AbstractValue::getTypeSizeMapping()->determineTypeSize(operandType);
  AbstractValue sizeValue=AbstractValue(typeSize); 
  //AbstractValue sizeValue=AbstractValue(4); 
  SingleEvalResultConstInt res;
  res.init(estate,sizeValue);
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalCastOp(SgCastExp* node, 
                                                        SingleEvalResultConstInt operandResult, 
                                                        EState estate) {
  SingleEvalResultConstInt res;
  // TODO: model effect of cast when sub language is extended
  //SgCastExp* castExp=isSgCastExp(node);
  //res.estate=estate;
  //res.result=operandResult.result;
  res.init(estate,operandResult.result);
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitwiseComplementOp(SgBitComplementOp* node, 
                                                                     SingleEvalResultConstInt operandResult, 
                                                                     EState estate) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=operandResult.result.operatorBitwiseComplement();
  return listify(res);
}

AbstractValue ExprAnalyzer::computeAbstractAddress(SgVarRefExp* varRefExp) {
  VariableId varId=_variableIdMapping->variableId(varRefExp);
  return AbstractValue(varId);
}
  
list<SingleEvalResultConstInt> ExprAnalyzer::evalArrowOp(SgArrowExp* node,
                                                         SingleEvalResultConstInt lhsResult, 
                                                         SingleEvalResultConstInt rhsResult,
                                                         EState estate) {
  throw CodeThorn::Exception("Error: arrow operator not supported yet: "+node->unparseToString());
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  // L->R : L evaluates to pointer value (address), R evaluates to offset value (a struct member always evaluates to an offset)
  AbstractValue address=AbstractValue::operatorAdd(lhsResult.result,rhsResult.result);
  res.result=estate.pstate()->readFromMemoryLocation(address);
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalDotOp(SgDotExp* node,
                                                       SingleEvalResultConstInt lhsResult, 
                                                       SingleEvalResultConstInt rhsResult,
                                                       EState estate) {
  //throw CodeThorn::Exception("Error: dot operator not supported yet: "+node->unparseToString());
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  // L.R : L evaluates to address, R evaluates to offset value (a struct member always evaluates to an offset)
  AbstractValue address=AbstractValue::operatorAdd(lhsResult.result,rhsResult.result);
  // only if rhs is *not* a dot-operator, needs the value be
  // read. Otherwise this is not the end of the access path and only the address is computed.
  if(!isSgDotExp(SgNodeHelper::getRhs(node))) {
    cout<<"DEBUG: dot op: reading from struct location."<<address.toString(_variableIdMapping)<<endl;
    res.result=estate.pstate()->readFromMemoryLocation(address);
  }
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalAddressOfOp(SgAddressOfOp* node, 
                                                             SingleEvalResultConstInt operandResult, 
                                                             EState estate) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  AbstractValue operand=operandResult.result;
  //cout<<"DEBUG: addressOfOpValue: "<<addressOfOperandValue.toRhsString(_variableIdMapping);
  // AbstractValue of a VariableId is a pointer to this variable.
#ifdef NEW_SEMANTICS_MODELLING
  // all memory locations are modelled as addresses. Therefore the address operator is a no-op here.
  res.result=AbstractValue(operand);
#else
  res.result=AbstractValue(operand.getVariableId());
#endif
  return listify(res);
}


list<SingleEvalResultConstInt> ExprAnalyzer::semanticEvalDereferenceOp(SingleEvalResultConstInt operandResult, 
                                                                       EState estate) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  AbstractValue derefOperandValue=operandResult.result;
  //cout<<"DEBUG: derefOperandValue: "<<derefOperandValue.toRhsString(_variableIdMapping);
  // null pointer check
  if(derefOperandValue.isTop()) {
    recordPotentialNullPointerDereferenceLocation(estate.label());
  } else if(derefOperandValue.isConstInt()) {
    int ptrIntVal=derefOperandValue.getIntValue();
    if(ptrIntVal==0) {
      recordDefinitiveNullPointerDereferenceLocation(estate.label());
    }
  }
  res.result=estate.pstate()->readFromMemoryLocation(derefOperandValue);
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalDereferenceOp(SgPointerDerefExp* node, 
                                                              SingleEvalResultConstInt operandResult, 
                                                              EState estate) {
  return semanticEvalDereferenceOp(operandResult,estate);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalPreComputationOp(EState estate, AbstractValue address, AbstractValue change) {
  SingleEvalResultConstInt res;
  AbstractValue oldValue=estate.pstate()->readFromMemoryLocation(address);
  AbstractValue newValue=oldValue+change;
  PState newPState=*estate.pstate();
  newPState.writeToMemoryLocation(address,newValue);
  ConstraintSet cset; // use empty cset (in prep to remove it)
  ROSE_ASSERT(_analyzer);
  res.init(_analyzer->createEState(estate.label(),newPState,cset),newValue);
  return listify(res);
  }

list<SingleEvalResultConstInt> ExprAnalyzer::evalPostComputationOp(EState estate, AbstractValue address, AbstractValue change) {
  // TODO change from precomp to postcomp
  SingleEvalResultConstInt res;
  AbstractValue oldValue=estate.pstate()->readFromMemoryLocation(address);
  AbstractValue newValue=oldValue+change;
  PState newPState=*estate.pstate();
  newPState.writeToMemoryLocation(address,newValue);
  ConstraintSet cset; // use empty cset (in prep to remove it)
  ROSE_ASSERT(_analyzer);
  res.init(_analyzer->createEState(estate.label(),newPState,cset),oldValue);
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalPreIncrementOp(SgPlusPlusOp* node, 
								SingleEvalResultConstInt operandResult, 
								EState estate) {
  AbstractValue address=operandResult.result;
  ROSE_ASSERT(address.isPtr());
  AbstractValue change=1;
  return evalPreComputationOp(estate,address,change);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalPreDecrementOp(SgMinusMinusOp* node, 
								SingleEvalResultConstInt operandResult, 
								EState estate) {
  AbstractValue address=operandResult.result;
  ROSE_ASSERT(address.isPtr());
  AbstractValue change=-1;
  return evalPreComputationOp(estate,address,change);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalPostIncrementOp(SgPlusPlusOp* node, 
								 SingleEvalResultConstInt operandResult, 
								 EState estate) {
  AbstractValue address=operandResult.result;
  ROSE_ASSERT(address.isPtr());
  AbstractValue change=1;
  return evalPostComputationOp(estate,address,change);
}


list<SingleEvalResultConstInt> ExprAnalyzer::evalPostDecrementOp(SgMinusMinusOp* node, 
								 SingleEvalResultConstInt operandResult, 
								 EState estate) {
  AbstractValue address=operandResult.result;
  ROSE_ASSERT(address.isPtr());
  AbstractValue change=-1;
  return evalPostComputationOp(estate,address,change);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalPlusPlusOp(SgPlusPlusOp* node, 
                                                            SingleEvalResultConstInt operandResult, 
                                                            EState estate) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  if(SgNodeHelper::isPrefixIncDecOp(node)) {
    // preincrement ++E
    return evalPreIncrementOp(node,operandResult,estate);
   } else if(SgNodeHelper::isPostfixIncDecOp(node)) {
    // postincrement E++
    return evalPostIncrementOp(node,operandResult,estate);
  }
  throw CodeThorn::Exception("Interal error: ExprAnalyzer::evalPlusPlusOp: "+node->unparseToString());
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalMinusMinusOp(SgMinusMinusOp* node, 
                                                              SingleEvalResultConstInt operandResult, 
                                                              EState estate) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  if(SgNodeHelper::isPrefixIncDecOp(node)) {
    // predecrement --E
    return evalPreDecrementOp(node,operandResult,estate);
  } else if(SgNodeHelper::isPostfixIncDecOp(node)) {
    // postdecrement E--
    return evalPostDecrementOp(node,operandResult,estate);
  }
  throw CodeThorn::Exception("Internal error: ExprAnalyzer::evalMinusMinusOp: "+node->unparseToString());
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalLValuePntrArrRefExp(SgPntrArrRefExp* node, EState estate) {
  // for now we ignore array refs on lhs
  // TODO: assignments in index computations of ignored array ref
  // see ExprAnalyzer.C: case V_SgPntrArrRefExp:
  // since nothing can change (because of being ignored) state remains the same
  PState oldPState=*estate.pstate();
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue(CodeThorn::Bot()));
  if(getSkipArrayAccesses()) {
    // TODO: remove constraints on array-element(s) [currently no constraints are computed for arrays]
    res.result=CodeThorn::Top();
    return listify(res);
  } else {
    SgExpression* arrExp=isSgExpression(SgNodeHelper::getLhs(node));
    SgExpression* indexExp=isSgExpression(SgNodeHelper::getRhs(node));
    if(SgVarRefExp* varRefExp=isSgVarRefExp(arrExp)) {
      PState pstate2=oldPState;
      VariableId arrayVarId=_variableIdMapping->variableId(varRefExp);
      AbstractValue arrayPtrValue;
      // two cases
      if(_variableIdMapping->hasArrayType(arrayVarId)) {
        // create array element 0 (in preparation to have index added, or, if not index is used, it is already the correct index (=0).
        arrayPtrValue=AbstractValue::createAddressOfArray(arrayVarId);
      } else if(_variableIdMapping->hasPointerType(arrayVarId)) {
        // in case it is a pointer retrieve pointer value
        AbstractValue ptr=AbstractValue::createAddressOfArray(arrayVarId);
        if(pstate2.varExists(ptr)) {
          //cout<<"DEBUG: pointer exists (OK): "<<ptr.toString(_variableIdMapping)<<endl;
          arrayPtrValue=pstate2.readFromMemoryLocation(ptr);
          //cout<<"DEBUG: arrayPtrValue: "<<arrayPtrValue.toString(_variableIdMapping)<<endl;
          // convert integer to VariableId
          if(arrayPtrValue.isTop()||arrayPtrValue.isBot()) {
            logger[ERROR] <<"Error: unsupported feature:"<<SgNodeHelper::sourceLineColumnToString(node)<<": "<<node->unparseToString()<<arrayPtrValue.toString(_variableIdMapping)<<" array index is top or bot. Not supported yet."<<endl;
            exit(1);
          }
          // logger[DEBUG]<<"defering pointer-to-array: ptr:"<<_variableIdMapping->variableName(arrayVarId);
        } else {
          logger[ERROR] <<"lhs array access: pointer variable does not exist in PState."<<endl;
          exit(1);
        }
          } else {
        logger[ERROR] <<"lhs array access: unknown type of array or pointer."<<endl;
        exit(1);
      }
      AbstractValue arrayElementAddress;
      //AbstractValue aValue=(*i).value();
      list<SingleEvalResultConstInt> resIntermediate=evaluateExpression(indexExp,estate);
      ROSE_ASSERT(resIntermediate.size()==1); // TODO: temporary restriction
      AbstractValue indexValue=(*(resIntermediate.begin())).value();
      AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrayPtrValue,indexValue);
      logger[TRACE]<<"DEBUG: arrayPtrPlusIndexValue: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
      
      // TODO: rewrite to use AbstractValue only
      {
        VariableId arrayVarId2=arrayPtrPlusIndexValue.getVariableId();
        int index2=arrayPtrPlusIndexValue.getIndexIntValue();
        if(!checkArrayBounds(arrayVarId2,index2)) {
          // TODO: add to analysis report (do not print)
          cerr<<"Program error detected at "<<SgNodeHelper::sourceLineColumnToString(node)<<" : write access out of bounds."<<endl;
        }
      }

      arrayElementAddress=arrayPtrPlusIndexValue;
      //cout<<"DEBUG: arrayElementAddress: "<<arrayElementAddress.toString(_variableIdMapping)<<endl;
      //logger[TRACE]<<"arrayElementVarId:"<<arrayElementAddress.toString()<<":"<<_variableIdMapping->variableName(arrayVarId)<<" Index:"<<index<<endl;
      ROSE_ASSERT(!arrayElementAddress.isBot());
      // read value of variable var id (same as for VarRefExp - TODO: reuse)
      // TODO: check whether arrayElementAddress (or array) is a constant array (arrayVarId)
      if(!pstate2.varExists(arrayElementAddress)) {
        // check that array is constant array (it is therefore ok that it is not in the state)
        //logger[TRACE] <<"lhs array-access index does not exist in state (creating it as address now). Array element id:"<<arrayElementAddress.toString(_variableIdMapping)<<" PState size:"<<pstate2.size()<<endl;
      }
      res.result=arrayElementAddress;
      return listify(res);
    } else {
      logger[ERROR] <<"array-access uses expr for denoting the array. Normalization missing."<<endl;
      logger[ERROR] <<"expr: "<<node->unparseToString()<<endl;
      logger[ERROR] <<"arraySkip: "<<getSkipArrayAccesses()<<endl;
      exit(1);
    }
  }
  // unreachable
  ROSE_ASSERT(false);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalLValueVarRefExp(SgVarRefExp* node, EState estate) {
  logger[TRACE]<<"DEBUG: evalLValueVarRefExp: "<<node->unparseToString()<<" EState label:"<<estate.label().toString()<<endl;
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue(CodeThorn::Bot()));
  const PState* pstate=estate.pstate();
  VariableId varId=_variableIdMapping->variableId(node);
  if(isStructMember(varId)) {
    int offset=structureAccessLookup.getOffset(varId);
    ROSE_ASSERT(_variableIdMapping);
    cout<<"DEBUG: evalLValueVarRefExp found STRUCT member: "<<_variableIdMapping->variableName(varId)<<" offset: "<<offset<<endl;
    //res.result=AbstractValue(offset);
    //return listify(res);
  }
  if(pstate->varExists(varId)) {
    if(_variableIdMapping->hasArrayType(varId)) {
      logger[TRACE]<<"DEBUG: lvalue array address(?): "<<node->unparseToString()<<"EState label:"<<estate.label().toString()<<endl;
      res.result=AbstractValue::createAddressOfArray(varId);
    } else {
      res.result=AbstractValue::createAddressOfVariable(varId);
    }
    return listify(res);
  } else {
    // special mode to represent information not stored in the state
    // i) unmodified arrays: data can be stored outside the state
    // ii) undefined variables mapped to 'top' (abstraction by removing variables from state)
    if(_variableIdMapping->hasArrayType(varId) && args.getBool("explicit-arrays")==false) {
      // variable is used on the rhs and it has array type implies it avalates to a pointer to that array
      //res.result=AbstractValue(varId.getIdCode());
      logger[TRACE]<<"DEBUG: lvalue array address (non-existing in state)(?): "<<node->unparseToString()<<endl;
      res.result=AbstractValue::createAddressOfArray(varId);
      return listify(res);
    } else {
      res.result=CodeThorn::Top();
      //cerr << "WARNING: variable not in PState (var="<<_variableIdMapping->uniqueVariableName(varId)<<"). Initialized with top."<<endl;
      return listify(res);
    }
  }
  // unreachable
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalRValueVarRefExp(SgVarRefExp* node, EState estate) {
  //cout<<"DEBUG: evalRValueVarRefExp: "<<node->unparseToString()<<endl;
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue(CodeThorn::Bot()));
  const PState* pstate=estate.pstate();
  VariableId varId=_variableIdMapping->variableId(node);
  ROSE_ASSERT(varId.isValid());
  // check if var is a struct member. if yes return struct-offset, otherwise continue.
  if(_variableIdMapping->hasClassType(varId)) {
    // TODO - not supported yet
    cout<<"DEBUG: has Class type."<<endl;
    res.result=AbstractValue::createAddressOfVariable(varId);
    return listify(res);
  }
#if 1
  if(isStructMember(varId)) {
    int offset=structureAccessLookup.getOffset(varId);
    ROSE_ASSERT(_variableIdMapping);
    cout<<"DEBUG: evalRValueVarRefExp found STRUCT member: "<<_variableIdMapping->variableName(varId)<<" offset: "<<offset<<endl;
    res.result=AbstractValue(offset);
    return listify(res);
  }
#endif
  if(pstate->varExists(varId)) {
    if(_variableIdMapping->hasArrayType(varId)) {
      res.result=AbstractValue::createAddressOfArray(varId);
    } else {
#ifdef NEW_SEMANTICS_MODELLING
      res.result=AbstractValue::createAddressOfVariable(varId);
#else
      res.result=const_cast<PState*>(pstate)->readFromMemoryLocation(varId);
#endif
    }
    return listify(res);
  } else {
    // special mode to represent information not stored in the state
    // i) unmodified arrays: data can be stored outside the state
    // ii) undefined variables mapped to 'top' (abstraction by removing variables from state)
    if(_variableIdMapping->hasArrayType(varId) && args.getBool("explicit-arrays")==false) {
      // variable is used on the rhs and it has array type implies it avalates to a pointer to that array
      //res.result=AbstractValue(varId.getIdCode());
      res.result=AbstractValue::createAddressOfArray(varId);
      return listify(res);
    } else {
      res.result=CodeThorn::Top();
      //cerr << "WARNING: variable not in PState (var="<<_variableIdMapping->uniqueVariableName(varId)<<"). Initialized with top."<<endl;
      return listify(res);
    }
  }
  // unreachable
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalValueExp(SgValueExp* node, EState estate) {
  ROSE_ASSERT(node);
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue(CodeThorn::Bot()));
  res.result=constIntLatticeFromSgValueExp(node);
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalFunctionCallArguments(SgFunctionCallExp* funCall, EState estate) {
  SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  for (auto arg : argsList) {
    //cout<<"DEBUG: functioncall argument: "<<arg->unparseToString()<<endl;
    // Requirement: code is normalized, does not contain state modifying operations in function arguments
    list<SingleEvalResultConstInt> resList=evaluateExpression(arg,estate);
    //cout<<"DEBUG: resList.size()"<<resList.size()<<endl;
  }
  SingleEvalResultConstInt res;
  AbstractValue evalResultValue=CodeThorn::Top();
  res.init(estate,evalResultValue);
 
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalFunctionCall(SgFunctionCallExp* funCall, EState estate) {
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue(CodeThorn::Top()));
  if(getStdFunctionSemantics()) {
    string funName=SgNodeHelper::getFunctionName(funCall);
    if(funName=="malloc") {
      return evalFunctionCallMalloc(funCall,estate);
    } else if(funName=="memcpy") {
      return evalFunctionCallMemCpy(funCall,estate);
    } else if(funName=="free") {
      return evalFunctionCallFree(funCall,estate);
    } else if(funName=="fflush") {
      // ignoring fflush
      // res initialized above
      return evalFunctionCallArguments(funCall,estate);
    }
  }
  if(getSkipSelectedFunctionCalls()) {
    return evalFunctionCallArguments(funCall,estate);
  } else {
    string s=funCall->unparseToString();
    throw CodeThorn::Exception("Unknown semantics of function call inside expression: "+s);
  }
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalFunctionCallMalloc(SgFunctionCallExp* funCall, EState estate) {
  // create two cases: (i) allocation successful, (ii) allocation fails (null pointer is returned, and no memory is allocated).
  SingleEvalResultConstInt res;
  static int memorylocid=0; // to be integrated in VariableIdMapping
  memorylocid++;
  stringstream ss;
  ss<<"$MEM"<<memorylocid;
  ROSE_ASSERT(_variableIdMapping);
  SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  if(argsList.size()==1) {
    // (i) create state for successful allocation of memory (do not reserve memory yet, only pointer is reserved and size of memory is recorded)
    // memory is allocated when written to it. Otherwise it is assumed to be uninitialized
    SgExpression* arg1=*argsList.begin();
    list<SingleEvalResultConstInt> resList=evaluateExpression(arg1,estate);
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
    list<SingleEvalResultConstInt> resList2;
    memLocVarId=_variableIdMapping->createAndRegisterNewMemoryRegion(ss.str(),memoryRegionSize);
    AbstractValue allocatedMemoryPtr=AbstractValue::createAddressOfArray(memLocVarId);
    res.init(estate,allocatedMemoryPtr);
    //cout<<"DEBUG: evaluating function call malloc:"<<funCall->unparseToString()<<endl;
    ROSE_ASSERT(allocatedMemoryPtr.isPtr());
    //cout<<"Generated malloc-allocated mem-chunk pointer is OK."<<endl;
    // create resList with two states now
    resList2.push_back(res);

#if 0
    // (ii) add memory allocation case: null pointer (allocation failed)
    SingleEvalResultConstInt resNullPtr;
    AbstractValue nullPtr=AbstractValue::createNullPtr();
    resNullPtr.init(estate,nullPtr);
    resList2.push_back(resNullPtr);
#endif
    return resList2;
  } else {
    // this will become an error in future
    cerr<<"WARNING: unknown malloc function "<<funCall->unparseToString()<<endl;
  }
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalFunctionCallFree(SgFunctionCallExp* funCall, EState estate) {
  SingleEvalResultConstInt res;
  SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  if(argsList.size()==1) {
    SgExpression* arg1=*argsList.begin();
    list<SingleEvalResultConstInt> resList=evaluateExpression(arg1,estate);
    if(resList.size()!=1) {
      cerr<<"Error: conditional control-flow in function argument expression not supported. Expression normalization required."<<endl;
      exit(1);
    }
    SingleEvalResultConstInt sres=*resList.begin();
    AbstractValue arg1val=sres.result;
    if(arg1val.isPtr()) {
      int memoryRegionSize=getMemoryRegionSize(arg1val);
      // can be marked as deallocated (currently this does not impact the analysis)
      //variableIdMapping->setSize(arg1Val.getVariableId(),-1);
      ROSE_ASSERT(memoryRegionSize>=0);
    }
    res.init(estate,AbstractValue(Top())); // void result (using top here)
  } else {
    // this will become an error in future
    cerr<<"WARNING: unknown free function "<<funCall->unparseToString()<<endl;
  }
  return listify(res);
}

int ExprAnalyzer::getMemoryRegionSize(CodeThorn::AbstractValue ptrToRegion) {
  ROSE_ASSERT(ptrToRegion.isPtr());
  VariableId ptrVariableId=ptrToRegion.getVariableId();
  //cout<<"DEBUG: ptrVariableId:"<<ptrVariableId<<" "<<_variableIdMapping->variableName(ptrVariableId)<<endl;
  int size=_variableIdMapping->getNumberOfElements(ptrVariableId);
  return size;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalFunctionCallMemCpy(SgFunctionCallExp* funCall, EState estate) {
  //cout<<"DETECTED: memcpy: "<<funCall->unparseToString()<<endl;
  SingleEvalResultConstInt res;
  // memcpy is a void function, no return value
  res.init(estate,AbstractValue(CodeThorn::Top())); 
  SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  if(argsList.size()==3) {
    AbstractValue memcpyArgs[3];
    int i=0;
    for(SgExpressionPtrList::iterator argIter=argsList.begin();argIter!=argsList.end();++argIter) {
      SgExpression* arg=*argIter;
      list<SingleEvalResultConstInt> resList=evaluateExpression(arg,estate);
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
      //cout<<"DEBUG: copy region now. "<<endl;
      for(int i=0;i<copyRegionSize;i++) {
        AbstractValue index(i);
        AbstractValue targetPtr=memcpyArgs[0]+index;
        AbstractValue sourcePtr=memcpyArgs[1]+index;
        cout<<"DEBUG: copying "<<targetPtr.toString(_variableIdMapping)<<" from "<<sourcePtr.toString(_variableIdMapping)<<endl;
        //TODO: cpymem
        //newPState=*estate.pstate();
        //newPState.writeToMemoryLocation(targetPtr,newPState.readFromMemoryLocation(sourcePtr));
      }
    }
    return listify(res);
  } else {
    cerr<<"Error: unknown memcpy function (number of arguments != 3)"<<funCall->unparseToString()<<endl;
    exit(1);
  }
  return listify(res);
}

bool ExprAnalyzer::checkArrayBounds(VariableId arrayVarId,int accessIndex) {
  // check array bounds
  int arraySize=_variableIdMapping->getNumberOfElements(arrayVarId);
  if(accessIndex<0||accessIndex>=arraySize) {  
    // this will throw a specific exception that will be caught by the analyzer to report verification results
    cerr<<"Detected out of bounds array access in application: ";
    cerr<<"array size: "<<arraySize<<", array index: "<<accessIndex<<" :: ";
    return false; // fail
  }
  return true; // pass
}

NullPointerDereferenceLocations ExprAnalyzer::getNullPointerDereferenceLocations() {
  return _nullPointerDereferenceLocations;
}

void ExprAnalyzer::recordDefinitiveNullPointerDereferenceLocation(Label label) {
  _nullPointerDereferenceLocations.recordDefinitiveDereference(label);
}

void ExprAnalyzer::recordPotentialNullPointerDereferenceLocation(Label label) {
  _nullPointerDereferenceLocations.recordPotentialDereference(label);
}

bool ExprAnalyzer::isStructMember(SPRAY::VariableId varId) {
  return structureAccessLookup.isStructMember(varId);
}
