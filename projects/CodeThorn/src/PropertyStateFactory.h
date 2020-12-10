#ifndef PROPERTY_STATE_FACTORY_H
#define PROPERTY_STATE_FACTORY_H

#include "PropertyState.h"

namespace CodeThorn {
  class PropertyStateFactory {
  public:
    PropertyStateFactory();
    virtual PropertyState* create()=0;
    virtual ~PropertyStateFactory();
  };
}

#endif
