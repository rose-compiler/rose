#include "sage3basic.h"
#include "IntervalPropertyState.h"

IntervalPropertyState::IntervalPropertyState() {
  setBot();
}

void IntervalPropertyState::toStream(ostream& os, VariableIdMapping* vim) {
  if(isBot()) {
    os<<"bot";
  } else {
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
}

bool IntervalPropertyState::approximatedBy(Lattice& other0) {
  IntervalPropertyState* other=dynamic_cast<IntervalPropertyState*> (&other0);
  ROSE_ASSERT(other);
  if(isBot())
    return true;
  if(!isBot()&&other->isBot())
    return false;
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    VariableId varId=(*i).first;
    if(!NumberIntervalLattice::isSubIntervalOf(intervals[varId],other->intervals[varId]))
      return false;
  }
  return true;
}

void IntervalPropertyState::combine(Lattice& other0){
  IntervalPropertyState* other=dynamic_cast<IntervalPropertyState*> (&other0);
  ROSE_ASSERT(other!=0);
  if(isBot()&&other->isBot())
    return;
  if(!isBot()&&other->isBot())
    return;
  if(isBot()&&!other->isBot()) 
    _bot=false;
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    VariableId varId=(*i).first;
    intervals[varId].join(other->intervals[varId]);
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
  _bot=false;
}

#if 0
  // adds pointer variable
  void IntervalPropertyState::addPointerVariable(VariableId);
  // adds array elements for indices 0 to number-1
  void IntervalPropertyState::addArrayElements(VariableId,int number);
#endif
