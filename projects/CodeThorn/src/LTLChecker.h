// -*- C++ -*-
#ifndef __LTLCHECKER_H__
#define __LTLCHECKER_H__

// Written 2012 by Adrian Prantl <adrian@llnl.gov>.

#include "StateRepresentation.h"
#include "LTL.h"
#include <map>

namespace LTL {
  /**
   * A dataflow-based checker for LTL formulae.
   * \date 2012
   * \author Adrian Prantl
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
