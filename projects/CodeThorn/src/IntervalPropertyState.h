#ifndef INTERVAL_PROPERTY_STATE_H
#define INTERVAL_PROPERTY_STATE_H

#include <iostream>
#include "VariableIdMapping.h"
#include "NumberIntervalLattice.h"
#include "PropertyState.h"

class IntervalPropertyState : public Lattice {
public:
  IntervalPropertyState();
  void toStream(std::ostream& os, VariableIdMapping* vim=0);
  bool approximatedBy(Lattice& other);
  void combine(Lattice& other);
  // adds integer variable
  void addVariable(VariableId varId);
#if 0
  // adds pointer variable
  void addPointerVariable(VariableId);
  // adds array elements for indices 0 to number-1
  void addArrayElements(VariableId,int number);
#endif
  typedef std::map<VariableId,NumberIntervalLattice> IntervalMapType;
  IntervalMapType intervals;
  bool isBot() { return _bot; }
  void setBot() { _bot=true; }
  void setEmptyState();
 private:
  bool _bot;
};

#endif
