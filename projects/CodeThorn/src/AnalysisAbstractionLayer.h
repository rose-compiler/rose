#ifndef ANALYSISABSTRACTIONLAYER_H
#define ANALYSISABSTRACTIONLAYER_H

// Author: Markus Schordan, 2013.

#include "VariableIdMapping.h"

using namespace SPRAY;

namespace AnalysisAbstractionLayer {
  VariableIdSet globalVariables(SgProject* project, VariableIdMapping* variableIdMapping);
  VariableIdSet usedVariablesInsideFunctions(SgProject* project, VariableIdMapping* variableIdMapping);
  VariableIdSet useVariables(SgNode* node, VariableIdMapping& vidm);
  VariableIdSet defVariables(SgNode* node, VariableIdMapping& vidm);
  VariableIdSet astSubTreeVariables(SgNode* node, VariableIdMapping& vidm);
}
#endif
