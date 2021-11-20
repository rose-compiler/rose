#ifndef SVCOMP_WITNESS_H
#define SVCOMP_WITNESS_H

#include "ExecutionTrace.h"
#include <string>

namespace CodeThorn {

  class EState;

  class SvcompWitness : public ExecutionTrace {

  public:
    SvcompWitness onlyStatesSatisfying(std::function<bool(EStatePtr)> predicate) const;
    void writeErrorAutomatonToFile(std::string filename);
  };
}

#endif
