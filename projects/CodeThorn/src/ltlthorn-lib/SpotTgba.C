#include "sage3basic.h"
#include "rose_config.h"
#ifdef HAVE_SPOT

#include "SpotTgba.h"

using namespace CodeThorn;

SpotTgba::SpotTgba(TransitionGraph& ctstg, const spot::ltl::atomic_prop_set& sap,
                   spot::bdd_dict& dic, LtlRersMapping ltlRersMapping) 
  :stg(ctstg), dict(dic), _ltlRersMapping(ltlRersMapping) {
  //register the atomic propositions used by this TGBA. They are passed in as a set
  // in the current implementation. Retrieving them from CodeThorn's STG might be possible
  spot::ltl::atomic_prop_set::iterator i;
  for(i=sap.begin(); i!=sap.end(); ++i) {
    propNum2DictNum[propName2Int((*i)->name())] =
                      dict.register_proposition(*i, this);
    //cout<<"DEBUG: SpotTgba Mapping: "<<(*i)->name()<<"="<<propName2Int((*i)->name())<<" <=> register_prop:"<<*i<<endl;
  }
}

SpotTgba::~SpotTgba(){
  //unregister atomic propositions from the dictionary used by this automaton
  dict.unregister_all_my_variables(this);
};

spot::state* SpotTgba::get_init_state() const {
	const EState* startState = stg.getStartEState();
	SpotState* init_state = new SpotState( *startState);
	return init_state;
}  
	

spot::tgba_succ_iterator* SpotTgba::succ_iter (const spot::state* local_state,
        			const spot::state*, const spot::tgba*) const {
  const SpotState* state = dynamic_cast<const SpotState*>(local_state);
  assert(state);
  auto inputSet=_ltlRersMapping.getInputValueSet();
  auto outputSet=_ltlRersMapping.getOutputValueSet();
  return new SpotSuccIter(stg, (state->getEState()), propNum2DictNum, inputSet, outputSet);
} 

spot::bdd_dict* SpotTgba::get_dict() const {
  return &dict;
}

std::string SpotTgba::format_state(const spot::state* state) const {
  const SpotState* toBeFormatted = dynamic_cast<const SpotState*>(state);
  assert(toBeFormatted);
  return toBeFormatted->toString();
}

bdd SpotTgba::all_acceptance_conditions() const {
  return bddfalse;
} 

bdd SpotTgba::neg_acceptance_conditions() const {
  return bddtrue;
}

bdd SpotTgba::compute_support_conditions(const spot::state* state) const {
  return bddtrue;
}

bdd SpotTgba::compute_support_variables(const spot::state* state) const {
  return bddtrue;
}

int SpotTgba::propName2Int(std::string propName) const {
  return _ltlRersMapping.getValue(propName.at(1));
  /* old version without mapping:
  char id = propName.at(1);
  return ((int) id ) - ((int) 'A') + 1;
  */
}

#endif // end of "#ifdef HAVE_SPOT"
