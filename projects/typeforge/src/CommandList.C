#include "sage3basic.h"
#include "CommandList.h"
#include "SpecFrontEnd.h"
#include "TFTransformation.h"
#include "CppStdUtilities.h"
#include "TFHandles.h"
#include <iostream>
#include <vector>
#include "SgNodeHelper.h"
#include <regex>
#include "CppStdUtilities.h"
#include <boost/algorithm/string.hpp>

namespace Typeforge {

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

#define DEBUG__buildTypeFromStringSpec 0

//Returns the SgType* that mathces the type defined by the string in the given scope. If no type matches will exit.
SgType* buildTypeFromStringSpec(string type, SgScopeStatement* providedScope) {
#if DEBUG__buildTypeFromStringSpec
  std::cout << "ENTER buildTypeFromStringSpec" << std::endl;
#endif
  SgType* newType=nullptr;
  regex e1("[_A-Za-z:]+|\\*|&|const|<|>");
  regex_token_iterator<string::iterator> rend;
  regex_token_iterator<string::iterator> a ( type.begin(), type.end(), e1 );
  bool buildConstType=false;
  bool isLongType=false;
  bool isShortType=false;
  while (a!=rend) {
    string typePart=*a++;
    if(typePart=="<" || typePart==">") {
      cerr<<"Error: unsupported type "<<type<<endl;
      cerr<<"Note: Parameterized types are not supported as type names. Try to use a typename introduced by a typedef instead."<<endl;
      exit(1);
    } else if(typePart=="float") {
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
    } else if(regex_match(typePart, regex("^[_A-Za-z:]+$"))) {
      if(newType!=nullptr) goto parseerror;
      // TV: I kept this logic even if I don't know why it exists...
      if(SgFunctionDefinition* funDef=isSgFunctionDefinition(providedScope)) {
        SgType* userDefinedType=findUserDefinedTypeByName(funDef,typePart);
        if(userDefinedType) {
          newType=userDefinedType;
        }
      }
      // TV: new logic handling qualified named and building opaque types in the global scope
      if (newType == nullptr) {
        SgScopeStatement * globalScope = SageInterface::getGlobalScope(providedScope);
        ROSE_ASSERT(globalScope != NULL);
        // TODO handle scoping: split `typePart` using `::` then lookup/create namespaces
        newType=SageBuilder::buildOpaqueType(typePart, globalScope);
#if DEBUG__buildTypeFromStringSpec
        std::cout << " - newType = " << newType << " ( " << newType->class_name() << "): " << newType->unparseToString() << "" << std::endl;
#endif
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
TypeCommand::TypeCommand(string loc, string fun, string toType, string fromType, bool base, bool listing, int number) : Command(base, listing, number){
  location = loc;
  funName  = fun;
  newType  = toType;
  oldType  = fromType;
}
 
int TypeCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  if(tt.getTraceFlag()) { cout<<"TRACE: TypeCommand::run started."<<endl;}
  if(funName == "$global") {
    SgGlobal* globalScope = root->get_globalScopeAcrossFiles();
    SgType* oldBuiltType=buildTypeFromStringSpec(oldType,globalScope);
    SgType* newBuiltType=buildTypeFromStringSpec(newType,globalScope);
    tt.addTypeTransformationToList(_list,newBuiltType,nullptr,"",base,oldBuiltType,listing);
    return false;
  } else {
    list<SgFunctionDeclaration*> listOfFunctionDeclarations;
    if(funName=="*") {
      listOfFunctionDeclarations=SgNodeHelper::listOfFunctionDeclarations(root);
    } else {
      listOfFunctionDeclarations=completeAst.findFunctionDeclarationsByName(funName);
      if(listOfFunctionDeclarations.empty()) {
        cerr<<"Error: Command "<<commandNumber<<": function "<<funName<<" does not exist."<<endl;
        return true;
      }
    }
    for (auto funDecl : listOfFunctionDeclarations) {
      SgType* oldBuiltType=buildTypeFromStringSpec(oldType,funDecl->get_scope());
      SgType* newBuiltType=buildTypeFromStringSpec(newType,funDecl->get_scope());
      if(tt.getTraceFlag()) { cout<<"TRACE: TypeCommand::run : adding type transformation to list: "<<oldBuiltType->unparseToString()<<" ==> "<<newBuiltType->unparseToString()<<endl;}
      tt.addTypeTransformationToList(_list,newBuiltType,funDecl,"TYPEFORGE"+location,base,oldBuiltType,listing);
    }
    return false;
  }
}  

//Will replace the type of the variable specified by varName and funName(funName=$global for globals) to newType
VarTypeCommand::VarTypeCommand(string name, string fun, string toType, bool base, bool listing, int number) : Command(base, listing, number){ 
  varName = name;
  funName = fun;
  newType = toType;
}

int VarTypeCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  SgGlobal* globalScope = root->get_globalScopeAcrossFiles();

  SgType* builtType = buildTypeFromStringSpec(newType,globalScope);
  if(builtType==nullptr) {
    cerr<<"Error: Command "<<commandNumber<<": unknown type "<<newType<<"."<<endl;
    return true;
  }

  if(funName=="$global") {
    tt.addNameTransformationToList(_list,builtType,nullptr,varName,base,listing);
  } else {
    list<SgFunctionDeclaration*> listOfFunctionDeclarations = completeAst.findFunctionDeclarationsByName(funName);
    if(listOfFunctionDeclarations.empty()) {
      cerr<<"Error: Command "<<commandNumber<<": function "<<funName<<" does not exist in file."<<endl;
      return true;
    }
    for (auto funDecl : listOfFunctionDeclarations) {
      tt.addNameTransformationToList(_list,builtType,funDecl,varName,base,listing);
    }
  }
  return false;
} 

//Replaces the type of the variable specified by the handlde with newType
HandleCommand::HandleCommand(string nodeHandle, string toType, bool base, bool listing, int number) : Command(base, listing, number){
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

//Performs specified transformation
TransformCommand::TransformCommand(string funName, string typeName, string transformName, int number) : Command(false, false, number){
  functionName = funName;
  accessTypeName = typeName;
  transformationName = transformName;
}

int TransformCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  list<SgFunctionDefinition*> listOfFunctionDefinitions;
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

//Adds includes to file
IncludeCommand::IncludeCommand(string funName, string inName, int number) : Command(false, false, number){
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

//Will replace #pragma with given insetion
PragmaCommand::PragmaCommand(string from, string to, int number) : Command(false, false, number){
  fromMatch = from;
  toReplace = to;
}

int PragmaCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  tfTransformation.addPragmaTransformation(fromMatch, toReplace);
  return false;
}

FileCommand::FileCommand(string file, int number) : Command(false, false, number){
  fileName = file;
}

int FileCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  tt.addFileChangeToList(_list, fileName);
  return false;
}

//Will replace the type of the variable specified by varName and funName(funName=$global for globals) to newType
SetTypeCommand::SetTypeCommand(string name, string fun, string toType, string varHandle, bool base, int number) : Command(base, false, number){ 
  varName = name;
  funName = fun;
  newType = toType;
  handle  = varHandle;
}

int SetTypeCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  tt.addSetChangeToList(_list, true);
  if(handle == ""){
    SgType* builtType;
    if(funName=="$global") {
      SgGlobal* globalScope = root->get_globalScopeAcrossFiles();
      builtType = buildTypeFromStringSpec(newType,globalScope);
      if(builtType==nullptr) {
        cerr<<"Error: Command "<<commandNumber<<": unknown type "<<newType<<"."<<endl;
        tt.addSetChangeToList(_list, false);
        return true;
      }
      tt.addNameTransformationToList(_list,builtType,nullptr,varName,base,listing);
    } else {
      list<SgFunctionDeclaration*> listOfFunctionDeclarations = completeAst.findFunctionDeclarationsByName(funName);
      if(listOfFunctionDeclarations.empty()) {
        cerr<<"Error: Command "<<commandNumber<<": function "<<funName<<" does not exist in file."<<endl;
        tt.addSetChangeToList(_list, false);
        return true;
      }
      builtType=buildTypeFromStringSpec(newType,listOfFunctionDeclarations.front()->get_scope());
      if(builtType==nullptr) {
        cerr<<"Error: Command "<<commandNumber<<": unknown type "<<newType<<"."<<endl;
        tt.addSetChangeToList(_list, false);
        return true;
      }
      for (auto funDecl : listOfFunctionDeclarations) {
        tt.addNameTransformationToList(_list,builtType,funDecl,varName,base,listing);
      }
    }
  } else {
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
  }
  tt.addSetChangeToList(_list, false);
  return false;
} 

//Replaces the type of the variable specified by the handlde with newType
ListSetsCommand::ListSetsCommand(string from, string to, bool base, int number) : Command(base, false, number){
  fromType  = from;
  toType = to; 
}

int ListSetsCommand::run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list){
  SgGlobal* globalScope = root->get_globalScopeAcrossFiles();
  SgType* builtType = buildTypeFromStringSpec(fromType,globalScope);
  if(builtType != nullptr){
    tt.writeSets(root, builtType, toType); 
    return false;
  }
  return true;
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
void CommandList::addVarTypeCommand(string varName, string funName, string newType, bool base, bool listing){
  VarTypeCommand* newCommand = new VarTypeCommand(varName, funName, newType, base, listing, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addSetTypeCommand(string varName, string funName, string newType, string handle, bool base){
  SetTypeCommand* newCommand = new SetTypeCommand(varName, funName, newType, handle, base, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addTypeCommand(string location, string funName, string newType, string oldType, bool base, bool listing){
  TypeCommand* newCommand = new TypeCommand(location, funName, newType, oldType, base, listing, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addHandleCommand(string handle, string newType, bool base, bool listing){
  HandleCommand* newCommand = new HandleCommand(handle, newType, base, listing, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addListSetsCommand(string fromType, string toType, bool base){
  ListSetsCommand* newCommand = new ListSetsCommand(fromType, toType, base, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addTransformCommand(string funName, string typeName, string transformName){
  TransformCommand* newCommand = new TransformCommand(funName, typeName, transformName, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addIncludeCommand(string funName, string includeName){
  IncludeCommand* newCommand = new IncludeCommand(funName, includeName, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addPragmaCommand(string fromMatch, string toReplace){
  PragmaCommand* newCommand = new PragmaCommand(fromMatch, toReplace, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::addFileCommand(string fileName){
  FileCommand* newCommand = new FileCommand(fileName, nextCommandNumber);
  commandsList.push_back(newCommand);
}

void CommandList::nextCommand(){
  nextCommandNumber++;
}

TFTypeTransformer::VarTypeVarNameTupleList CommandList::getTransformationList(){
  return _list;
}

}

