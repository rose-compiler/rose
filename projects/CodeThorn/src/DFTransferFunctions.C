#ifndef DFTRANSFERFUNCTIONS_CPP
#define DFTRANSFERFUNCTIONS_CPP

#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
using namespace CodeThorn;

#include "DFTransferFunctions.hpp"

template<typename LatticeType>
DFTransferFunctions<LatticeType>::DFTransferFunctions() {
}

template<typename LatticeType>
LatticeType DFTransferFunctions<LatticeType>::transfer(Label lab, LatticeType element) {
  if(element.isBot())
    element.setEmptySet();
  SgNode* node=_labeler->getNode(lab);
  //cout<<"Analyzing:"<<node->class_name()<<endl;
  
  if(_labeler->isFunctionCallLabel(lab)) {
    if(SgFunctionCallExp* funCall=isSgFunctionCallExp(getLabeler()->getNode(lab))) {
      SgExpressionPtrList& arguments=SgNodeHelper::getFunctionCallActualParameterList(funCall);
      transferFunctionCall(lab, funCall, arguments, element);
      return element;
    }
  }
  if(_labeler->isFunctionCallReturnLabel(lab)) {
    if(SgFunctionCallExp* funCall=isSgFunctionCallExp(getLabeler()->getNode(lab))) {
      transferFunctionCallReturn(lab, funCall, element);
      return element;
    }
  }
  if(_labeler->isFunctionEntryLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(lab))) {
      // 1) obtain formal parameters
      assert(funDef);
      SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      transferFunctionEntry(lab, funDef, formalParameters, element);
      return element;
    } else {
      ROSE_ASSERT(0);
    }
  }
  
  if(_labeler->isFunctionExitLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(_labeler->getNode(lab))) {
      // 1) determine all local variables (including formal parameters) of function
      // 2) delete all local variables from state
      // 2a) remove variable from state
      
      // ad 1)
      set<SgVariableDeclaration*> varDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
      // ad 2)
      VariableIdMapping::VariableIdSet localVars=_variableIdMapping->determineVariableIdsOfVariableDeclarations(varDecls);
      SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      VariableIdMapping::VariableIdSet formalParams=_variableIdMapping->determineVariableIdsOfSgInitializedNames(formalParamInitNames);
      VariableIdMapping::VariableIdSet vars=localVars+formalParams;
      transferFunctionExit(lab,funDef,vars,element); // TEST ONLY
      return element;
    } else {
      ROSE_ASSERT(0);
    }
  }
  
  if(isSgExprStatement(node))
    node=SgNodeHelper::getExprStmtChild(node);

  if(SgExpression* expr=isSgExpression(node)) {
    transferExpression(lab,expr,element);
  }
  if(SgVariableDeclaration* vardecl=isSgVariableDeclaration(node)) {
    transferDeclaration(lab,vardecl,element);
  }
  return element;
}

template<typename LatticeType>
void DFTransferFunctions<LatticeType>::transferExpression(Label lab, SgExpression* node, LatticeType& element) {
  // identity function
}
  
template<typename LatticeType>
void DFTransferFunctions<LatticeType>::transferDeclaration(Label label, SgVariableDeclaration* decl, LatticeType& element) {
  // identity function
}

template<typename LatticeType>
void DFTransferFunctions<LatticeType>::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, LatticeType& element) {
  // identity function
}

template<typename LatticeType>
void DFTransferFunctions<LatticeType>::transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, LatticeType& element) {
  // identity function
}

template<typename LatticeType>
void DFTransferFunctions<LatticeType>::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, LatticeType& element) {
  // identity function
}

template<typename LatticeType>
void DFTransferFunctions<LatticeType>::transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, LatticeType& element) {
  // identity function
}

#endif
