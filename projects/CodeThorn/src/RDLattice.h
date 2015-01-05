#ifndef RD_LATTICE_H
#define RD_LATTICE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "Lattice.h"

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class RDLattice : public Lattice {
 public:
  typedef set<pair<Label,VariableId> >::iterator iterator;
  RDLattice();
  iterator begin();
  iterator end();
  size_t size();
  void insertPair(Label,VariableId);
  void erasePair(Label,VariableId);
  void removeAllPairsWithVariableId(VariableId var);
  bool isBot();
  void setBot();
  void toStream(ostream& os, VariableIdMapping* vim=0);
  bool exists(pair<Label,VariableId>);
  void setEmptySet();
  void combine(Lattice& b);
  bool approximatedBy(Lattice& b);

 private:
  set<pair<Label,VariableId> > rdSet;
  bool _bot;
};

#endif

