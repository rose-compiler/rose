#ifndef INTERVALPROPERTYSTATEFACTORY_H
#define INTERVALPROPERTYSTATEFACTORY_H

#include "PropertyState.h"

namespace SPRAY {

class IntervalPropertyStateFactory : public PropertyStateFactory {
 public:
  IntervalPropertyStateFactory();
  virtual PropertyState* create();
  virtual ~IntervalPropertyStateFactory();
};

}

#endif
