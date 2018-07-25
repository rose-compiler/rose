#include "sage3basic.h"
#include "TFTypeTransformer.h"
#include "SgNodeHelper.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "CppStdUtilities.h"
#include <ToolConfig.hpp>
#include "abstract_handle.h"
#include "roseAdapter.h"

using namespace std;
using namespace AbstractHandle;

// static member
bool TFTypeTransformer::_traceFlag=false;

void TFTypeTransformer::addToTransformationList(std::list<VarTypeVarNameTuple>& list,SgType* type, SgFunctionDefinition* funDef,string varNames){
  TFTypeTransformer::addToTransformationList(list,type,funDef,varNames,false,nullptr,nullptr,false);
}

void TFTypeTransformer::addHandleTransformationToList(std::list<VarTypeVarNameTuple>& list,SgType* type,bool base,SgNode* handleNode, bool listing){
  TFTypeTransformer::addToTransformationList(list,type,nullptr,"",base,nullptr,handleNode,listing);
} 

void TFTypeTransformer::addTypeTransformationToList(std::list<VarTypeVarNameTuple>& list,SgType* type, SgFunctionDefinition* funDef, std::string varNames, bool base, SgType* fromType, bool listing){
  TFTypeTransformer::addToTransformationList(list,type,funDef,varNames,base,fromType,nullptr,listing);
}
 
void TFTypeTransformer::addNameTransformationToList(std::list<VarTypeVarNameTuple>& list,SgType* type, SgFunctionDefinition* funDef, std::string varNames, bool base, bool listing){
  TFTypeTransformer::addToTransformationList(list,type,funDef,varNames,base,nullptr,nullptr,listing);
}

void TFTypeTransformer::addToTransformationList(std::list<VarTypeVarNameTuple>& list,SgType* type, SgFunctionDefinition* funDef,string varNames, bool base, SgType* fromType, SgNode* handleNode, bool listing) {
  vector<string> varNamesVector=CppStdUtilities::splitByComma(varNames);
  for (auto name:varNamesVector) {
    TFTypeTransformer::VarTypeVarNameTuple p=std::make_tuple(type,funDef,name,base,fromType,handleNode,listing);
    list.push_back(p);
  }
}

void TFTypeTransformer::nathan_setConfig(ToolConfig* config){
  _outConfig = config;
  //_outConfig->getActions().clear();
  _outConfig->setToolID("typeforge");
}
//TODO
void TFTypeTransformer::nathan_setConfigFile(string fileName){
  _writeConfig = fileName;
  if(_outConfig == nullptr){
    _outConfig = new ToolConfig();
    _outConfig->setToolID("typeforge");
  }
}

std::string nathan_getNodeFileName(SgNode* node){
  SgNode* currentNode = node;
  SgSourceFile* file = nullptr;
  while(file == nullptr && currentNode != nullptr){
    file = isSgSourceFile(currentNode);
    currentNode = currentNode->get_parent();
  }
  if(currentNode == nullptr) return "";
  else return file->getFileName();
}

void TFTypeTransformer::nathan_addToActionList(string varName, string scope, SgType* fromType, SgType* toType, SgNode* handleNode, bool base){
  if(!fromType || !toType || !handleNode) return;
  if(_writeConfig == "") return;
  if(varName == "") return;
  abstract_node* anode = buildroseNode(handleNode);
  abstract_handle* ahandle = new abstract_handle(anode);
  if(base) _outConfig->addReplaceVarBaseType(ahandle->toString(), varName, scope, nathan_getNodeFileName(handleNode), fromType->unparseToString(), toType->unparseToString()); 
  else _outConfig->addReplaceVarType(ahandle->toString(), varName, scope, nathan_getNodeFileName(handleNode), fromType->unparseToString(), toType->unparseToString()); 
}
void TFTypeTransformer::transformCommandLineFiles(SgProject* project) {
  // make all floating point casts explicit
  makeAllCastsExplicit(project);
  // transform casts in AST
  transformCastsInCommandLineFiles(project);
}

