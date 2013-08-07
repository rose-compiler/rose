#ifndef ANALYSISABSTRACTIONLAYER_H
#define ANALYSISABSTRACTIONLAYER_H

// Author: Markus Schordan, 2013.

#include "VariableIdMapping.h"

using namespace CodeThorn;

namespace AnalysisAbstractionLayer {
  VariableIdSet globalVariables(SgProject* project, VariableIdMapping* variableIdMapping);
  VariableIdSet usedVariablesInsideFunctions(SgProject* project, VariableIdMapping* variableIdMapping);
  VariableIdSet useVariablesInExpression(SgNode* expr, VariableIdMapping& vidm);
  VariableIdSet defVariablesInExpression(SgNode* expr, VariableIdMapping& vidm);
}
#endif
