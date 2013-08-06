// Author: Markus Schordan, 2013.

#include "sage3basic.h"
#include "AnalysisAbstractionLayer.h"

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

VariableIdSet AnalysisAbstractionLayer::useVariablesInExpression(SgNode* node) {
  VariableIdSet resultSet;
  // TODO: USEDEF FUNCTIONS HERE (USE)
  return resultSet;
}

VariableIdSet AnalysisAbstractionLayer::defVariablesInExpression(SgNode* node) {
  VariableIdSet resultSet;
  // TODO: USEDEF FUNCTIONS HERE (DEF)
  return resultSet;
}

