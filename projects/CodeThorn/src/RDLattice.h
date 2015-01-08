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
  typedef std::set<std::pair<SPRAY::Label,VariableId> >::iterator iterator;
  RDLattice();
  iterator begin();
  iterator end();
  size_t size();
  void insertPair(SPRAY::Label,VariableId);
  void erasePair(SPRAY::Label,VariableId);
  void removeAllPairsWithVariableId(VariableId var);
  bool isBot();
  void setBot();
  void toStream(std::ostream& os, VariableIdMapping* vim=0);
  bool exists(std::pair<SPRAY::Label,VariableId>);
  void setEmptySet();
  void combine(Lattice& b);
  bool approximatedBy(Lattice& b);

 private:
  std::set<std::pair<SPRAY::Label,VariableId> > rdSet;
  bool _bot;
};

#endif

