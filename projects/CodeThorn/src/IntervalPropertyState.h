#ifndef INTERVAL_PROPERTY_STATE_H
#define INTERVAL_PROPERTY_STATE_H

#include "sage3basic.h"
#include "VariableIdMapping.h"
#include "NumberIntervalLattice.h"
#include "PropertyState.h"
#include <iostream>

class IntervalPropertyState : public PropertyState {
public:
  IntervalPropertyState();
  void toStream(std::ostream& os, VariableIdMapping* vim=0);
  bool approximatedBy(PropertyState* other);
  void combine(PropertyState* other);
  // adds integer variable
  void addVariable(VariableId varId);
#if 0
  // adds pointer variable
  void addPointerVariable(VariableId);
  // adds array elements for indices 0 to number-1
  void addArrayElements(VariableId,int number);
#endif
  map<VariableId,NumberIntervalLattice > intervals;
};

#endif
