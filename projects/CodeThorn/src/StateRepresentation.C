/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "StateRepresentation.h"
#include "ExprAnalyzer.h"
#include "AType.h"
#include <algorithm>
#include "CollectionOperators.h"
#include "CommandLineOptions.h"

using namespace std;

void InputOutput::recordVariable(OpType op0,VariableId varId) {
  op=op0;
  var=varId;
}

void InputOutput::recordFailedAssert() {
  op=FAILED_ASSERT;
}

void InputOutput::recordConst(OpType op0,AType::ConstIntLattice val) {
  cerr<<"IO with constants not supported yet."<<endl;
  exit(1);
}

string InputOutput::toString() const {
  string str;
  switch(op) {
  case NONE: str="none";break;
  case STDIN_VAR: str="stdin:"+var.variableName();break;
  case STDOUT_VAR: str="stdout:"+var.variableName();break;
  case STDERR_VAR: str="stderr:"+var.variableName();break;
  case STDOUT_CONST: str="out:"+val.toString();break;
  case STDERR_CONST: str="out:"+val.toString();break;
  case FAILED_ASSERT: str="failedassert";break;
  default:
	cerr<<"FATAL ERROR: unknown IO operation abstraction.";
	exit(1);
  }
  return str;
}

bool operator<(const InputOutput& c1, const InputOutput& c2) {
  return c1.op<c2.op || ((c1.op==c2.op) && (c1.var<c2.var)) || ((c1.var==c2.var) && (AType::strictWeakOrderingIsSmaller(c1.val,c2.val)));
}

bool operator==(const InputOutput& c1, const InputOutput& c2) {
  return c1.op==c2.op && c1.var==c2.var && (AType::strictWeakOrderingIsEqual(c1.val,c2.val));
}

bool operator!=(const InputOutput& c1, const InputOutput& c2) {
  return !(c1==c2);
}

string State::toString() const {
  stringstream ss;
  ss << "("<<this << "{";
  for(State::const_iterator j=begin();j!=end();++j) {
    ss << "("<<(*j).first.longVariableName()<<","<<varValueToString((*j).first)<<") ";
  }
  ss<<"})";
  return ss.str();
}

void State::deleteVar(VariableId varId) {
  for(State::iterator i=begin();i!=end();++i) {
	if((*i).first==varId)
	  erase(i);
  }
}

bool State::varExists(VariableId varId) const {
  State::const_iterator i=find(varId);
  return !(i==end());
}

bool State::varIsConst(VariableId varId) const {
  State::const_iterator i=find(varId);
  if(i!=end()) {
	AValue val=(*i).second.getValue();
	return val.isConstInt();
  } else {
	throw "Error: State::varIsConst : variable does not exist.";
  }
}

string State::varValueToString(VariableId varId) const {
  stringstream ss;
  AValue val=((*(const_cast<State*>(this)))[varId]).getValue();
  return val.toString();
}

StateId StateSet::stateId(const State* state) {
  return stateId(*state);
}

StateId StateSet::stateId(const State state) {
  StateId id=0;
  for(StateSet::iterator i=begin();i!=end();++i) {
	if(state==*i)
	  return id;
	id++;
  }
  return NO_STATE;
}

string StateSet::stateIdString(const State* state) {
  stringstream ss;
  ss<<stateId(state);
  return ss.str();
}

const State* StateSet::processNewState(State& s) {
  ProcessingResult res=processState(s);
  assert(res.first==false);
  return res.second;
}

const State* StateSet::processNewOrExistingState(State& s) {
  ProcessingResult res=StateSet::processState(s);
  return res.second;
}

StateSet::ProcessingResult StateSet::processState(State& s) {
  if(const State* existingStatePtr=statePtr(s)) {
	assert(existingStatePtr);
	return make_pair(true,existingStatePtr);
  } else {
	push_back(s);
	const State* existingStatePtr=statePtr(s);
	assert(existingStatePtr);
	return make_pair(false,existingStatePtr);
  }
  assert(0);
}

const State* StateSet::statePtr(State& s) {
#if 1
  for(StateSet::iterator i=begin();i!=end();++i) {
	if(*i==s)
	  return &*i;
  }
  return 0;
#else
  StateSet::iterator i=find(s);
  if(i==end())
	return 0;
  else
	return &*i;
#endif
}

bool StateSet::stateExists(State& s) {
  return statePtr(s)!=0;
}

string StateSet::toString() {
  stringstream ss;
  ss << "@"<<this<<": StateSet={";
  int si=0;
  for(StateSet::iterator i=begin();i!=end();++i) {
	if(i!=begin())
	  ss<<", ";
    ss << "S"<<si++<<": "<<(*i).toString();
  }
  ss << "}";
  return ss.str();
}

