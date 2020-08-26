/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"
#include "ExprAnalyzer.h"
#include "CodeThornException.h"
#include "Analyzer.h" // dependency on analyzer->transferAssignOp 
#include "CppStdUtilities.h"
#include "CodeThornCommandLineOptions.h"
#include "CodeThornLib.h"
#include "PredefinedSemanticFunctions.h"
#include "AstTerm.h"

using namespace CodeThorn;
using namespace CodeThorn;
using namespace Sawyer::Message;

Sawyer::Message::Facility ExprAnalyzer::logger;

ExprAnalyzer::ExprAnalyzer() {
  initDiagnostics();
  initViolatingLocations();
}

void ExprAnalyzer::setVariableIdMapping(VariableIdMappingExtended* variableIdMapping) {
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

CodeThorn::InterpreterMode ExprAnalyzer::getInterpreterMode() {
  return _interpreterMode;
}
void ExprAnalyzer::setInterpreterMode(CodeThorn::InterpreterMode im) {
  _interpreterMode=im;
}

string ExprAnalyzer::getInterpreterModeFileName() {
  return _interpreterModeFileName;
}
void ExprAnalyzer::setInterpreterModeFileName(string imFileName) {
  _interpreterModeFileName=imFileName;
}

void ExprAnalyzer::setAnalyzer(Analyzer* analyzer) {
  ROSE_ASSERT(analyzer);
  _analyzer=analyzer;
}

void ExprAnalyzer::setSkipUnknownFunctionCalls(bool skip) {
  _skipSelectedFunctionCalls=skip;
}

bool ExprAnalyzer::getSkipUnknownFunctionCalls() {
  return _skipSelectedFunctionCalls;
}

void ExprAnalyzer::setSkipArrayAccesses(bool skip) {
  _skipArrayAccesses=skip;
}

bool ExprAnalyzer::getSkipArrayAccesses() {
  return _skipArrayAccesses;
}

void ExprAnalyzer::setIgnoreUndefinedDereference(bool skip) {
  _ignoreUndefinedDereference=skip;
}

bool ExprAnalyzer::getIgnoreUndefinedDereference() {
  return _ignoreUndefinedDereference;
}

void ExprAnalyzer::setIgnoreFunctionPointers(bool skip) {
  _ignoreFunctionPointers=skip;
}

bool ExprAnalyzer::getIgnoreFunctionPointers() {
  return _ignoreFunctionPointers;
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

void CodeThorn::ExprAnalyzer::setOptionOutputWarnings(bool flag) {
  _optionOutputWarnings=flag;
}
bool CodeThorn::ExprAnalyzer::getOptionOutputWarnings() {
  return _optionOutputWarnings;
}

bool ExprAnalyzer::checkIfVariableAndDetermineVarId(SgNode* node, VariableId& varId) {
  assert(node);
  if(SgNodeHelper::isArrayAccess(node)) {
    // 1) array variable id
    // 2) eval array-index expr
    // 3) if const then compute variable id otherwise return non-valid var id (would require set)
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

// macro for repeating code pattern to get correctly typed values from value expressions
#define CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SG_VALUE_EXP_TYPE, CPP_TYPE) \
case V_ ## SG_VALUE_EXP_TYPE: {\
  SG_VALUE_EXP_TYPE* exp=is ## SG_VALUE_EXP_TYPE(valueExp);\
  ROSE_ASSERT(exp);\
  CPP_TYPE val=exp->get_value();\
  return AbstractValue(val); }

AbstractValue ExprAnalyzer::abstractValueFromSgValueExp(SgValueExp* valueExp, EvalMode mode) {
  ROSE_ASSERT(valueExp);
  switch(valueExp->variantT()) {
    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgFloatVal, float);
    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgDoubleVal, double);
    //    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgLongDoubleVal, long double);
    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgShortVal, short int);
    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgIntVal, int);
    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgLongIntVal, long int);
    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgLongLongIntVal, long long int);
    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedCharVal, unsigned char);
    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedShortVal, unsigned short);
    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedIntVal, unsigned int);
    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedLongVal, unsigned long int);
    CASE_ABSTRACT_VALUE_FROM_SG_VALUE_EXP(SgUnsignedLongLongIntVal, unsigned long long int);
    // following are all cases that require specific handling in ROSE AST
  case  V_SgCharVal: {
    SgCharVal* exp=isSgCharVal(valueExp);
    unsigned char val=(unsigned char)(signed char)exp->get_value();
    return AbstractValue(val);
  }
  case V_SgWcharVal: {
    SgWcharVal* exp=isSgWcharVal(valueExp);
    long int val=exp->get_value();
    return AbstractValue(val);
  }
  case V_SgComplexVal: {
    return AbstractValue(CodeThorn::Top());
  }
  case V_SgNullptrValExp: {
    return AbstractValue((int)0); // null pointer value
  }
  case V_SgEnumVal: {
    SgEnumVal* exp=isSgEnumVal(valueExp);
    int val=exp->get_value();
    return AbstractValue(val);
  }
  case V_SgStringVal: {
    SgStringVal* stringVal=isSgStringVal(valueExp);
    if(mode==MODE_EMPTY_STATE) {
      // string val addresses are only available if variable ids are available
      return AbstractValue::createTop();
    }
    // handle string literals
    std::string s=stringVal->get_value();
    VariableId stringValVarId=_variableIdMapping->getStringLiteralVariableId(stringVal);
    AbstractValue val=AbstractValue::createAddressOfVariable(stringValVarId);
    SAWYER_MESG(logger[TRACE])<<"Created abstract string literal value: "<<"\""<<s<<"\""<<": abstract value: "<<val.toString(_variableIdMapping)<<endl;
    return val;
  }
  case V_SgBoolValExp: {
    SgBoolValExp* exp=isSgBoolValExp(valueExp);
    // ROSE uses an integer for a bool
    int val=exp->get_value();
    if(val==0)
      return AbstractValue(false);
    else if(val==1)
      return AbstractValue(true);
    else {
      logger[ERROR]<<"Error: unknown bool value (not 0 or 1): SgBoolExp::get_value()=="<<val<<endl;
      exit(1);
    }
  }
  default:
    return AbstractValue::createTop();
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

#define CASE_EXPR_ANALYZER_EVAL(ROSENODENAME,EVALFUNCTIONNAME) case V_ ## ROSENODENAME: resultList.splice(resultList.end(),EVALFUNCTIONNAME(is ## ROSENODENAME(node),lhsResult,rhsResult,estate,mode));break

#define CASE_EXPR_ANALYZER_EVAL_UNARY_OP(ROSENODENAME,EVALFUNCTIONNAME) case V_ ## ROSENODENAME: resultList.splice(resultList.end(),EVALFUNCTIONNAME(is ## ROSENODENAME(node),operandResult,estate,mode));break

list<SingleEvalResultConstInt> ExprAnalyzer::evaluateLExpression(SgNode* node,EState estate) {
  list<SingleEvalResultConstInt> resList;
  AbstractValue result;
  if(SgVarRefExp* varExp=isSgVarRefExp(node)) {
    return evalLValueVarRefExp(varExp,estate);
  } else if(SgPntrArrRefExp* arrRef=isSgPntrArrRefExp(node)) {
    return evalLValuePntrArrRefExp(arrRef,estate);
  } else if(SgDotExp* dotExp=isSgDotExp(node)) {
    return evalLValueExp(dotExp,estate);
  } else if(SgArrowExp* arrowExp=isSgArrowExp(node)) {
    return evalLValueExp(arrowExp,estate);
  } else if(SgPointerDerefExp* ptrDerefExp=isSgPointerDerefExp(node)) {
    return evalLValuePointerDerefExp(ptrDerefExp,estate);
  } else {
    cerr<<"Error: unsupported lvalue expression: "<<node->unparseToString()<<endl;
    cerr<<"     : "<<SgNodeHelper::sourceLineColumnToString(node)<<" : "<<AstTerm::astTermWithNullValuesToString(node)<<endl;
    exit(1);
  }
  // unreachable
  ROSE_ASSERT(false);
}

bool ExprAnalyzer::isLValueOp(SgNode* node) {
  // assign operators not included yet
  return isSgAddressOfOp(node)
    || SgNodeHelper::isPrefixIncDecOp(node)
    || SgNodeHelper::isPostfixIncDecOp(node)
    ;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evaluateShortCircuitOperators(SgNode* node,EState estate, EvalMode mode) {
  SgNode* lhs=SgNodeHelper::getLhs(node);
  list<SingleEvalResultConstInt> lhsResultList=evaluateExpression(lhs,estate,mode);
  list<SingleEvalResultConstInt> resultList;
  for(list<SingleEvalResultConstInt>::iterator liter=lhsResultList.begin();
      liter!=lhsResultList.end();
      ++liter) {
    switch(node->variantT()) {
    case V_SgAndOp: {
      SingleEvalResultConstInt lhsResult=*liter;
      // short circuit semantics
      if(lhsResult.isTrue()||lhsResult.isTop()||lhsResult.isBot()) {
        SgNode* rhs=SgNodeHelper::getRhs(node);
        list<SingleEvalResultConstInt> rhsResultList=evaluateExpression(rhs,estate,mode);
        for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
            riter!=rhsResultList.end();
            ++riter) {
          SingleEvalResultConstInt rhsResult=*riter;
          resultList.splice(resultList.end(),evalAndOp(isSgAndOp(node),lhsResult,rhsResult,estate,mode));
        }
      } else {
        // rhs not executed
        ROSE_ASSERT(lhsResult.isFalse());
        // result must be zero (=false)
        resultList.push_back(lhsResult);
      }
      break;
    }
    case V_SgOrOp: {
      SingleEvalResultConstInt lhsResult=*liter;
      if(lhsResult.isFalse()||lhsResult.isTop()||lhsResult.isBot()) {
        SgNode* rhs=SgNodeHelper::getRhs(node);
        list<SingleEvalResultConstInt> rhsResultList=evaluateExpression(rhs,estate,mode);
        for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
            riter!=rhsResultList.end();
            ++riter) {
          SingleEvalResultConstInt rhsResult=*riter;
          resultList.splice(resultList.end(),evalOrOp(isSgOrOp(node),lhsResult,rhsResult,estate,mode));
        }
      } else {
        // rhs not executed
        ROSE_ASSERT(lhsResult.isTrue());
        // bugfix MS 50/06/2020: convert any true value to true/1: (10||x)=>1
        lhsResult.result=AbstractValue(true);
        resultList.push_back(lhsResult);
      }
      break;
    }
    default:
      cerr << "Binary short circuit op:"<<SgNodeHelper::nodeToString(node)<<"(nodetype:"<<node->class_name()<<")"<<endl;
      throw CodeThorn::Exception("Error: evaluateExpression::unknown binary short circuit operation.");
    }
  }
  return resultList;
}

