#include "sage3basic.h"
#include "TypeTransformer.h"
#include "SgNodeHelper.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "CppStdUtilities.h"

using namespace std;

void TypeTransformer::addToTransformationList(std::list<VarTypeVarNamePair>& list,SgType* type, string varNames) {
  vector<string> varNamesVector=CppStdUtilities::splitByComma(varNames);
  for (auto name:varNamesVector) {
    TypeTransformer::VarTypeVarNamePair p=std::make_pair(type,name);
    list.push_back(p);
  }
}

void TypeTransformer::transformCommandLineFiles(SgProject* project) {
  // make all floating point casts explicit
  makeAllCastsExplicit(project);
  // transform casts in AST
  transformCastsInCommandLineFiles(project);
}

void TypeTransformer::transformCommandLineFiles(SgProject* project,VarTypeVarNamePairList& list) {
  for (auto typeNamePair:list) {
    SgType* newVarType=typeNamePair.first;
    string varName=typeNamePair.second;
    changeVariableType(project, varName, newVarType);
    transformCommandLineFiles(project);
  }
}

void TypeTransformer::transformCastsInCommandLineFiles(SgProject* project) {
  _castTransformer.transformCommandLineFiles(project);
}

void TypeTransformer::changeVariableType(SgProject* root, string varNameToFind, SgType* type) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      SgInitializedName* varInitName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
      if(varInitName) {
	SgSymbol* varSym=SgNodeHelper::getSymbolOfInitializedName(varInitName);
	if(varSym) {
	  string varName=SgNodeHelper::symbolToString(varSym);
	  if(varName==varNameToFind) {
	    cout<<"STATUS: found declaration of var "<<varNameToFind<<". Changed type to "<<type->unparseToString()<<"."<<endl;
	    SgTypeFloat* ft=SageBuilder::buildFloatType();
	    varInitName->set_type(ft);
	  }
	}
      }
    }
  }
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
