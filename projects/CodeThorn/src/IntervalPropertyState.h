#ifndef INTERVAL_PROPERTY_STATE_H
#define INTERVAL_PROPERTY_STATE_H

#include <iostream>
#include "VariableIdMapping.h"
#include "NumberIntervalLattice.h"
#include "PropertyState.h"

namespace CodeThorn {
class IntervalPropertyState : public Lattice {
public:
  IntervalPropertyState();
  void toStream(std::ostream& os, VariableIdMapping* vim=0) override;
  bool approximatedBy(Lattice& other) const override;
  bool approximatedByAsymmetric(Lattice& other) const override;
  void combine(Lattice& other) override;
  void combineAsymmetric(Lattice& other);
 private:
  void combineInternal(Lattice& other, JoinMode joinMode);
 public:
  void overwrite(Lattice& other0);
  // adds integer variable
  void addVariable(VariableId varId);
  void setVariable(VariableId varId,NumberIntervalLattice num);
  NumberIntervalLattice getVariable(CodeThorn::VariableId varId);
  CodeThorn::VariableIdSet allVariableIds();
  // removes variable from state. Returns true if variable existed in state, otherwise false.
  bool variableExists(VariableId varId);
  bool removeVariable(VariableId varId);
  void topifyAllVariables();
  void topifyVariableSet(VariableIdSet varIdSet);
#if 0
  // adds pointer variable
  void addPointerVariable(VariableId);
  // adds array elements for indices 0 to number-1
  void addArrayElements(VariableId,int number);
#endif
  typedef std::map<VariableId,NumberIntervalLattice> IntervalMapType;
  bool isBot() const override { return _bot; }
  void setBot() { _bot=true; }
  void setEmptyState();
 private:
  IntervalMapType intervals;
  bool _bot;
};

} // end of namespace CodeThorn

#endif
