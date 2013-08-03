// Author: Markus Schordan, 2013.

#include "sage3basic.h"
#include "AstVariableIdInterface.h"

VariableIdMapping::VariableIdSet
AstVariableIdInterface::globalVariables(SgProject* project, VariableIdMapping* variableIdMapping) {
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
  VariableIdMapping::VariableIdSet globalVarsIdSet;
  for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
	VariableId globalVarId=variableIdMapping->variableId(*i);
	globalVarsIdSet.insert(globalVarId);
  }
  return globalVarsIdSet;
}

VariableIdMapping::VariableIdSet 
AstVariableIdInterface::usedVariablesInsideFunctions(SgProject* project, VariableIdMapping* variableIdMapping) {
  list<SgVarRefExp*> varRefExpList=SgNodeHelper::listOfUsedVarsInFunctions(project);
  VariableIdMapping::VariableIdSet setOfUsedVars;
  for(list<SgVarRefExp*>::iterator i=varRefExpList.begin();i!=varRefExpList.end();++i) {
	setOfUsedVars.insert(variableIdMapping->variableId(*i));
  }
  return setOfUsedVars;
}
