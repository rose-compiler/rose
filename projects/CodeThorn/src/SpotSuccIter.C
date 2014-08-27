#include "sage3basic.h"
#include "SpotSuccIter.h"

using namespace CodeThorn;

SpotSuccIter::SpotSuccIter(TransitionGraph& tg, const EState& state, 
   				  const std::map<int, int>& p2d, std::set<int> propNumbers) 
					: stg(tg), propNum2DictNum(p2d), propNums(propNumbers) {
  outEdges = stg.outEdges(&state);
}

void SpotSuccIter::first() {
  iter = outEdges.begin();
}

void SpotSuccIter::next() {
  assert(!done());
  iter++;
}

bool SpotSuccIter::done() const {
  return iter == outEdges.end();
}

spot::state* SpotSuccIter::current_state() const {
  assert(!done());
  SpotState* nextState = new SpotState( *((*iter)->target) );  //dereferencing three times: 1.) iterator 2.) pointer to Transition 3.) EState* to reference
  return nextState;
}

bdd SpotSuccIter::current_condition() const {
  assert(!done()); 
  return generateSpotTransition( *(*iter) );
}

bdd SpotSuccIter::current_acceptance_conditions() const {
  assert(!done());
  return bddfalse;
}

bdd SpotSuccIter::generateSpotTransition(const Transition& t) const {
  bdd transCond = bddtrue;
  const EState* myTarget=t.target;
  InputOutput io =myTarget->io; 
  AType::ConstIntLattice myIOVal=myTarget->determineUniqueIOValue();

  // checks only meaningful when CodeThorn is run in symbolic execution mode (without input alphabet)
  /*
  // generate transition condition
  if(myTarget->io.isStdInIO()||myTarget->io.isStdOutIO()) {
    if(!myIOVal.isConstInt()) {
      //assert(myIOVal.isConstInt());
      cerr<<"Error: IOVal is NOT const.\n"<<"EState: "<<myTarget->toString()<<endl;
      exit(1);
    }
  }
  */
  int ioValAtTarget = myIOVal.getIntValue();
  //add the target state's I/O value as a non-negated proposition
  transCond &= bdd_ithvar(propNum2DictNum.at(ioValAtTarget));
  //all other I/O values do not appear at the target state, add them in negated form
  for (std::set<int>::iterator i=propNums.begin(); i !=propNums.end(); ++i) {
    if ((*i) != ioValAtTarget) {
      transCond &= bdd_nithvar(propNum2DictNum.at(*i));
    }
  }
  myTarget = NULL;
  return transCond;
}
