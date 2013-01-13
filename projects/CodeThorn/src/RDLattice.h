#include "Labeler.h"
#include "VariableIdMapping.h"

#ifndef RD_LATTICE_H
#define RD_LATTICE_H

using CodeThorn::Label;
using CodeThorn::VariableId;

class RDLattice {
 public:
  typedef set<pair<Label,VariableId> >::iterator iterator;
  iterator begin();
  iterator end();
  void insertPair(Label,VariableId);
  void erasePair(Label,VariableId);
  bool isBot();
  void toStream(ostream& os);
  bool exists(pair<Label,VariableId>);
#if 1
  RDLattice& combine(RDLattice& b);
  bool approximatedBy(RDLattice& b);
#endif
 private:
  set<pair<Label,VariableId> > rdSet;
};

#endif

