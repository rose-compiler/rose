#ifndef LATTICE_H
#define LATTICE_H

#include "sage3basic.h"
#include "VariableIdMapping.h"

class Lattice {
 public:
  Lattice();
  virtual bool isBot();
  virtual void toStream(ostream& os, VariableIdMapping* vim=0);
  virtual bool approximatedBy(Lattice& other)=0;
  virtual void combine(Lattice& other);
  ~Lattice();
};

#endif