void TFTypeTransformer::transformCommandLineFiles(SgProject* project,VarTypeVarNameTupleList& list) {
  for (auto typeNameTuple:list) {
    SgType* newVarType=std::get<0>(typeNameTuple);
    SgFunctionDefinition* funDef=std::get<1>(typeNameTuple);
    string varName=std::get<2>(typeNameTuple);
    bool base = std::get<3>(typeNameTuple);
    SgType* fromType = std::get<4>(typeNameTuple);
    SgNode* handleNode = std::get<5>(typeNameTuple);
    bool listing = std::get<6>(typeNameTuple);
    SgNode* root=nullptr;
    if(funDef)
      root=funDef;
    else
      root=project;
    if(!handleNode){
      int numChanges=changeVariableType(root, varName, newVarType, base, fromType, listing);
      if(numChanges==0 && fromType == nullptr) {
        cout<<"Warning: Did not find variable "<<varName;
        if(funDef) {
          cout<<" in function "<<SgNodeHelper::getFunctionName(funDef)<<".";
        } else {
          cout<<" anywhere in file."<<endl;
        }
        cout<<endl;
      } else if(numChanges>1 && fromType == nullptr) {
        cout<<"Warning: Found more than one declaration of variable "<<varName<<endl;
      }
      if(fromType == nullptr) _totalNumChanges+=numChanges;
      else _totalTypeNameChanges+=numChanges;
      transformCommandLineFiles(project);
    }else{
      _totalHandleChanges+=nathan_changeHandleType(handleNode, newVarType, base, listing);
    }
  }
  if(_writeConfig != ""){
    _outConfig->saveConfig(_writeConfig);
  }
}

void TFTypeTransformer::transformCastsInCommandLineFiles(SgProject* project) {
  _castTransformer.transformCommandLineFiles(project);
}

//returns a new type with same structure as root but with newBaseType as a base
SgType* TFTypeTransformer::nathan_rebuildBaseType(SgType* root, SgType* newBaseType){
  //handle array type
  if(SgArrayType* arrayType = isSgArrayType(root)){
    SgType* base = nathan_rebuildBaseType(arrayType->get_base_type(), newBaseType);
    SgExpression* index = arrayType->get_index();
    SgExprListExp* dim_info = arrayType->get_dim_info();
    if(dim_info != nullptr){
      return SageBuilder::buildArrayType(base, dim_info);
    }
    else if(index != nullptr){
      return SageBuilder::buildArrayType(base, index);
    }
    else{
      return SageBuilder::buildArrayType(base);
    }
  }
  //handle pointer type
  else if(SgPointerType* pointerType = isSgPointerType(root)){
    SgType* base = nathan_rebuildBaseType(pointerType->get_base_type(), newBaseType);
    SgPointerType* newPointer = SageBuilder::buildPointerType(base);
    return newPointer;
  }
  //handle typedef, does not build new typedef. builds type around structure defined in typedef
  else if(SgTypedefType* defType = isSgTypedefType(root)){
    return nathan_rebuildBaseType(defType->get_base_type(), newBaseType);
  }
  //handle reference type
  else if(SgReferenceType* refType = isSgReferenceType(root)){
    SgType* base = nathan_rebuildBaseType(refType->get_base_type(), newBaseType);
    SgReferenceType* newReference = SageBuilder::buildReferenceType(base);
    return newReference;
  }
  //handle type modifiers(const, restrict, volatile)
  else if(SgModifierType* modType = isSgModifierType(root)){
    SgType* base =  nathan_rebuildBaseType(modType->get_base_type(), newBaseType);
    SgTypeModifier modifier = modType->get_typeModifier();
    SgModifierType* newMod;
    if(modifier.isRestrict()){
      newMod = SageBuilder::buildRestrictType(base);
    }
    else{
      SgConstVolatileModifier cmod = modifier.get_constVolatileModifier();
      if(cmod.isConst()){
        newMod = SageBuilder::buildConstType(base);
      }
      else if(cmod.isVolatile()){
        newMod = SageBuilder::buildVolatileType(base);
      }
      else{
        newMod = SageBuilder::buildModifierType(base);
      }
    }
    return newMod;
  }
  //reached base so return new base instead
  else{
    return newBaseType;
  }
}

