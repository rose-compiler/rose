#ifndef INTERVALPROPERTYSTATEFACTORY_H
#define INTERVALPROPERTYSTATEFACTORY_H

#include "PropertyState.h"
#include "PropertyStateFactory.h"

namespace CodeThorn {

class IntervalPropertyStateFactory : public PropertyStateFactory {
 public:
  IntervalPropertyStateFactory();
  virtual PropertyState* create();
  virtual ~IntervalPropertyStateFactory();
};

}

#endif
