#include "sage3basic.h"
#include "TFAnalysis.h"
#include "TFHandles.h"
#include <iostream>
#include <vector>
#include "SgNodeHelper.h"
#include <boost/graph/graphviz.hpp>
#include "TFToolConfig.h"

using namespace std;

SgType* getBaseType(SgType* type){
  if(SgArrayType* arrayType = isSgArrayType(type)) return arrayType->get_base_type();
  if(SgPointerType* ptrType = isSgPointerType(type)) return ptrType->get_base_type();
  if(SgTypedefType* typeDef = isSgTypedefType(type)) return typeDef->get_base_type();
  if(SgReferenceType* refType = isSgReferenceType(type)) return refType->get_base_type();
  if(SgModifierType* modType = isSgModifierType(type)) return modType->get_base_type();
  return nullptr;
}

bool isArrayPointerType(SgType* type){
  if(type == nullptr) return false;
  if(isSgArrayType(type)) return true;
  if(isSgPointerType(type)) return true;
  return isArrayPointerType(getBaseType(type));
}

bool checkMatch(bool base, SgType* typeOne, SgType* typeTwo){
  return typeOne == nullptr || typeTwo == nullptr || typeOne == typeTwo || (base && typeOne->findBaseType() == typeTwo->findBaseType());
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

TFAnalysis::TFAnalysis(){}

int TFAnalysis::variableSetAnalysis(SgProject* project, SgType* matchType, bool base){
  RoseAst wholeAST(project);
  list<SgVariableDeclaration*> listOfGlobalVars = SgNodeHelper::listOfGlobalVars(project);
  if(listOfGlobalVars.size() > 0){
    for(auto varDec : listOfGlobalVars){
      SgInitializedName* initName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDec);
      if(!initName) continue;
      SgInitializer* init = initName->get_initializer();
      if(!init) continue;
      SgType* keyType = initName->get_type();
      if(!checkMatch(base, keyType, matchType)) continue;
      addToMap(varDec, varDec);  
      if(!isArrayPointerType(keyType)) continue;
      SgExpression* exp = init;
      linkVariables(varDec, keyType, exp);
    }
  }
  list<SgFunctionDefinition*> listOfFunctionDefinitions = SgNodeHelper::listOfFunctionDefinitions(project);
  for(auto funDef : listOfFunctionDefinitions){  
    SgInitializedNamePtrList& initNameList = SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
    SgFunctionDeclaration* funDec = funDef->get_declaration();
    if(checkMatch(base, funDec->get_type()->get_return_type(), matchType)) addToMap(funDec, funDec);
    for(auto init : initNameList) if(checkMatch(base, init->get_type(), matchType)) addToMap(init, init);
    RoseAst ast(funDef);
    for(RoseAst::iterator i = ast.begin(); i!=ast.end(); i++){
      SgNode* key = nullptr;
      SgType* keyType = nullptr;
      SgExpression* exp = nullptr;
      if(SgAssignOp* assignOp = isSgAssignOp(*i)){
        SgExpression* lhs = assignOp->get_lhs_operand();
        if(SgVarRefExp* varRef = isSgVarRefExp(lhs)){
          keyType = varRef->get_type();
          if(!isArrayPointerType(keyType)) continue;    
          SgVariableSymbol* varSym = varRef->get_symbol();
	  key = varSym->get_declaration()->get_declaration();
        }
        exp = assignOp->get_rhs_operand();
      }
      else if(SgVariableDeclaration* varDec = isSgVariableDeclaration(*i)){
        SgInitializedName* initName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDec);
        if(!initName) continue;
        if(checkMatch(base, matchType, initName->get_type())) addToMap(varDec, varDec);
        SgInitializer* init = initName->get_initializer();
        if(!init) continue;
        keyType = initName->get_type();
        if(!isArrayPointerType(keyType)) continue;
        key = initName->get_declaration();
        exp = init;
      }
      else if(SgFunctionCallExp* callExp = isSgFunctionCallExp(*i)){
        SgFunctionDefinition* funDef = SgNodeHelper::determineFunctionDefinition(callExp);
        if(!funDef) continue;
        SgInitializedNamePtrList& initNameList = SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
        SgExpressionPtrList& expList = callExp->get_args()->get_expressions();
        auto initIter = initNameList.begin();
        auto expIter  = expList.begin(); 
        while(initIter != initNameList.end()){
          if(isArrayPointerType((*initIter)->get_type())){
            if(checkMatch(base, matchType, (*initIter)->get_type())) linkVariables((*initIter), (*initIter)->get_type(), (*expIter));
          }
          ++initIter;
          ++expIter;
        }
      }
      else if(SgReturnStmt* ret = isSgReturnStmt(*i)){
        exp = ret->get_expression();
        keyType = exp->get_type();
        if(!isArrayPointerType(keyType)) continue;
        key = funDec;
         
      }
      if(!checkMatch(base, keyType, matchType)) continue;
      if(key && keyType && exp) linkVariables(key, keyType, exp);
    }
  }
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
  return 0;
}

set<SgNode*>* TFAnalysis::getSet(SgNode* node){
  for(auto i = listSets.begin(); i != listSets.end(); ++i){
    if((*i)->count(node)) return *i;
  }
  return nullptr;
}