int TFTypeTransformer::nathan_changeHandleType(SgNode* handle, SgType* newType, bool base, bool listing){
  SgInitializedName* initName = isSgInitializedName(handle);
  if(SgVariableDeclaration* varDec = isSgVariableDeclaration(handle)){
    initName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDec);
  }
  if(initName != nullptr){
    SgType* changeType = newType;
    if(base){
      SgType* oldType = initName->get_type();
      changeType = nathan_rebuildBaseType(oldType, newType);
    }
    if(!listing){
      SgSymbol* varSym = SgNodeHelper::getSymbolOfInitializedName(initName);
      string varName = SgNodeHelper::symbolToString(varSym);
      TFTypeTransformer::trace("Found declaration of variable "+varName+". Changed type to "+changeType->unparseToString());
      initName->set_type(changeType);
      return 1;
    }
  }else if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(handle)){
      SgFunctionDefinition* funDef = funDec->get_definition(); 
      SgType* funRetType=SgNodeHelper::getFunctionReturnType(funDef);
      SgFunctionType* funType = funDec->get_type();
      if(base){
        newType = nathan_rebuildBaseType(funRetType, newType);
      }
      string funName = SgNodeHelper::getFunctionName(funDef);
      TFTypeTransformer::trace("Found return "+((funName=="")? "" : "in "+funName)+". Changed type to "+newType->unparseToString());
      if(!listing){
        funType->set_orig_return_type(newType);
        return 1;
      }
  }
  return 0;
}

int TFTypeTransformer::nathan_changeType(SgInitializedName* varInitName, SgType* newType, SgType* oldType, std::string varName, bool base, SgFunctionDefinition* funDef, SgNode* handleNode,bool listing){
  SgType* baseType;
  if(base){
    SgType* oldInitType = varInitName->get_type();
    baseType = nathan_rebuildBaseType(oldInitType, newType);
  }else{
    baseType = newType;
  }
  string scopeName = "global";
  if(funDef){
    scopeName = SgNodeHelper::getFunctionName(funDef);
    scopeName = "function:<" + scopeName + ">";
  }
  if(listing){
    nathan_addToActionList(varName, scopeName, oldType, newType, handleNode, base);
    return 0; 
  } 
  else{
    TFTypeTransformer::trace("Found declaration of variable "+varName+" in "+scopeName+". Changed type to "+baseType->unparseToString());
    varInitName->set_type(baseType);
    return 1;
  }
}

//void nathan_addToActionList(string varName, string scope, SgType* fromType, SgType* toType, SgNode* handleNode){
int TFTypeTransformer::changeTypeIfInitNameMatches(SgInitializedName* varInitName,SgNode* root,string varNameToFind,SgType* newType) {
  return TFTypeTransformer::changeTypeIfInitNameMatches(varInitName, root, varNameToFind, newType, false, nullptr,false);
}

int TFTypeTransformer::changeTypeIfInitNameMatches(SgInitializedName* varInitName,SgNode* root,string varNameToFind,SgType* newType,bool base, SgNode* handleNode,bool listing) {
  int foundVar=0;
  if(varInitName) {
    SgSymbol* varSym=SgNodeHelper::getSymbolOfInitializedName(varInitName);
    if(varSym) {
      string varName=SgNodeHelper::symbolToString(varSym);
      if(varName==varNameToFind) {
        foundVar += nathan_changeType(varInitName, newType, nullptr, varName, base, isSgFunctionDefinition(root), handleNode,listing);
      }
    }
  }
  return foundVar;
}

int TFTypeTransformer::nathan_changeTypeIfFromTypeMatches(SgInitializedName* varInitName, SgNode* root, SgType* newType, SgType* fromType, bool base, SgNode* handleNode, bool listing){
  int foundVar = 0;
  if(varInitName){
    SgType* oldType = varInitName->get_type();
    if(base){
      oldType = oldType->findBaseType();
    }
    if(oldType == fromType){
      SgSymbol* varSym = SgNodeHelper::getSymbolOfInitializedName(varInitName);
      string varName = SgNodeHelper::symbolToString(varSym);
      foundVar+=nathan_changeType(varInitName, newType, fromType, varName, base, isSgFunctionDefinition(root), handleNode,listing);
    }
  }
  return foundVar;
}

int TFTypeTransformer::changeVariableType(SgNode* root, string varNameToFind, SgType* newType) {
  return TFTypeTransformer::changeVariableType(root, varNameToFind, newType, false, nullptr, false);
}

