#include "rose_config.h"
#ifdef HAVE_SPOT

#ifndef SPOT_TGBA_HH
#define SPOT_TGBA_HH

#include <cassert>

//CodeThorn includes
#include "SpotState.h"
#include "SpotSuccIter.h"
#include "EState.h"

//SPOT includes
#include "tgba/tgba.hh"
#include "tgba/succiter.hh"
#include "ltlvisit/apcollect.hh"
#include "tgba/bdddict.hh"
#include "bdd.h"
#include "tgba/state.hh"
#include "tgba/succiter.hh"
#include "tgba/state.hh"
#include "ltlast/atomic_prop.hh"

using namespace std;

namespace CodeThorn {
  /*! 
   * \brief An adapter which takes a CodeThorn TransitionGraph and adheres 
   to SPOT's TGBA interface (see SpotConnection).
   * \author Marc Jasper
   * \date 2014, 2015, 2016, 2017.
   */
  class SpotTgba : public spot::tgba {
  public:
    SpotTgba(TransitionGraph& ctstg, const spot::ltl::atomic_prop_set& sap, 
             spot::bdd_dict& dic, LtlRersMapping ltlRersMapping);
    ~SpotTgba();
    // return the initial state of this tgba
    spot::state* get_init_state() const;
    //return an iterator over "local_state"'s successors
    spot::tgba_succ_iterator* succ_iter (const spot::state* local_state,
          				 const spot::state*, const spot::tgba*) const;
    //return the dictionary used to store the atomic propositions used for transitions
    spot::bdd_dict* get_dict() const;
    //return a string representation of the given state
    std::string format_state(const spot::state* state) const;
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
    // a simple translation from letter to integer (LTL variables a.k.a. I/O alphabet)
    int propName2Int(std::string propName) const;
  private:
    // CodeThorn's TransitionGraph (adaptee) 
    TransitionGraph& stg;
    // the dictionary of atomic propositions used by this automaton
    spot::bdd_dict& dict;
    //map matching atomic propositions as used in the LTL formula with their 
    // corresponding slots in the bdd_dict dictionary table. 
    std::map<int, int> propNum2DictNum;
    //sets of atomic propositions containing all possible input/output values (int representation)
    LtlRersMapping _ltlRersMapping;
  };
}  //end of namespace CodeThorn

#endif

#endif // end of "#ifdef HAVE_SPOT"