AbstractValue ExprAnalyzer::evaluateExpressionWithEmptyState(SgExpression* expr) {
  logger[TRACE]<<"evaluateExpressionWithEmptyState(1):"<<expr->unparseToString()<<endl;
  ROSE_ASSERT(AbstractValue::getVariableIdMapping());
  EState emptyEState;
  PState emptyPState;
  emptyEState.setPState(&emptyPState);
  ExprAnalyzer::EvalMode evalMode=ExprAnalyzer::MODE_EMPTY_STATE;
  list<SingleEvalResultConstInt> resList=evaluateExpression(expr,emptyEState,evalMode);
  logger[TRACE]<<"evaluateExpressionWithEmptyState(2):"<<expr->unparseToString()<<endl;
  if(resList.size()!=1) {
    return AbstractValue::createTop();
  } else {
    return (*resList.begin()).value();
  }
}


list<SingleEvalResultConstInt> ExprAnalyzer::evaluateExpression(SgNode* node,EState estate, EvalMode mode) {
  ROSE_ASSERT(estate.pstate()); // ensure state exists

  // initialize with default values from argument(s)
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=AbstractValue::createBot();

  if(mode==ExprAnalyzer::MODE_EMPTY_STATE) {
    if(isSgAssignOp(node)||isSgFunctionCallExp(node)||isSgVarRefExp(node)||isSgPlusPlusOp(node)||isSgMinusMinusOp(node)||isSgCompoundAssignOp(node)||isSgConditionalExp(node)) {
      res.result=AbstractValue::createTop();
      return listify(res);
    }
    //cout<<"DEBUG: evalExp at: "<<AstTerm::astTermWithNullValuesToString(node)<<endl;
  }

#if 0
  if(SgNodeHelper::isPostfixIncDecOp(node)) {
    cerr << "Error: incdec-op not supported in conditions."<<endl;
    exit(1);
  }
#endif
  if(SgStatementExpression* gnuExtensionStmtExpr=isSgStatementExpression(node)) {
    cout<<"WARNING: ignoring GNU extension StmtExpr."<<endl;
    res.result=AbstractValue::createTop();
    return listify(res);
  }

  
  if(SgConditionalExp* condExp=isSgConditionalExp(node)) {
    return evalConditionalExpr(condExp,estate,mode);
  }

  if(dynamic_cast<SgBinaryOp*>(node)) {
    // special handling of short-circuit operators
    if(isSgAndOp(node)||isSgOrOp(node)) {
      return evaluateShortCircuitOperators(node,estate,mode);
    }
    SgNode* lhs=SgNodeHelper::getLhs(node);
    list<SingleEvalResultConstInt> lhsResultList=evaluateExpression(lhs,estate,mode);
    SgNode* rhs=SgNodeHelper::getRhs(node);
    list<SingleEvalResultConstInt> rhsResultList=evaluateExpression(rhs,estate,mode);
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

        switch(node->variantT()) {
          CASE_EXPR_ANALYZER_EVAL(SgEqualityOp,evalEqualOp);
          CASE_EXPR_ANALYZER_EVAL(SgNotEqualOp,evalNotEqualOp);
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
          CASE_EXPR_ANALYZER_EVAL(SgCommaOpExp,evalCommaOp);
        case V_SgAssignOp: {
          list<SingleEvalResultConstInt> l=evaluateLExpression(lhs,estate);
          ROSE_ASSERT(l.size()==1);
          auto lhsAddress=*l.begin();
          resultList.splice(resultList.end(),evalAssignOp(isSgAssignOp(node),lhsResult /*ignored*/,rhsResult,estate,mode));
          break;
        }
        default:
          cerr << "Binary Op:"<<SgNodeHelper::nodeToString(node)<<"(nodetype:"<<node->class_name()<<")"<<endl;
          throw CodeThorn::Exception("Error: evaluateExpression::unknown binary operation.");
        }
      }
    }
    return resultList;
  }

  if(isLValueOp(node)) {
    if(mode==ExprAnalyzer::MODE_EMPTY_STATE) {
      // arbitrary value
      res.init(estate,CodeThorn::Top());
      return listify(res);
    }
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
    list<SingleEvalResultConstInt> operandResultList=evaluateExpression(child,estate,mode);
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
        logger[ERROR]<<"evaluateExpression::unknown unary operation @"<<node->sage_class_name()<<endl;
        exit(1);
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
    return evalValueExp(exp,estate,mode);
  }
  switch(node->variantT()) {
  case V_SgVarRefExp:
    return evalRValueVarRefExp(isSgVarRefExp(node),estate,mode);
  case V_SgFunctionCallExp: {
    return evalFunctionCall(isSgFunctionCallExp(node),estate);
  }
  case V_SgNullExpression: {
    list<SingleEvalResultConstInt> resultList;
    res.result=AbstractValue::createTop();
    resultList.push_front(res);
    return resultList;
  }
  case V_SgFunctionRefExp: {
    if(getIgnoreFunctionPointers()) {
      // just ignore the call (this is unsound and only for testing)
      list<SingleEvalResultConstInt> resultList;
      res.result=AbstractValue::createTop();
      resultList.push_front(res);
      return resultList;
    } else {
      // use of function addresses as values. Being implemented now.
      //SAWYER_MESG(logger[WARN])<<"Imprecision: function pointer value: evaluating SgFunctionRefExp as top: "<<SgNodeHelper::sourceLineColumnToString(node)<<": "<<node->unparseToString()<<endl;
      return evalFunctionRefExp(isSgFunctionRefExp(node),estate,mode);
    }
  }
  case V_SgThisExp: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return listify(res);
  }
  case V_SgNewExp: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return listify(res);
  }
  case V_SgDeleteExp: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return listify(res);
  }
  case V_SgAssignInitializer: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return listify(res);
  }
  case V_SgConstructorInitializer: {
      res.result=AbstractValue::createTop();
      printLoggerWarning(estate);
      return listify(res);
  }
  default:
    throw CodeThorn::Exception("Error: evaluateExpression::unknown node in expression: "+string(node->sage_class_name())+" at "+SgNodeHelper::sourceFilenameToString(node)+" in file "+SgNodeHelper::sourceFilenameToString(node));
  } // end of switch
  throw CodeThorn::Exception("Error: evaluateExpression failed.");
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// EVAL FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////

