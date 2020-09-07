#ifndef RERS_COUNTEREXAMPLE_H
#define RERS_COUNTEREXAMPLE_H

#include "ExecutionTrace.h"
#include "ltlthorn-lib/LTLRersMapping.h"

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
    RersCounterexample onlyStatesSatisfying(std::function<bool(const EState*)> predicate) const;
    RersCounterexample onlyIStates() const;
    RersCounterexample onlyIOStates() const;
    std::string toRersIString(LtlRersMapping& ltlRersMapping) const;  // MS 8/6/20: changed to use mapping
    std::string toRersIOString(LtlRersMapping& ltlRersMapping) const; // MS 8/6/20: changed to use mapping
  private:
    std::string toRersIOString(LtlRersMapping& ltlRersMapping, bool withOutput) const; // MS 8/6/20: changed to use mapping
  };
}

#endif
