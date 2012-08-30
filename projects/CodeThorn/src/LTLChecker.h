// -*- C++ -*-
#ifndef __LTLCHECKER_H__
#define __LTLCHECKER_H__

#include "StateRepresentation.h"
#include "LTL.h"
#include <map>

namespace ltl {
  /**
   * A naive checker for LTL formulae.
   */
  class Checker {
  public:
    Checker(EStateSet& ess, TransitionGraph& g);
    /// verify the LTL formula f
    bool verify(const Formula& f);

  protected:
    TransitionGraph& transitionGraph;
    EStateSet& eStateSet;
  };

};

#endif
