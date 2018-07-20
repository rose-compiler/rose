#include "TFCommandList.h"
#include "sage3basic.h"
#include "TFSpecFrontEnd.h"
#include "TFTransformation.h"
#include "CppStdUtilities.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include "AstTerm.h"
#include "SgNodeHelper.h"
#include "AstProcessing.h"
#include "AstMatching.h"
#include "TFTypeTransformer.h"
#include "TFSpecFrontEnd.h"
#include "CastStats.h"
#include "CastTransformer.h"
#include "CastGraphVis.h"
#include "CppStdUtilities.h"
#include <utility>
#include <functional>
#include <regex>
#include <algorithm>
#include <list>
#include "TFTransformation.h"
#include <ToolConfig.hpp>
#include "abstract_handle.h"
#include "roseAdapter.h"

using namespace std;
using namespace AbstractHandle;

SgType* checkType(SgInitializedName* varInitName, string typeName) {
  SgType* varInitType=varInitName->get_type();
  SgType* baseType=varInitType->findBaseType();
  if(baseType) {
    if(SgNamedType* namedType=isSgNamedType(baseType)) {
      string nameTypeString=namedType->get_name();
      if(nameTypeString==typeName) {
        return baseType;
      }
    }
  }
  return nullptr;
}

SgType* findUserDefinedTypeByName(SgFunctionDefinition* funDef, string userDefinedTypeName) {
  SgInitializedNamePtrList& initNamePtrList=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
  for(auto varInitName : initNamePtrList) {
    if(SgType* type=checkType(varInitName,userDefinedTypeName))
      return type;
  }
  RoseAst ast(funDef);
  for (auto node : ast) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
      SgInitializedName* varInitName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
      if(SgType* type=checkType(varInitName,userDefinedTypeName))
        return type;
    }
  }
  return nullptr;
}

SgType* buildTypeFromStringSpec(string type, SgScopeStatement* providedScope) {
  SgType* newType=nullptr;
  std::regex e("[_A-Za-z]+|\\*|&|const");
  std::regex_token_iterator<std::string::iterator> rend;
  std::regex_token_iterator<std::string::iterator> a ( type.begin(), type.end(), e );
  bool buildConstType=false;
  bool isLongType=false;
  bool isShortType=false;
  while (a!=rend) {
    string typePart=*a++;
    if(typePart=="float") {
      if(isLongType||isShortType) {
        cerr<<"Error: wrong type: float cannot be short or long."<<endl;
        exit(1);
      }
      newType=SageBuilder::buildFloatType();
    } else if(typePart=="double") {
      if(!isLongType)
        newType=SageBuilder::buildDoubleType();
      else
        newType=SageBuilder::buildLongDoubleType();
    } else if(typePart=="short") {
      isShortType=true;
    } else if(typePart=="long") {
      isLongType=true;
    } else if(typePart==" ") {
      continue;
    } else if(typePart=="*") {
      if(newType==nullptr) goto parseerror;
      newType=SageBuilder::buildPointerType(newType);
    } else if(typePart=="&") {
      if(newType==nullptr) goto parseerror;
      newType=SageBuilder::buildReferenceType(newType);
    } else if(typePart=="const") {
      buildConstType=true;
    } else if(std::regex_match(typePart, std::regex("^[_A-Za-z]+$"))) {
      if(SgFunctionDefinition* funDef=isSgFunctionDefinition(providedScope)) {
        SgScopeStatement* funScope=funDef->get_scope();
        SgType* userDefinedType=findUserDefinedTypeByName(funDef,typePart);
        if(userDefinedType) {
          newType=userDefinedType;
        } else {
          newType=SageBuilder::buildOpaqueType(typePart, funScope);
        }
      } else {
        newType=SageBuilder::buildOpaqueType(typePart, providedScope);
      }
    } else {
    parseerror:
      cerr<<"Error: unsupported type: "<<type<<", unresolved:"<<typePart<<"."<<endl;
      exit(1);
    }
    if(buildConstType && newType) {
      newType=SageBuilder::buildConstType(newType);
      buildConstType=false;
    }
  }
  return newType;
}


Command::Command(bool changeBase, bool justList, int number){
  base = changeBase;
  listing = justList;
  commandNumber = number;
}

TypeCommand::TypeCommand(std::string loc, std::string fun, std::string toType, std::string fromType, bool base, bool listing, int number) : Command(base, listing, number){
    location = loc;
    funName  = fun;
    newType  = toType;
    oldType  = fromType;
  }
 
int TypeCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  if(funName == "$global") {
    SgGlobal* globalScope = root->get_globalScopeAcrossFiles();
    SgType* oldBuiltType=buildTypeFromStringSpec(oldType,globalScope);
    SgType* newBuiltType=buildTypeFromStringSpec(newType,globalScope);
    tt.addTypeTransformationToList(_list,newBuiltType,nullptr,"",base,oldBuiltType,listing);
    return false;
  } else {
    std::list<SgFunctionDefinition*> listOfFunctionDefinitions;
    if(funName=="*") {
      listOfFunctionDefinitions=SgNodeHelper::listOfFunctionDefinitions(root);
    } else {
      SgFunctionDefinition* funDef=completeAst.findFunctionByName(funName);
      if(funDef==nullptr) {
        cout<<"WARNING: function "<<funName<<" does not exist."<<endl;
        return true;
      } else {
        listOfFunctionDefinitions.push_back(funDef);
      }
    }
    for (auto funDef : listOfFunctionDefinitions) {
      SgType* oldBuiltType=buildTypeFromStringSpec(oldType,funDef);
      SgType* newBuiltType=buildTypeFromStringSpec(newType,funDef);
      tt.addTypeTransformationToList(_list,newBuiltType,funDef,"TYPEFORGE"+location,base,oldBuiltType,listing);
    }
    return false;
  }
}  

