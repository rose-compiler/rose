// Author: Markus Schordan, 2013.

#include "sage3basic.h"

#include "RDAnalyzer.h"

using namespace std;
using namespace CodeThorn;

#include "CollectionOperators.h"

RDAnalyzer::RDAnalyzer() {
}

RDAnalyzer::iterator RDAnalyzer::begin() {
  return _analyzerData.begin();
}
  
RDAnalyzer::iterator RDAnalyzer::end() {
  return _analyzerData.end();
}
size_t RDAnalyzer::size() {
  return _analyzerData.size();
}

// TODO: refactor in separate functions
RDLattice RDAnalyzer::transfer(Label lab, RDLattice element) {
  if(element.isBot())
    element.setEmptySet();
  SgNode* node=_labeler->getNode(lab);
  //cout<<"Analyzing:"<<node->class_name()<<endl;

  ///////////////////////////////////////////
  // remove undeclared variable at function exit
  if(_labeler->isFunctionExitLabel(lab)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(_labeler->getNode(lab))) {
      // 1) determine all local variables (including formal parameters) of function
      // 2) delete all local variables from state
      // 2a) remove variable from state
      
      // ad 1)
      set<SgVariableDeclaration*> varDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
      // ad 2)
      VariableIdMapping::VariableIdSet localVars=_variableIdMapping.determineVariableIdsOfVariableDeclarations(varDecls);
      SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      VariableIdMapping::VariableIdSet formalParams=_variableIdMapping.determineVariableIdsOfSgInitializedNames(formalParamInitNames);
      VariableIdMapping::VariableIdSet vars=localVars+formalParams;
      for(VariableIdMapping::VariableIdSet::iterator i=vars.begin();i!=vars.end();++i) {
        VariableId varId=*i;
        element.eraseAllPairsWithVariableId(varId);
      }
      return element;
    }
  }
  ///////////////////////////////////////////
  
  if(isSgExprStatement(node))
    node=SgNodeHelper::getExprStmtChild(node);

  if(SgAssignOp* assignOp=isSgAssignOp(node)) {
    transfer_assignment(assignOp,lab,element);
  }
#if 0
  cout << "RDAnalyzer: called transfer function. result: ";
  element.toStream(cout,&_variableIdMapping);
  cout<<endl;
#endif
  return element;
}

void RDAnalyzer::transfer_assignment(SgAssignOp* node, Label& lab, RDLattice& element) {
    // update analysis information
    // this is only correct for RERS12-C programs
    // 1) remove all pairs with lhs-variableid
    // 2) add (lab,lhs.varid)
    
    // (for programs with pointers we require a set here)
#if 1
  // TODO: USEDEF FUNCTIONS HERE (ACTIVATE)
  VariableIdSet lhsVarIds=AnalysisAbstractionLayer::defVariablesInExpression(node,_variableIdMapping);  
#else
    VariableIdSet lhsVarIds=determineLValueVariableIdSet(SgNodeHelper::getLhs(node));
#endif
  if(lhsVarIds.size()>1) {
      // since multiple memory locations may be modified, we cannot know which one will be updated and can only add information
      for(VariableIdMapping::VariableIdSet::iterator i=lhsVarIds.begin();i!=lhsVarIds.end();++i) {
        element.insertPair(lab,*i);
      }
    } else if(lhsVarIds.size()==1) {
      // one unique memory location (variable). We can remove all pairs with this variable
      VariableId var=*lhsVarIds.begin();
      element.eraseAllPairsWithVariableId(var);
      element.insertPair(lab,var);
    }
}

// TODO: USEDEF FUNCTIONS : ELIMINATE THIS FUNCTION
// this function assumes that a pointer to an AST subtree representing a LHS of an assignment has been passed
VariableIdMapping::VariableIdSet RDAnalyzer::determineLValueVariableIdSet(SgNode* node) {
  VariableIdMapping::VariableIdSet resultSet;
  // only x=... is supported yet
  if(SgVarRefExp* lhsVar=isSgVarRefExp(node)) {
    resultSet.insert(_variableIdMapping.variableId(lhsVar));
  } else {
    cout<<"WARNING: unsupported lhs of assignment: "<<SgNodeHelper::nodeToString(node)<<" ... grabbing all variables."<<std::endl;
    RoseAst ast(node);
    for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
      if(SgVarRefExp* lhsVar_i=isSgVarRefExp(node))
        resultSet.insert(_variableIdMapping.variableId(lhsVar_i));
    }
  }
  return resultSet;
}
