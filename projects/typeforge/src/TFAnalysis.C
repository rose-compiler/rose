#include "sage3basic.h"
#include "TFAnalysis.h"
#include <iostream>
#include <vector>
#include "SgNodeHelper.h"
#include "abstract_handle.h"

using namespace std;
using namespace AbstractHandle;

bool isArrayPointerType(SgType* type){
  if(type == nullptr) return false;
  if(isSgArrayType(type)) return true;
  if(isSgPointerType(type)) return true;
  SgType* stripped = type->stripTypedefsAndModifiers();
  if(type == stripped) return false;
  return isArrayPointerType(stripped);
}

TFAnalysis::TFAnalysis(){}

int TFAnalysis::variableSetAnalysis(SgProject* project){
  RoseAst ast(project);
  for(RoseAst::iterator i = ast.begin(); i!=ast.end(); i++){
    SgInitializedName* lhsInitName = nullptr;
    SgType* lhsBaseType = nullptr;
    SgExpression* rhsExp = nullptr;
    if(SgAssignOp* assignOp = isSgAssignOp(*i)){
      SgExpression* lhs = assignOp->get_lhs_operand();
      if(SgVarRefExp* varRef = isSgVarRefExp(lhs)){
        lhsBaseType = varRef->get_type();
        if(!isArrayPointerType(lhsBaseType)) continue;
        lhsBaseType = lhsBaseType->findBaseType();
        SgVariableSymbol* varSym = varRef->get_symbol();
        lhsInitName = varSym->get_declaration();
      }
      rhsExp = assignOp->get_rhs_operand();
    }
    else if(SgAssignInitializer* assignInit = isSgAssignInitializer(*i)){
      lhsInitName = isSgInitializedName(assignInit->get_parent());
      if(!lhsInitName) continue;
      rhsExp      = assignInit->get_operand();
      lhsBaseType = lhsInitName->get_type();
      if(!isArrayPointerType(lhsBaseType)) continue;
      lhsBaseType = lhsBaseType->findBaseType(); 
    }
    else if(SgFunctionCallExp* callExp = isSgFunctionCallExp(*i)){
      SgFunctionDeclaration* funDec = callExp->getAssociatedFunctionDeclaration();
      SgInitializedNamePtrList& initNameList = funDec->get_args();
      SgExpressionPtrList& expList = callExp->get_args()->get_expressions();
      auto initIter = initNameList.begin();
      auto expIter  = expList.begin(); 
      while(initIter != initNameList.end()){
        if(isArrayPointerType((*initIter)->get_type())) linkVariables((*initIter), (*initIter)->get_type()->findBaseType(), (*expIter));
        ++initIter;
        ++expIter;
      }
    }
    if(lhsInitName && lhsBaseType && rhsExp) linkVariables(lhsInitName, lhsBaseType, rhsExp);
  }
  return 0;
}

void TFAnalysis::writeAnalysis(string fileName){

}

void TFAnalysis::linkVariables(SgInitializedName* initName, SgType* type, SgExpression* exp){
  RoseAst ast(exp);
  for(RoseAst::iterator i = ast.begin(); i!=ast.end(); i++){
    if(SgFunctionCallExp* funCall = isSgFunctionCallExp(*i)){
      if(funCall->get_type()->findBaseType() == type){
        SgFunctionDeclaration* funDec = funCall->getAssociatedFunctionDeclaration();
        addToMap(initName, funDec);
      }
      i.skipChildrenOnForward();
    }
    else if(SgVarRefExp* varRef = isSgVarRefExp(*i)){
      if(varRef->get_type()->findBaseType() == type){
        SgVariableSymbol* varSym = varRef->get_symbol();
        SgInitializedName* refInitName = varSym->get_declaration();
        addToMap(initName, refInitName);
      }
    }
  }
}

void TFAnalysis::addToMap(SgNode* originNode, SgNode* targetNode){
  if(!setMap.count(originNode)){
    setMap[originNode] = new set<SgNode*>;
  }
  setMap[originNode]->insert(targetNode);
}
