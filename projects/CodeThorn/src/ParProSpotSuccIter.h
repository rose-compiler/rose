#include "rose_config.h"
#ifdef HAVE_SPOT

#ifndef PAR_PRO_SPOT_SUCC_ITERATOR_HH
#define PAR_PRO_SPOT_SUCC_ITERATOR_HH

#include <cassert>
#include <string>
#include <list>

// CodeThorn includes
#include "ParProTransitionGraph.h"

//CodeThron includes
#include "ParProSpotState.h"
#include "ParProEState.h"

// BOOST includes
#include "boost/unordered_set.hpp"

//SPOT includes
#include "tgba/succiter.hh"
#include "bdd.h"
#include "tgba/bdddict.hh"
#include "tgba/state.hh"

using namespace std;

namespace CodeThorn {

typedef boost::unordered_map<string, int> String2Int;

  /*! 
   * \brief SPOT-specific iterator over the successor states of a ParProEState (see SpotConnection).
   * \author Marc Jasper
   * \date 2016, 2017.
   */
  class ParProSpotSuccIter : public spot::tgba_succ_iterator {
  public:
    ParProSpotSuccIter(ParProTransitionGraph& tg, const ParProEState& state,
		       const String2Int& propName2DictNum);
    void first();  //sets the iterator to the first successor
    void next();
    bool done() const;
    // get the state that the iterator is currently pointing at
    spot::state* current_state() const;
    // retrieve the condition of the transition leading to the currently targeted successor state
    bdd current_condition() const;
    // (optional implementation) returns a string that represents the condition of the transition that the iterator currently points at
    std::string format_current_condition() const;
    // get the acceptance condition on the transition leading to current state
    // Not implemented since there are no sets of accepting conditions in a CodeThorn's SpotTgba
    bdd current_acceptance_conditions() const;
  private:
    // generates the condition necessary to traverse the transition to current state
    bdd generateSpotTransition(const ParProTransition& t) const;
    string generateFormattedSpotTransition(const ParProTransition& t) const;

    // the source state whose outgoing transitions are of interest
    const ParProEState& _source;
    // the CodeThorn TransitionGraph on which the LTL formulae are tested
    ParProTransitionGraph& stg;
    //maps the LTLs' atomic propositions to their entries in the bdd dictionary
    const String2Int propName2DictNum;
    //the outgoing transitions that are used to iterate over successor states
    ParProTransitions _outEdges;
    //the iterator over outgoing transitions
    ParProTransitions::const_iterator iter;
    // two flags for simulating self loops for leaf nodes
    bool _needToAddSelfLoop;
    bool _upcomingSelfLoop;
  };
}

#endif

#endif // end of "#ifdef HAVE_SPOT"
