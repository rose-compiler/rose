#include "sage3basic.h"
#include "rose_config.h"
#ifdef HAVE_SPOT

#include "ParProSpotTgba.h"

using namespace CodeThorn;

ParProSpotTgba::ParProSpotTgba(ParProTransitionGraph& ctstg, const spot::ltl::atomic_prop_set& sap, 
			       spot::bdd_dict& dic): stg(ctstg), dict(dic) {
  //register the atomic propositions used by this TGBA. Maybe move this task to class SpotConnection.
  spot::ltl::atomic_prop_set::iterator i;
  for(i=sap.begin(); i!=sap.end(); ++i) {
    propName2DictNum[(*i)->name()] = dict.register_proposition(*i, this);
  }
}

ParProSpotTgba::~ParProSpotTgba(){
  //unregister atomic propositions from the dictionary used by this automaton
  dict.unregister_all_my_variables(this);
};

spot::state* ParProSpotTgba::get_init_state() const {
	const ParProEState* startState = stg.getStartState();
	ParProSpotState* init_state = new ParProSpotState( *startState);
	return init_state;
}  
	

spot::tgba_succ_iterator* ParProSpotTgba::succ_iter (const spot::state* local_state,
        			const spot::state*, const spot::tgba*) const {
  const ParProSpotState* state = dynamic_cast<const ParProSpotState*>(local_state);
  assert(state);
  return new ParProSpotSuccIter(stg, (state->getEState()), propName2DictNum);
} 

spot::bdd_dict* ParProSpotTgba::get_dict() const {
  return &dict;
}

std::string ParProSpotTgba::format_state(const spot::state* state) const {
  const ParProSpotState* toBeFormatted = dynamic_cast<const ParProSpotState*>(state);
  assert(toBeFormatted);
  return toBeFormatted->toString();
}

std::string ParProSpotTgba::transition_annotation(const spot::tgba_succ_iterator* iter) const {
  const ParProSpotSuccIter* toBeFormatted = dynamic_cast<const ParProSpotSuccIter*>(iter);
  assert(toBeFormatted);
  return toBeFormatted->format_current_condition();
}

bdd ParProSpotTgba::all_acceptance_conditions() const {
  return bddfalse;
} 

bdd ParProSpotTgba::neg_acceptance_conditions() const {
  return bddtrue;
}

bdd ParProSpotTgba::compute_support_conditions(const spot::state* state) const {
  return bddtrue;
}

bdd ParProSpotTgba::compute_support_variables(const spot::state* state) const {
  return bddtrue;
}

#endif // end of "#ifdef HAVE_SPOT"
