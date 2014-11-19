// Author: Markus Schordan, 2013.

#ifndef RDANALYSISASTATTRIBUTE_H
#define RDANALYSISASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "IntervalAstAttributeInterface.h"
#include "IntervalLattice.h"
#include <string>

using std::string;

class IntervalAstAttribute : public IntervalAstAttributeInterface {
 public:
  virtual bool isBottomElement();
  virtual VariableIdSet allVariableIds();
  virtual LabelSet allLabels();
  virtual LabelSet definitionsOfVariableId(VariableId varId);
  virtual VariableIdSet variableIdsOfDefinition(Label def);
  virtual iterator begin();
  virtual iterator end();
  virtual ~IntervalAstAttribute();
 public:
  IntervalAstAttribute(IntervalLattice* elem);
  void toStream(ostream& os, VariableIdMapping* vim);
  string toString();
 private:
  IntervalLattice* _elem;
};

#endif
