#ifndef MemPropertyStateFactory_H
#define MemPropertyStateFactory_H

#include "PropertyState.h"
#include "PropertyStateFactory.h"

namespace CodeThorn {

class MemPropertyStateFactory : public PropertyStateFactory {
 public:
  MemPropertyStateFactory();
  virtual PropertyState* create();
  virtual ~MemPropertyStateFactory();
};

}

#endif

