// Author: Markus Schordan, 2013.

#ifndef INTERVALANALYSISASTATTRIBUTE_H
#define INTERVALANALYSISASTATTRIBUTE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "IntervalAstAttributeInterface.h"
#include "IntervalPropertyState.h"
#include <string>

using std::string;

class IntervalAstAttribute : public IntervalAstAttributeInterface {
 public:
  virtual bool isBottomElement();
  virtual VariableIdSet allVariableIds();
  virtual ~IntervalAstAttribute();
 public:
  IntervalAstAttribute(IntervalPropertyState* elem);
  void toStream(ostream& os, VariableIdMapping* vim);
  string toString();
  typedef IntervalPropertyState::IntervalMapType::iterator iterator;
  iterator begin();
  iterator end();
 private:
  IntervalPropertyState* _elem;
};

#endif
