// Author: Markus Schordan, 2013.

#ifndef RDANALYSISASTATTRIBUTE_H
#define RDANALYSISASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"

using namespace CodeThorn;

class RDAnalysisAstAttribute : public AstAttribute {
 public:
  typedef pair<Label,VariableId> RDPair;
  typedef set<RDPair> RDPairSet;
  bool isBottomElement();
  bool isTopElement();
  VariableIdSet allVariableIds();
  LabelSet allLabels();
  LabelSet definitionsOfVariableId(VariableId varId);
  VariableIdSet variableIdsOfDefinition(Label def);
  typedef RDPairSet::iterator iterator;
};

#endif
