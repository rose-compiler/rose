#ifndef PATRANSFERFUNCTIONS_CPP
#define PATRANSFERFUNCTIONS_CPP

#include "sage3basic.h"

using namespace std;

#include "CollectionOperators.h"
#include "DFTransferFunctions.h"

using namespace SPRAY;

DFTransferFunctions::DFTransferFunctions():_labeler(0),_variableIdMapping(0){}

void DFTransferFunctions::transfer(Label lab, Lattice& element) {
  ROSE_ASSERT(_labeler);
  SgNode* node=_labeler->getNode(lab);
  //cout<<"Analyzing:"<<node->class_name()<<endl;
  //cout<<"DEBUG: transfer: @"<<lab<<": "<<node->class_name()<<":"<<node->unparseToString()<<endl;
  if(_labeler->isFunctionCallLabel(lab)) {
    if(isSgExprStatement(node)) {
      node=SgNodeHelper::getExprStmtChild(node);
    }
    if(SgFunctionCallExp* funCall=isSgFunctionCallExp(node)) {
      SgExpressionPtrList& arguments=SgNodeHelper::getFunctionCallActualParameterList(funCall);
      transferFunctionCall(lab, funCall, arguments, element);
      return;
    }
  }
  if(_labeler->isFunctionCallReturnLabel(lab)) {
    if(SgFunctionCallExp* funCall=isSgFunctionCallExp(getLabeler()->getNode(lab))) {
      SgVarRefExp* lhsVar=0;
      if(SgAssignOp* assignOp=isSgAssignOp(funCall->get_parent())) {
        if(SgVarRefExp* lhs=isSgVarRefExp(SgNodeHelper::getLhs(assignOp))) {
          lhsVar=lhs;
        }
      }
      transferFunctionCallReturn(lab, lhsVar, funCall, element);
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
  
  if(isSgExprStatement(node))
    node=SgNodeHelper::getExprStmtChild(node);
  
  // desugar SgExprStatement
  if(isSgReturnStmt(node)) {
    node=SgNodeHelper::getFirstChild(node);
  }
  if(SgCaseOptionStmt* caseStmt=isSgCaseOptionStmt(node)) {
    SgStatement* blockStmt=isSgBasicBlock(caseStmt->get_parent());
    ROSE_ASSERT(blockStmt);
    SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
    ROSE_ASSERT(switchStmt);
    SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
    ROSE_ASSERT(condStmt);
    transferSwitchCase(lab,condStmt, caseStmt,element);
    return;
  }
  if(SgExpression* expr=isSgExpression(node)) {
    transferExpression(lab,expr,element);
    return;
  }
  if(SgVariableDeclaration* vardecl=isSgVariableDeclaration(node)) {
    transferDeclaration(lab,vardecl,element);
    return;
  }
  return;
}


void DFTransferFunctions::transferExpression(Label lab, SgExpression* node, Lattice& element) {
  cout<<"DEFAULT: transferExp."<<endl;
  // default identity function
}
  

void DFTransferFunctions::transferDeclaration(Label label, SgVariableDeclaration* decl, Lattice& element) {
  // default identity function
}

void DFTransferFunctions::transferSwitchCase(Label lab,SgStatement* condStmt, SgCaseOptionStmt* caseStmt,Lattice& pstate) {
  // default identity function
}

void DFTransferFunctions::transferFunctionCall(Label lab, SgFunctionCallExp* callExp, SgExpressionPtrList& arguments, Lattice& element) {
  // default identity function
}


void DFTransferFunctions::transferFunctionCallReturn(Label lab, SgVarRefExp*, SgFunctionCallExp* callExp, Lattice& element) {
  // default identity function
}


void DFTransferFunctions::transferFunctionEntry(Label lab, SgFunctionDefinition* funDef,SgInitializedNamePtrList& formalParameters, Lattice& element) {
  // default identity function
}


void DFTransferFunctions::transferFunctionExit(Label lab, SgFunctionDefinition* funDef, VariableIdSet& localVariablesInFunction, Lattice& element) {
  // default identity function
}

#endif
