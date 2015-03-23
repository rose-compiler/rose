#ifndef ANALYSISABSTRACTIONLAYER_H
#define ANALYSISABSTRACTIONLAYER_H

// Author: Markus Schordan, 2013.

#include "VariableIdMapping.h"

namespace AnalysisAbstractionLayer {
  SPRAY::VariableIdSet globalVariables(SgProject* project, SPRAY::VariableIdMapping* variableIdMapping);
  SPRAY::VariableIdSet usedVariablesInGlobalVariableInitializers(SgProject* project, SPRAY::VariableIdMapping* variableIdMapping);  
  SPRAY::VariableIdSet usedVariablesInsideFunctions(SgProject* project, SPRAY::VariableIdMapping* variableIdMapping);
  SPRAY::VariableIdSet astSubTreeVariables(SgNode* node, SPRAY::VariableIdMapping& vidm);
  SPRAY::VariableIdSet useVariables(SgNode* node, SPRAY::VariableIdMapping& vidm);
  SPRAY::VariableIdSet defVariables(SgNode* node, SPRAY::VariableIdMapping& vidm);
}
#endif
