#include "sage3basic.h"
#include <iostream>
using namespace std;

#include "CollectionOperators.h"
#include "PropertyState.h"
#include "NumberIntervalLattice.h"
#include "Labeler.h"
#include "VariableIdMapping.h"
#include "IntervalTransferFunctions.h"
#include "AnalysisAbstractionLayer.h"

CodeThorn::IntervalTransferFunctions::IntervalTransferFunctions():
  _cppExprEvaluator(0),
  _domain(0)
{
}

CodeThorn::IntervalTransferFunctions::~IntervalTransferFunctions() {
  if(_cppExprEvaluator)
    delete _cppExprEvaluator;
}

/*! 
  * \author Markus Schordan
  * \date 2015.
 */
void CodeThorn::IntervalTransferFunctions::transferSwitchCase(Label lab,SgStatement* condStmt, SgCaseOptionStmt* caseStmt,Lattice& pstate) {
  IntervalPropertyState& ips=dynamic_cast<IntervalPropertyState&>(pstate);
  if(isSgExprStatement(condStmt)) {
    SgExpression* cond=isSgExpression(SgNodeHelper::getExprStmtChild(condStmt));
    ROSE_ASSERT(cond);
    NumberIntervalLattice knownValueIntervalOfSwitchVar;
    /* this varId is only used if the switch-cond is a variable and we can bound the interval of the case numbers to this variable. 
       this varId will only be removed at the end of the function (it only gives better precision if conditionals depend on this variable
       inside the switch-stmt (a rather rare case)
    */
    VariableId varId;
    if(SgVarRefExp* varRefExp=isSgVarRefExp(cond)) {
      // if the switch-cond is a variable set the interval to the case label (this function is called for each case label)
      varId=getVariableIdMapping()->variableId(varRefExp);
      ROSE_ASSERT(varId.isValid());
      knownValueIntervalOfSwitchVar=ips.getVariable(varId);
    } else {
      // the switch-cond is not a variable. We evaluate the expression but do not maintain its interval in a program variable
      knownValueIntervalOfSwitchVar=evalExpression(lab, cond, pstate);
    }
    // handle case NUM:
    SgExpression* caseExpr=caseStmt->get_key();
    ROSE_ASSERT(caseExpr);
    //cout<<"INFO: transferSwitchCase: VAR"<<varRefExp->unparseToString()<<"=="<<caseExpr->unparseToString()<<endl;
    NumberIntervalLattice num;
    SgExpression* caseExprOptionalRangeEnd=caseStmt->get_key_range_end();
    if(caseExprOptionalRangeEnd==0) {
      // case NUM:
      num=evalExpression(lab, caseExpr, pstate);
    } else {
      // case NUM1 ... NUM2:
      NumberIntervalLattice numStart=evalExpression(lab, caseExpr, pstate);
      NumberIntervalLattice numEnd=evalExpression(lab, caseExprOptionalRangeEnd, pstate);
      num=NumberIntervalLattice::join(numStart,numEnd);
      //cout<<"DEBUG: range: "<<num.toString()<<endl;
    }
#if 0
    if(SgIntVal* sgIntVal=isSgIntVal(caseExpr)) {
      int val=sgIntVal->get_value();
      num.setLow(val);
      num.setHigh(val);
    }
#endif
    if(!NumberIntervalLattice::haveOverlap(knownValueIntervalOfSwitchVar,num)) {
      //cout<<"INFO: state detected non-reachable."<<endl;
      ips.setBot();
      return;
    }
    if(varId.isValid()) {
#if 1
      ips.setVariable(varId,num);
#else
      // join with any value that flows in [for testing only]
      ips.setVariable(varId,NumberIntervalLattice::join(num,ips.getVariable(varId)));
#endif
    }
  } else {
    cerr<<"Error: switch condition not a SgExprStmt. Unsupported program structure."<<endl;
    exit(1);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2015.
 */
void CodeThorn::IntervalTransferFunctions::transferCondition(Edge edge, Lattice& pstate) {
  IntervalPropertyState& ips=dynamic_cast<IntervalPropertyState&>(pstate);
  Label lab0=edge.source();
  //Label lab1=edge.target();
  SgNode* node=getLabeler()->getNode(lab0);
  if(isSgExprStatement(node)) {
    node=SgNodeHelper::getExprStmtChild(node);
  }
  if(SgExpression* expr=isSgExpression(node)) {
    NumberIntervalLattice res=evalExpression(lab0,expr,pstate);
    // schroder3 (2016-08-25): Removed assertions that checked whether the result is either true,
    //  false, bot, or top because the expression result can also be an interval that is neither
    //  true, false, bot, nor top (e.g. "if(0.5) { }" returns the interval [0, 1] as condition
    //  result). Just set the lattice to bot if the result is bot or if the branch is unreachable
    //  (and do nothing in all other cases):
    if(res.isBot() || (res.isFalse() && edge.isType(CodeThorn::EDGE_TRUE))
                   || (res.isTrue() && edge.isType(CodeThorn::EDGE_FALSE))
    ) {
      //cout<<"INFO: detected non-reachable state."<<endl;
      //cout<<"DEBUG: EDGE: "<<edge.toString()<<endl;
      //cout<<"RESULT: "<<res.toString()<<endl;
      ips.setBot();
    }
    return;
  } else {
    cerr<<"Error: interval analysis: unsupported condition type."<<endl;
    exit(1);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2015.
 */
void CodeThorn::IntervalTransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& pstate) {
  // schroder3 (2016-08-25): Added if to ignore SgNullExpressions (e.g. ";;")
  if(!isSgNullExpression(node)) {
    evalExpression(lab,node,pstate); // ignore return value
  }
}

void CodeThorn::IntervalTransferFunctions::transferReturnStmtExpr(Label lab, SgExpression* node, Lattice& pstate) {
  IntervalPropertyState& ips=dynamic_cast<IntervalPropertyState&>(pstate);
  NumberIntervalLattice res=evalExpression(lab,node,pstate);
  VariableId resVarId=getResultVariableId();
  ips.setVariable(resVarId,res);
}

/*! 
  * \author Markus Schordan
  * \date 2015.
 */
CodeThorn::NumberIntervalLattice CodeThorn::IntervalTransferFunctions::evalExpression(Label lab, SgExpression* node, Lattice& pstate) {
  //ROSE_ASSERT(getVariableIdMapping()); TODO
  NumberIntervalLattice niLattice;
  //cout<<"TINFO: transferExpression "<<node->unparseToString()<<endl;
  _cppExprEvaluator->setPropertyState(&pstate);
  //cout<<"PSTATE:";pstate.toStream(cout,getVariableIdMapping());cout<<endl;
  niLattice=_cppExprEvaluator->evaluate(node);
  // schroder3 (2016-08-09): Check the result before returning it to the caller. If the
  //  result of an expression is bot (e.g. because of a division by zero) then set the
  //  interval property state to bot too because the following code is unreachable.
  if(niLattice.isBot()) {
    IntervalPropertyState& ips = dynamic_cast<IntervalPropertyState&>(pstate);
    ips.setBot();
  }
  return niLattice;
}

/*! 
  * \author Markus Schordan
  * \date 2015.
 */
void CodeThorn::IntervalTransferFunctions::transferDeclaration(Label lab, SgVariableDeclaration* declnode, Lattice& element) {
  ROSE_ASSERT(this!=0);
  SgInitializedName* node=SgNodeHelper::getInitializedNameOfVariableDeclaration(declnode);
  ROSE_ASSERT(getVariableIdMapping());
  VariableId varId=getVariableIdMapping()->variableId(node);
  IntervalPropertyState& ips=dynamic_cast<IntervalPropertyState&>(element);
  ips.addVariable(varId);
  SgExpression* initExp=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(declnode);

  // Static local variables in functions are always set to top irrespective of their initialization,
  // since this would require a calling context abstraction which is not available yet.
  // in all other cases the initializer is analyzed.
  if(SageInterface::isStatic(declnode)) {
    NumberIntervalLattice res; // 
    res.setTop();
    ips.setVariable(varId,res);
  } else if(initExp) {
    NumberIntervalLattice res=evalExpression(lab,initExp,ips);
    ROSE_ASSERT(!res.isBot());
    ips.setVariable(varId,res);
  }
}


/*! 
  * \author Markus Schordan
  * \date 2015.
 */
void CodeThorn::IntervalTransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments,Lattice& element) {
  int paramNr=0;
  IntervalPropertyState& ips=dynamic_cast<IntervalPropertyState&>(element);
  // TODO: handle external function call: do not add paramters and model pointer arguments
  //cout<<"DEBUG: label: "<<lab.toString()<<" is-external: "<<getLabeler()->isExternalFunctionCallLabel(lab)<<endl;
  if(getLabeler()->isExternalFunctionCallLabel(lab)) {
    //cout<<"DEBUG: external function call detected: "<<callExp->unparseToString()<<endl;
    // arguments must be processed for worst-case assumptions: any pointer/address passed can be used by external function to potentially modifiy reachable memory cells
    // an external function my modify any address-taken variable in the program (including global variables)
    CodeThorn::PointerAnalysisInterface* pa=getPointerAnalysisInterface();
    ips.topifyVariableSet(pa->getModByPointer());
  } else {
    //cout<<"DEBUG: function call detected: "<<callExp->unparseToString()<<endl;
    for(SgExpressionPtrList::iterator i=arguments.begin();i!=arguments.end();++i) {
      VariableId paramId=getParameterVariableId(paramNr);
      ips.addVariable(paramId);
      ips.setVariable(paramId,evalExpression(lab,*i,ips));
      paramNr++;
    }
  }
}
/*! 
  * \author Markus Schordan
  * \date 2015.
 */
void CodeThorn::IntervalTransferFunctions::transferFunctionCallReturn(Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element) {
  IntervalPropertyState& ips=dynamic_cast<IntervalPropertyState&>(element);
  if(isSgCompoundAssignOp(callExp->get_parent())) {
    cerr<<"Error: transferFunctionCallReturn: compound assignment of function call results not supported. Normalization required."<<endl;
    exit(1);
  }
  // determine variable-id of dedicated variable for holding the return value
  VariableId resVarId=getResultVariableId();
  if(lhsVar!=0) {
    //cout<<"DEBUG: updated var=f(...)."<<endl;
    VariableId varId=getVariableIdMapping()->variableId(lhsVar);  
    // set lhs-var to the return-value
    ips.setVariable(varId,ips.getVariable(resVarId));
  }
  // remove the return-variable from the state (the return-variable is temporary)
  ips.removeVariable(resVarId);
}

/*! 
  * \author Markus Schordan
  * \date 2015.
 */
void CodeThorn::IntervalTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element) {
  // generate Intervals for each parameter variable
  int paramNr=0;
  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
      i!=formalParameters.end();
      ++i) {
    SgInitializedName* formalParameterName=*i;
    VariableId formalParameterVarId=getVariableIdMapping()->variableId(formalParameterName);
    IntervalPropertyState& ips=dynamic_cast<IntervalPropertyState&>(element);
    // get value of actual parameter
    VariableId paramId=getParameterVariableId(paramNr);
    // remove parameter variable
    ips.addVariable(formalParameterVarId);
    ips.setVariable(formalParameterVarId,ips.getVariable(paramId));
    ips.removeVariable(paramId);
    paramNr++;
  }
}

