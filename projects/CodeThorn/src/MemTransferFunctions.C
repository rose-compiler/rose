#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
#include "MemTransferFunctions.h"
#include "AnalysisAbstractionLayer.h"

using namespace CodeThorn;

MemTransferFunctions::MemTransferFunctions() {
}

void MemTransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& element0) {
  // throws bad_cast exception when downcasting to the wrong type
  MemPropertyState& element=dynamic_cast<MemPropertyState&>(element0);
  cout<<"DEBUG: transferExpression: "<<node->unparseToString()<<endl;
}

/*!
  * \author Markus Schordan
  * \date 2013.
 */
void MemTransferFunctions::transferDeclaration(Label lab, SgVariableDeclaration* declNode, Lattice& element0) {
  MemPropertyState& element=dynamic_cast<MemPropertyState&>(element0);
  cout<<"DEBUG: transferDeclaration: "<<declNode->unparseToString()<<endl;
}

/*!
  * \author Markus Schordan
  * \date 2013.
 */
void MemTransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments,Lattice& element0) {
  MemPropertyState& element=dynamic_cast<MemPropertyState&>(element0);
  cout<<"DEBUG: transferFunctionCall: "<<callExp->unparseToString()<<endl;

  // uses and defs in argument-expressions
  int paramNr=0;
  for(SgExpressionPtrList::iterator i=arguments.begin();i!=arguments.end();++i) {
    VariableId paramId=getParameterVariableId(paramNr);
    transferExpression(lab,*i,element);
    paramNr++;
  }
}

void MemTransferFunctions::transferFunctionCallReturn(Label lab, VariableId lhsVarId, SgFunctionCallExp* callExp, Lattice& element0) {
  MemPropertyState& element=dynamic_cast<MemPropertyState&>(element0);
  cout<<"DEBUG: transferFunctionCallReturn: "<<callExp->unparseToString()<<endl;
  if(lhsVarId.isValid()) {
  } else {
    // void function call, nothing to do.
  }
}

/*!
  * \author Markus Schordan
  * \date 2013, 2015.
 */
void MemTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element0) {
  MemPropertyState& element=dynamic_cast<MemPropertyState&>(element0);

  // generate Mems for each parameter variable
  int paramNr=0;
  for(SgInitializedNamePtrList::iterator i=formalParameters.begin();
      i!=formalParameters.end();
      ++i) {

    // generate formal parameter
    SgInitializedName* formalParameterName=*i;
    assert(formalParameterName);
    VariableId formalParameterVarId=getVariableIdMapping()->variableId(formalParameterName);
    paramNr++;
  }
}

/*!
  * \author Markus Schordan
  * \date 2013.
 */
void MemTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* callExp, VariableIdSet& localVariablesInFunction, Lattice& element0) {
  MemPropertyState& element=dynamic_cast<MemPropertyState&>(element0);

  // remove all declared variable at function exit (including function parameter variables)
  for(VariableIdSet::iterator i=localVariablesInFunction.begin();i!=localVariablesInFunction.end();++i) {
    VariableId varId=*i;
  }
}

void MemTransferFunctions::transferReturnStmtExpr(Label lab, SgExpression* node, Lattice& element0) {
  MemPropertyState& element=dynamic_cast<MemPropertyState&>(element0);
  transferExpression(lab,node,element);
  VariableId resVarId=getResultVariableId();
}

void MemTransferFunctions::initializeExtremalValue(Lattice& element) {
  MemPropertyState* rdElement=dynamic_cast<MemPropertyState*>(&element);
  //rdElement->setEmptySet();
  cout<<"INFO: initialized extremal value."<<endl;
  rdElement->setBot(false);
}