// define order for EState elements (necessary for EStateSet)
#if 1
bool operator<(const EState& c1, const EState& c2) {
  return (c1.label<c2.label) || ((c1.label==c2.label) && (c1.state<c2.state)) || (c1.state==c2.state && c1.constraints<c2.constraints) || ((c1.constraints==c2.constraints) && (c1.io<c2.io));
}
#endif

bool operator==(const EState& c1, const EState& c2) {
  bool result=((c1.label==c2.label) && (c1.state==c2.state));
  if(boolOptions["precision-equality-constraints"])
	result = result && (c1.constraints==c2.constraints);
  if(boolOptions["precision-equality-io"])
	result = result && (c1.io==c2.io);
  return result;
}

bool operator!=(const EState& c1, const EState& c2) {
  return !(c1==c2);
}


EStateId EStateSet::eStateId(const EState* eState) {
  return eStateId(*eState);
}

EStateId EStateSet::eStateId(const EState eState) {
  EStateId id=0;
  for(EStateSet::iterator i=begin();i!=end();++i) {
	if(eState==*i)
	  return id;
	id++;
  }
  return NO_ESTATE;
}

string EStateSet::eStateIdString(const EState* eState) {
  stringstream ss;
  ss<<eStateId(eState);
  return ss.str();
}

const EState* EStateSet::processNewEState(EState& s) {
  ProcessingResult res=processEState(s);
  assert(res.first==false);
  return res.second;
}

const EState* EStateSet::processNewOrExistingEState(EState& s) {
  ProcessingResult res=processEState(s);
  return res.second;
}

EStateSet::ProcessingResult EStateSet::processEState(EState s) {
  if(const EState* existingEStatePtr=eStatePtr(s)) {
	return make_pair(true,existingEStatePtr);
  } else {
	push_back(s);
	const EState* existingEStatePtr=eStatePtr(s);
	assert(existingEStatePtr);
	return make_pair(false,existingEStatePtr);
  }
  assert(0);
}

bool EStateSet::eStateExists(EState& s) {
  return eStatePtr(s)!=0;
}

void EStateSet::addNewEState(EState newEState) { 
  assert(eStatePtr(newEState)==0);
  push_back(newEState);
}

int EStateSet::numberOfIoTypeEStates(InputOutput::OpType op) {
  int counter=0;
  for(EStateSet::iterator i=begin();i!=end();++i) {
	if((*i).io.op==op)
	  counter++;
  }
  return counter;
} 

const EState* EStateSet::eStatePtr(EState& s) {
#if 1
  // we use this as the find algorithm cannot be used for this data structure yet.
  for(EStateSet::iterator i=begin();i!=end();++i) {
	if(*i==s)
	  return &*i;
  }
  return 0;
#else
  EStateSet::iterator i=find(s);
  if(i==end())
	return 0;
  else
	return &*i;
#endif
}

set<const EState*> TransitionGraph::transitionSourceEStateSetOfLabel(Label lab) {
  set<const EState*> eStateSet;
  for(TransitionGraph::iterator j=begin();j!=end();++j) {
	if((*j).source->label==lab)
	  eStateSet.insert((*j).source);
  }
  return eStateSet;
}

set<const EState*> TransitionGraph::eStateSetOfLabel(Label lab) {
  set<const EState*> eStateSet;
  for(TransitionGraph::iterator j=begin();j!=end();++j) {
	if((*j).source->label==lab)
	  eStateSet.insert((*j).source);
	if((*j).target->label==lab)
	  eStateSet.insert((*j).target);
  }
  return eStateSet;
}

string EState::toString() const {
  stringstream ss;
  ss << "EState";
  ss << "(label="<<label<<", state=";
  if(state)
	ss <<state->toString();
  else
	ss <<"NULL";
  ss <<", constraints="<<constraints.toString();
  ss <<", io="<<io.toString();
  ss<<")";
  return ss.str();
}

string EStateList::toString() {
  stringstream ss;
  ss<<"EStateWorkList=[";
  for(EStateList::iterator i=begin();
	  i!=end();
	  ++i) {
	ss<<(*i).toString()<<",";
  }
  ss<<"]";
  return ss.str();
}

string EStateSet::toString() {
  stringstream ss;
  ss<<"EStateSet={";
  for(EStateSet::iterator i=begin();
	  i!=end();
	  ++i) {
	ss<<(*i).toString()<<",";
  }
  ss<<"}";
  return ss.str();
}

