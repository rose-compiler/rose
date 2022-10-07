#include "rose_config.h"
#ifdef HAVE_SPOT

#ifndef SPOT_SUCC_ITERATOR_HH
#define SPOT_SUCC_ITERATOR_HH

#include <cassert>
#include <string>
#include <list>

//CodeThron includes
#include "SpotState.h"
#include "EState.h"

//SPOT includes
#include "tgba/succiter.hh"
#include "bdd.h"
#include "tgba/bdddict.hh"
#include "tgba/state.hh"

using namespace std;

namespace CodeThorn {
  /*! 
   * \brief SPOT-specific iterator over the successor states of an EState (see SpotConnection).
   * \author Marc Jasper
   * \date 2014, 2015, 2016, 2017.
   */
  class SpotSuccIter : public spot::tgba_succ_iterator {
  public:
    SpotSuccIter(TransitionGraph& tg, const EState& state,
		    const std::map<int, int>& propNum2DictNum, std::set<int> inVars , std::set<int> outVars);
    void first();  //sets the iterator to the first successor
    void next();
    bool done() const;
    // get the state the iterator is currently pointing at
    spot::state* current_state() const;
    // retrieve the condition of the transition leading to the currently targeted successor state
    bdd current_condition() const;
    // get the acceptance condition on the transition leading to current state
    // Not implemented since there are no sets of accepting conditions in a CodeThorn's SpotTgba
    bdd current_acceptance_conditions() const;
  private:
    // generates the condition necessary to traverse the transition to current state
    bdd generateSpotTransition(const Transition& t) const;
    // generates a boolean conjunction of non-negated variables with the remaining
    // elements of the alphabet as negated variables.
    bdd conjunctivePredicate(std::set<int> nonNegated, std::set<int> alphabet) const;

    // the CodeThorn TransitionGraph on which the LTL formulae are tested
    TransitionGraph& stg;
    //maps the numbers of LTL atomic propositions to their entries in the bdd dictionary
    std::map<int, int> propNum2DictNum;
    //the outgoing transitions that are used to iterate over successor states
    TransitionPtrSet outEdges;
    //the iterator over outgoing transitions
    TransitionPtrSet::const_iterator iter;
    //sets of atomic propositions containing all possible input/output values (int representation)
    std::set<int> ltlInVars;
    std::set<int> ltlOutVars;
  };
}

#endif

#endif // end of "#ifdef HAVE_SPOT"
