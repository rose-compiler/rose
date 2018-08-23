#include "sage3basic.h"
#include "TFAnalysis.h"
#include <iostream>
#include <vector>
#include "SgNodeHelper.h"
#include "abstract_handle.h"

using namespace std;
using namespace AbstractHandle;

SgType* stripType(SgType* type){
  if(SgArrayType* arrayType = isSgArrayType(type)) return arrayType->get_base_type();
  if(SgPointerType* ptrType = isSgPointerType(type)) return ptrType->get_base_type();
  if(SgTypedefType* typeDef = isSgTypedefType(type)) return stripType(typeDef->get_base_type());
  if(SgReferenceType* refType = isSgReferenceType(type)) return refType->get_base_type();
  if(SgModifierType* modType = isSgModifierType(type)) return stripType(modType->get_base_type());
  return nullptr;
}

bool isArrayPointerType(SgType* type){
  if(type == nullptr) return false;
  if(isSgArrayType(type)) return true;
  if(isSgPointerType(type)) return true;
  return isArrayPointerType(stripType(type));
}

bool sameType(SgType* typeOne, SgType* typeTwo){
  return typeOne->findBaseType() == typeTwo->findBaseType();
  if(typeOne == typeTwo) return true;
  if(SgArrayType* array1 = isSgArrayType(typeOne)){
    if(SgArrayType* array2 = isSgArrayType(typeTwo)) return sameType(array1->get_base_type(), array2->get_base_type());
  }
  if(SgPointerType* pointer1 = isSgPointerType(typeOne)){
    if(SgPointerType* pointer2 = isSgPointerType(typeTwo)) return sameType(pointer1->get_base_type(), pointer2->get_base_type());
  }
  if(SgReferenceType* ref1 = isSgReferenceType(typeOne)){
    if(SgReferenceType* ref2 = isSgReferenceType(typeTwo)) return sameType(ref1->get_base_type(), ref2->get_base_type());
  }
  if(SgTypedefType* typeDef = isSgTypedefType(typeOne)) return sameType(typeDef->get_base_type(), typeTwo);
  if(SgTypedefType* typeDef = isSgTypedefType(typeTwo)) return sameType(typeOne, typeDef->get_base_type());
  if(SgModifierType* modType = isSgModifierType(typeOne)) return sameType(modType->get_base_type(), typeTwo);
  if(SgModifierType* modType = isSgModifierType(typeTwo)) return sameType(typeOne, modType->get_base_type());
  return false;  
}

TFAnalysis::TFAnalysis(){}

int TFAnalysis::variableSetAnalysis(SgProject* project){
  list<SgVariableDeclaration*> listOfGlobalVars = SgNodeHelper::listOfGlobalVars(project);
  if(listOfGlobalVars.size() > 0){
    for(auto varDec : listOfGlobalVars){
      SgInitializedName* lhsInitName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDec);
      if(!lhsInitName) continue;
      SgInitializer* init = lhsInitName->get_initializer();
      if(!init) continue;
      SgType* lhsType = lhsInitName->get_type();
      if(!isArrayPointerType(lhsType)) continue;
      SgExpression* rhsExp = init;
      linkVariables(lhsInitName, lhsType, rhsExp);
    }
  }
  list<SgFunctionDefinition*> listOfFunctionDefinitions = SgNodeHelper::listOfFunctionDefinitions(project);
  for(auto funDef : listOfFunctionDefinitions){  
    RoseAst ast(funDef);
    for(RoseAst::iterator i = ast.begin(); i!=ast.end(); i++){
      SgInitializedName* lhsInitName = nullptr;
      SgType* lhsType = nullptr;
      SgExpression* rhsExp = nullptr;
      if(SgAssignOp* assignOp = isSgAssignOp(*i)){
        SgExpression* lhs = assignOp->get_lhs_operand();
        if(SgVarRefExp* varRef = isSgVarRefExp(lhs)){
          lhsType = varRef->get_type();
          if(!isArrayPointerType(lhsType)) continue;
          SgVariableSymbol* varSym = varRef->get_symbol();
          lhsInitName = varSym->get_declaration();
        }
        rhsExp = assignOp->get_rhs_operand();
      }
      else if(SgVariableDeclaration* varDec = isSgVariableDeclaration(*i)){
        lhsInitName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDec);
        if(!lhsInitName) continue;
        SgInitializer* init = lhsInitName->get_initializer();
        if(!init) continue;
        lhsType = lhsInitName->get_type();
        if(!isArrayPointerType(lhsType)) continue;
        rhsExp = init;
      }
      else if(SgFunctionCallExp* callExp = isSgFunctionCallExp(*i)){
        SgFunctionDefinition* funDef = SgNodeHelper::determineFunctionDefinition(callExp);
        if(!funDef) continue;
        SgInitializedNamePtrList& initNameList = SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
        SgExpressionPtrList& expList = callExp->get_args()->get_expressions();
        auto initIter = initNameList.begin();
        auto expIter  = expList.begin(); 
        while(initIter != initNameList.end()){
          if(isArrayPointerType((*initIter)->get_type())) linkVariables((*initIter), (*initIter)->get_type(), (*expIter));
          ++initIter;
          ++expIter;
        }
      }
      if(lhsInitName && lhsType && rhsExp) linkVariables(lhsInitName, lhsType, rhsExp);
    }
  }
  return 0;
}

