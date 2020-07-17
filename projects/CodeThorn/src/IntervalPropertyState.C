#include "sage3basic.h"
#include "IntervalPropertyState.h"

using namespace CodeThorn;

CodeThorn::IntervalPropertyState::IntervalPropertyState() {
  setBot();
  setBackEdge();
}

void CodeThorn::IntervalPropertyState::toStream(ostream& os, VariableIdMapping* vim) {
  if(isBot()) {
    os<<"bot";
  } else {
    os<<"{";
    for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
      if(i!=intervals.begin())
        os<<", ";
      VariableId varId=(*i).first;
      CodeThorn::NumberIntervalLattice niElem=(*i).second;
      if(vim)
        os<<vim->variableName(varId)<<"->"<<niElem.toString();
      else
        os<<varId.toString()<<"->"<<niElem.toString();
    }
    os<<"}";
  }
}

bool CodeThorn::IntervalPropertyState::approximatedBy(Lattice& other0) const {
  IntervalPropertyState* other=dynamic_cast<IntervalPropertyState*> (&other0);
  ROSE_ASSERT(other);
  if(isBot())
    return true;
  if(other->isBot())
    return false;
    
  for(IntervalMapType::const_iterator i=intervals.begin();i!=intervals.end();++i) {
    VariableId varId=(*i).first;
    if(!CodeThorn::NumberIntervalLattice::isSubIntervalOf(const_cast<CodeThorn::IntervalPropertyState*>(this)->intervals[varId],
                                                          other->intervals[varId]))
      return false;
  }
  return true;
}

bool CodeThorn::IntervalPropertyState::approximatedByAsymmetric(Lattice& other) const {
  // PP currrently not used
  return approximatedBy(other);
}

// schroder3 (2016-08-05): Merges the "other0" IntervalPropertyState into this IntervalPropertyState.
//  The interval of each variable in this state is joined with the interval of the corresponding
//  variable in the "other0" state.
void CodeThorn::IntervalPropertyState::combineInternal(IntervalPropertyState& other0, JoinMode joinMode) {
  IntervalPropertyState* other=&other0;
  ROSE_ASSERT(other!=0);
  if(other->isBot())
    return;
    
  if(isBot())
    _bot=false;
  
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    VariableId varId=(*i).first;
    if(other->intervals.find(varId)==other->intervals.end()) {
      // ps's variable is not in other's ps
      // nothing to do - assume bot in this case
    }else {
      intervals[varId].join(other->intervals[varId], joinMode);
    }
  }
  
  for(IntervalMapType::iterator i=other->intervals.begin();i!=other->intervals.end();++i) {
    VariableId varId=(*i).first;
    if(intervals.find(varId)==intervals.end()) {
      // other ps's variable is not in this ps yet
      intervals[varId]=(*i).second;
    }else {
      // schroder3 (2016-08-05): This should already been handled in the upper for-loop and this
      //  should therefore never change anything:
      NumberIntervalLattice previous = intervals[varId].getCopy();
      intervals[varId].join(other->intervals[varId], joinMode);
      ROSE_ASSERT(intervals[varId] == previous);
    }
  }
}

void CodeThorn::IntervalPropertyState::combine(Lattice& other0){
  // backedge is a property of an edge state, but not at a label state
  ROSE_ASSERT(isBackEdge() == false);
  ROSE_ASSERT(typeid(IntervalPropertyState) == typeid(other0));
  
  IntervalPropertyState& that0 = static_cast<IntervalPropertyState&>(other0);
  
  // PP: use join appropriate for edge direction
  combineInternal(that0, that0.isBackEdge() ? JM_InfinityAsymmetric : JM_Exact);
}

void CodeThorn::IntervalPropertyState::combineAsymmetric(Lattice& other0){
  // PP: currently not used; functionality integrated with combine 
  // schroder3 (2016-08-05): Use the asymmetric join:
  combineInternal(static_cast<IntervalPropertyState&>(other0), JM_InfinityAsymmetric);
}

// adds integer variable
void CodeThorn::IntervalPropertyState::addVariable(VariableId varId) {
  setVariable(varId,NumberIntervalLattice());
}

// assign integer variable
void CodeThorn::IntervalPropertyState::setVariable(VariableId varId, NumberIntervalLattice num) {
  _bot=false;
  intervals[varId]=num;
}

// assign integer variable
NumberIntervalLattice CodeThorn::IntervalPropertyState::getVariable(VariableId varId) {
  return intervals[varId];
}

void CodeThorn::IntervalPropertyState::setEmptyState() {
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    intervals[(*i).first]=NumberIntervalLattice();
  }
  _bot=false;
}

#if 0
  // adds pointer variable
  void CodeThorn::IntervalPropertyState::addPointerVariable(VariableId);
  // adds array elements for indices 0 to number-1
  void CodeThorn::IntervalPropertyState::addArrayElements(VariableId,int number);
#endif

VariableIdSet CodeThorn::IntervalPropertyState::allVariableIds() {
  VariableIdSet set;
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    set.insert((*i).first);
  }
  return set;
}

void CodeThorn::IntervalPropertyState::topifyVariableSet(VariableIdSet varIdSet) {
  for(VariableIdSet::iterator i=varIdSet.begin();i!=varIdSet.end();++i) {
    intervals[*i]=CodeThorn::NumberIntervalLattice::top();
  }
}
void CodeThorn::IntervalPropertyState::topifyAllVariables() {
  for(IntervalMapType::iterator i=intervals.begin();i!=intervals.end();++i) {
    intervals[(*i).first]=CodeThorn::NumberIntervalLattice::top();
  }
}

bool CodeThorn::IntervalPropertyState::variableExists(VariableId varId) {
  return intervals.find(varId)!=intervals.end();
}

bool CodeThorn::IntervalPropertyState::removeVariable(VariableId varId) {
  // for STL map numErased can only be 0 or 1.
  size_t numErased=intervals.erase(varId);
  return numErased==1;
}