VarTypeCommand::VarTypeCommand(std::string name, std::string fun, std::string toType, bool base, bool listing, int number) : Command(base, listing, number){ 
  varName = name;
  funName = fun;
  newType = toType;
}
  
int VarTypeCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  SgFunctionDefinition* funDef;
  SgType* builtType;
  if(funName=="$global") {
    funDef=nullptr; 
    SgGlobal* globalScope = root->get_globalScopeAcrossFiles();
    builtType = buildTypeFromStringSpec(newType,globalScope);
  } else {
    funDef=completeAst.findFunctionByName(funName);
    if(funDef==0) {
      cerr<<"Error: function "<<funName<<" does not exist in file."<<endl;
      return true;
    }
    builtType=buildTypeFromStringSpec(newType,funDef);
  }
  if(builtType==nullptr) {
    cerr<<"Error: unknown type "<<newType<<" in command "<<commandNumber<<"."<<endl;
    return true;
  } else {
    tt.addNameTransformationToList(_list,builtType,funDef,varName,base,listing);
    return false;
  }
} 

HandleCommand::HandleCommand(std::string nodeHandle, std::string toType, bool base, bool listing, int number) : Command(base, listing, number){
  handle  = nodeHandle;
  newType = toType; 
}
 
int HandleCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  //TODO Add error messages
  abstract_node* rootNode = buildroseNode(root);
  abstract_handle* rootHandle = new abstract_handle(rootNode);
  abstract_handle* ahandle = new abstract_handle(rootHandle,handle);
  if(ahandle != nullptr){
    if(abstract_node* anode = ahandle->getNode()){
      SgNode* targetNode = (SgNode*) anode->getNode();
      if(SgVariableDeclaration* varDec = isSgVariableDeclaration(targetNode)){
        SgScopeStatement* scope = varDec->get_scope();
        SgType* newBuiltType=buildTypeFromStringSpec(newType,scope);
        tt.addHandleTransformationToList(_list,newBuiltType,base,targetNode,listing);
        return false;
      }
      else if(SgInitializedName* initName = isSgInitializedName(targetNode)){
        SgScopeStatement* scope = isSgDeclarationStatement(initName->get_parent())->get_scope();
        SgType* newBuiltType=buildTypeFromStringSpec(newType,scope);
        tt.addHandleTransformationToList(_list,newBuiltType,base,targetNode,listing);
        return false;
      }
      else if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(targetNode)){
        SgFunctionDefinition* funDef = funDec->get_definition();
        SgType* newBuiltType=buildTypeFromStringSpec(newType,funDef);
        SgType* oldType = SgNodeHelper::getFunctionReturnType(funDef);
        if(base) oldType = oldType->findBaseType();
        //TODO Do handle based transformation
        tt.addTypeTransformationToList(_list,newBuiltType,funDef,"TYPEFORGEret",base,oldType,listing);
        return false;
      }
    }
  }
  return true;
}

TransformCommand::TransformCommand(std::string funName, std::string typeName, std::string transformName, int number) : Command(false, false, number){
  functionName = funName;
  accessTypeName = typeName;
  transformationName = transformName;
}

int TransformCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  std::list<SgFunctionDefinition*> listOfFunctionDefinitions;
  if(functionName=="*") {
    listOfFunctionDefinitions=SgNodeHelper::listOfFunctionDefinitions(root);
  } else {
    SgFunctionDefinition* funDef=completeAst.findFunctionByName(functionName);
    if(funDef==nullptr) {
      cerr<<"Error: Command "<<commandNumber<<": function "<<functionName<<" not found."<<endl;
      return true;
    } else {
      listOfFunctionDefinitions.push_back(funDef);
    }
  }
  for (auto funDef : listOfFunctionDefinitions) {
    SgType* accessType=buildTypeFromStringSpec(accessTypeName,funDef);
    if(tt.getTraceFlag()) { cout<<"TRACE: transformation: "<<transformationName<<endl;}
    if(transformationName=="readwrite_access_transformation") {
      tfTransformation.transformHancockAccess(accessType,funDef);
    } else if(transformationName=="arrayofstructs_access_transformation") {
      tfTransformation.transformArrayOfStructsAccesses(accessType,funDef);
    } else if(transformationName=="ad_intermediate_instrumentation") {
      tfTransformation.instrumentADIntermediate(funDef);
    }
  }
  return false; 
}

CommandList::CommandList(std::string spec){
  commandsList = {};
}

int CommandList::runCommands(SgProject* root, TFTypeTransformer& tt, TFTransformation& tfTransformation){
  RoseAst completeAst(root);
  for(auto command : commandsList){
    if(command->run(root, completeAst, tt, tfTransformation, _list)) return true;
  }
  return false;
}

void CommandList::addVarTypeCommand(std::string varName, std::string funName, std::string newType, bool base, bool listing){
  VarTypeCommand* newCommand = new VarTypeCommand(varName, funName, newType, base, listing, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addTypeCommand(std::string location, std::string funName, std::string newType, std::string oldType, bool base, bool listing){
  TypeCommand* newCommand = new TypeCommand(location, funName, newType, oldType, base, listing, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addHandleCommand(std::string handle, std::string newType, bool base, bool listing){
  HandleCommand* newCommand = new HandleCommand(handle, newType, base, listing, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addTransformCommand(std::string funName, std::string typeName, std::string transformName){
  TransformCommand* newCommand = new TransformCommand(funName, typeName, transformName, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::nextCommand(){
  nextCommandNumber++;
}

TFTypeTransformer::VarTypeVarNameTupleList CommandList::getTransformationList(){
  return _list;
}
