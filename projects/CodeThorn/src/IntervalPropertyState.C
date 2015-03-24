#include "sage3basic.h"
#include "IntervalPropertyState.h"

using namespace SPRAY;

SPRAY::IntervalPropertyState::IntervalPropertyState() {
  setBot();
}

void SPRAY::IntervalPropertyState::toStream(ostream& os, VariableIdMapping* vim) {
  if(isBot()) {
    os<<"bot";
  } else {
    os<<"{";
    for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
      if(i!=intervals.begin())
        os<<", ";
      VariableId varId=(*i).first;
      SPRAY::NumberIntervalLattice niElem=(*i).second;
      if(vim)
        os<<vim->variableName(varId)<<"->"<<niElem.toString();
      else
        os<<varId.toString()<<"->"<<niElem.toString();
    }
    os<<"}";
  }
}

bool SPRAY::IntervalPropertyState::approximatedBy(Lattice& other0) {
  IntervalPropertyState* other=dynamic_cast<IntervalPropertyState*> (&other0);
  ROSE_ASSERT(other);
  if(isBot())
    return true;
  if(!isBot()&&other->isBot())
    return false;
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    VariableId varId=(*i).first;
    if(!SPRAY::NumberIntervalLattice::isSubIntervalOf(intervals[varId],other->intervals[varId]))
      return false;
  }
  return true;
}

void SPRAY::IntervalPropertyState::combine(Lattice& other0){
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
  for(IntervalMapType::iterator i=other->intervals.begin();i!=other->intervals.end();++i) {
    VariableId varId=(*i).first;
    if(intervals.find(varId)==intervals.end()) {
      // other ps's variable is not in this ps yet
      intervals[varId]=(*i).second;
    }else {
      intervals[varId].join(other->intervals[varId]);
    }
  }
}

// adds integer variable
void SPRAY::IntervalPropertyState::addVariable(VariableId varId) {
  intervals[varId]=NumberIntervalLattice();
}

// assign integer variable
void SPRAY::IntervalPropertyState::setVariable(VariableId varId, NumberIntervalLattice num) {
  intervals[varId]=num;
}

// assign integer variable
NumberIntervalLattice SPRAY::IntervalPropertyState::getVariable(VariableId varId) {
  return intervals[varId];
}

void SPRAY::IntervalPropertyState::setEmptyState() {
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    intervals[(*i).first]=NumberIntervalLattice();
  }
  _bot=false;
}

#if 0
  // adds pointer variable
  void SPRAY::IntervalPropertyState::addPointerVariable(VariableId);
  // adds array elements for indices 0 to number-1
  void SPRAY::IntervalPropertyState::addArrayElements(VariableId,int number);
#endif

VariableIdSet SPRAY::IntervalPropertyState::allVariableIds() {
  VariableIdSet set;
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    set.insert((*i).first);
  }
  return set;
}
