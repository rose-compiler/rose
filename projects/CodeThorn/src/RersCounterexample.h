#ifndef RERS_COUNTEREXAMPLE_H
#define RERS_COUNTEREXAMPLE_H

#include "ExecutionTrace.h"

namespace CodeThorn {

  class EState;
  class TransitionGraph;
  class Analyzer;

/**
 * @brief An input/output counterexample in a syntax resembling that of RERS
 * 
 * \author Marc Jasper
 * \date 2017
 */
  class RersCounterexample : public ExecutionTrace {

  public:
    std::string toRersIString() const;
    std::string toRersIOString() const;

    RersCounterexample onlyStatesSatisfying(std::function<bool(const EState*)> predicate) const;
    RersCounterexample onlyIStates() const;
    RersCounterexample onlyIOStates() const;
  private:
    std::string toRersIOString(bool withOutput) const;
    char toRersChar(int value) const;
  };
}

#endif
