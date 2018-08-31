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

//Constructors for directive list 
TransformDirective::TransformDirective(bool transformBase, bool onlyList, SgType* to_type){
  base = transformBase; listing = onlyList; toType = to_type;
}

NameTransformDirective::NameTransformDirective(string varName, SgFunctionDefinition* functionDefinition, bool base, bool listing, SgType* toType) : TransformDirective(base, listing, toType){
  name = varName; funDef = functionDefinition;
}

TypeTransformDirective::TypeTransformDirective(string functionLocation, SgFunctionDefinition* functionDefinition, SgType* from_type, bool base, bool listing, SgType* toType) : TransformDirective(base, listing, toType){
  location = functionLocation; funDef = functionDefinition; fromType = from_type;
}

HandleTransformDirective::HandleTransformDirective(SgNode* handleNode, bool base, bool listing, SgType* toType) : TransformDirective(base, listing, toType){
  node = handleNode;
}

FileTransformDirective::FileTransformDirective(string file) : TransformDirective(false, false, nullptr){
  fileName = file;
}

//Methods for adding to directive list
void TFTypeTransformer::addHandleTransformationToList(std::list<VarTypeVarNameTuple>& list,SgType* type,bool base,SgNode* handleNode, bool listing){
  list.insert(list.begin(),new HandleTransformDirective(handleNode, base, listing, type));
} 

void TFTypeTransformer::addTypeTransformationToList(std::list<VarTypeVarNameTuple>& list,SgType* type, SgFunctionDefinition* funDef, std::string varNames, bool base, SgType* fromType, bool listing){
  list.push_back(new TypeTransformDirective(varNames, funDef, fromType, base, listing, type));  
}
 
void TFTypeTransformer::addNameTransformationToList(std::list<VarTypeVarNameTuple>& list,SgType* type, SgFunctionDefinition* funDef, std::string varNames, bool base, bool listing){
  vector<string> varNamesVector=CppStdUtilities::splitByComma(varNames);
  for (auto name:varNamesVector) {
    list.push_back(new NameTransformDirective(name, funDef, base, listing, type));
  }
}

void TFTypeTransformer::addFileChangeToList(std::list<VarTypeVarNameTuple>& list, string file){
  list.push_back(new FileTransformDirective(file));
}

//Methods to run directive list
int NameTransformDirective::run(SgProject* project, TFTypeTransformer* tt){
  SgNode* root = nullptr;
  if(funDef) root = funDef;
  else root = project;
  int changes = tt->changeVariableType(root, name, toType, base, nullptr, listing);
  if(changes == 0){
    cout<<"Warning: Did not find variable "<<name;
    if(funDef) cout<<" in function "<<SgNodeHelper::getFunctionName(funDef)<<"."<<endl;
    else cout<<" in globals."<<endl;
  }else if(changes > 1) cout<<"Warning: Found more than one declaration of variable "<<name<<"."<<endl;
  return changes;
}

int TypeTransformDirective::run(SgProject* project, TFTypeTransformer* tt){
  SgNode* root = nullptr;
  if(funDef) root = funDef;
  else root = project;
  return tt->changeVariableType(root, location, toType, base, fromType, listing);
}

int HandleTransformDirective::run(SgProject* project, TFTypeTransformer* tt){
  return tt->nathan_changeHandleType(node, toType, base, listing);
}

int FileTransformDirective::run(SgProject* project, TFTypeTransformer* tt){
  if(fileName == "") tt->writeConfig();
  else tt->setConfigFile(fileName);
  return 0;  
}

//TypeTransformer stores changes during analysis phase then performs the changes when done.
int Transformer::transform(){
  for(auto i = transformations.begin(); i != transformations.end(); i++){
    SgNode* node     = i->first;
    string  location = get<0>(i->second);
    SgType* type     = get<1>(i->second);
    TFTypeTransformer::trace("Changing "+location+" type to "+type->unparseToString());
    if(SgInitializedName* initName = isSgInitializedName(node)){
      initName->set_type(type);
    }
    else if(SgFunctionType* funType = isSgFunctionType(node)){
      funType->set_orig_return_type(type);
    }
  }
  return transformationsCount;
}

