// Author: Markus Schordan, 2013.

#include "sage3basic.h"
#include "AnalysisAbstractionLayer.h"

#include "addressTakenAnalysis.h"
#include "defUseQuery.h"

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
  //cout<<"DEBUG: varRefExpList-size:"<<varRefExpList.size()<<endl;
  VariableIdSet setOfUsedVars;
  for(list<SgVarRefExp*>::iterator i=varRefExpList.begin();i!=varRefExpList.end();++i) {
    setOfUsedVars.insert(variableIdMapping->variableId(*i));
  }
  return setOfUsedVars;
}

// TODO: this function ignores all reported memory access to unnamed memory cells
void extractVariableIdSetFromVarsInfo(VariableIdSet& varIdSet, VarsInfo& varsInfo) {
    VariableIdInfoMap& vim=varsInfo.first;
    for(VariableIdInfoMap::iterator i=vim.begin();i!=vim.end();++i) {
      varIdSet.insert((*i).first);
    }
}

VariableIdSet AnalysisAbstractionLayer::useVariables(SgNode* node, VariableIdMapping& vidm) {
  VariableIdSet resultSet;
  VarsInfo useVarsInfo=getDefUseVarsInfo(node, vidm).getUseVarsInfo();
  extractVariableIdSetFromVarsInfo(resultSet,useVarsInfo);
  return resultSet;
}

VariableIdSet AnalysisAbstractionLayer::defVariables(SgNode* node, VariableIdMapping& vidm) {
  VariableIdSet resultSet;
  VarsInfo defVarsInfo=getDefUseVarsInfo(node, vidm).getDefVarsInfo();
  //cout<<"DEFISEVARSINFO: "<<DefUseVarsInfo::varsInfoPrettyPrint(defVarsInfo,vidm)<<endl;
  //cout<<"VariableIdInfoMap-size:"<<defVarsInfo.first.size()<<endl;
  extractVariableIdSetFromVarsInfo(resultSet,defVarsInfo);
  ROSE_ASSERT(defVarsInfo.first.size()==resultSet.size());
  //ROSE_ASSERT(defVarsInfo.first.size()<=1);
  return resultSet;
}

VariableIdSet AnalysisAbstractionLayer::astSubTreeVariables(SgNode* node, VariableIdMapping& vidm) {
  VariableIdSet vset;
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    VariableId vid; // default creates intentionally an invalid id.
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      vid=vidm.variableId(varDecl);
    } else if(SgVarRefExp* varRefExp=isSgVarRefExp(*i)) {
      vid=vidm.variableId(varRefExp);
    } else if(SgInitializedName* initName=isSgInitializedName(*i)) {
      vid=vidm.variableId(initName);
    }
    if(vid.isValid())
      vset.insert(vid);
  }
  return vset;
}
