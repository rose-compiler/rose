#include "IntervalPropertyState.h"

IntervalPropertyState::IntervalPropertyState() {
}

void IntervalPropertyState::toStream(ostream& os, VariableIdMapping* vim) {
}

bool IntervalPropertyState::approximatedBy(PropertyState* other) {
  return false;
}

void IntervalPropertyState::combine(PropertyState* other){
}

// adds integer variable
void IntervalPropertyState::addVariable(VariableId varId) {
  intervals[varId]=NumberIntervalLattice();
}

#if 0
  // adds pointer variable
  void IntervalPropertyState::addPointerVariable(VariableId);
  // adds array elements for indices 0 to number-1
  void IntervalPropertyState::addArrayElements(VariableId,int number);
#endif
