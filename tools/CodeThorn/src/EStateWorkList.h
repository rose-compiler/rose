#ifndef EStateWorkList_H
#define EStateWorkList_H

#include <list>
#include <cstddef>

#include "EState.h"
#include "GeneralWorkList.h"

namespace CodeThorn {

  class EState;

  class EStateWorkList : public GeneralWorkList<EStatePtr> {
  public:
    virtual ~EStateWorkList();
  protected:
  };
}

#endif
