#ifndef LV_LATTICE_H
#define LV_LATTICE_H

#include "Labeler.h"
#include "VariableIdMapping.h"
#include "Lattice.h"

namespace CodeThorn {

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
class LVLattice : public Lattice {
 public:
    typedef std::set<VariableId>::iterator iterator;
  LVLattice();
  iterator begin();
  iterator end();
  size_t size();
  void insertVariableId(VariableId);
  void removeVariableId(VariableId);
  bool isBot();
  void setBot();
  void toStream(std::ostream& os, VariableIdMapping* vim=0);
  bool exists(VariableId);
  void setEmptySet();
  void combine(Lattice& b);
  void combine(LVLattice& b);// TODO: delete
  bool approximatedBy(Lattice& b);
  bool approximatedBy(LVLattice& b); // TODO: delete

 private:
  std::set<VariableId> lvSet;
  bool _bot;
};

} // end namespace CodeThorn

#endif