string makeSetString(set<SgNode*>* variableSet){
  string setString = "";
  for(auto j = variableSet->begin(); j != variableSet->end(); ++j){
    string name = "";
    string funName = getFunctionNameOfNode(*j) + ":"; 
    SgSymbol* varSym = nullptr;
    if(SgInitializedName* leftInit = isSgInitializedName(*j)) varSym = SgNodeHelper::getSymbolOfInitializedName(leftInit);
    else if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(*j)) varSym = SgNodeHelper::getSymbolOfFunctionDeclaration(funDec);
    else if(SgVariableDeclaration* varDec = isSgVariableDeclaration(*j)) varSym = SgNodeHelper::getSymbolOfVariableDeclaration(varDec);
    if(varSym) name = SgNodeHelper::symbolToString(varSym);
    
    if(setString != "") setString = setString + "==";
    setString = setString + funName + name; 
  }
  return setString;
}

void TFAnalysis::writeAnalysis(SgType* type, string toTypeString){
  for(auto i = setMap.begin(); i != setMap.end(); ++i){
    string leftName = "";
    string rightFunName = getFunctionNameOfNode(i->first);
    SgSymbol* varSym = nullptr;
    if(SgInitializedName* leftInit = isSgInitializedName(i->first)) varSym = SgNodeHelper::getSymbolOfInitializedName(leftInit);
    else if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(i->first)) varSym = SgNodeHelper::getSymbolOfFunctionDeclaration(funDec);
    else if(SgVariableDeclaration* varDec = isSgVariableDeclaration(i->first)) varSym = SgNodeHelper::getSymbolOfVariableDeclaration(varDec);
    if(varSym){
      leftName = SgNodeHelper::symbolToString(varSym);
//      cout<<rightFunName<<":"<<leftName<<" => "<<makeSetString(i->second)<<endl;
    }
  }
  for(auto i = listSets.begin(); i != listSets.end(); ++i){
    string nameString = makeSetString(*i);
    string handle = TFHandles::getHandleVectorString(*(*i));
    TFToolConfig::addChangeVarBaseType(handle, nameString, "*", "", type->unparseToString(), toTypeString);
  }
}

void TFAnalysis::writeGraph(string fileName){
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> SetGraph;
  SetGraph graph(0);
  map<SgNode*, SetGraph::vertex_descriptor> desMap;
  vector<string> names;
  for(auto i = setMap.begin(); i != setMap.end(); ++i){
    string name = "";
    string funName = getFunctionNameOfNode(i->first);
    SgSymbol* varSym = nullptr;
    if(SgInitializedName* leftInit = isSgInitializedName(i->first)) varSym = SgNodeHelper::getSymbolOfInitializedName(leftInit);
    else if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(i->first)) varSym = SgNodeHelper::getSymbolOfFunctionDeclaration(funDec);
    else if(SgVariableDeclaration* varDec = isSgVariableDeclaration(i->first)) varSym = SgNodeHelper::getSymbolOfVariableDeclaration(varDec);
    if(varSym) name = SgNodeHelper::symbolToString(varSym);
    name = funName + "::" +name;
    SetGraph::vertex_descriptor vDes = boost::add_vertex(graph);
    desMap[i->first] = vDes;
    names.push_back(name);
  } 
  for(auto i = setMap.begin(); i != setMap.end(); ++i){
    for(auto j = i->second->begin(); j != i->second->end(); ++j){
      if(desMap[i->first] < desMap[*j]) boost::add_edge(desMap[i->first], desMap[*j], graph);
    }
  }
  fstream fileStream;
  fileStream.open(fileName, ios::out | ios::trunc);
  boost::write_graphviz(fileStream, graph, boost::make_label_writer((&names[0])));
  fileStream.close();
}

void TFAnalysis::linkVariables(SgNode* key, SgType* type, SgExpression* expression){
  RoseAst ast(expression);
  for(RoseAst::iterator i = ast.begin(); i!=ast.end(); i++){
    if(SgExpression* exp = isSgExpression(*i)){
      if(sameType(exp->get_type(), type)){
        if(SgFunctionCallExp* funCall = isSgFunctionCallExp(exp)){
          SgFunctionDeclaration* funDec = funCall->getAssociatedFunctionDeclaration();
          SgFunctionDefinition* funDef = SgNodeHelper::determineFunctionDefinition(funCall);
          if(!funDef) continue;
          funDec = funDef->get_declaration();
          addToMap(key, funDec);
          i.skipChildrenOnForward();
        }
        else if(SgPntrArrRefExp* refExp = isSgPntrArrRefExp(exp)){
          linkVariables(key, refExp->get_lhs_operand()->get_type(), refExp->get_lhs_operand());
          i.skipChildrenOnForward();
        }
        else if(SgPointerDerefExp* refExp = isSgPointerDerefExp(exp)){
          linkVariables(key, refExp->get_operand()->get_type(), refExp->get_operand());
          i.skipChildrenOnForward();
        }
        else if(SgVarRefExp* varRef = isSgVarRefExp(exp)){
          SgVariableSymbol* varSym = varRef->get_symbol();
          if(varSym){  
            SgInitializedName* refInitName = varSym->get_declaration();
            SgNode* target = refInitName;
            if(!SgNodeHelper::isFunctionParameterVariableSymbol(varSym)) target = refInitName->get_declaration();
            if(target){
              addToMap(key, target);
            }
          }
        }
      }
    }
  }
}

void TFAnalysis::addToMap(SgNode* originNode, SgNode* targetNode){
  if(!setMap.count(originNode)){
    setMap[originNode] = new set<SgNode*>;
    setMap[originNode]->insert(originNode);
  }
  if(!setMap.count(targetNode)){
    setMap[targetNode] = new set<SgNode*>;
    setMap[targetNode]->insert(targetNode);
  }
  setMap[originNode]->insert(targetNode);
  setMap[targetNode]->insert(originNode);
}
