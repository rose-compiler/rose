#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalyzer.h"
#include "WorkList.h"

using namespace std;
using namespace CodeThorn;

#include "WorkList.C"
#include "DFAnalyzer.C"

class RDAnalyzer : public DFAnalyzer<RDLattice> {
public:
  RDAnalyzer():DFAnalyzer<RDLattice>(){}
  RDLattice transfer(Label l, RDLattice element) {
	cout << "RDAnalyzer: called transfer function."<<endl;
	return element;
  }
};

set<SgVariableDeclaration*> determineGlobalVarDeclarations(SgProject* root) {
  set<SgVariableDeclaration*> globVars;
  if(SgProject* project=isSgProject(root)) {
	cout << "STATUS: Number of global variables: ";
	list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
	cout << globalVars.size()<<endl;
	
	list<SgVarRefExp*> varRefExpList=SgNodeHelper::listOfUsedVarsInFunctions(project);
  } else {
	cout << "INIT: no global scope.";
  }	
  return globVars;
}

SgNode* determineStartFunction(string functionToStartAt, SgNode* root) {
  std::string funtofind=functionToStartAt;
  RoseAst completeast(root);
  SgNode* startFunRoot=completeast.findFunctionByName(funtofind);
  if(startFunRoot==0) { 
    std::cerr << "Function '"<<funtofind<<"' not found.\n"; exit(1);
  }
  return startFunRoot;
}

int main(int argc, char* argv[]) {
  cout << "INIT: Parsing and creating AST."<<endl;
  SgProject* root = frontend(argc,argv);
  RDAnalyzer rdAnalyzer;
  rdAnalyzer.initialize(root);
  set<SgVariableDeclaration*> globalVarDecls=determineGlobalVarDeclarations(root);
  SgNode* startFunRoot=determineStartFunction("main",root);
  rdAnalyzer.determineExtremalLabels(startFunRoot);
  rdAnalyzer.run();
  return 0;
}
