
#ifndef PROPERTY_STATE_H
#define PROPERTY_STATE_H

#include "sage3basic.h"
#include "Lattice.h"
#include "VariableIdMapping.h"

namespace CodeThorn {

#if 1
  typedef Lattice PropertyState;
#else
  class PropertyState : public Lattice {
  };
#endif
  
}
#endif
