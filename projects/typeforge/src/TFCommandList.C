#include "sage3basic.h"
#include "TFCommandList.h"
#include "TFSpecFrontEnd.h"
#include "TFTransformation.h"
#include "CppStdUtilities.h"
#include "TFHandles.h"
#include <iostream>
#include <vector>
#include "SgNodeHelper.h"
#include <regex>
#include "CppStdUtilities.h"
#include <boost/algorithm/string.hpp>

using namespace std;

//Returns base type of varInitName if the name ofthe base type matches typename. Else returns nullptr.
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

//Returns the type that matches the userDefinedTypeName inside the given function if it exists. Else returns nullptr.
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

//Returns the SgType* that mathces the type defined by the string in the given scope. If no type matches will exit.
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

//Type command will take all instances of oldType and change them to newType inside the given function(or $global)
//and for specified location(ret,args,body)
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
//cout<<newBuiltType->unparseToString()<<"\n";
//cout<<newType<<"\n";
    tt.addTypeTransformationToList(_list,newBuiltType,nullptr,"",base,oldBuiltType,listing);
    return false;
  } else {
    std::list<SgFunctionDefinition*> listOfFunctionDefinitions;
    if(funName=="*") {
      listOfFunctionDefinitions=SgNodeHelper::listOfFunctionDefinitions(root);
    } else {
      SgFunctionDefinition* funDef=completeAst.findFunctionByName(funName);
      if(funDef==nullptr) {
        cerr<<"Error: Command "<<commandNumber<<": function "<<funName<<" does not exist."<<endl;
        return true;
      } else {
        listOfFunctionDefinitions.push_back(funDef);
      }
    }
    for (auto funDef : listOfFunctionDefinitions) {
      SgType* oldBuiltType=buildTypeFromStringSpec(oldType,funDef);
      SgType* newBuiltType=buildTypeFromStringSpec(newType,funDef);
//cout<<newType<<"\n";    
//cout<<newBuiltType->unparseToString()<<"\n";
      tt.addTypeTransformationToList(_list,newBuiltType,funDef,"TYPEFORGE"+location,base,oldBuiltType,listing);
    }
    return false;
  }
}  

//Will replace the type of the variable specified by varName and funName(funName=$global for globals) to newType
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
      cerr<<"Error: Command "<<commandNumber<<": function "<<funName<<" does not exist in file."<<endl;
      return true;
    }
    builtType=buildTypeFromStringSpec(newType,funDef);
  }
  if(builtType==nullptr) {
    cerr<<"Error: Command "<<commandNumber<<": unknown type "<<newType<<"."<<endl;
    return true;
  } else {
    tt.addNameTransformationToList(_list,builtType,funDef,varName,base,listing);
    return false;
  }
} 

//Replaces the type of the variable specified by the handlde with newType
HandleCommand::HandleCommand(std::string nodeHandle, std::string toType, bool base, bool listing, int number) : Command(base, listing, number){
  handle  = nodeHandle;
  newType = toType; 
}

int HandleCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  vector<SgNode*> nodeVector = TFHandles::getNodeVectorFromString(root, handle);
  for(auto i = nodeVector.begin(); i != nodeVector.end(); ++i){
    if(SgVariableDeclaration* varDec = isSgVariableDeclaration(*i)){
      SgScopeStatement* scope = varDec->get_scope();
      SgType* newBuiltType=buildTypeFromStringSpec(newType,scope);
      tt.addHandleTransformationToList(_list,newBuiltType,base,varDec,listing);
    }
    else if(SgInitializedName* initName = isSgInitializedName(*i)){
      SgScopeStatement* scope = isSgDeclarationStatement(initName->get_parent())->get_scope();
      SgType* newBuiltType=buildTypeFromStringSpec(newType,scope);
      tt.addHandleTransformationToList(_list,newBuiltType,base,initName,listing);
    }
    else if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(*i)){
      SgFunctionDefinition* funDef = funDec->get_definition();
      SgType* newBuiltType=buildTypeFromStringSpec(newType,funDef);
      tt.addHandleTransformationToList(_list,newBuiltType,base,funDec,listing);
    }
  }
  return false;
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
      tfTransformation.addReadWriteTransformation(funDef, accessType);
    } else if(transformationName=="arrayofstructs_access_transformation") {
      tfTransformation.addArrayStructTransformation(funDef, accessType);
    } else if(transformationName=="ad_intermediate_instrumentation") {
      tfTransformation.addADTransformation(funDef);
    }
  }
  return false; 
}

IncludeCommand::IncludeCommand(std::string funName, std::string inName, int number) : Command(false, false, number){
  functionName = funName;
  includeName = inName;
}

int IncludeCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  SgFilePtrList listOfFiles;
  if(functionName=="*") {
    listOfFiles = root->get_files();
  } else {
    SgNode* currentNode=completeAst.findFunctionByName(functionName);
    while(currentNode != nullptr && !isSgFile(currentNode)){
      currentNode = currentNode->get_parent();
    }
    if(currentNode==nullptr) {
      return false;
    } else {
      if(SgFile* file = isSgFile(currentNode)){
        listOfFiles.push_back(file);
      }
    }
  }
  for (auto file : listOfFiles) {
    if(SgSourceFile* source = isSgSourceFile(file)){
      tfTransformation.addIncludeTransformation(includeName, false, source);
    }  
  }
  return false; 
}

PragmaCommand::PragmaCommand(std::string from, std::string to, int number) : Command(false, false, number){
  fromMatch = from;
  toReplace = to;
}

int PragmaCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  tfTransformation.addPragmaTransformation(fromMatch, toReplace);
  return false;
}

FileCommand::FileCommand(std::string file, int number) : Command(false, false, number){
  fileName = file;
}

int FileCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  tt.addFileChangeToList(_list, fileName);
  return false;
}

CommandList::CommandList(){
  commandsList = {};
}
//Call the run command on all commands stored in the list.
int CommandList::runCommands(SgProject* root, TFTypeTransformer& tt, TFTransformation& tfTransformation){
  RoseAst completeAst(root);
  for(auto command : commandsList){
    if(command->run(root, completeAst, tt, tfTransformation, _list)) return true;
  }
  return false;
}

//Set of methods for adding commands to the command list
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

void CommandList::addIncludeCommand(std::string funName, std::string includeName){
  IncludeCommand* newCommand = new IncludeCommand(funName, includeName, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addPragmaCommand(std::string fromMatch, std::string toReplace){
  PragmaCommand* newCommand = new PragmaCommand(fromMatch, toReplace, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addFileCommand(std::string fileName){
  FileCommand* newCommand = new FileCommand(fileName, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::nextCommand(){
  nextCommandNumber++;
}

TFTypeTransformer::VarTypeVarNameTupleList CommandList::getTransformationList(){
  return _list;
}
