// Author: Markus Schordan, 2013.

#include "sage3basic.h"
#include "AnalysisAbstractionLayer.h"

#include "addressTakenAnalysis/addressTakenAnalysis.h"
#include "addressTakenAnalysis/defUseQuery.h"

VariableIdSet
AnalysisAbstractionLayer::globalVariables(SgProject* project, VariableIdMapping* variableIdMapping) {
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
  VariableIdMapping::VariableIdSet globalVarsIdSet;
  for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
	VariableId globalVarId=variableIdMapping->variableId(*i);
	globalVarsIdSet.insert(globalVarId);
  }
  return globalVarsIdSet;
}

VariableIdSet 
AnalysisAbstractionLayer::usedVariablesInsideFunctions(SgProject* project, VariableIdMapping* variableIdMapping) {
  list<SgVarRefExp*> varRefExpList=SgNodeHelper::listOfUsedVarsInFunctions(project);
  VariableIdSet setOfUsedVars;
  for(list<SgVarRefExp*>::iterator i=varRefExpList.begin();i!=varRefExpList.end();++i) {
	setOfUsedVars.insert(variableIdMapping->variableId(*i));
  }
  return setOfUsedVars;
}

// TODO: this function ignores all reported memory access to unnamed memory cells
void extractVariableIdSetFromVarsInfo(VariableIdSet& varIdSet, VarsInfo& varsInfo) {
	VariableIdInfoMap& vim=varsInfo.first;
	cout<<"VariableIdInfoMap-size:"<<vim.size()<<endl;
	for(VariableIdInfoMap::iterator i=vim.begin();i!=vim.end();++i) {
	  varIdSet.insert((*i).first);
	}
}

VariableIdSet AnalysisAbstractionLayer::useVariablesInExpression(SgNode* node, VariableIdMapping& vidm) {
  VariableIdSet resultSet;
  VarsInfo useVarsInfo=getDefUseVarsInfo(node, vidm).getUseVarsInfo();
  extractVariableIdSetFromVarsInfo(resultSet,useVarsInfo);
  return resultSet;
}

VariableIdSet AnalysisAbstractionLayer::defVariablesInExpression(SgNode* node, VariableIdMapping& vidm) {
  VariableIdSet resultSet;
  VarsInfo defVarsInfo=getDefUseVarsInfo(node, vidm).getDefVarsInfo();
  extractVariableIdSetFromVarsInfo(resultSet,defVarsInfo);
  return resultSet;
}

