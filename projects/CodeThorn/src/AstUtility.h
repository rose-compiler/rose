#ifndef ANALYSISABSTRACTIONLAYER_H
#define ANALYSISABSTRACTIONLAYER_H

// Author: Markus Schordan, 2013.

#include "VariableIdMapping.h"
#include "PointerAnalysisInterface.h"
#include "AbstractValue.h"
#include "CodeThornOptions.h"

namespace AstUtility {
  CodeThorn::VariableIdSet globalVariables(SgProject* project, CodeThorn::VariableIdMapping* variableIdMapping);
  CodeThorn::VariableIdSet usedVariablesInGlobalVariableInitializers(SgProject* project, CodeThorn::VariableIdMapping* variableIdMapping);  
  CodeThorn::VariableIdSet usedVariablesInsideFunctions(SgProject* project, CodeThorn::VariableIdMapping* variableIdMapping);
  CodeThorn::VariableIdSet astSubTreeVariables(SgNode* node, CodeThorn::VariableIdMapping& vidm);
  CodeThorn::VariableIdSet useVariables(SgNode* node, CodeThorn::VariableIdMapping& vidm, /*const*/ CodeThorn::PointerAnalysisInterface* _pointerAnalysisInterface = 0);
  CodeThorn::VariableIdSet defVariables(SgNode* node, CodeThorn::VariableIdMapping& vidm, /*const*/ CodeThorn::PointerAnalysisInterface* _pointerAnalysisInterface = 0);

  bool isExprRoot(SgNode* node);
  std::list<SgExpression*> exprRootList(SgNode *node);
  std::set<CodeThorn::AbstractValue> determineSetOfCompoundIncVars(CodeThorn::VariableIdMapping* vim, SgNode* astRoot);
  std::set<CodeThorn::VariableId> determineSetOfConstAssignVars2(CodeThorn::VariableIdMapping* vim, SgNode* astRoot);
  CodeThorn::AbstractValueSet determineVarsInAssertConditions(SgNode* node, CodeThorn::VariableIdMapping* variableIdMapping);

  void exprEvalTest(int argc, char* argv[],CodeThornOptions& ctOpt);
}
#endif
