
#ifndef PROPERTY_STATE_H
#define PROPERTY_STATE_H

#include "sage3basic.h"
#include "Lattice.h"
#include "VariableIdMapping.h"

namespace CodeThorn {

typedef Lattice PropertyState;

class PropertyStateFactory {
 public:
  PropertyStateFactory();
  virtual PropertyState* create()=0;
  virtual ~PropertyStateFactory();
};

}
#endif
