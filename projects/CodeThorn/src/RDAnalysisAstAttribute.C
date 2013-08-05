#include "sage3basic.h"
#include "RDAnalysisAstAttribute.h"

bool RDAnalysisAstAttribute::isBottomElement() {
  return false;
}
bool RDAnalysisAstAttribute::isTopElement() {
  return false;
}
VariableIdSet RDAnalysisAstAttribute::allVariableIds() {
  VariableIdSet set;
  return set;
}
LabelSet RDAnalysisAstAttribute::allLabels() {
  LabelSet ls;
  return ls;
  }
LabelSet RDAnalysisAstAttribute::definitionsOfVariableId(VariableId varId) {
  LabelSet ls;
	return ls;
}
VariableIdSet RDAnalysisAstAttribute::variableIdsOfDefinition(Label def) {
  VariableIdSet set;
  return set;
}

