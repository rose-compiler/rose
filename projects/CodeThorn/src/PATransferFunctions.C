#ifndef PATRANSFERFUNCTIONS_CPP
#define PATRANSFERFUNCTIONS_CPP

#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
using namespace CodeThorn;

#include "PATransferFunctions.h"


PATransferFunctions::PATransferFunctions():_labeler(0),_variableIdMapping(0),_domain(0){}



void PATransferFunctions::transfer(Label lab, Lattice& element) {
  //  if(element.isBot())
  //  element.setEmptySet();
  cout<<"DEBUG: transfer: started @"<<lab.getId()<<endl;
  ROSE_ASSERT(_labeler);
  SgNode* node=_labeler->getNode(lab);
  //cout<<"Analyzing:"<<node->class_name()<<endl;
  cout<<"DEBUG: transfer: node: "<<node<<endl;
  
  if(_labeler->isFunctionCallLabel(lab)) {
    if(SgFunctionCallExp* funCall=isSgFunctionCallExp(getLabeler()->getNode(lab))) {
      SgExpressionPtrList& arguments=SgNodeHelper::getFunctionCallActualParameterList(funCall);
      transferFunctionCall(lab, funCall, arguments, element);
      return;
    }
  }
  if(_labeler->isFunctionCallReturnLabel(lab)) {
    if(SgFunctionCallExp* funCall=isSgFunctionCallExp(getLabeler()->getNode(lab))) {
      transferFunctionCallReturn(lab, funCall, element);
      return;
    }
  }
  if(_labeler->isFunctionEntryLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(lab))) {
      // 1) obtain formal parameters
      assert(funDef);
      SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      transferFunctionEntry(lab, funDef, formalParameters, element);
      return;
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
      return;
    } else {
      ROSE_ASSERT(0);
    }
  }
  
  // desugar SgExprStatement
  if(isSgExprStatement(node))
    node=SgNodeHelper::getExprStmtChild(node);

  if(SgExpression* expr=isSgExpression(node)) {
    transferExpression(lab,expr,element);
  }
  cout<<"DEBUG: transfer: checking for SgVariableDeclaration."<<endl;
  if(SgVariableDeclaration* vardecl=isSgVariableDeclaration(node)) {
    transferDeclaration(lab,vardecl,element);
  }
  return;
}


void PATransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& element) {
  // default identity function
}
  

void PATransferFunctions::transferDeclaration(Label label, SgVariableDeclaration* decl, Lattice& element) {
  // default identity function
}


void PATransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element) {
  // default identity function
}


void PATransferFunctions::transferFunctionCallReturn(Label lab, SgFunctionCallExp* callExp, Lattice& element) {
  // default identity function
}


void PATransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element) {
  // default identity function
}


void PATransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element) {
  // default identity function
}

#endif
