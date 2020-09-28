// Author: Markus Schordan, 2013.

#include "sage3basic.h"
#include "AstUtility.h"

#include "addressTakenAnalysis.h"
#include "defUseQuery.h"
#include "Miscellaneous2.h"
#include "CodeThornException.h"
#include <set>
#include <list>
#include "ReachabilityAnalysis.h"

using namespace CodeThorn;
using namespace AstUtility;
using namespace std;

CodeThorn::VariableIdSet
AstUtility::globalVariables(SgProject* project, VariableIdMapping* variableIdMapping) {
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
  CodeThorn::VariableIdMapping::VariableIdSet globalVarsIdSet;
  for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
    VariableId globalVarId=variableIdMapping->variableId(*i);
    globalVarsIdSet.insert(globalVarId);
  }
  return globalVarsIdSet;
}

CodeThorn::VariableIdSet
AstUtility::usedVariablesInGlobalVariableInitializers(SgProject* project, VariableIdMapping* variableIdMapping) {
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
  CodeThorn::VariableIdMapping::VariableIdSet usedVarsInInitializersIdSet;
  for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
    SgExpression* initExp=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(*i);
    CodeThorn::VariableIdSet usedVarsInInitializer;
    usedVarsInInitializer=AstUtility::astSubTreeVariables(initExp, *variableIdMapping);
    usedVarsInInitializersIdSet.insert(usedVarsInInitializer.begin(),usedVarsInInitializer.end());
  }
  return usedVarsInInitializersIdSet;
}


CodeThorn::VariableIdSet 
AstUtility::usedVariablesInsideFunctions(SgProject* project, VariableIdMapping* variableIdMapping) {
  list<SgVarRefExp*> varRefExpList=SgNodeHelper::listOfUsedVarsInFunctions(project);
  //cout<<"DEBUG: varRefExpList-size:"<<varRefExpList.size()<<endl;
  CodeThorn::VariableIdSet setOfUsedVars;
  for(list<SgVarRefExp*>::iterator i=varRefExpList.begin();i!=varRefExpList.end();++i) {
    //cout<<"DEBUG: checking variable "<<(*i)->unparseToString();
    VariableId id = variableIdMapping->variableId(*i);
    if(!id.isValid()) {
      ostringstream exceptionMsg;
      exceptionMsg << "Error: AstUtility::usedVariablesInsideFunctions: Invalid variable id for SgVarRefExp "
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

VariableIdSet AstUtility::useVariables(SgNode* node, VariableIdMapping& vidm,
                                                     /*const*/ PointerAnalysisInterface* _pointerAnalysisInterface) {
  VariableIdSet resultSet;
  VarsInfo useVarsInfo=getDefUseVarsInfo(node, vidm).getUseVarsInfo();
  extractVariableIdSetFromVarsInfo(resultSet,useVarsInfo);

  // schroder3 (2016-08-16): Add the variables from which the address was taken if there is a use through e.g. a pointer.
  expandVarIdSetByAddressTakenVarsIfNecessary(resultSet, useVarsInfo, _pointerAnalysisInterface, vidm);

  return resultSet;
}

VariableIdSet AstUtility::defVariables(SgNode* node, VariableIdMapping& vidm,
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

CodeThorn::VariableIdSet AstUtility::astSubTreeVariables(SgNode* node, VariableIdMapping& vidm) {
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

bool AstUtility::isExprRoot(SgNode* node) {
  if(SgExpression* exp=isSgExpression(node)) {
    return isSgStatement(exp->get_parent());
  }
  return false;
}

std::list<SgExpression*> AstUtility::exprRootList(SgNode *node) {
  RoseAst ast(node);
  list<SgExpression*> exprList;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(isExprRoot(*i)) {
      SgExpression* expr=isSgExpression(*i);
      ROSE_ASSERT(expr);
      exprList.push_back(expr);
      i.skipChildrenOnForward();
    }
  }
  return exprList;
}

std::set<AbstractValue> AstUtility::determineSetOfCompoundIncVars(VariableIdMapping* vim, SgNode* astRoot) {
  ROSE_ASSERT(vim);
  ROSE_ASSERT(astRoot);
  RoseAst ast(astRoot) ;
  set<AbstractValue> compoundIncVarsSet;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgCompoundAssignOp* compoundAssignOp=isSgCompoundAssignOp(*i)) {
      SgVarRefExp* lhsVar=isSgVarRefExp(SgNodeHelper::getLhs(compoundAssignOp));
      if(lhsVar) {
        compoundIncVarsSet.insert(vim->variableId(lhsVar));
      }
    }
  }
  return compoundIncVarsSet;
}

std::set<CodeThorn::VariableId> AstUtility::determineSetOfConstAssignVars2(VariableIdMapping* vim, SgNode* astRoot) {
  ROSE_ASSERT(vim);
  ROSE_ASSERT(astRoot);
  RoseAst ast(astRoot) ;
  set<VariableId> constAssignVars;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgAssignOp* assignOp=isSgAssignOp(*i)) {
      SgVarRefExp* lhsVar=isSgVarRefExp(SgNodeHelper::getLhs(assignOp));
      SgIntVal* rhsIntVal=isSgIntVal(SgNodeHelper::getRhs(assignOp));
      if(lhsVar && rhsIntVal) {
        constAssignVars.insert(vim->variableId(lhsVar));
      }
    }
  }
  return constAssignVars;
}

AbstractValueSet AstUtility::determineVarsInAssertConditions(SgNode* node, VariableIdMapping* variableIdMapping) {
  AbstractValueSet usedVarsInAssertConditions;
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgIfStmt* ifstmt=isSgIfStmt(*i)) {
      SgNode* cond=SgNodeHelper::getCond(ifstmt);
      if(cond) {
        int errorLabelCode=-1;
        errorLabelCode=ReachabilityAnalysis::isConditionOfIfWithLabeledAssert(cond);
        if(errorLabelCode>=0) {
          //cout<<"Assertion cond: "<<cond->unparseToString()<<endl;
          //cout<<"Stmt: "<<ifstmt->unparseToString()<<endl;
          std::vector<SgVarRefExp*> vars=SgNodeHelper::determineVariablesInSubtree(cond);
          //cout<<"Num of vars: "<<vars.size()<<endl;
          for(std::vector<SgVarRefExp*>::iterator j=vars.begin();j!=vars.end();++j) {
            VariableId varId=variableIdMapping->variableId(*j);
            usedVarsInAssertConditions.insert(AbstractValue(varId));
          }
        }
      }
    }
  }
  return usedVarsInAssertConditions;
}
// utility function
SgNode* AstUtility::findExprNodeInAstUpwards(VariantT variant,SgNode* node) {
  while(node!=nullptr&&isSgExpression(node)&&(node->variantT()!=variant)) {
    node=node->get_parent();
  }
  if(node)
    // if the search did not find the node and continued to the stmt level
    // this check ensures that a nullptr is returned
    return isSgExpression(node);
  else
    return nullptr;
}
