#include "sage3basic.h"
#include "rose_config.h"
#ifdef HAVE_SPOT

#include "SpotSuccIter.h"

using namespace CodeThorn;

SpotSuccIter::SpotSuccIter(TransitionGraph& tg, const EState& state, 
   				  const std::map<int, int>& p2d, std::set<int> inVars, std::set<int> outVars) 
					: stg(tg), propNum2DictNum(p2d), ltlInVars(inVars), ltlOutVars(outVars) {
  outEdges = stg.outEdges(&state);
}

void SpotSuccIter::first() {
  iter = outEdges.begin();
}

void SpotSuccIter::next() {
  ROSE_ASSERT(!done());
  iter++;
}

bool SpotSuccIter::done() const {
  return iter == outEdges.end();
}

spot::state* SpotSuccIter::current_state() const {
  ROSE_ASSERT(!done());
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

  ROSE_ASSERT(myTarget);
  InputOutput io =myTarget->io; 
  if (io.isStdErrIO() || io.isFailedAssertIO()) { // Error states are ignored and are treated as dead ends. 
                                                  // They do not contain LTL specific behavior and no states follow them.
    return bddfalse;
  } 

  if(io.isNonIO()) {
    cerr<<"ERROR: non-IO state in SpotSuccIter::generateSpotTransition: IO:"<<t.target->io.toString()<<endl;
    exit(1);
  }

  AbstractValue myIOVal=myTarget->determineUniqueIOValue();
  if(!myIOVal.isConstInt()) {
    cerr<<myTarget->toString()<<endl;
    cerr<<"Error: IOVal is not constant (determineUniqueIOValue could determine a unique IO value)."<<endl;
    cerr<<"myIOVal: "<<myIOVal.toString()<<endl;
    ROSE_ASSERT(myIOVal.isConstInt());
  }
  // check if there exists a single input or output value (remove for support of symbolic analysis)
  if(myTarget->io.isStdInIO()||myTarget->io.isStdOutIO()) {
    if(!myIOVal.isConstInt()) {
      cerr<<"Error: IOVal is NOT const.\n"<<"EState: "<<myTarget->toString()<<endl;
      exit(1);
    }
  }
  //determine possible input / output values at target state
  //cout << "DEBUG: generateSpotTransition. target state's label: " << myTarget->label() << "   target state's InputOutput operator: " << myTarget->io.op << endl;
  int ioValAtTarget = myIOVal.getIntValue();

#if 0
  //convert the single input/output value into set representations (for future symbolic analysis mode)
  std::set<int> possibleInputValues, possibleOutputValues;
  if(myTarget->io.isStdInIO()) {
    possibleInputValues.insert(ioValAtTarget);
  } else if (myTarget->io.isStdOutIO()) {
    possibleOutputValues.insert(ioValAtTarget);
  } else {
    cerr << "ERROR: cannot create a TGBA transition to a state that is neither input nor output."  << endl;
    exit(1);
  }
  //combine input and output part of the transition condition
  transCond &= conjunctivePredicate(possibleInputValues, ltlInVars);
  transCond &= conjunctivePredicate(possibleOutputValues, ltlOutVars); 
#else
  //old version working with one input or output value only
  //add the target state's I/O value as a non-negated proposition
  transCond &= bdd_ithvar(propNum2DictNum.at(ioValAtTarget));
  //all other I/O values do not appear at the target state, add them in negated form
  for (std::set<int>::iterator i=ltlInVars.begin(); i !=ltlInVars.end(); ++i) {
    if ((*i) != ioValAtTarget) {
      transCond &= bdd_nithvar(propNum2DictNum.at(*i));
    }
  }
  for (std::set<int>::iterator i=ltlOutVars.begin(); i !=ltlOutVars.end(); ++i) {
    if ((*i) != ioValAtTarget) {
      transCond &= bdd_nithvar(propNum2DictNum.at(*i));
    }
  }
#endif
  myTarget = NULL;
  return transCond;
}

bdd SpotSuccIter::conjunctivePredicate(std::set<int> nonNegated, std::set<int> alphabet) const {
  bdd result = bddtrue;
  for (std::set<int>::iterator i=alphabet.begin(); i !=alphabet.end(); ++i) {
    if (nonNegated.find(*i) != nonNegated.end()) {
      result &= bdd_ithvar(propNum2DictNum.at(*i));
    } else {
      result &= bdd_nithvar(propNum2DictNum.at(*i));
    }
  }	
  return result;
}

#endif // end of "#ifdef HAVE_SPOT"