int TFTypeTransformer::changeVariableType(SgNode* root, string varNameToFind, SgType* newType, bool base, SgType* fromType, bool listing) {
  RoseAst ast(root);
  int foundVar=0;
  //process type changes inside of a function
  if(SgFunctionDefinition* funDef=isSgFunctionDefinition(root)) {
    // need to process formal params and return explicitly because not found in traversal of functionDef (is traversed from function decl)
    SgInitializedNamePtrList& initNamePtrList=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
    for(auto varInitName : initNamePtrList) {
      if(fromType != nullptr && varNameToFind == "TYPEFORGEargs"){
        foundVar+=nathan_changeTypeIfFromTypeMatches(varInitName,root,newType,fromType,base, varInitName,listing);
      }    
      else if(varNameToFind != "" && fromType == nullptr){
        foundVar+=changeTypeIfInitNameMatches(varInitName,root,varNameToFind,newType,base, varInitName,listing);
      }
    }
    //Change return type
    if(fromType != nullptr && varNameToFind == "TYPEFORGEret"){
      SgType* funRetType=SgNodeHelper::getFunctionReturnType(funDef);
      SgType* funBaseType = funRetType;
      if(base){
        funBaseType = funRetType->findBaseType();
      }
      if(funBaseType == fromType){
        SgFunctionDeclaration* funDecl = funDef->get_declaration();
        SgFunctionType* funType = funDecl->get_type();
        SgType* replaceType = newType;
        if(base){
          replaceType = nathan_rebuildBaseType(funRetType, newType);
        }
        string funName = SgNodeHelper::getFunctionName(root);
        TFTypeTransformer::trace("Found return "+((funName=="")? "" : "in "+funName)+". Changed type to "+replaceType->unparseToString());
        if(listing) nathan_addToActionList("$return", funName, fromType, newType, funDecl, base);
        else{
          funType->set_orig_return_type(replaceType);
          foundVar++;
        }
      }
    }
    //Change type in body of function
    if(varNameToFind != "TYPEFORGEret" && varNameToFind != "TYPEFORGEargs"){
      for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
        SgInitializedName* varInitName=nullptr;
        if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
          varInitName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
          if(fromType != nullptr && varNameToFind == "TYPEFORGEbody"){
            foundVar+=nathan_changeTypeIfFromTypeMatches(varInitName,root,newType,fromType,base,varDecl,listing);
          }      
          else if(varNameToFind != "" && fromType == nullptr){
            foundVar+=changeTypeIfInitNameMatches(varInitName,root,varNameToFind,newType,base,varDecl,listing);
          }
        }
      }
    }
  }else{
    //chnage type of globals
    if(varNameToFind != "TYPEFORGEret" && varNameToFind != "TYPEFORGEargs"){
      std::list<SgVariableDeclaration*> listOfGlobalVars = SgNodeHelper::listOfGlobalVars(isSgProject(root));
      if(listOfGlobalVars.size()>0){
        for(auto varDecl: listOfGlobalVars){
          SgInitializedName* varInitName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
          if(fromType != nullptr){
            foundVar+=nathan_changeTypeIfFromTypeMatches(varInitName,root,newType,fromType,base,varDecl,listing);
          }   
          else if(fromType == nullptr){
            foundVar+=changeTypeIfInitNameMatches(varInitName,root,varNameToFind,newType,base,varDecl,listing);
          }
        }
      }
    }
  }
  return foundVar;
}

void TFTypeTransformer::makeAllCastsExplicit(SgProject* root) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgCastExp* castExp=isSgCastExp(*i)) {
      if(castExp->isCompilerGenerated()) {
	castExp->unsetCompilerGenerated();
      }
    }
  }
}

