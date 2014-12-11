#ifndef RD_LATTICE_H
#define RD_LATTICE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "Lattice.h"

namespace SPRAY {

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class LVLattice : public Lattice {
 public:
  typedef set<VariableId>::iterator iterator;
  LVLattice();
  iterator begin();
  iterator end();
  size_t size();
  void insertVariableId(VariableId);
  void eraseVariableId(VariableId);
  bool isBot();
  void setBot();
  void toStream(ostream& os, VariableIdMapping* vim=0);
  bool exists(VariableId);
  void setEmptySet();
  void combine(Lattice& b);
  void combine(LVLattice& b);// TODO: delete
  bool approximatedBy(Lattice& b);
  bool approximatedBy(LVLattice& b); // TODO: delete

 private:
  set<VariableId> lvSet;
  bool _bot;
};

} // end namespace SPRAY

#endif

