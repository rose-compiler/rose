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
  iterator begin() const;
  iterator end() const;
  size_t size() const;
  void insertVariableId(VariableId);
  void removeVariableId(VariableId);
  bool isBot() const;
  void setBot();
  void toStream(std::ostream& os, VariableIdMapping* vim=0);
  bool exists(VariableId);
  void setEmptySet();
  void combine(Lattice& b);
  void combine(LVLattice& b);// TODO: delete
  bool approximatedBy(Lattice& b) const;
  bool approximatedBy(LVLattice& b); // TODO: delete

 private:
  std::set<VariableId> lvSet;
  bool _bot;
};

} // end namespace CodeThorn

#endif

