// Author: Markus Schordan, 2013.

#ifndef RDANALYSISASTATTRIBUTE_H
#define RDANALYSISASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "RDAstAttributeInterface.h"
#include "RDLattice.h"
#include <string>

using std::string;

class RDAstAttribute : public RDAstAttributeInterface {
 public:
  RDAstAttribute(RDLattice* elem);
  bool isBottomElement();
  VariableIdSet allVariableIds();
  LabelSet allLabels();
  LabelSet definitionsOfVariableId(VariableId varId);
  VariableIdSet variableIdsOfDefinition(Label def);
  iterator begin();
  iterator end();
  virtual ~RDAstAttribute();
  void toStream(ostream& os, VariableIdMapping* vim);
  string toString();
 private:
  RDLattice* _elem;
};

#endif
