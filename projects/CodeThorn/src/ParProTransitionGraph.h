// Author: Marc Jasper, 2016.

#ifndef PAR_PRO_TRANSITION_GRAPH_H
#define PAR_PRO_TRANSITION_GRAPH_H

#include "sage3basic.h"

// SPRAY includes
// #include "Flow.h"

// CodeThorn includes


// BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"

namespace SPRAY {

  // the state transition graph of a parallel program
  class ParProTransitionGraph {
  public:
    ParProTransitionGraph() : _preciseStg(true), _completeStg(false) {}
    bool isPrecise() { return _preciseStg; }
    void setIsPrecise(bool p) { _preciseStg = p; }
    bool isComplete() { return _completeStg; }
    void setIsComplete(bool c) { _completeStg = c; }

  private:
    bool _preciseStg;
    bool _completeStg;
  };

} // end of namespace SPRAY

#endif