string getFunctionNameOfNode(SgNode* node){
  SgFunctionDefinition* funDef = SgNodeHelper::getClosestParentFunctionDefinitionOfLocatedNode(isSgLocatedNode(node));
  if(!funDef){
    SgNode* parent = node;
    while(parent != nullptr){
      parent = parent->get_parent();
      if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(parent)){
        return SgNodeHelper::getFunctionName(funDec);
      }
    }
    return "$global";
  }
  else return SgNodeHelper::getFunctionName(funDef);
}

bool setIntersect(set<SgNode*>* set1, set<SgNode*>* set2){
  for(auto i = set2->begin(); i != set2->end(); ++i){
    if(set1->count(*i)) return true;
  }
  return false;
}

void inPlaceUnion(set<SgNode*>* set1, set<SgNode*>* set2){
  for(auto i = set2->begin(); i != set2->end(); ++i){
    set1->insert(*i);
  }
}

set<SgNode*>* copySet(set<SgNode*>* oldSet){
  set<SgNode*>* newSet = new set<SgNode*>;
  for(auto i = oldSet->begin(); i != oldSet->end(); ++i){
    newSet->insert(*i);
  }
  return newSet;
}

string makeSetString(set<SgNode*>* variableSet){
  string setString = "";
  for(auto j = variableSet->begin(); j != variableSet->end(); ++j){
    string name = "";
    string funName = getFunctionNameOfNode(*j) + ":"; 
    if(SgInitializedName* rightInit = isSgInitializedName(*j)){
      SgSymbol* varSym = SgNodeHelper::getSymbolOfInitializedName(rightInit);
      if(varSym){
        name = SgNodeHelper::symbolToString(varSym);
      }
    }
    else if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(*j)){
      funName = "";
      SgSymbol* funSym = funDec->get_symbol_from_symbol_table();
      if(funSym){
        name = SgNodeHelper::symbolToString(funSym) + " Return";
      }
    }
    if(setString != "") setString = setString + " & ";
    setString = setString + funName + name; 
  }
  return setString;
}

void TFAnalysis::writeAnalysis(string fileName){
  for(auto i = setMap.begin(); i != setMap.end(); ++i){
    string leftName = "";
    string rightFunName = getFunctionNameOfNode(i->first);
    if(SgInitializedName* leftInit = isSgInitializedName(i->first)){
      SgSymbol* varSym = SgNodeHelper::getSymbolOfInitializedName(leftInit);
      if(varSym){
        leftName = SgNodeHelper::symbolToString(varSym);
      }
    }
    cout<<rightFunName<<":"<<leftName<<" => "<<makeSetString(i->second)<<endl;
  }
  cout<<"\n\n";
  for(auto i = setMap.begin(); i != setMap.end(); ++i){
    bool intersect = false;
    set<SgNode*>* found = nullptr;
    for(auto j = listSets.begin(); j != listSets.end(); ++j){
      intersect = setIntersect(*j, i->second);
      if((*j)->count(i->first)) intersect = true;
      if(found != nullptr && intersect){ 
        inPlaceUnion(found, i->second);
        inPlaceUnion(found, *j);
        (found)->insert(i->first);
        j = listSets.erase(j);
        ++j;
      }
      else if(intersect){
        inPlaceUnion(*j, i->second);
        (*j)->insert(i->first);
        found = *j;
      }
    }
    if(!intersect){
      set<SgNode*>* copy = copySet(i->second);
      copy->insert(i->first);
      listSets.push_back(copy);
    }
  }
  for(auto i = listSets.begin(); i != listSets.end(); ++i){
    cout<<makeSetString(*i)<<"\n";
  }
}

void TFAnalysis::linkVariables(SgInitializedName* initName, SgType* type, SgExpression* expression){
  RoseAst ast(expression);
  for(RoseAst::iterator i = ast.begin(); i!=ast.end(); i++){
    if(SgExpression* exp = isSgExpression(*i)){
      if(sameType(exp->get_type(), type)){
        if(SgFunctionCallExp* funCall = isSgFunctionCallExp(exp)){
          SgFunctionDeclaration* funDec = funCall->getAssociatedFunctionDeclaration();
          addToMap(initName, funDec);
          i.skipChildrenOnForward();
        }
        else if(SgPntrArrRefExp* refExp = isSgPntrArrRefExp(exp)){
          linkVariables(initName, refExp->get_lhs_operand()->get_type(), refExp->get_lhs_operand());
          i.skipChildrenOnForward();
        }
        else if(SgPointerDerefExp* refExp = isSgPointerDerefExp(exp)){
          linkVariables(initName, refExp->get_operand()->get_type(), refExp->get_operand());
          i.skipChildrenOnForward();
        }
        else if(SgVarRefExp* varRef = isSgVarRefExp(exp)){
          SgVariableSymbol* varSym = varRef->get_symbol();
          SgInitializedName* refInitName = varSym->get_declaration();
          addToMap(initName, refInitName);
        }
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
