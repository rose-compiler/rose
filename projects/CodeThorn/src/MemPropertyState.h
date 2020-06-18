#ifndef MemPROPERTYSTATE_H
#define MemPROPERTYSTATE_H

#include "PropertyState.h"

namespace CodeThorn {

/*!
  * \author Markus Schordan
 */
class MemPropertyState : public Lattice {
 public:
  MemPropertyState();
  ~MemPropertyState();
  //void toStream(std::ostream& os, CodeThorn::VariableIdMapping* vim);
  virtual std::string toString(CodeThorn::VariableIdMapping* vim=0);
  virtual bool approximatedBy(Lattice& other);
  virtual void combine(Lattice& other);
};

 typedef MemPropertyState MemLattice;
}

#endif