void TFTypeTransformer::annotateImplicitCastsAsComments(SgProject* root) {
  RoseAst ast(root);
  std::string matchexpression="$CastNode=SgCastExp($CastOpChild)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  //std::cout << "Number of matched patterns with bound variables: " << r.size() << std::endl;
  list<string> report;
  int statementTransformations=0;
  for(MatchResult::reverse_iterator i=r.rbegin();i!=r.rend();++i) {
    statementTransformations++;
    SgCastExp* castExp=isSgCastExp((*i)["$CastNode"]);
    ROSE_ASSERT(castExp);
    SgExpression* childNode=isSgExpression((*i)["$CastOpChild"]);
    ROSE_ASSERT(childNode);
    if(castExp->isCompilerGenerated()) {
      SgType* castType=castExp->get_type();
      string castTypeString=castType->unparseToString();
      SgType* castedType=childNode->get_type();
      string castedTypeString=castedType->unparseToString();
      string reportLine="compiler generated cast: "
        +SgNodeHelper::sourceLineColumnToString(castExp->get_parent())
        +": "+castTypeString+" <== "+castedTypeString;
      if(castType==castedType) {
        reportLine+=" [ no change in type. ]";
      }
      // line are created in reverse order
      report.push_front(reportLine); 
      
      string newSourceCode;
      newSourceCode="/*CAST("+castTypeString+")*/";
      newSourceCode+=castExp->unparseToString();
      castExp->unsetCompilerGenerated(); // otherwise it is not replaced
      SgNodeHelper::replaceAstWithString(castExp,newSourceCode);
    }
  }
  for(list<string>::iterator i=report.begin();i!=report.end();++i) {
    cout<<*i<<endl;
  }
  //m.printMarkedLocations();
  //m.printMatchOperationsSequence();
  cout<<"Number of compiler generated casts: "<<statementTransformations<<endl;
}

void TFTypeTransformer::setTraceFlag(bool traceFlag) {
  _traceFlag=traceFlag;
}

bool TFTypeTransformer::getTraceFlag() {
  return _traceFlag;
}

void TFTypeTransformer::trace(string s) {
  if(TFTypeTransformer::_traceFlag) {
    cout<<"TRACE: "<<s<<endl;
  }
}

int TFTypeTransformer::getTotalNumChanges() {
  return _totalNumChanges;
}

int TFTypeTransformer::getTotalTypeNameChanges(){
  return _totalTypeNameChanges;
}

int TFTypeTransformer::getTotalHandleChanges(){
  return _totalHandleChanges;
}


void TFTypeTransformer::generateCsvTransformationStats(std::string fileName,int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation) {
  stringstream ss;
  ss<<numTypeReplace
    <<","<<tt.getTotalNumChanges()
    <<","<<tfTransformation.readTransformations
    <<","<<tfTransformation.writeTransformations
    <<","<<tfTransformation.arrayOfStructsTransformations
    <<","<<tfTransformation.adIntermediateTransformations
    <<endl;
  CppStdUtilities::writeFile(fileName,ss.str());
}

void TFTypeTransformer::printTransformationStats(int numTypeReplace,TFTypeTransformer& tt, TFTransformation& tfTransformation) {
  stringstream ss;
  int numTypeBasedReplacements=tt.getTotalTypeNameChanges();
  int numVarNameBasedReplacements=tt.getTotalNumChanges();
  int numHandleBasedReplacements=tt.getTotalHandleChanges();
  int arrayReadAccesses=tfTransformation.readTransformations;
  int arrayWriteAccesses=tfTransformation.writeTransformations;
  int arrayOfStructsAccesses=tfTransformation.arrayOfStructsTransformations;
  int adIntermediateTransformations=tfTransformation.adIntermediateTransformations;
  cout<<"STATS: number of variable types changed (based on type-name): "<<numTypeBasedReplacements<<endl;
  cout<<"STATS: number of variable types changed (based on var-name): "<<numVarNameBasedReplacements<<endl;
  cout<<"STATS: number of variable types changed (based on handle): "<<numHandleBasedReplacements<<endl;
  cout<<"STATS: number of transformed array read accesses: "<<arrayReadAccesses<<endl;
  cout<<"STATS: number of transformed array write accesses: "<<arrayWriteAccesses<<endl;
  cout<<"STATS: number of transformed arrays of structs accesses: "<<arrayOfStructsAccesses<<endl;
  cout<<"STATS: number of ad_intermediate transformations: "<<adIntermediateTransformations<<endl;
  int totalTransformations=numTypeBasedReplacements+numVarNameBasedReplacements+numHandleBasedReplacements+arrayReadAccesses+arrayWriteAccesses+arrayOfStructsAccesses+adIntermediateTransformations;
  cout<<"STATS: total number of transformations: "<<totalTransformations<<endl;
}

