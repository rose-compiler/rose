// Author: Markus Schordan, 2013.

#include "sage3basic.h"
#include "AnalysisAbstractionLayer.h"

#include "addressTakenAnalysis.h"
#include "defUseQuery.h"
#include "Miscellaneous2.h"
#include "CodeThornException.h"

using namespace CodeThorn;
using namespace AnalysisAbstractionLayer;
using namespace std;

CodeThorn::VariableIdSet
AnalysisAbstractionLayer::globalVariables(SgProject* project, VariableIdMapping* variableIdMapping) {
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
  CodeThorn::VariableIdMapping::VariableIdSet globalVarsIdSet;
  for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
    VariableId globalVarId=variableIdMapping->variableId(*i);
    globalVarsIdSet.insert(globalVarId);
  }
  return globalVarsIdSet;
}

CodeThorn::VariableIdSet
AnalysisAbstractionLayer::usedVariablesInGlobalVariableInitializers(SgProject* project, VariableIdMapping* variableIdMapping) {
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
  CodeThorn::VariableIdMapping::VariableIdSet usedVarsInInitializersIdSet;
  for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
    SgExpression* initExp=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(*i);
    CodeThorn::VariableIdSet usedVarsInInitializer;
    usedVarsInInitializer=AnalysisAbstractionLayer::astSubTreeVariables(initExp, *variableIdMapping);
    usedVarsInInitializersIdSet.insert(usedVarsInInitializer.begin(),usedVarsInInitializer.end());
  }
  return usedVarsInInitializersIdSet;
}


CodeThorn::VariableIdSet 
AnalysisAbstractionLayer::usedVariablesInsideFunctions(SgProject* project, VariableIdMapping* variableIdMapping) {
  list<SgVarRefExp*> varRefExpList=SgNodeHelper::listOfUsedVarsInFunctions(project);
  //cout<<"DEBUG: varRefExpList-size:"<<varRefExpList.size()<<endl;
  CodeThorn::VariableIdSet setOfUsedVars;
  for(list<SgVarRefExp*>::iterator i=varRefExpList.begin();i!=varRefExpList.end();++i) {
    //cout<<"DEBUG: checking variable "<<(*i)->unparseToString();
    VariableId id = variableIdMapping->variableId(*i);
    if(!id.isValid()) {
      ostringstream exceptionMsg;
      exceptionMsg << "Error: AnalysisAbstractionLayer::usedVariablesInsideFunctions: Invalid variable id for SgVarRefExp "
                   << (*i)->unparseToString() << ", Symbol: " << (*i)->get_symbol() << endl;
      cerr<<exceptionMsg.str();
      //exit(1);
      //throw CodeThorn::Exception(exceptionMsg.str());
    }
    setOfUsedVars.insert(id);
  }
  return setOfUsedVars;
}

// TODO: this function ignores all reported memory access to unnamed memory cells
void extractVariableIdSetFromVarsInfo(CodeThorn::VariableIdSet& varIdSet, VarsInfo& varsInfo) {
    VariableIdInfoMap& vim=varsInfo.first;
    for(VariableIdInfoMap::iterator i=vim.begin();i!=vim.end();++i) {
      varIdSet.insert((*i).first);
    }
}

// schroder3 (2016-08-16): Helper function that extends the given varIdSet by all address-taken variables if the
//  flag of the VarsInfo set is set or if the varIdSet contains an address-taken variable.
void expandVarIdSetByAddressTakenVarsIfNecessary(VariableIdSet& varIdSet, const VarsInfo& defdRespUsedVars,
                                                 PointerAnalysisInterface* addressTakenAnalysis, VariableIdMapping& vidm) {
  VariableIdSet addressTakenVarIds = addressTakenAnalysis ?
                                     addressTakenAnalysis->getModByPointer() :
                                     PointerAnalysisEmptyImplementation(&vidm).getModByPointer();
  if(defdRespUsedVars.second || setIntersect(varIdSet, addressTakenVarIds).size() > 0) {
    // union sets
    varIdSet += addressTakenVarIds;
  }
}

VariableIdSet AnalysisAbstractionLayer::useVariables(SgNode* node, VariableIdMapping& vidm,
                                                     /*const*/ PointerAnalysisInterface* _pointerAnalysisInterface) {
  VariableIdSet resultSet;
  VarsInfo useVarsInfo=getDefUseVarsInfo(node, vidm).getUseVarsInfo();
  extractVariableIdSetFromVarsInfo(resultSet,useVarsInfo);

  // schroder3 (2016-08-16): Add the variables from which the address was taken if there is a use through e.g. a pointer.
  expandVarIdSetByAddressTakenVarsIfNecessary(resultSet, useVarsInfo, _pointerAnalysisInterface, vidm);

  return resultSet;
}

VariableIdSet AnalysisAbstractionLayer::defVariables(SgNode* node, VariableIdMapping& vidm,
                                                     /*const*/ PointerAnalysisInterface* _pointerAnalysisInterface) {
  VariableIdSet resultSet;
  VarsInfo defVarsInfo=getDefUseVarsInfo(node, vidm).getDefVarsInfo();
  //cout<<"DEFISEVARSINFO: "<<DefUseVarsInfo::varsInfoPrettyPrint(defVarsInfo,vidm)<<endl;
  //cout<<"VariableIdInfoMap-size:"<<defVarsInfo.first.size()<<endl;
  extractVariableIdSetFromVarsInfo(resultSet,defVarsInfo);
  ROSE_ASSERT(defVarsInfo.first.size()==resultSet.size());
  //ROSE_ASSERT(defVarsInfo.first.size()<=1);

  // schroder3 (2016-08-16): Add the variables from which the address was taken if there is a definition/ assignment
  //  through e.g. a pointer.
  expandVarIdSetByAddressTakenVarsIfNecessary(resultSet, defVarsInfo, _pointerAnalysisInterface, vidm);

  return resultSet;
}

CodeThorn::VariableIdSet AnalysisAbstractionLayer::astSubTreeVariables(SgNode* node, VariableIdMapping& vidm) {
  CodeThorn::VariableIdSet vset;
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