int Transformer::addTransformation(string key, SgType* newType, SgNode* node){
  if(transformations.count(node) != 0){
    return 0;
  }else{
    ReplacementTuple newTuple = std::make_tuple(key, newType);
    transformations[node] = newTuple;
    transformationsCount++;
    return 1;
  }
}

//Mangae config file if user specifies
void TFTypeTransformer::setConfigFile(string fileName){
  _writeConfig = fileName;
  if(_outConfig == nullptr){
    try{
      _outConfig = new ToolConfig(fileName);
    }catch(...){
      _outConfig = new ToolConfig();
    }
    _outConfig->setToolID("typeforge");
  }
}

void TFTypeTransformer::writeConfig(){
  if(_writeConfig != ""){
    _outConfig->saveConfig(_writeConfig);
  }
  _writeConfig = "";
  delete _outConfig;
  _outConfig = nullptr;
}

//Returns the name of the file the specified node is part of
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

//Adds an entry in the config file
void TFTypeTransformer::nathan_addToActionList(string varName, string scope, SgType* fromType, SgType* toType, SgNode* handleNode, bool base){
  if(!fromType || !toType || !handleNode || !_outConfig) return;
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
  analyzeTransformations(project, list);
  executeTransformations(project);
}

void TFTypeTransformer::analyzeTransformations(SgProject* project, VarTypeVarNameTupleList& list){
  for (auto directive:list) {
    _totalNumChanges += directive->run(project, this);
  }
}

void TFTypeTransformer::executeTransformations(SgProject* project){
  _transformer.transform();
  transformCommandLineFiles(project);
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
      TFTypeTransformer::trace("Found declaration of variable "+varName+".");// Change type to "+changeType->unparseToString());
      _transformer.addTransformation(varName, changeType, initName);
      //initName->set_type(changeType);
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
      if(!listing){
        TFTypeTransformer::trace("Found return "+((funName=="")? "" : "in "+funName)+".");// Change type to "+newType->unparseToString());
        _transformer.addTransformation(funName+":$return", newType, funType);
        //funType->set_orig_return_type(newType);
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
    TFTypeTransformer::trace("Found declaration of variable "+varName+" in "+scopeName+".");// Change type to "+baseType->unparseToString());
    _transformer.addTransformation(scopeName+":"+varName,baseType,varInitName);
    //varInitName->set_type(baseType);
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
        TFTypeTransformer::trace("Found return "+((funName=="")? "" : "in "+funName)+".");// Changed type to "+replaceType->unparseToString());
        if(listing) nathan_addToActionList("$return", funName, fromType, newType, funDecl, base);
        else{
          _transformer.addTransformation(funName+":$return", funType, replaceType);
          //funType->set_orig_return_type(replaceType);
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
  int numReplacementsFound=tt.getTotalNumChanges();
  int arrayReadAccesses=tfTransformation.readTransformations;
  int arrayWriteAccesses=tfTransformation.writeTransformations;
  int arrayOfStructsAccesses=tfTransformation.arrayOfStructsTransformations;
  int adIntermediateTransformations=tfTransformation.adIntermediateTransformations;
  cout<<"STATS: number of variable type replacements found: "<<numReplacementsFound<<endl;
  cout<<"STATS: number of transformed array read accesses: "<<arrayReadAccesses<<endl;
  cout<<"STATS: number of transformed array write accesses: "<<arrayWriteAccesses<<endl;
  cout<<"STATS: number of transformed arrays of structs accesses: "<<arrayOfStructsAccesses<<endl;
  cout<<"STATS: number of ad_intermediate transformations: "<<adIntermediateTransformations<<endl;
  int totalTransformations=numReplacementsFound+arrayReadAccesses+arrayWriteAccesses+arrayOfStructsAccesses+adIntermediateTransformations;
  cout<<"STATS: total number of transformations: "<<totalTransformations<<endl;
}

