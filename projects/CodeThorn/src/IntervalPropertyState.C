#include "sage3basic.h"
#include "IntervalPropertyState.h"

IntervalPropertyState::IntervalPropertyState() {
}

void IntervalPropertyState::toStream(ostream& os, VariableIdMapping* vim) {
  os<<"{";
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    if(i!=intervals.begin())
      os<<", ";
    VariableId varId=(*i).first;
    NumberIntervalLattice niElem=(*i).second;
    cout<<vim->variableName(varId)<<"->"<<niElem.toString();
  }
  os<<"}";
}

bool IntervalPropertyState::approximatedBy(PropertyState& other0) {
  IntervalPropertyState& other=dynamic_cast<IntervalPropertyState&> (other0);
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    VariableId varId=(*i).first;
    if(!NumberIntervalLattice::isSubIntervalOf(intervals[varId],other.intervals[varId]))
      return false;
  }
  return true;
}

void IntervalPropertyState::combine(PropertyState& other0){
  IntervalPropertyState& other=dynamic_cast<IntervalPropertyState&> (other0);
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    VariableId varId=(*i).first;
    intervals[varId].join(other.intervals[varId]);
  }
}

// adds integer variable
void IntervalPropertyState::addVariable(VariableId varId) {
  intervals[varId]=NumberIntervalLattice();
}

void IntervalPropertyState::setEmptyState() {
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    intervals[(*i).first]=NumberIntervalLattice();
  }
}

#if 0
  // adds pointer variable
  void IntervalPropertyState::addPointerVariable(VariableId);
  // adds array elements for indices 0 to number-1
  void IntervalPropertyState::addArrayElements(VariableId,int number);
#endif
