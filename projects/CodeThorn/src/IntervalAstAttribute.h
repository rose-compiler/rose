// Author: Markus Schordan, 2013.

#ifndef INTERVALANALYSISASTATTRIBUTE_H
#define INTERVALANALYSISASTATTRIBUTE_H

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
  virtual ~IntervalAstAttribute();
 public:
  IntervalAstAttribute(IntervalLattice<int>* elem);
  void toStream(ostream& os, VariableIdMapping* vim);
  string toString();
 private:
  IntervalLattice<int>* _elem;
};

#endif
