// Author: Markus Schordan, 2013.

#ifndef INTERVALANALYSISASTATTRIBUTE_H
#define INTERVALANALYSISASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "IntervalAstAttributeInterface.h"
#include "IntervalPropertyState.h"
#include <string>

using namespace std;

class IntervalAstAttribute : public IntervalAstAttributeInterface {
 public:
  virtual bool isBottomElement();
  virtual VariableIdSet allVariableIds();
  virtual ~IntervalAstAttribute();
 public:
  IntervalAstAttribute(IntervalPropertyState* elem);
  void toStream(ostream& os, VariableIdMapping* vim);
  string toString();
#if 0
  typedef IntervalPropertyState::IntervalMapType::iterator iterator;
  iterator begin();
  iterator end();
#endif
 private:
  IntervalPropertyState* _elem;
};

#endif
