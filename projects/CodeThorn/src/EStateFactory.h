#ifndef EStateFactory_H
#define EStateFactory_H

#include "PropertyState.h"
#include "PropertyStateFactory.h"

namespace CodeThorn {

class EStateFactory : public PropertyStateFactory {
 public:
  EStateFactory();
  virtual EState* create();
  virtual ~EStateFactory();
};

}

#endif

