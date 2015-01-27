
#ifndef PROPERTY_STATE_H
#define PROPERTY_STATE_H

#include "sage3basic.h"
#include "Lattice.h"
#include "VariableIdMapping.h"

#if 0
class PropertyState : public Lattice {
 public:
  PropertyState();
  virtual void toStream(ostream& os, VariableIdMapping* vim=0);
  virtual bool approximatedBy(PropertyState& other);
  virtual void combine(PropertyState& other);
};
#else
typedef Lattice PropertyState;
#endif

class PropertyStateFactory {
 public:
  PropertyStateFactory();
  virtual PropertyState* create()=0;
  virtual ~PropertyStateFactory();
};

typedef PropertyStateFactory LatticeElementFactory;

#endif
