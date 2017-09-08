#ifndef SVCOMP_WITNESS_H
#define SVCOMP_WITNESS_H

#include "ExecutionTrace.h"

namespace CodeThorn {

  class EState;

  class SvcompWitness : public ExecutionTrace {

  public:
    SvcompWitness onlyStatesSatisfying(std::function<bool(const EState*)> predicate) const;
  };
}

#endif
