#include "sage3basic.h"
#include "RDAnalysisAstAttribute.h"
RDAnalysisAstAttribute::RDAnalysisAstAttribute(RDLattice* elem):_elem(elem) {
}
bool RDAnalysisAstAttribute::isBottomElement() {
  return _elem->isBot();
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
RDAnalysisAstAttribute::iterator
RDAnalysisAstAttribute::begin() {
  return _elem->begin();
}
RDAnalysisAstAttribute::iterator
RDAnalysisAstAttribute::end() {
  return _elem->end();
}
RDAnalysisAstAttribute::~RDAnalysisAstAttribute() {
}
