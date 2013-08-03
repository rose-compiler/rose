#ifndef ASTVARIABLEIDINTERFACE_H
#define ASTVARIABLEIDINTERFACE_H

// Author: Markus Schordan, 2013.

#include "VariableIdMapping.h"

using namespace CodeThorn;

namespace AstVariableIdInterface {
  VariableIdSet globalVariables(SgProject* project, VariableIdMapping* variableIdMapping);
  VariableIdSet usedVariablesInsideFunctions(SgProject* project, VariableIdMapping* variableIdMapping);
}
#endif
