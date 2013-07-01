// Author: Markus Schordan, 2013.

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
  RDLattice transfer(Label lab, RDLattice element) {
	SgNode* node=_labeler->getNode(lab);
	cout<<"Analyzing:"<<node->class_name()<<endl;
	if(isSgExprStatement(node))
	  node=SgNodeHelper::getExprStmtChild(node);
	if(isSgAssignOp(node)) {
	  // update analysis information
	  // this is only correct for RERS12-C programs
	  // 1) remove all pairs with lhs-variableid
	  // 2) add (lab,lhs.varid)
	  
	  // (for programs with pointers we require a set here)
	  set<VariableId> lhsVarIds=determineLValueVariableIdSet(SgNodeHelper::getLhs(node));
	  if(lhsVarIds.size()>1) {
		// since multiple memory locations may be modified, we cannot know which one will be updated and can only add information
		for(set<VariableId>::iterator i=lhsVarIds.begin();i!=lhsVarIds.end();++i) {
		  element.insertPair(lab,*i);
		}
	  } else if(lhsVarIds.size()==1) {
		// one unique memory location (variable). We can remove all pairs with this variable
		VariableId var=*lhsVarIds.begin();
		element.eraseAllPairsWithVariableId(var);
		element.insertPair(lab,var);
	  }

	}
	cout << "RDAnalyzer: called transfer function. result: ";
	element.toStream(cout,&_variableIdMapping);
	cout<<endl;

	return element;
  }
  // this function assumes that a pointer to an AST subtree representing a LHS of an assignment has been passed
  set<VariableId> determineLValueVariableIdSet(SgNode* node) {
	set<VariableId> resultSet;
	// only x=... is supported yet
	cout<<"LHS: "<<node->class_name()<<endl;
	if(SgVarRefExp* lhsVar=isSgVarRefExp(node)) {
	  resultSet.insert(_variableIdMapping.variableId(lhsVar));
	} else {
	  // TODO
	  cout<<"WARNING: unsupported lhs of assignment: "<<SgNodeHelper::nodeToString(node)<<std::endl;
	}
	return resultSet;
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
  SgNode* startFunRoot=0; //determineStartFunction("main",root);
  rdAnalyzer.determineExtremalLabels(startFunRoot);
  rdAnalyzer.run();
  return 0;
}
