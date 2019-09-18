#ifndef ANALYSISABSTRACTIONLAYER_H
#define ANALYSISABSTRACTIONLAYER_H

// Author: Markus Schordan, 2013.

#include "VariableIdMapping.h"
#include "PointerAnalysisInterface.h"

namespace AnalysisAbstractionLayer {
  CodeThorn::VariableIdSet globalVariables(SgProject* project, CodeThorn::VariableIdMapping* variableIdMapping);
  CodeThorn::VariableIdSet usedVariablesInGlobalVariableInitializers(SgProject* project, CodeThorn::VariableIdMapping* variableIdMapping);  
  CodeThorn::VariableIdSet usedVariablesInsideFunctions(SgProject* project, CodeThorn::VariableIdMapping* variableIdMapping);
  CodeThorn::VariableIdSet astSubTreeVariables(SgNode* node, CodeThorn::VariableIdMapping& vidm);
  CodeThorn::VariableIdSet useVariables(SgNode* node, CodeThorn::VariableIdMapping& vidm, /*const*/ CodeThorn::PointerAnalysisInterface* _pointerAnalysisInterface = 0);
  CodeThorn::VariableIdSet defVariables(SgNode* node, CodeThorn::VariableIdMapping& vidm, /*const*/ CodeThorn::PointerAnalysisInterface* _pointerAnalysisInterface = 0);
}
#endif
