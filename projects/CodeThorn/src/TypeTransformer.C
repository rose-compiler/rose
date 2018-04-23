#include "sage3basic.h"
#include "TypeTransformer.h"
#include "SgNodeHelper.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "CppStdUtilities.h"

using namespace std;

// static member
bool TypeTransformer::_traceFlag=false;

void TypeTransformer::addToTransformationList(std::list<VarTypeVarNameTuple>& list,SgType* type, SgFunctionDefinition* funDef,string varNames) {
  vector<string> varNamesVector=CppStdUtilities::splitByComma(varNames);
  for (auto name:varNamesVector) {
    TypeTransformer::VarTypeVarNameTuple p=std::make_tuple(type,funDef,name);
    list.push_back(p);
  }
}

void TypeTransformer::transformCommandLineFiles(SgProject* project) {
  // make all floating point casts explicit
  makeAllCastsExplicit(project);
  // transform casts in AST
  transformCastsInCommandLineFiles(project);
}

void TypeTransformer::transformCommandLineFiles(SgProject* project,VarTypeVarNameTupleList& list) {
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
      cout<<"Warning: Found more than one instance of variable "<<varName<<endl;
    }
    transformCommandLineFiles(project);
  }
}

void TypeTransformer::transformCastsInCommandLineFiles(SgProject* project) {
  _castTransformer.transformCommandLineFiles(project);
}

int TypeTransformer::changeVariableType(SgNode* root, string varNameToFind, SgType* type) {
  RoseAst ast(root);
  bool foundVar=0;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      SgInitializedName* varInitName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
      if(varInitName) {
	SgSymbol* varSym=SgNodeHelper::getSymbolOfInitializedName(varInitName);
	if(varSym) {
	  string varName=SgNodeHelper::symbolToString(varSym);
	  if(varName==varNameToFind) {
            string funName;
            if(isSgFunctionDefinition(root)) 
              funName=SgNodeHelper::getFunctionName(root);
	    trace("Found declaration of variable "+varNameToFind+((funName=="")? "" : " in function "+funName)+". Changed type to "+type->unparseToString()+".");
	    SgTypeFloat* ft=SageBuilder::buildFloatType();
	    varInitName->set_type(ft);
            foundVar++;
	  }
	}
      }
    }
  }
  return foundVar;
}

void TypeTransformer::makeAllCastsExplicit(SgProject* root) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgCastExp* castExp=isSgCastExp(*i)) {
      if(castExp->isCompilerGenerated()) {
	castExp->unsetCompilerGenerated();
      }
    }
  }
}

void TypeTransformer::annotateImplicitCastsAsComments(SgProject* root) {
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

void TypeTransformer::setTraceFlag(bool traceFlag) {
  _traceFlag=traceFlag;
}

void TypeTransformer::trace(string s) {
  if(TypeTransformer::_traceFlag) {
    cout<<"TRACE: "<<s<<endl;
  }
}
