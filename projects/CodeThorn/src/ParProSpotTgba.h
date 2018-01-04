#include "rose_config.h"
#ifdef HAVE_SPOT

#ifndef PAR_PRO_SPOT_TGBA_HH
#define PAR_PRO_SPOT_TGBA_HH

#include <cassert>

//CodeThorn includes
#include "ParProSpotState.h"
#include "ParProSpotSuccIter.h"
#include "ParProEState.h"

// BOOST includes
#include "boost/unordered_set.hpp"

//SPOT includes
#include "tgba/tgba.hh"
#include "tgba/succiter.hh"
#include "ltlvisit/apcollect.hh"
#include "tgba/bdddict.hh"
#include "bdd.h"
#include "tgba/state.hh"
#include "ltlast/atomic_prop.hh"

using namespace std;

namespace CodeThorn {
  /*! 
   * \brief An adapter which takes a CodeThorn ParProTransitionGraph and adheres 
   to SPOT's TGBA interface (see SpotConnection).
   * \author Marc Jasper
   * \date 2016, 2017.
   */
  class ParProSpotTgba : public spot::tgba {
  public:
    ParProSpotTgba(ParProTransitionGraph& ctstg, const spot::ltl::atomic_prop_set& sap, 
		   spot::bdd_dict& dic);
    ~ParProSpotTgba();
    // return the initial state of this tgba
    spot::state* get_init_state() const;
    //return an iterator over "local_state"'s successors
    spot::tgba_succ_iterator* succ_iter (const spot::state* local_state,
          				 const spot::state*, const spot::tgba*) const;
    //return the dictionary used to store the atomic propositions used for transitions
    spot::bdd_dict* get_dict() const;
    //return a string representation of the given state
    std::string format_state(const spot::state* state) const;
    // returns a string representing the condition of the transition that "iter" is currently pointing at
    std::string transition_annotation(const spot::tgba_succ_iterator* iter) const;
    //returns bdd::false, no particular accepting states exist. All (partial) paths through
    // the automaton define valid runs
    bdd all_acceptance_conditions() const;
    //returns bdd_true, see above
    bdd neg_acceptance_conditions() const;
  
  protected:
    //two functions to help the SPOT library reducing the number of successor states traversed
    // currently not implemented (returns bdd::true, no filtering takes place)
    bdd compute_support_conditions(const spot::state* state) const;
    bdd compute_support_variables(const spot::state* state) const;

  private:
    // CodeThorn's TransitionGraph (adaptee) 
    ParProTransitionGraph& stg;
    // the dictionary of atomic propositions used by this automaton
    spot::bdd_dict& dict;
    //map atomic propositions as used in the LTL formulae to their 
    // corresponding slots in the bdd_dict dictionary table. 
    boost::unordered_map<std::string, int> propName2DictNum;
  };
}  //end of namespace CodeThorn

#endif

#endif // end of "#ifdef HAVE_SPOT"