// evaluation functions
list<SingleEvalResultConstInt> ExprAnalyzer::evalConditionalExpr(SgConditionalExp* condExp, EState estate, EvalMode mode) {
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
      SAWYER_MESG(logger[WARN])<<"evaluating condition of conditional operator gives two equal results"<<endl;
    }
  }
  if(condResultList.size()>1) {
    logger[ERROR]<<"evaluating condition of conditional operator gives more than one result. Normalization required."<<endl;
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

list<SingleEvalResultConstInt> ExprAnalyzer::evalCommaOp(SgCommaOpExp* node,
                                                      SingleEvalResultConstInt lhsResult,
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate, EvalMode mode) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  // lhsResult is ignored in comma op
  // result of the expression is the rhs's value
  res.result=rhsResult.result;
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalEqualOp(SgEqualityOp* node,
                                                         SingleEvalResultConstInt lhsResult,
                                                         SingleEvalResultConstInt rhsResult,
                                                         EState estate, EvalMode mode) {
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
                                                            EState estate, EvalMode mode) {
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
                                                      EState estate, EvalMode mode) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorAnd(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalOrOp(SgOrOp* node,
                                                      SingleEvalResultConstInt lhsResult,
                                                      SingleEvalResultConstInt rhsResult,
                                                      EState estate, EvalMode mode) {
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
                                                      EState estate, EvalMode mode) {
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
                                                      EState estate, EvalMode mode) {
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
                                                      EState estate, EvalMode mode) {

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
                                                      EState estate, EvalMode mode) {

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
                                                      EState estate, EvalMode mode) {

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
                                                              EState estate, EvalMode mode) {
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
                                                             EState estate, EvalMode mode) {
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
                                                              EState estate, EvalMode mode) {
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
                                   EState estate, EvalMode mode) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorMoreOrEq(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalGreaterThanOp(SgGreaterThanOp* node,
                                SingleEvalResultConstInt lhsResult,
                                SingleEvalResultConstInt rhsResult,
                                EState estate, EvalMode mode) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorMore(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalAssignOp(SgAssignOp* node,
                           SingleEvalResultConstInt lhsResult,
                           SingleEvalResultConstInt rhsResult,
                           EState estate, EvalMode mode) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  Edge fakeEdge;
  std::list<EState> estateList=_analyzer->transferAssignOp(node,fakeEdge,&estate);
  ROSE_ASSERT(estateList.size()==1);
  res.result=rhsResult.result; // value result of assignment
  res.estate=*estateList.begin();
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalLessOrEqualOp(SgLessOrEqualOp* node,
                                SingleEvalResultConstInt lhsResult,
                                SingleEvalResultConstInt rhsResult,
                                EState estate, EvalMode mode) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorLessOrEq(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalLessThanOp(SgLessThanOp* node,
                             SingleEvalResultConstInt lhsResult,
                             SingleEvalResultConstInt rhsResult,
                             EState estate, EvalMode mode) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=(lhsResult.result.operatorLess(rhsResult.result));
  resultList.push_back(res);
  return resultList;
}

list<SingleEvalResultConstInt>
ExprAnalyzer::evalBitwiseShiftLeftOp(SgLshiftOp* node,
                             SingleEvalResultConstInt lhsResult,
                             SingleEvalResultConstInt rhsResult,
                             EState estate, EvalMode mode) {
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
                             EState estate, EvalMode mode) {
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
                                 EState estate, EvalMode mode) {
  SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp: "<<node->unparseToString()<<endl;
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;

  if(mode==ExprAnalyzer::MODE_EMPTY_STATE) {
    // arbitrary value
    res.result=CodeThorn::Top();
    resultList.push_back(res);
    return resultList;
  }

  SgNode* arrayExpr=SgNodeHelper::getLhs(node);
  SAWYER_MESG(logger[TRACE])<<"arrayExpr: "<<arrayExpr->unparseToString()<<endl;

  if(indexExprResult.value().isTop()||getSkipArrayAccesses()==true) {
    // set result to top when index is top [imprecision]
    // assume top for array elements if skipped
    // Precision: imprecise
    SAWYER_MESG(logger[TRACE])<<"ExprAnalyzer::evalArrayReferenceOp: returns top"<<endl;
    res.result=CodeThorn::Top();
    resultList.push_back(res);
    return resultList;
  } else {
    if(SgVarRefExp* varRefExp=isSgVarRefExp(arrayExpr)) {
      AbstractValue arrayPtrValue=arrayExprResult.result;
      const PState* const_pstate=estate.pstate();
      PState pstate2=*const_pstate; // also removes constness
      VariableId arrayVarId=_variableIdMapping->variableId(varRefExp);
      // two cases
      if(_variableIdMapping->hasArrayType(arrayVarId)) {
        if(_variableIdMapping->isFunctionParameter(arrayVarId)) {
          // function parameter of array type contains a pointer value in C/C++
          arrayPtrValue=readFromMemoryLocation(estate.label(),&pstate2,arrayVarId); // pointer value of array function paramter only (without index)
          SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp:"<<" arrayPtrValue (of function parameter) read from memory, arrayPtrValue: "<<arrayPtrValue.toString(_variableIdMapping)<<endl;
        } else {
          arrayPtrValue=AbstractValue::createAddressOfArray(arrayVarId);
          SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp: created array address (from array type): "<<arrayPtrValue.toString(_variableIdMapping)<<endl;
        }
      } else if(_variableIdMapping->hasPointerType(arrayVarId)) {
        // in case it is a pointer retrieve pointer value
        SAWYER_MESG(logger[DEBUG])<<"pointer-array access."<<endl;
        if(pstate2.varExists(arrayVarId)) {
          arrayPtrValue=readFromMemoryLocation(estate.label(),&pstate2,arrayVarId); // pointer value (without index)
          SAWYER_MESG(logger[TRACE])<<"evalArrayReferenceOp:"<<" arrayPtrValue read from memory (in state), arrayPtrValue:"<<arrayPtrValue.toString(_variableIdMapping)<<endl;
          if(!(arrayPtrValue.isTop()||arrayPtrValue.isBot()||arrayPtrValue.isPtr()||arrayPtrValue.isNullPtr())) {
            logger[ERROR]<<"@"<<SgNodeHelper::lineColumnNodeToString(node)<<": value not a pointer value: "<<arrayPtrValue.toString()<<endl;
            logger[ERROR]<<estate.toString(_variableIdMapping)<<endl;
            exit(1);
          }
        } else {
          //cerr<<"Error: pointer variable does not exist in PState: "<<arrayVarId.toString()<<endl  ;
          // TODO PRECISION 2
          // variable may have been not written because abstraction is too coarse (subsummed in write to top)
          // => reading from anywhere, returning any value
          res.result=CodeThorn::Top();
          resultList.push_back(res);
          return resultList;
        }
      } else {
        cerr<<"Error: unknown type of array or pointer."<<endl;
        exit(1);
      }
      AbstractValue indexExprResultValue=indexExprResult.value();
      AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrayPtrValue,indexExprResultValue);
      if(arrayPtrPlusIndexValue.isNullPtr()) {
        recordDefinitiveViolatingLocation(ANALYSIS_NULL_POINTER,estate.label()); // NP_SOUNDNESS
        // there is no state following a definitive null pointer
        // dereference. An error-state recording this property is
        // created to allow analysis of errors on the programs
        // transition graph. In addition the property is also recorded in the _nullPointerDereferenceLocations list.
        res.result=CodeThorn::Top(); // consider returning bot here?
        // verification error states are detected in the solver and no successor states are computed.
        res.estate.io.recordVerificationError();
        resultList.push_back(res);
        return resultList;
      }
      if(pstate2.varExists(arrayPtrValue)) {
        // required for the following index computation (nothing to do here)
      } else {
        if(arrayPtrValue.isTop()) {
          //logger[ERROR]<<"@"<<SgNodeHelper::lineColumnNodeToString(node)<<" evalArrayReferenceOp: pointer is top. Pointer abstraction too coarse."<<endl;
          // TODO: PRECISION 1
          res.result=CodeThorn::Top();
          recordPotentialNullPointerDereferenceLocation(estate.label()); // NP_SOUNDNESS
          //if(_analyzer->getAbstractionMode()!=3) recordPotentialOutOfBoundsAccessLocation(estate.label());
          recordPotentialViolatingLocation(ANALYSIS_UNINITIALIZED,estate.label()); // UNINIT_SOUNDNESS
          resultList.push_back(res);
          return resultList;
        } else {
          res.result=CodeThorn::Top();
          resultList.push_back(res);
          return resultList;
        }
        exit(1); // not reachable
      }
      if(pstate2.varExists(arrayPtrPlusIndexValue)) {
        // address of denoted memory location
        switch(mode) {
        case MODE_VALUE:
          res.result=readFromMemoryLocation(estate.label(),&pstate2,arrayPtrPlusIndexValue);
          SAWYER_MESG(logger[TRACE])<<"retrieved array element value:"<<res.result<<endl;
          return listify(res);
        case MODE_ADDRESS:
          res.result=arrayPtrPlusIndexValue;
          return listify(res);
        default:
          SAWYER_MESG(logger[FATAL])<<"Internal error: evalArrayReferenceOp: unsupported EvalMode."<<endl;
          exit(1);
        }
      } else {
        SAWYER_MESG(logger[WARN])<<"evalArrayReferenceOp:"<<" memory location not in state: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
        SAWYER_MESG(logger[WARN])<<"evalArrayReferenceOp:"<<pstate2.toString(_variableIdMapping)<<endl;

        if(mode==MODE_ADDRESS) {
          SAWYER_MESG(logger[FATAL])<<"Internal error: ExprAnalyzer::evalArrayReferenceOp: address mode not possible for variables not in state."<<endl;
          exit(1);
        }
        // array variable NOT in state. Special space optimization case for constant array.
        if(_variableIdMapping->hasArrayType(arrayVarId) /* MS 5/20/2020: removed mode: && _analyzer->getOptionsRef().explicitArrays==false*/) {
          SgExpressionPtrList& initList=_variableIdMapping->getInitializerListOfArrayVariable(arrayVarId);
          int elemIndex=0;
          // TODO: slow linear lookup (TODO: pre-compute all values and provide access function)
          for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
            SgExpression* exp=*i;
            SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
            if(assignInit) {
              SgExpression* initExp=assignInit->get_operand_i();
              ROSE_ASSERT(initExp);
              if(SgIntVal* intValNode=isSgIntVal(initExp)) {
                int intVal=intValNode->get_value();
                //newPState.writeToMemoryLocation(arrayElemId,CodeThorn::AbstractValue(AbstractValue(intVal)));
                int index2=arrayPtrPlusIndexValue.getIndexIntValue();
                if(elemIndex==index2) {
                  AbstractValue val=AbstractValue(intVal); // TODO BYTEMODE
                  res.result=val;
                  return listify(res);
                }
              } else {
                SAWYER_MESG(logger[WARN])<<"unsupported array initializer value (assuming any value):"<<exp->unparseToString()<<" AST:"<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
                AbstractValue val=AbstractValue::createTop();
                res.result=val;
                return listify(res);
              }
            } else {
              SAWYER_MESG(logger[FATAL])<<"no assign initialize:"<<exp->unparseToString()<<" AST:"<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
              exit(1);
            }
            elemIndex++;
          }
          SAWYER_MESG(logger[ERROR])<<"Error: access to element of constant array (not in state). Not supported."<<endl;
          exit(1);
        } else if(_variableIdMapping->isStringLiteralAddress(arrayVarId)) {
          SAWYER_MESG(logger[ERROR])<<"Error: Found string literal address, but data not present in state."<<endl;
          exit(1);
        } else {
          //cout<<estate.toString(_variableIdMapping)<<endl;
          SAWYER_MESG(logger[TRACE])<<"Program error detected: potential out of bounds access (P1) : array: "<<arrayPtrValue.toString(_variableIdMapping)<<", access: address: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
          //cout<<"DEBUG: array-element: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
          //cerr<<"PState: "<<pstate->toString(_variableIdMapping)<<endl;
          //cerr<<"AST: "<<node->unparseToString()<<endl;
          recordPotentialViolatingLocation(ANALYSIS_NULL_POINTER,estate.label()); // NP_SOUNDNESS
          // continue after potential out-of-bounds access (assume any value can have been read)
          AbstractValue val=AbstractValue::createTop();
          res.result=val;
          return listify(res);
        }
      }
    } else {
      SAWYER_MESG(logger[WARN])<<"Array-access uses expr for denoting the array (not supported yet) ";
      SAWYER_MESG(logger[WARN])<<"@"<<SgNodeHelper::lineColumnNodeToString(node)<<" ";
      SAWYER_MESG(logger[WARN])<<"expr: "<<arrayExpr->unparseToString()<<" ";
      SAWYER_MESG(logger[WARN])<<"arraySkip: "<<getSkipArrayAccesses()<<endl;
    }
    return resultList;
  }
  ROSE_ASSERT(false); // not reachable
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalNotOp(SgNotOp* node,
                                                       SingleEvalResultConstInt operandResult,
                                                       EState estate, EvalMode mode) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=operandResult.result.operatorNot();
  return listify(res);
}
list<SingleEvalResultConstInt> ExprAnalyzer::evalUnaryMinusOp(SgMinusOp* node,
                                                              SingleEvalResultConstInt operandResult,
                                                              EState estate, EvalMode mode) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=operandResult.result.operatorUnaryMinus();
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalSizeofOp(SgSizeOfOp* node,
                                                              EState estate, EvalMode mode) {
  logger[TRACE]<<"evalSizeofOp(started):"<<node->unparseToString()<<endl;
  SgType* operandType=node->get_operand_type();
  CodeThorn::TypeSize typeSize=0; // remains zero if no size can be determined
  AbstractValue sizeValue=AbstractValue::createTop();

  if(operandType) {
    logger[TRACE]<<"evalSizeofOp(2):"<<node->unparseToString()<<endl;
    typeSize=_variableIdMapping->getTypeSize(operandType);
    logger[TRACE]<<"evalSizeofOp(3):"<<node->unparseToString()<<endl;
  } else if(SgExpression* exp=node->get_operand_expr()) {
    logger[TRACE]<<"evalSizeofOp(4):"<<node->unparseToString()<<endl;
    if(SgVarRefExp* varRefExp=isSgVarRefExp(exp)) {
      typeSize=_variableIdMapping->getTypeSize(_variableIdMapping->variableId(varRefExp));
    } else if(SgType* expType=exp->get_type()) {
      typeSize=_variableIdMapping->getTypeSize(expType);
    } else {
      logger[WARN] <<"sizeof: could not determine any type of sizeof argument and unsupported argument expression: "<<SgNodeHelper::sourceLineColumnToString(exp)<<": "<<exp->unparseToString()<<endl<<AstTerm::astTermWithNullValuesToDot(exp)<<endl;
    }
    logger[TRACE]<<"evalSizeofOp(5):"<<node->unparseToString()<<endl;
  } else {
    logger[WARN] <<"sizeof: could not determine any type of sizeof argument and no expression found either: "<<SgNodeHelper::sourceLineColumnToString(exp)<<": "<<exp->unparseToString()<<endl;
  }
  logger[TRACE]<<"evalSizeofOp(6):"<<node->unparseToString()<<endl;

  // determines sizeValue based on typesize
    if(typeSize==0) {
      logger[WARN]<<"sizeof: could not determine size (= zero) of argument, assuming top "<<SgNodeHelper::sourceLineColumnToString(node)<<": "<<node->unparseToString()<<endl;
      sizeValue=AbstractValue::createTop();
    } else {
      SAWYER_MESG(logger[TRACE])<<"DEBUG: @"<<SgNodeHelper::sourceLineColumnToString(node)<<": sizeof("<<typeSize<<")"<<endl;
      sizeValue=AbstractValue(typeSize);
      SAWYER_MESG(logger[TRACE])<<"DEBUG: @"<<SgNodeHelper::sourceLineColumnToString(node)<<": sizevalue of sizeof("<<typeSize<<"):"<<sizeValue.toString()<<endl;
    }
    SingleEvalResultConstInt res;
    res.init(estate,sizeValue);
    logger[TRACE]<<"evalSizeofOp(finished):"<<node->unparseToString()<<endl;
    return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalCastOp(SgCastExp* node,
                                                        SingleEvalResultConstInt operandResult,
                                                        EState estate, EvalMode mode) {
  // TODO: truncation of values
  // TODO: adapt pointer value element size
  SgType* targetType=node->get_type();
  targetType=targetType->stripType(SgType::STRIP_TYPEDEF_TYPE|SgType::STRIP_MODIFIER_TYPE);
  if(AbstractValue::byteMode) {
    if(SgPointerType* ptrType=isSgPointerType(targetType)) {
      SgType* elementType=ptrType->get_base_type();
      long int elementTypeSize=_variableIdMapping->getTypeSize(elementType);
      logger[DEBUG]<<"casting pointer to element type size: "<<elementTypeSize<<":"<<elementType->unparseToString()<<endl;
      if(AbstractValue::byteMode) {
        operandResult.result.setElementTypeSize(elementTypeSize);
      }
    }
  }
  SingleEvalResultConstInt res;
  res.init(estate,operandResult.result);
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalBitwiseComplementOp(SgBitComplementOp* node,
                                                                     SingleEvalResultConstInt operandResult,
                                                                     EState estate, EvalMode mode) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  res.result=operandResult.result.operatorBitwiseComplement();
  return listify(res);
}

AbstractValue ExprAnalyzer::computeAbstractAddress(SgVarRefExp* varRefExp) {
  VariableId varId=_variableIdMapping->variableId(varRefExp);
  return AbstractValue::createAddressOfVariable(varId);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalArrowOp(SgArrowExp* node,
                                                         SingleEvalResultConstInt lhsResult,
                                                         SingleEvalResultConstInt rhsResult,
                                                         EState estate, EvalMode mode) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;

  if(mode==ExprAnalyzer::MODE_EMPTY_STATE) {
    // arbitrary value
    res.result=CodeThorn::Top();
    resultList.push_back(res);
    return resultList;
  }

  // L->R : L evaluates to pointer value (address), R evaluates to offset value (a struct member always evaluates to an offset)
  //AbstractValue address=lhsResult.result;
  //cout<<"DEBUG: ArrowOp: address(lhs):"<<address.toString(_variableIdMapping)<<endl;
  //AbstractValue referencedAddress=estate.pstate()->readFromMemoryLocation(address);
  AbstractValue referencedAddress=lhsResult.result;
  bool continueExec=checkAndRecordNullPointer(referencedAddress, estate.label());
  if(continueExec) {
    SAWYER_MESG(logger[TRACE])<<"ArrowOp: referencedAddress(lhs):"<<referencedAddress.toString(_variableIdMapping)<<endl;
    AbstractValue offset=rhsResult.result;
    AbstractValue denotedAddress=AbstractValue::operatorAdd(referencedAddress,offset);
    SAWYER_MESG(logger[TRACE])<<"ArrowOp: denoted Address(lhs):"<<denotedAddress.toString(_variableIdMapping)<<endl;

    switch(mode) {
    case MODE_VALUE:
      SAWYER_MESG(logger[TRACE])<<"Arrow op: reading value from arrowop-struct location."<<denotedAddress.toString(_variableIdMapping)<<endl;
      res.result=readFromMemoryLocation(estate.label(),estate.pstate(),denotedAddress);
    break;
    case MODE_ADDRESS:
      res.result=denotedAddress;
      break;
    case MODE_EMPTY_STATE:
      throw CodeThorn::Exception("Interal error: ExprAnalyzer::evalArrowOp: empty state mode.");
      // intentionally no default case, all cases must be represented
    }
    resultList.push_back(res);
    return resultList;
  } else {
    list<SingleEvalResultConstInt> empty;
    return empty;
  }
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalDotOp(SgDotExp* node,
                                                       SingleEvalResultConstInt lhsResult,
                                                       SingleEvalResultConstInt rhsResult,
                                                       EState estate, EvalMode mode) {
  list<SingleEvalResultConstInt> resultList;
  SingleEvalResultConstInt res;
  res.estate=estate;
  // L.R : L evaluates to address, R evaluates to offset value (a struct member always evaluates to an offset)
  //SAWYER_MESG(logger[DEBUG])<<"DotOp: lhs:"<<lhsResult.result.toString(_variableIdMapping)<<" rhs: "<<rhsResult.result.toString(_variableIdMapping)<<endl;
  //cout<<"DotOp: "<<node->unparseToString()<<":"<<lhsResult.result.toString(_variableIdMapping)<<"."<<rhsResult.result.toString(_variableIdMapping)<<endl;
  
  if(mode==ExprAnalyzer::MODE_EMPTY_STATE) {
    // arbitrary value
    res.result=CodeThorn::Top();
    resultList.push_back(res);
    return resultList;
  }

  // lhs of dot-op should never be 0 (except if uninitialized memory is used)
  bool continueExec=checkAndRecordNullPointer(lhsResult.result, estate.label());
  if(continueExec) {
    AbstractValue address=AbstractValue::operatorAdd(lhsResult.result,rhsResult.result);
    // only if rhs is *not* a dot-operator, needs the value be
    // read. Otherwise this is not the end of the access path and only the address is computed.
    if(true||!isSgDotExp(SgNodeHelper::getRhs(node))) {
      // reached end of dot sequence (a.b.<here>c)
      switch(mode) {
      case MODE_VALUE:
        SAWYER_MESG(logger[TRACE])<<"Dot op: reading from struct location."<<address.toString(_variableIdMapping)<<endl;
        res.result=readFromMemoryLocation(estate.label(),estate.pstate(),address);
        break;
      case MODE_ADDRESS:
        res.result=address;
        break;
      case MODE_EMPTY_STATE:
        throw CodeThorn::Exception("Interal error: ExprAnalyzer::evalArrowOp: empty state mode.");
        // intentionally no default case, all cases must be represented
      } 
    } else {
      // evaluation of dot sequence (a.<here>b.c)
      res.result=address;
    }
    resultList.push_back(res);
    return resultList;
  } else {
    list<SingleEvalResultConstInt> empty;
    return empty;
  }
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalAddressOfOp(SgAddressOfOp* node,
                                                             SingleEvalResultConstInt operandResult,
                                                             EState estate, EvalMode mode) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  AbstractValue operand=operandResult.result;
  SAWYER_MESG(logger[INFO])<<"AddressOfOp: "<<node->unparseToString()<<" - operand: "<<operand.toString(_variableIdMapping)<<endl;
  if(operand.isTop()||operand.isBot()) {
    res.result=operand;
  } else {
    res.result=operand;
  }
  return listify(res);
}

bool ExprAnalyzer::checkAndRecordNullPointer(AbstractValue derefOperandValue, Label label) {
  if(derefOperandValue.isTop()) {
    recordPotentialNullPointerDereferenceLocation(label);
    return true;
  } else if(derefOperandValue.isConstInt()) {
    int ptrIntVal=derefOperandValue.getIntValue();
    if(ptrIntVal==0) {
      recordDefinitiveNullPointerDereferenceLocation(label);
      return false;
    }
  }
  return true;
}

list<SingleEvalResultConstInt> ExprAnalyzer::semanticEvalDereferenceOp(SingleEvalResultConstInt operandResult,
                                                                       EState estate, EvalMode mode) {
  SingleEvalResultConstInt res;
  res.estate=estate;
  AbstractValue derefOperandValue=operandResult.result;
  SAWYER_MESG(logger[DEBUG])<<"derefOperandValue: "<<derefOperandValue.toRhsString(_variableIdMapping);

  if(mode==ExprAnalyzer::MODE_EMPTY_STATE) {
    // arbitrary value
    res.result=CodeThorn::Top();
    return listify(res);
  }

  // null pointer check
  bool continueExec=checkAndRecordNullPointer(derefOperandValue, estate.label());
  if(continueExec) {
    switch(mode) {
    case ExprAnalyzer::MODE_VALUE :res.result=readFromMemoryLocation(estate.label(),estate.pstate(),derefOperandValue);break;
    case ExprAnalyzer::MODE_ADDRESS:res.result=derefOperandValue;;break;
    default:
      cerr<<"Error: ExprAnalyzer::semanticEvalDereferenceOp: unknown evaluation mode: "<<mode<<endl;
      exit(1);
    }
    return listify(res);
  } else {
    // Alternative to above null pointer dereference recording: build
    // proper error state and check error state in solver.  once this
    // is added above null pointer recording should be adapated to use
    // the generated error state.
    // TODO: create null-pointer deref error state
    list<SingleEvalResultConstInt> empty;
    return empty;
  }
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalDereferenceOp(SgPointerDerefExp* node,
                                                              SingleEvalResultConstInt operandResult,
                                                              EState estate, EvalMode mode) {
  return semanticEvalDereferenceOp(operandResult,estate,mode);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalPreComputationOp(EState estate, AbstractValue address, AbstractValue change) {
  SingleEvalResultConstInt res;
  AbstractValue oldValue=readFromMemoryLocation(estate.label(),estate.pstate(),address);
  AbstractValue newValue=AbstractValue::operatorAdd(oldValue,change);
  CallString cs=estate.callString;
  PState newPState=*estate.pstate();
  writeToMemoryLocation(estate.label(),&newPState,address,newValue);
  ConstraintSet cset; // use empty cset (in prep to remove it)
  ROSE_ASSERT(_analyzer);
  res.init(_analyzer->createEState(estate.label(),cs,newPState,cset),newValue);
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalPostComputationOp(EState estate, AbstractValue address, AbstractValue change) {
  // TODO change from precomp to postcomp
  SingleEvalResultConstInt res;
  AbstractValue oldValue=readFromMemoryLocation(estate.label(),estate.pstate(),address);
  AbstractValue newValue=oldValue+change;
  CallString cs=estate.callString;
  PState newPState=*estate.pstate();
  writeToMemoryLocation(estate.label(),&newPState,address,newValue);
  ConstraintSet cset; // use empty cset (in prep to remove it)
  ROSE_ASSERT(_analyzer);
  res.init(_analyzer->createEState(estate.label(),cs,newPState,cset),oldValue);
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalPreIncrementOp(SgPlusPlusOp* node,
								SingleEvalResultConstInt operandResult,
								EState estate, EvalMode mode) {
  AbstractValue address=operandResult.result;
  ROSE_ASSERT(address.isPtr()||address.isTop());
  AbstractValue change=1;
  return evalPreComputationOp(estate,address,change);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalPreDecrementOp(SgMinusMinusOp* node,
								SingleEvalResultConstInt operandResult,
								EState estate, EvalMode mode) {
  AbstractValue address=operandResult.result;
  ROSE_ASSERT(address.isPtr()||address.isTop());
  AbstractValue change=-1;
  return evalPreComputationOp(estate,address,change);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalPostIncrementOp(SgPlusPlusOp* node,
								 SingleEvalResultConstInt operandResult,
								 EState estate, EvalMode mode) {
  AbstractValue address=operandResult.result;
  ROSE_ASSERT(address.isPtr()||address.isTop());
  AbstractValue change=1;
  return evalPostComputationOp(estate,address,change);
}


list<SingleEvalResultConstInt> ExprAnalyzer::evalPostDecrementOp(SgMinusMinusOp* node,
								 SingleEvalResultConstInt operandResult,
								 EState estate, EvalMode mode) {
  AbstractValue address=operandResult.result;
  ROSE_ASSERT(address.isPtr()||address.isTop());
  AbstractValue change=-1;
  return evalPostComputationOp(estate,address,change);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalPlusPlusOp(SgPlusPlusOp* node,
                                                            SingleEvalResultConstInt operandResult,
                                                            EState estate, EvalMode mode) {
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
                                                              EState estate, EvalMode mode) {
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


// for evaluating LValue Arrow, Dot,
list<SingleEvalResultConstInt> ExprAnalyzer::evalLValueExp(SgNode* node, EState estate, EvalMode mode) {
  ROSE_ASSERT(isSgDotExp(node)||isSgArrowExp(node));
  PState oldPState=*estate.pstate();
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue::createBot());

  SgExpression* arrExp=isSgExpression(SgNodeHelper::getLhs(node));
  SgExpression* indexExp=isSgExpression(SgNodeHelper::getRhs(node));

  list<SingleEvalResultConstInt> lhsResultList=evaluateExpression(arrExp,estate,MODE_VALUE);
  list<SingleEvalResultConstInt> rhsResultList=evaluateExpression(indexExp,estate,MODE_VALUE);
  list<SingleEvalResultConstInt> resultList;
  for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
      riter!=rhsResultList.end();
      ++riter) {
    for(list<SingleEvalResultConstInt>::iterator liter=lhsResultList.begin();
	liter!=lhsResultList.end();
	++liter) {
      SAWYER_MESG(logger[DEBUG])<<"lhs-val: "<<(*liter).result.toString()<<endl;
      SAWYER_MESG(logger[DEBUG])<<"rhs-val: "<<(*riter).result.toString()<<endl;
      list<SingleEvalResultConstInt> intermediateResultList;
      if(SgDotExp* dotExp=isSgDotExp(node)) {
        intermediateResultList=evalDotOp(dotExp,*liter,*riter,estate,MODE_ADDRESS);
      } else if(SgArrowExp* arrowExp=isSgArrowExp(node)) {
        intermediateResultList=evalArrowOp(arrowExp,*liter,*riter,estate,MODE_ADDRESS);
      } else {
        cerr<<"Internal error: ExprAnalyzer::evalLValueExp: wrong oeprator node type: "<<node->class_name()<<endl;
        exit(1);
      }
      // move elements from intermediateResultList to resultList
      resultList.splice(resultList.end(), intermediateResultList);
    }
  }
  return resultList;
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalLValuePntrArrRefExp(SgPntrArrRefExp* node, EState estate, EvalMode mode) {
  // for now we ignore array refs on lhs
  // TODO: assignments in index computations of ignored array ref
  // see ExprAnalyzer.C: case V_SgPntrArrRefExp:
  // since nothing can change (because of being ignored) state remains the same
  SAWYER_MESG(logger[DEBUG])<<"evalLValuePntrArrRefExp"<<endl;
  PState oldPState=*estate.pstate();
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue::createBot());
  if(getSkipArrayAccesses()) {
    res.result=CodeThorn::Top();
    return listify(res);
  } else {
    SgExpression* arrExp=isSgExpression(SgNodeHelper::getLhs(node));
    SgExpression* indexExp=isSgExpression(SgNodeHelper::getRhs(node));

    list<SingleEvalResultConstInt> lhsResultList=evaluateExpression(arrExp,estate,MODE_VALUE);
    list<SingleEvalResultConstInt> rhsResultList=evaluateExpression(indexExp,estate,MODE_VALUE);
    list<SingleEvalResultConstInt> resultList;
    for(list<SingleEvalResultConstInt>::iterator riter=rhsResultList.begin();
        riter!=rhsResultList.end();
        ++riter) {
      for(list<SingleEvalResultConstInt>::iterator liter=lhsResultList.begin();
          liter!=lhsResultList.end();
          ++liter) {
        //cout<<"DEBUG: lhs-val: "<<(*liter).result.toString()<<endl;
        //cout<<"DEBUG: rhs-val: "<<(*riter).result.toString()<<endl;
        list<SingleEvalResultConstInt> intermediateResultList=evalArrayReferenceOp(node,*liter,*riter,estate,MODE_ADDRESS);
        // move elements from intermediateResultList to resultList
        resultList.splice(resultList.end(), intermediateResultList);
      }
    }
    return resultList;  
  }
  // unreachable
  ROSE_ASSERT(false);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalLValueVarRefExp(SgVarRefExp* node, EState estate, EvalMode mode) {
  SAWYER_MESG(logger[TRACE])<<"DEBUG: evalLValueVarRefExp: "<<node->unparseToString()<<" label:"<<estate.label().toString()<<endl;
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue::createBot());
  const PState* pstate=estate.pstate();
  VariableId varId=_variableIdMapping->variableId(node);
  if(isMemberVariable(varId)) {
    ROSE_ASSERT(_variableIdMapping);
    int offset=AbstractValue::getVariableIdMapping()->getOffset(varId);
    SAWYER_MESG(logger[TRACE])<<"DEBUG: evalLValueVarRefExp found STRUCT member: "<<_variableIdMapping->variableName(varId)<<" offset: "<<offset<<endl;
    //res.result=AbstractValue(offset);
    //return listify(res);
  }
  if(pstate->varExists(varId)) {
    if(_variableIdMapping->hasArrayType(varId)) {
      SAWYER_MESG(logger[TRACE])<<"DEBUG: lvalue array address(?): "<<node->unparseToString()<<"EState label:"<<estate.label().toString()<<endl;
      res.result=AbstractValue::createAddressOfArray(varId);
    } else {
      res.result=AbstractValue::createAddressOfVariable(varId);
    }
    return listify(res);
  } else {
    // special mode to represent information not stored in the state
    // i) unmodified arrays: data can be stored outside the state
    // ii) undefined variables mapped to 'top' (abstraction by removing variables from state)
    if(_variableIdMapping->hasArrayType(varId) && _analyzer->getOptionsRef().arraysNotInState==true) {
      // variable is used on the rhs and it has array type implies it avalates to a pointer to that array
      //res.result=AbstractValue(varId.getIdCode());
      SAWYER_MESG(logger[TRACE])<<"DEBUG: lvalue array address (non-existing in state)(?): "<<node->unparseToString()<<endl;
      res.result=AbstractValue::createAddressOfArray(varId);
      return listify(res);
    } else {
      Label lab=estate.label();
      res.result=CodeThorn::Top();
      logger[WARN] << "at label "<<lab<<": "<<(_analyzer->getLabeler()->getNode(lab)->unparseToString())<<": variable not in PState (LValue VarRefExp) (var="<<_variableIdMapping->uniqueVariableName(varId)<<"). Initialized with top."<<endl;
      //cerr << "WARNING: estate: "<<estate.toString(_variableIdMapping)<<endl;
      return listify(res);
    }
  }
  // unreachable
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalLValuePointerDerefExp(SgPointerDerefExp* node, EState estate) {
  SAWYER_MESG(logger[TRACE])<<"DEBUG: evalLValuePtrDerefExp: "<<node->unparseToString()<<" label:"<<estate.label().toString()<<endl;
  // abstract_value(*p) = abstract_eval(p) : the value of 'p' is an abstract address stored in p, which is the lvalue of *p
  SgExpression* operand=node->get_operand_i();
  list<SingleEvalResultConstInt> operandResultList=evaluateExpression(operand,estate,ExprAnalyzer::MODE_VALUE);
  return operandResultList;
}


std::list<SingleEvalResultConstInt> ExprAnalyzer::evalFunctionRefExp(SgFunctionRefExp* node, EState estate, EvalMode mode) {
  //cout<<"DEBUG: evalFunctionRefExp:"<<node->unparseToString()<<" : "<<AstTerm::astTermWithNullValuesToString(node)<<endl;
  if(mode==ExprAnalyzer::MODE_EMPTY_STATE) {
    SingleEvalResultConstInt res;
    res.init(estate,AbstractValue::createTop());
    return listify(res);
  }  
  // create address of function
  SingleEvalResultConstInt res;
  ROSE_ASSERT(_analyzer);

  SgFunctionSymbol* functionSym=node->get_symbol_i();
  ROSE_ASSERT(functionSym);
  SgFunctionDeclaration* funDecl=functionSym->get_declaration();
  //SgFunctionDeclaration* funDecl=SgNodeHelper::findFunctionDeclarationWithFunctionSymbol(functionSym);
  SgDeclarationStatement* defFunDecl=funDecl->get_definingDeclaration();
  if(defFunDecl) {
    // if not resolved, funDef will be 0, and functionEntryLabel will be an invalid label id.
    funDecl=isSgFunctionDeclaration(defFunDecl);
  }
  ROSE_ASSERT(funDecl);
  //cout<<"DEBUG: isForwardDecl:"<<SgNodeHelper::isForwardFunctionDeclaration(funDecl)<<endl;
  //cout<<"DEBUG: fundecl:"<<funDecl->unparseToString()<<endl;
  SgFunctionDefinition* funDef=funDecl->get_definition();
  ROSE_ASSERT(funDef);
  Label funLab=_analyzer->getLabeler()->functionEntryLabel(funDef);
  
  // label of corresponding entry label of function of node; if function is external, then label is an invalid label.
  //cout<<"DEBUG: evalFunctionRefExp: label:"<<funLab.toString()<<endl;
  res.init(estate,AbstractValue::createAddressOfFunction(funLab));
  return listify(res);
}
list<SingleEvalResultConstInt> ExprAnalyzer::evalRValueVarRefExp(SgVarRefExp* node, EState estate, EvalMode mode) {
  //  cout<<"DEBUG: evalRValueVarRefExp:"<<node->unparseToString()<<" : "<<AstTerm::astTermWithNullValuesToString(node)<<endl;
  if(mode==ExprAnalyzer::MODE_EMPTY_STATE) {
    SingleEvalResultConstInt res;
    res.init(estate,AbstractValue::createTop());
    return listify(res);
  }  
  SAWYER_MESG(logger[TRACE])<<"evalRValueVarRefExp: "<<node->unparseToString()<<" id:"<<_variableIdMapping->variableId(isSgVarRefExp(node)).toString()<<"MODE:"<<mode<<endl;
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue::createBot());

  const PState* pstate=estate.pstate();
  VariableId varId=_variableIdMapping->variableId(node);
  ROSE_ASSERT(varId.isValid());
  // check if var is a struct member. if yes return struct-offset.
  //cout<<"DEBUG1: VarRefExp: "<<_variableIdMapping->variableName(varId)<<endl;
  if(isMemberVariable(varId)) {
    int offset=AbstractValue::getVariableIdMapping()->getOffset(varId);
    ROSE_ASSERT(_variableIdMapping);
    SAWYER_MESG(logger[TRACE])<<"DEBUG: evalRValueVarRefExp found STRUCT member: "<<_variableIdMapping->variableName(varId)<<" offset: "<<offset<<endl;
    res.result=AbstractValue(offset); // TODO BYTEMODE ?
    //cout<<"DEBUG1 STRUCT MEMBER: "<<_variableIdMapping->variableName(varId)<<" offset: "<<offset<<endl;
    return listify(res);
  }
  // TODO: as rvalue it represents the entire class
  if(_variableIdMapping->hasClassType(varId)) {
    res.result=AbstractValue::createAddressOfVariable(varId);
    return listify(res);
  }
  if(pstate->varExists(varId)) {
    if(_variableIdMapping->hasArrayType(varId)) {
      res.result=AbstractValue::createAddressOfArray(varId);
    } else {
      if(_variableIdMapping->hasReferenceType(varId)) {
        res.result=readFromReferenceMemoryLocation(estate.label(),pstate,varId);
        logger[INFO]<<"reading from memory reference location: value: "<<res.result.toString()<<endl;
      } else {
        res.result=readFromMemoryLocation(estate.label(),pstate,varId);
      }
    }
    return listify(res);
  } else {
    // special mode to represent information not stored in the state
    // i) unmodified arrays: data can be stored outside the state
    // ii) undefined variables mapped to 'top' (abstraction by removing variables from state)
    if(_variableIdMapping->hasArrayType(varId) && _analyzer->getOptionsRef().arraysNotInState==true) {
      // variable is used on the rhs and it has array type implies it avalates to a pointer to that array
      //res.result=AbstractValue(varId.getIdCode());
      res.result=AbstractValue::createAddressOfArray(varId);
      return listify(res);
    } else {
      res.result=CodeThorn::Top();
      //cerr << "WARNING: variable not in PState (var="<<_variableIdMapping->uniqueVariableName(varId)<<"). Initialized with top."<<endl;
      Label lab=estate.label();
      SAWYER_MESG(logger[WARN]) << "at label "<<lab<<": "<<(_analyzer->getLabeler()->getNode(lab)->unparseToString())<<": variable not in PState (RValue VarRefExp) (var="<<_variableIdMapping->uniqueVariableName(varId)<<"). Initialized with top."<<endl;

      return listify(res);
    }
  }
  // unreachable
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalValueExp(SgValueExp* node, EState estate, EvalMode mode) {
  ROSE_ASSERT(node);
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue::createBot());
  res.result=abstractValueFromSgValueExp(node,mode);
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalFunctionCallArguments(SgFunctionCallExp* funCall, EState estate) {
  SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  for (auto arg : argsList) {
    SAWYER_MESG(logger[TRACE])<<"evaluating function call argument: "<<arg->unparseToString()<<endl;
    // Requirement: code is normalized, does not contain state modifying operations in function arguments
    list<SingleEvalResultConstInt> resList=evaluateExpression(arg,estate);
  }
  SingleEvalResultConstInt res;
  AbstractValue evalResultValue=CodeThorn::Top();
  res.init(estate,evalResultValue);

  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::evalFunctionCall(SgFunctionCallExp* funCall, EState estate) {
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue::createTop());
  SAWYER_MESG(logger[TRACE])<<"Evaluating function call: "<<funCall->unparseToString()<<endl;
  SAWYER_MESG(logger[TRACE])<<"AST function call: "<<AstTerm::astTermWithNullValuesToString(funCall)<<endl;
  if(getStdFunctionSemantics()) {
    string funName=SgNodeHelper::getFunctionName(funCall);
    if(funName=="malloc") {
      return evalFunctionCallMalloc(funCall,estate);
    } else if(funName=="free") {
      return evalFunctionCallFree(funCall,estate);
    } else if(funName=="strlen") {
      return PredefinedSemanticFunctions::evalFunctionCallStrLen(this,funCall,estate);
    } else if(funName=="memcpy") {
      return PredefinedSemanticFunctions::evalFunctionCallMemCpy(this,funCall,estate);
    } else if(funName=="fflush") {
      // ignoring fflush
      // res initialized above
      return evalFunctionCallArguments(funCall,estate);
    } else if(funName=="time"||funName=="srand"||funName=="rand") {
      // arguments must already be analyzed (normalized code) : TODO check that it is a single variable
      // result is top (time/srand/rand return any value)
      return listify(res); // return top (initialized above (res.init))
    } else if(funName=="__assert_fail") {
      // TODO: create state
      evalFunctionCallArguments(funCall,estate);
      estate.io.recordVerificationError();
      return listify(res);
    } else if(funName=="printf" && (getInterpreterMode()==IM_ENABLED)) {
      // call fprint function in mode CONCRETE and generate output
      // (1) obtain arguments from estate
      // (2) marshall arguments
      // (3) perform function call (causing side effect on stdout (or written to provided file))
      return execFunctionCallPrintf(funCall,estate);
    } else if(funName=="scanf" && (getInterpreterMode()==IM_ENABLED)) {
      // call scanf function in mode CONCRETE and generate output
      // (1) obtain arguments from estate
      // (2) marshall arguments
      // (3) perform function call (causing side effect on stdin)
      return execFunctionCallScanf(funCall,estate);
    }
  }
  if(getSkipUnknownFunctionCalls()) {
    return evalFunctionCallArguments(funCall,estate);
  } else {
    string s=funCall->unparseToString();
    throw CodeThorn::Exception("unknown semantics of function call inside expression: "+s);
  }
}

list<SingleEvalResultConstInt> ExprAnalyzer::execFunctionCallPrintf(SgFunctionCallExp* funCall, EState estate) {
  //cout<<"DEBUG: ExprAnalyzer::execFunctionCallPrintf"<<endl;
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue::createTop()); // default value for void function call
  ROSE_ASSERT(_variableIdMapping);
  SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  auto iter=argsList.begin();
  ROSE_ASSERT(iter!=argsList.end());
  SgNode* arg=*iter++;
  while(SgCastExp* castExp=isSgCastExp(arg)) {
    arg=castExp->get_operand_i();
  }
  if(SgStringVal* formatStringVal=isSgStringVal(arg)) {
    ROSE_ASSERT(formatStringVal);
    string formatString=formatStringVal->get_value();
    vector<string> avStringVector;
    for(size_t i=1;i<argsList.size();i++) {
      SgExpression* arg=*iter++;
      list<SingleEvalResultConstInt> argResList=evaluateExpression(arg,estate);
      if(argResList.size()!=1) {
        cerr<<"Error: conditional control-flow in printf argument not supported. Expression normalization required."<<endl;
        exit(1);
      } else {
        AbstractValue av=(*argResList.begin()).value();
        avStringVector.push_back(av.toString(_variableIdMapping));
      }
    }
    // replace all uses of %? with respective AVString
    string concAVString;
    size_t j=0;
    for(size_t i=0;i<formatString.size();++i) {
      if(formatString[i]=='%') {
        i++; // skip next character
        if(j>=avStringVector.size()) {
          // number of arguments and uses of '%' don't match in input
          // program. This could be reported as program error.  For now
          // we just do not produce an output (as the original program
          // does not either)
          // TODO: report input program error
          continue; // continue to print other characters
        }
        concAVString+=avStringVector[j++];
      } else if(formatString[i]=='\\') {
        if(i+1<=formatString.size()-1 && formatString[i+1]=='%') {
          i+=1; // process additional character '%'
          concAVString+="\%";
        } else if(i+1<=formatString.size()-1 && formatString[i+1]=='n') {
          concAVString+='\n'; // this generates a proper newline
          i+=1; // process additional character 'n'
        } else {
          concAVString+='\\';
        }
      } else {
        // any other character
        concAVString+=formatString[i];
      }
    }
    string fileName=getInterpreterModeFileName();
    if(fileName!="") {
      bool ok=CppStdUtilities::appendFile(fileName,concAVString);
      if(!ok) {
        cerr<<"Error: could not open output file "<<fileName<<endl;
        exit(1);
      }
    } else {
      cout<<concAVString;
    }
  } else {
    // first argument is not a string, must be some constant
    // since it is only printed, it is not modifying the state
    // TODO: it could be recorded as output state
    
    cout<<"WARNING: unsupported printf argument: "<<(arg)->class_name()<<" : "<<(arg)->unparseToString()<<endl;
  }
  return listify(res);
}

list<SingleEvalResultConstInt> ExprAnalyzer::execFunctionCallScanf(SgFunctionCallExp* funCall, EState estate) {
  //cout<<"DEBUG: ExprAnalyzer::execFunctionCallScanf"<<endl;
  SingleEvalResultConstInt res;
  res.init(estate,AbstractValue::createTop()); // default value for void function call
  ROSE_ASSERT(_variableIdMapping);
  SgExpressionPtrList& argsList=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  auto iter=argsList.begin();
  ROSE_ASSERT(iter!=argsList.end());
  SgStringVal* formatStringVal=isSgStringVal(*iter++);
  ROSE_ASSERT(formatStringVal);
  string formatString=formatStringVal->get_value();
  vector<string> avStringVector;
  for(size_t i=1;i<argsList.size();i++) {
    SgExpression* arg=*iter++;
    list<SingleEvalResultConstInt> argResList=evaluateExpression(arg,estate);
    if(argResList.size()!=1) {
      cerr<<"Error: conditional control-flow in printf argument not supported. Expression normalization required."<<endl;
        exit(1);
    } else {
      AbstractValue av=(*argResList.begin()).value();
      avStringVector.push_back(av.toString(_variableIdMapping));
    }
  }
  // replace all uses of %? with respective AVString
  string concAVString;
  size_t j=0;
  for(size_t i=0;i<formatString.size();++i) {
    if(formatString[i]=='%') {
      char controlChar=formatString[i];
      i++; // skip next character
      if(j>=avStringVector.size()) {
        // number of arguments and uses of '%' don't match in input
        // program. This could be reported as program error.  For now
        // we just do not produce an output (as the original program
        // does not either)
        // TODO: report input program error
        continue; // continue to print other characters
      }
      switch(controlChar) {
      case 'd': {
        int val;
        int numParsed=scanf("%d",&val); // read integer from stdin
        AbstractValue av=((numParsed==1)?argsList[j]:AbstractValue::createTop()); // assume any value in case of error
        // write val into state at address argsList[j]
        if(av.isPtr()) {
          PState pstate=*estate.pstate();
          writeToMemoryLocation(estate.label(),&pstate,av,val);
          // TODO: pstate is not used yet, because estate is only read but not returned (hence this is a noop and not an update)
          cout<<"Warning: interpreter mode: scanf: memory location "<<av.toString(_variableIdMapping)<<" not updated (not implemented yet)."<<endl;
        } else {
          cerr<<"Warning: interpreter mode: scanf writing to non-address value (ignored)"<<endl;
        }
        break;
      }
      default:
        cerr<<"Warning: interpreter mode: scanf using unknown type "<<endl;
      }
    } else if(formatString[i]=='\\') {
      if(i+1<=formatString.size()-1 && formatString[i+1]=='%') {
        i+=1; // process additional character '%'
        concAVString+="\%";
      } else if(i+1<=formatString.size()-1 && formatString[i+1]=='n') {
        i+=1; // process additional character 'n'
      } else {
        // nothing to do (do not match)
      }
    } else {
      // any other character
    }
  }
  return listify(res);
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
    logger[TRACE]<<"function call malloc: allocated at: "<<allocatedMemoryPtr.toString()<<endl;
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
#if 0
    // TODO: update of state (not allowed here)
    // generate representation of allocated memory (we only have the address so far)
    if(memoryRegionSize>0 && memoryRegionSize<=_analyzer->getOptionsRef().maxExactMemorySizeRepresentation) {
      // reserve memory
      for(int i=0;i<memoryRegionSize;++i) {
        AbstractValue arrayElemAddr=AbstractValue::createAddressOfArrayElement(memLocVarId,AbstractValue::createIntegerValue(CodeThorn::BITYPE_UINT,i));
        AbstractValue undefVal=AbstractValue::createUndefined();
        PState* pstate=estate.pstate();
        pstate->writeToMemoryLocation(arrayElemAddr,undefVal);
      }
    }
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
      int memoryRegionSize=getMemoryRegionNumElements(arg1val);
      // can be marked as deallocated (currently this does not impact the analysis)
      //variableIdMapping->setSize(arg1Val.getVariableId(),-1);
      // top maps to -1
      ROSE_ASSERT(memoryRegionSize>=-1);
    }
    res.init(estate,AbstractValue::createTop()); // void result (using top here)
  } else {
    // this will become an error in future
    cerr<<"WARNING: unknown free function "<<funCall->unparseToString()<<endl;
  }
  return listify(res);
}

// returns size, or -1 (=any) in case pointer is top.
int ExprAnalyzer::getMemoryRegionNumElements(CodeThorn::AbstractValue ptrToRegion) {
  if(ptrToRegion.isTop()) {
    return -1;
  }
  ROSE_ASSERT(ptrToRegion.isPtr());
  VariableId ptrVariableId=ptrToRegion.getVariableId();
  int size=_variableIdMapping->getNumberOfElements(ptrVariableId);
  SAWYER_MESG(logger[TRACE])<<"getMemoryRegionNumElements(ptrToRegion): ptrToRegion with ptrVariableId:"<<ptrVariableId<<" "<<_variableIdMapping->variableName(ptrVariableId)<<" numberOfElements: "<<size<<endl;
  return size;
}

int ExprAnalyzer::getMemoryRegionElementSize(CodeThorn::AbstractValue ptrToRegion) {
  if(ptrToRegion.isTop()) {
    return -1;
  }
  ROSE_ASSERT(ptrToRegion.isPtr());
  VariableId ptrVariableId=ptrToRegion.getVariableId();
  int size=_variableIdMapping->getElementSize(ptrVariableId);
  SAWYER_MESG(logger[TRACE])<<"getMemoryRegionNumElements(ptrToRegion): ptrToRegion with ptrVariableId:"<<ptrVariableId<<" "<<_variableIdMapping->variableName(ptrVariableId)<<" numberOfElements: "<<size<<endl;
  return size;
}


// true if access is correct. false if out-of-bounds access.
// TODO: rewrite using new abstract values with array address references
bool ExprAnalyzer::accessIsWithinArrayBounds(VariableId arrayVarId,int accessIndex) {
  // check array bounds
  int arraySize=_variableIdMapping->getNumberOfElements(arrayVarId);
  return !(accessIndex<0||accessIndex>=arraySize);
}

enum MemoryAccessBounds ExprAnalyzer::checkMemoryAccessBounds(AbstractValue address) {
  if(address.isTop()) {
    return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
  } if(address.isBot()) {
    return ACCESS_NON_EXISTING;
  } if(address.isNullPtr()) {
    return ACCESS_DEFINITELY_NP;
  } else {
    AbstractValue offset=address.getIndexValue();
    if(offset.isTop()) {
      return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
    } else if(offset.isBot()) {
      return ACCESS_NON_EXISTING;
    } else {
      VariableId memId=address.getVariableId();
      // this must be the only remaining case
      if(offset.isConstInt()) {
        // check array bounds
        int memRegionSize=_variableIdMapping->getNumberOfElements(memId);
        if(memRegionSize==0)
          return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS; // will become ACCESS_DEFINITELY_OUTSIDE_BOUNDS;
        int accessIndex=offset.getIntValue();
        if(!(accessIndex<0||accessIndex>=memRegionSize)) {
          return ACCESS_DEFINITELY_INSIDE_BOUNDS;
        } else {
          return ACCESS_DEFINITELY_OUTSIDE_BOUNDS;
        }
      } else {
        return ACCESS_POTENTIALLY_OUTSIDE_BOUNDS;
      }
    }
  }
}    

ProgramLocationsReport ExprAnalyzer::getProgramLocationsReport(enum AnalysisSelector analysisSelector) {
  ProgramLocationsReport report;
#pragma omp critical(VIOLATIONRECORDING)
  report=_violatingLocations.at(analysisSelector);
  return report;
}

// deprecated
ProgramLocationsReport ExprAnalyzer::getViolatingLocations(enum AnalysisSelector analysisSelector) {
  return getProgramLocationsReport(analysisSelector);
}

void ExprAnalyzer::initViolatingLocations() {
  for(int i=ANALYSIS_NULL_POINTER;i<ANALYSIS_NUM;i++) {
    _violatingLocations.push_back(ProgramLocationsReport());
  }
}
std::string ExprAnalyzer::analysisSelectorToString(AnalysisSelector sel) {
  switch(sel) {
  case ANALYSIS_NULL_POINTER: return "null-pointer";
  case ANALYSIS_OUT_OF_BOUNDS: return "out-of-bounds";
  case ANALYSIS_UNINITIALIZED: return "unitialized-value";
  default:
    SAWYER_MESG(logger[FATAL])<<"ExprAnalyzer::analysisSelectorToString: unknown selector."<<endl;
    exit(1);
  }
}

std::string ExprAnalyzer::sourceLocationAndNodeToString(Label lab) {
  SgNode* node=_analyzer->getLabeler()->getNode(lab);
  return SgNodeHelper::sourceLocationAndNodeToString(node);
}

void ExprAnalyzer::recordDefinitiveViolatingLocation(enum AnalysisSelector analysisSelector, Label label) {
#pragma omp critical(VIOLATIONRECORDING)
  {
    _violatingLocations.at(analysisSelector).recordDefinitiveLocation(label);
    if(_printDetectedViolations) {
      cout<<analysisSelectorToString(analysisSelector)
          <<": definitive violation at L"<<label.toString()<<":"
          <<sourceLocationAndNodeToString(label)
          <<endl;
    }
  }
}

void ExprAnalyzer::recordPotentialViolatingLocation(enum AnalysisSelector analysisSelector, Label label) {
#pragma omp critical(VIOLATIONRECORDING)
  {
    _violatingLocations.at(analysisSelector).recordPotentialLocation(label);
    if(_printDetectedViolations)
      cout<<analysisSelectorToString(analysisSelector)<<": potential violation at label "<<label.toString()<<endl;
  }
}

void ExprAnalyzer::recordDefinitiveNullPointerDereferenceLocation(Label label) {
  recordDefinitiveViolatingLocation(ANALYSIS_NULL_POINTER,label);
}

void ExprAnalyzer::recordPotentialNullPointerDereferenceLocation(Label label) {
  recordPotentialViolatingLocation(ANALYSIS_NULL_POINTER,label);
}

void ExprAnalyzer::recordDefinitiveOutOfBoundsAccessLocation(Label label) {
  recordDefinitiveViolatingLocation(ANALYSIS_OUT_OF_BOUNDS,label);
  if(_printDetectedViolations)
    cout<<"Violation detected: definitive out of bounds access at label "<<label.toString()<<endl;
}

void ExprAnalyzer::recordPotentialOutOfBoundsAccessLocation(Label label) {
  recordPotentialViolatingLocation(ANALYSIS_OUT_OF_BOUNDS,label);
  if(_printDetectedViolations)
    cout<<"Violation detected: potential out of bounds access at label "<<label.toString()<<endl;
}

void ExprAnalyzer::recordDefinitiveUninitializedAccessLocation(Label label) {
  recordDefinitiveViolatingLocation(ANALYSIS_UNINITIALIZED,label);
  if(_printDetectedViolations)
    cout<<"Violation detected: definitive uninitialized value read at label "<<label.toString()<<endl;
}

void ExprAnalyzer::recordPotentialUninitializedAccessLocation(Label label) {
  recordPotentialViolatingLocation(ANALYSIS_UNINITIALIZED,label);
  if(_printDetectedViolations)
    cout<<"Violation detected: potential uninitialized value read at label "<<label.toString()<<endl;
}

bool ExprAnalyzer::getPrintDetectedViolations() {
  return _printDetectedViolations;
}
void ExprAnalyzer::setPrintDetectedViolations(bool flag) {
  _printDetectedViolations=flag;
}

bool ExprAnalyzer::isMemberVariable(CodeThorn::VariableId varId) {
  return AbstractValue::getVariableIdMapping()->isMemberVariable(varId);
}

bool ExprAnalyzer::definitiveErrorDetected() {
  for(int i=ANALYSIS_NULL_POINTER;i<ANALYSIS_NUM;i++) {
    if(_violatingLocations.at(i).numDefinitiveLocations()>0)
      return true;
  }
  return false;
}

bool ExprAnalyzer::potentialErrorDetected() {
  for(int i=ANALYSIS_NULL_POINTER;i<ANALYSIS_NUM;i++) {
    if(_violatingLocations.at(i).numPotentialLocations()>0)
      return true;
  }
  return false;
}

AbstractValue ExprAnalyzer::readFromMemoryLocation(Label lab, const PState* pstate, AbstractValue memLoc) {
  // inspect memory location here
  if(memLoc.isNullPtr()) {
    recordDefinitiveNullPointerDereferenceLocation(lab);
    //return AbstractValue::createBot();
    return AbstractValue::createTop();
  }
  if(memLoc.isTop()) {
    recordPotentialNullPointerDereferenceLocation(lab);
    recordPotentialOutOfBoundsAccessLocation(lab);
    recordPotentialViolatingLocation(ANALYSIS_UNINITIALIZED,lab);
  } else if(!pstate->memLocExists(memLoc)) {
    //recordPotentialOutOfBoundsAccessLocation(lab);
    recordPotentialUninitializedAccessLocation(lab);
  }
  AbstractValue val=pstate->readFromMemoryLocation(memLoc);
  // investigate value here
  if(val.isUndefined()) {
    recordPotentialUninitializedAccessLocation(lab);
  }
  return val;
}

void ExprAnalyzer::writeToMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue) {
  // inspect everything here
  if(memLoc.isTop()) {
    logger[WARN]<<"writing to arbitrary memloc: "<<lab.toString()<<":"<<memLoc.toString(_variableIdMapping)<<":="<<newValue.toString(_variableIdMapping)<<endl;
    recordPotentialOutOfBoundsAccessLocation(lab);
  } else if(!pstate->memLocExists(memLoc)) {
    if(!newValue.isUndefined()) {
      recordPotentialOutOfBoundsAccessLocation(lab);
      logger[WARN]<<"writing defined value to memloc not in state: "<<lab.toString()<<":"<<memLoc.toString(_variableIdMapping)<<":="<<newValue.toString(_variableIdMapping)<<endl;
    }
  }
  pstate->writeToMemoryLocation(memLoc,newValue);
}

AbstractValue ExprAnalyzer::readFromReferenceMemoryLocation(Label lab, const PState* pstate, AbstractValue memLoc) {
  AbstractValue referedMemLoc=readFromMemoryLocation(lab,pstate,memLoc);
  return readFromMemoryLocation(lab,pstate,referedMemLoc);
}

void ExprAnalyzer::writeToReferenceMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue) {
  AbstractValue referedMemLoc=readFromMemoryLocation(lab,pstate,memLoc);
  writeToMemoryLocation(lab,pstate,referedMemLoc,newValue);
}

void ExprAnalyzer::initializeMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc, AbstractValue newValue) {
  reserveMemoryLocation(lab,pstate,memLoc);
  writeToMemoryLocation(lab,pstate,memLoc,newValue);
 }

void ExprAnalyzer::reserveMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc) {
  writeUndefToMemoryLocation(lab,pstate,memLoc);
}

void ExprAnalyzer::writeUndefToMemoryLocation(Label lab, PState* pstate, AbstractValue memLoc) {
  AbstractValue undefValue=AbstractValue::createUndefined();
  VariableId varId=memLoc.getVariableId();
  if(AbstractValue::byteMode) {
    memLoc.setElementTypeSize(_variableIdMapping->getElementSize(varId)); // TODO: structs vars?
  }
  writeToMemoryLocation(lab,pstate,memLoc,undefValue);
}

void ExprAnalyzer::writeUndefToMemoryLocation(PState* pstate, AbstractValue memLoc) {
  AbstractValue undefValue=AbstractValue::createUndefined();
  if(AbstractValue::byteMode && memLoc.getElementTypeSize()==0) {
    VariableId varId=memLoc.getVariableId();
    memLoc.setElementTypeSize(_variableIdMapping->getElementSize(varId)); // TODO: structs vars?
  }
  pstate->writeToMemoryLocation(memLoc,undefValue);
}

void ExprAnalyzer::printLoggerWarning(EState& estate) {
  Label lab=estate.label();
  if(_analyzer) {
    SAWYER_MESG(logger[WARN]) << "at label "<<lab<<": "<<(_analyzer->getLabeler()->getNode(lab)->unparseToString())<<": this pointer set to top."<<endl;
  } else {
    SAWYER_MESG(logger[WARN]) << "at label "<<lab<<": "<<": this pointer set to top."<<endl;
  }
}
