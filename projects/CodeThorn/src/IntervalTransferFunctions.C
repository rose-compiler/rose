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

SPRAY::IntervalTransferFunctions::IntervalTransferFunctions():
  _cppExprEvaluator(0),
  _domain(0)
{
}

#if 0
SPRAY::IntervalTransferFunctions::IntervalTransferFunctions(
                                                     NumberIntervalLattice* domain, 
                                                     PropertyState* p, 
                                                     Labeler* l, 
                                                     VariableIdMapping* vid)
  :_domain(domain),_labeler(l),_variableIdMapping(vid) {
  _cppExprEvaluator=new SPRAY::CppExprEvaluator(domain,p,vid);
}
#endif

SPRAY::IntervalTransferFunctions::~IntervalTransferFunctions() {
  if(_cppExprEvaluator)
    delete _cppExprEvaluator;
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::IntervalTransferFunctions::transferSwitchCase(Label lab,SgStatement* condStmt, SgCaseOptionStmt* caseStmt,Lattice& pstate) {
  IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(&pstate);
  ROSE_ASSERT(ips);
  //ROSE_ASSERT(_variableIdMapping); TODO
  // temporary: handle only special case of var and constant
  if(isSgExprStatement(condStmt)) {
    // TODO
    SgNode* cond=SgNodeHelper::getExprStmtChild(condStmt);
    if(SgVarRefExp* varRefExp=isSgVarRefExp(cond)) {
      VariableId varId=_variableIdMapping->variableId(varRefExp);
      ROSE_ASSERT(varId.isValid());
      SgExpression* caseExpr=caseStmt->get_key();
      ROSE_ASSERT(caseExpr);
      //cout<<"INFO: transferSwitchCase: VAR"<<varRefExp->unparseToString()<<"=="<<caseExpr->unparseToString()<<endl;
      if(SgIntVal* sgIntVal=isSgIntVal(caseExpr)) {
        ROSE_ASSERT(_cppExprEvaluator);
        NumberIntervalLattice num;
#if 0
        // not stable yet
        num=_cppExprEvaluator->evaluate(caseExpr);
#else
        int val=sgIntVal->get_value();
        num.setLow(val);
        num.setHigh(val);
        NumberIntervalLattice knownValueIntervalOfSwitchVar=ips->getVariable(varId);
	if(!NumberIntervalLattice::haveOverlap(knownValueIntervalOfSwitchVar,num)) {
	  //cout<<"INFO: state detected non-reachable."<<endl;
	  ips->setBot();
	  return;
	}
#endif
        ips->setVariable(varId,num);
      }
      //cout<<"DONE."<<endl;
    }
  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::IntervalTransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& pstate) {
  evalExpression(lab,node,pstate); // ignore return value
}

void SPRAY::IntervalTransferFunctions::transferReturnStmtExpr(Label lab, SgExpression* node, Lattice& pstate) {
  IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(&pstate);
  ROSE_ASSERT(ips);
  NumberIntervalLattice res=evalExpression(lab,node,pstate);
  VariableId resVarId=getResultVariableId();
  ips->setVariable(resVarId,res);
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
SPRAY::NumberIntervalLattice SPRAY::IntervalTransferFunctions::evalExpression(Label lab, SgExpression* node, Lattice& pstate) {
  //ROSE_ASSERT(_variableIdMapping); TODO
  NumberIntervalLattice niLattice;
  //cout<<"TINFO: transferExpression "<<node->unparseToString()<<endl;
  _cppExprEvaluator->setPropertyState(&pstate);
  //cout<<"PSTATE:";pstate.toStream(cout,_variableIdMapping);cout<<endl;
  niLattice=_cppExprEvaluator->evaluate(node);
  return niLattice;
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::IntervalTransferFunctions::transferDeclaration(Label lab, SgVariableDeclaration* declnode, Lattice& element) {
  ROSE_ASSERT(this!=0);
  SgInitializedName* node=SgNodeHelper::getInitializedNameOfVariableDeclaration(declnode);
  ROSE_ASSERT(_variableIdMapping);
  VariableId varId=_variableIdMapping->variableId(node);
  IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(&element);
  ROSE_ASSERT(ips);
  ips->addVariable(varId);
  SgExpression* initExp=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(declnode);
  if(initExp) {
    NumberIntervalLattice res=_cppExprEvaluator->evaluate(initExp,ips);
    ROSE_ASSERT(!res.isBot());
    ips->setVariable(varId,res);
  }
}


/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::IntervalTransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments,Lattice& element) {
  // uses and defs in argument-expressions
  int paramNr=0;
  IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(&element);
  for(SgExpressionPtrList::iterator i=arguments.begin();i!=arguments.end();++i) {
    // TODO: add one variable $paramX for each parameter to the state and bind it to the value of the argument
    VariableId paramId=getParameterVariableId(paramNr);
    ips->addVariable(paramId);
    ips->setVariable(paramId,evalExpression(lab,*i,element));
  }
}
/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::IntervalTransferFunctions::transferFunctionCallReturn(Label lab, SgVarRefExp* lhsVar, SgFunctionCallExp* callExp, Lattice& element) {
  IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(&element);
  //NumberIntervalLattice number;
  //number=evalExpression(lab,callExp,element);

  //TODO: also need to handle compound assignment op
  if(isSgCompoundAssignOp(callExp->get_parent())) {
    cerr<<"Error: transferFunctionCallReturn: compound assignment of function call results not supported yet."<<endl;
    exit(1);
  }
  VariableId resVarId=getResultVariableId();
  if(lhsVar) {
    cout<<"DEBUG: updated var=f(...)."<<endl;
    VariableId varId=_variableIdMapping->variableId(lhsVar);  
    ips->setVariable(varId,ips->getVariable(resVarId));
  }
  ips->removeVariable(resVarId);
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::IntervalTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element) {
  // generate Intervals for each parameter variable
  int paramNr=0;
  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
      i!=formalParameters.end();
      ++i) {
    SgInitializedName* formalParameterName=*i;
    VariableId formalParameterVarId=_variableIdMapping->variableId(formalParameterName);
    IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(&element);
    // get value of actual parameter
    VariableId paramId=getParameterVariableId(paramNr);
    // remove parameter variable
    ips->addVariable(formalParameterVarId);
    ips->setVariable(formalParameterVarId,ips->getVariable(paramId));
    ips->removeVariable(paramId);
    paramNr++;
  }
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
void SPRAY::IntervalTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element) {
  // remove all declared variables at function exit (including function parameter variables)
  for(VariableIdSet::iterator i=localVariablesInFunction.begin();i!=localVariablesInFunction.end();++i) {
    VariableId varId=*i;
    IntervalPropertyState* ips=dynamic_cast<IntervalPropertyState*>(&element);
    ips->removeVariable(varId);
  }
}

SPRAY::CppExprEvaluator* SPRAY::IntervalTransferFunctions::getCppExprEvaluator() {
  return _cppExprEvaluator;
}

void SPRAY::IntervalTransferFunctions::setCppExprEvaluator(SPRAY::CppExprEvaluator* expEval) {
  _cppExprEvaluator=expEval;
}
