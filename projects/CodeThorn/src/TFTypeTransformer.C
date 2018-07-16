#include "sage3basic.h"
#include "TFTypeTransformer.h"
#include "SgNodeHelper.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "CppStdUtilities.h"

using namespace std;

// static member
bool TFTypeTransformer::_traceFlag=false;

void TFTypeTransformer::addToTransformationList(std::list<VarTypeVarNameTuple>& list,SgType* type, SgFunctionDefinition* funDef,string varNames) {
  vector<string> varNamesVector=CppStdUtilities::splitByComma(varNames);
  for (auto name:varNamesVector) {
    TFTypeTransformer::VarTypeVarNameTuple p=std::make_tuple(type,funDef,name);
    list.push_back(p);
  }
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
    SgNode* root=nullptr;
    if(funDef)
      root=funDef;
    else
      root=project;

    int numChanges=changeVariableType(root, varName, newVarType);
    if(numChanges==0) {
      cout<<"Warning: Did not find variable "<<varName;
      if(funDef) {
        cout<<" in function "<<SgNodeHelper::getFunctionName(funDef)<<".";
      } else {
        cout<<" anywhere in file."<<endl;
      }
      cout<<endl;
    } else if(numChanges>1) {
      cout<<"Warning: Found more than one declaration of variable "<<varName<<endl;
    }
    _totalNumChanges+=numChanges;
    transformCommandLineFiles(project);
  }
}

void TFTypeTransformer::transformCastsInCommandLineFiles(SgProject* project) {
  _castTransformer.transformCommandLineFiles(project);
}


int TFTypeTransformer::changeTypeIfInitNameMatches(SgInitializedName* varInitName,
                                               SgNode* root,
                                               string varNameToFind,
                                               SgType* newType) {
  int foundVar=0;
  if(varInitName) {
    SgSymbol* varSym=SgNodeHelper::getSymbolOfInitializedName(varInitName);
    if(varSym) {
      string varName=SgNodeHelper::symbolToString(varSym);
      if(varName==varNameToFind) {
        string funName;
        if(isSgFunctionDefinition(root)) 
          funName=SgNodeHelper::getFunctionName(root);
        trace("Found declaration of variable "+varNameToFind+((funName=="")? "" : " in function "+funName)+". Changed type to "+newType->unparseToString()+".");
        varInitName->set_type(newType);
        foundVar++;
      }
    }
  }
  return foundVar;
}

int TFTypeTransformer::changeVariableType(SgNode* root, string varNameToFind, SgType* newType) {
  RoseAst ast(root);
  bool foundVar=0;
  // need to process formal params explicitly because not found in traversal of functionDef (is traversed from function decl)
  if(SgFunctionDefinition* funDef=isSgFunctionDefinition(root)) {
    SgInitializedNamePtrList& initNamePtrList=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
    for(auto varInitName : initNamePtrList) {
      foundVar+=changeTypeIfInitNameMatches(varInitName,root,varNameToFind,newType);
    }
  }

  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgInitializedName* varInitName=nullptr;
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      varInitName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
    }
    //   else if(SgInitializedName* varInitName0=isSgInitializedName(*i)) {
    //  varInitName=varInitName0;
    //}
    foundVar+=changeTypeIfInitNameMatches(varInitName,root,varNameToFind,newType);
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
  int numTypeBasedReplacements=numTypeReplace;
  int numVarNameBasedReplacements=tt.getTotalNumChanges();
  int arrayReadAccesses=tfTransformation.readTransformations;
  int arrayWriteAccesses=tfTransformation.writeTransformations;
  int arrayOfStructsAccesses=tfTransformation.arrayOfStructsTransformations;
  int adIntermediateTransformations=tfTransformation.adIntermediateTransformations;
  cout<<"STATS: number of variable types changed (based on type-name): "<<numTypeBasedReplacements<<endl;
  cout<<"STATS: number of variable types changed (based on var-name): "<<numVarNameBasedReplacements<<endl;
  cout<<"STATS: number of transformed array read accesses: "<<arrayReadAccesses<<endl;
  cout<<"STATS: number of transformed array write accesses: "<<arrayWriteAccesses<<endl;
  cout<<"STATS: number of transformed arrays of structs accesses: "<<arrayOfStructsAccesses<<endl;
  cout<<"STATS: number of ad_intermediate transformations: "<<adIntermediateTransformations<<endl;
  int totalTransformations=numTypeBasedReplacements+numVarNameBasedReplacements+arrayReadAccesses+arrayWriteAccesses+arrayOfStructsAccesses+adIntermediateTransformations;
  cout<<"STATS: total number of transformations: "<<totalTransformations<<endl;
}