/*! 
  * \author Markus Schordan
  * \date 2015.
 */
void CodeThorn::IntervalTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element) {
  // remove all declared variables at function exit (including function parameter variables)
  for(VariableIdSet::iterator i=localVariablesInFunction.begin();i!=localVariablesInFunction.end();++i) {
    VariableId varId=*i;
    IntervalPropertyState& ips=dynamic_cast<IntervalPropertyState&>(element);
    ips.removeVariable(varId);
  }
}

CodeThorn::CppExprEvaluator* CodeThorn::IntervalTransferFunctions::getCppExprEvaluator() {
  return _cppExprEvaluator;
}

void CodeThorn::IntervalTransferFunctions::setCppExprEvaluator(CodeThorn::CppExprEvaluator* expEval) {
  _cppExprEvaluator=expEval;
}

void CodeThorn::IntervalTransferFunctions::setSkipUnknownFunctionCalls(bool flag) {
  ROSE_ASSERT(getCppExprEvaluator());
  getCppExprEvaluator()->setSkipUnknownFunctionCalls(flag);
}

void CodeThorn::IntervalTransferFunctions::initializeExtremalValue(Lattice& element) {
  CodeThorn::IntervalPropertyState* pstate=dynamic_cast<CodeThorn::IntervalPropertyState*>(&element);
  pstate->setEmptyState();
  //iElement->... init to empty state, not being bottom
  cout<<"INFO: initialized extremal value."<<endl;
}
