// Author: Markus Schordan, 2013.

#include "RDAnalyzer.h"

using namespace std;
using namespace CodeThorn;

RDAnalyzer::RDAnalyzer() {
}

RDLattice RDAnalyzer::transfer(Label lab, RDLattice element) {
  SgNode* node=_labeler->getNode(lab);
  cout<<"Analyzing:"<<node->class_name()<<endl;
  if(isSgExprStatement(node))
	node=SgNodeHelper::getExprStmtChild(node);

  if(SgAssignOp* assignOp=isSgAssignOp(node)) {
	transfer_assignment(assignOp,lab,element);
  }

  cout << "RDAnalyzer: called transfer function. result: ";
  element.toStream(cout,&_variableIdMapping);
  cout<<endl;
  
  return element;
}

void RDAnalyzer::transfer_assignment(SgAssignOp* node, Label& lab, RDLattice& element) {
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

// this function assumes that a pointer to an AST subtree representing a LHS of an assignment has been passed
set<VariableId> RDAnalyzer::determineLValueVariableIdSet(SgNode* node) {
  set<VariableId> resultSet;
  // only x=... is supported yet
  if(SgVarRefExp* lhsVar=isSgVarRefExp(node)) {
	resultSet.insert(_variableIdMapping.variableId(lhsVar));
  } else {
	// TODO
	cout<<"WARNING: unsupported lhs of assignment: "<<SgNodeHelper::nodeToString(node)<<" ... grabbing all variables."<<std::endl;
	RoseAst ast(node);
	for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
	  if(SgVarRefExp* lhsVar_i=isSgVarRefExp(node))
		resultSet.insert(_variableIdMapping.variableId(lhsVar_i));
	}
  }
  return resultSet;
}
