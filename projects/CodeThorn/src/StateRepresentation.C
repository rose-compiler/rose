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

#if 1
bool operator<(const Constraint& c1, const Constraint& c2) {
  if(c1.var<c2.var)
	return true;
  if(c1.var==c2.var && c1.op<c2.op)
	return true;
  if(c1.op==c2.op && (c1.intVal<c2.intVal)) 
	return true;
  return false;
}

bool operator==(const Constraint& c1, const Constraint& c2) {
  return c1.var==c2.var && c1.op==c2.op && (c1.intVal==c2.intVal);
}

bool operator!=(const Constraint& c1, const Constraint& c2) {
  return !(c1==c2);
}
#endif

Constraint::Constraint(ConstraintOp op0,VariableId lhs, AValue rhs):op(op0),var(lhs),intVal(CppCapsuleAValue(rhs)) {
} 
Constraint::Constraint(ConstraintOp op0,VariableId lhs, CppCapsuleAValue rhs):op(op0),var(lhs),intVal(rhs) {
} 

string Constraint::toString() const {
  stringstream ss;
  ss<<var.longVariableName()<<(*this).opToString()<<intVal.toString();
  return ss.str();
}

string Constraint::opToString() const {
  switch(op) {
  case EQ_VAR_CONST: return "==";
  case NEQ_VAR_CONST: return "!=";
  case DEQ_VAR_CONST: return "##";
  default:
	cerr << "Error: Constraint: unkown operator"<<endl;
	exit(1);
  }
}

bool ConstraintSet::deqConstraintExists() {
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).op==Constraint::DEQ_VAR_CONST)
	  return true;
  }
  return false;
}

ConstraintSet ConstraintSet::invertedConstraints() {
  ConstraintSet result;
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	Constraint c=*i;
	switch(c.op) {
	case Constraint::EQ_VAR_CONST:
	  result.insert(Constraint(Constraint::NEQ_VAR_CONST,c.var,c.intVal));
	  break;
	case Constraint::NEQ_VAR_CONST:
	  result.insert(Constraint(Constraint::EQ_VAR_CONST,c.var,c.intVal));
	  break;
	case Constraint::DEQ_VAR_CONST:
	  result.insert(Constraint(Constraint::DEQ_VAR_CONST,c.var,c.intVal)); // unchanged
	  break;
	default:
	  throw "Error: ConstraintSet::invertedConstraints: unknown operator.";
	}
  }
  return result;
}

void ConstraintSet::invertConstraints() {
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	Constraint c=*i;
	switch(c.op) {
	case Constraint::EQ_VAR_CONST:
	  // c ist const because it is an element in a sorted set
	  erase(c); // we remove c from the set (but c remains unchanged and available)
	  insert(Constraint(Constraint::NEQ_VAR_CONST,c.var,c.intVal));
	  break;
	case Constraint::NEQ_VAR_CONST:
	  // c ist const because it is an element in a sorted set
	  erase(c); // we remove c from the set (but c remains unchanged and available)
	  insert(Constraint(Constraint::EQ_VAR_CONST,c.var,c.intVal));
	  break;
	case Constraint::DEQ_VAR_CONST:
	  // remains unchanged
	  break;
	default:
	  throw "Error: ConstraintSet::invertedConstraints: unknown operator.";
	}
  }
}

void  ConstraintSet::duplicateConstraints(VariableId lhsVarId, VariableId rhsVarId) {
  deleteConstraints(lhsVarId);
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	Constraint c=*i;
	if(c.var==rhsVarId)
	  insert(Constraint(c.op,lhsVarId,c.intVal));
  }
}


void ConstraintSet::insert(Constraint c) {
  // we have to look which version of constraint already exists (it can only be at most one)
  switch(c.op) {
  case Constraint::EQ_VAR_CONST: {
	if(constraintExists(Constraint::DEQ_VAR_CONST,c.var,c.intVal))
	  return;
	if(constraintExists(Constraint::NEQ_VAR_CONST,c.var,c.intVal)) {
	  erase(Constraint(Constraint::NEQ_VAR_CONST,c.var,c.intVal));
	  set<Constraint>::insert(Constraint(Constraint::DEQ_VAR_CONST,c.var,c.intVal));
	  return;
	}
	ConstraintSet::iterator i=findSpecific(Constraint::EQ_VAR_CONST,c.var);
	if(i!=end()) {
	  if(!((*i).intVal==c.intVal)) {
		// other x==num exists with num!=c.num ==> DEQ
		erase(*i);
		set<Constraint>::insert(Constraint(Constraint::DEQ_VAR_CONST,c.var,c.intVal));
		return;
	  } else {
		// nothing todo (same constraint already exists)
		return;
	  }
	}
	// all remaining constraints can be removed (can only be inqualities)
	deleteConstraints(c.var);
	set<Constraint>::insert(Constraint(Constraint::EQ_VAR_CONST,c.var,c.intVal));
	return;
  }
  case Constraint::NEQ_VAR_CONST: {
	if(constraintExists(Constraint::DEQ_VAR_CONST,c.var,c.intVal))
	  return;
	if(constraintExists(Constraint::EQ_VAR_CONST,c.var,c.intVal)) {
	  erase(Constraint(Constraint::EQ_VAR_CONST,c.var,c.intVal));
	  set<Constraint>::insert(Constraint(Constraint::DEQ_VAR_CONST,c.var,c.intVal));
	  return;
	}
	break;
  }
  case Constraint::DEQ_VAR_CONST:
	erase(Constraint(Constraint::EQ_VAR_CONST,c.var,c.intVal));
	erase(Constraint(Constraint::NEQ_VAR_CONST,c.var,c.intVal));
	set<Constraint>::insert(Constraint(Constraint::DEQ_VAR_CONST,c.var,c.intVal));
	return;
  default:
	throw "INTERNAL ERROR: ConstraintSet::insert: unknown operator.";
  }
  // all other cases (no constraint already in the set is effected)
  set<Constraint>::insert(c);
}

ConstraintSet ConstraintSet::operator+(ConstraintSet& s2) {
  ConstraintSet result;
  result=*this;
  for(ConstraintSet::iterator i2=s2.begin();i2!=s2.end();++i2)
	result.insert(*i2);
  return result;
}

ConstraintSet& ConstraintSet::operator+=(ConstraintSet& s2) {
  for(ConstraintSet::iterator i2=s2.begin();i2!=s2.end();++i2)
	insert(*i2);
  return *this;
}

bool ConstraintSet::constraintExists(Constraint::ConstraintOp op, VariableId varId, CppCapsuleAValue intVal) const { 
  Constraint tmp(op,varId,intVal);
  return constraintExists(tmp);
}
bool ConstraintSet::constraintExists(Constraint::ConstraintOp op, VariableId varId, AValue intVal) const { 
  return constraintExists(op,varId,CppCapsuleAValue(intVal));
}
bool ConstraintSet::constraintExists(const Constraint& tmp) const { 
#if 0
  // we use this as the find algorithm does not properly work yet (TODO: investigate)
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if(*i==tmp)
	  return true;
  }
  return false;
#else
  ConstraintSet::const_iterator foundElemIter=find(tmp);
  return foundElemIter!=end();
#endif
}

ConstraintSet ConstraintSet::findSpecificSet(Constraint::ConstraintOp op, VariableId varId) const {
  ConstraintSet cs;
  // find op-constraint for variable varname
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).var==varId && (*i).op==op)
	  cs.insert(Constraint((*i).op,(*i).var,(*i).intVal));
  }
  return cs;
}

ConstraintSet::iterator ConstraintSet::findSpecific(Constraint::ConstraintOp op, VariableId varId) const {
  // find op-constraint for variable varname
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).var==varId && (*i).op==op)
	  return i;
  }
  return end();
}

AType::ConstIntLattice ConstraintSet::varConstIntLatticeValue(const VariableId varId) const {
  AType::ConstIntLattice c;
  ConstraintSet::iterator i=findSpecific(Constraint::EQ_VAR_CONST,varId);
  if(i==end()) {
	// no EQ_VAR_CONST constraint for this variable
	return AType::ConstIntLattice(AType::Top());
  } else {
	return AType::ConstIntLattice((*i).intVal.getValue());
  }
}


ConstraintSet ConstraintSet::deleteVarConstraints(VariableId varId) {
  deleteConstraints(varId);
  return *this;
}

void ConstraintSet::deleteConstraints(VariableId varId) {
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).var==varId)
	  erase(i);
  }
}

string ConstraintSet::toString() const {
  stringstream ss;
  ss<<"{";
  for(set<Constraint>::iterator i=begin();i!=end();++i) {
	if(i!=begin()) 
	  ss<<",";
	ss<<(*i).toString();
  }
  ss<<"}";
  return ss.str();
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
  return (c1.label==c2.label) && (c1.state==c2.state) && (c1.constraints==c2.constraints) && (c1.io==c2.io);
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

#if 0
const EState* EStateSet::processEState(EState s) {
  if(const EState* existingEStatePtr=eStatePtr(s)) {
	return existingEStatePtr;
  } else {
	push_back(s);
	const EState* existingEStatePtr=eStatePtr(s);
	assert(existingEStatePtr);
	return existingEStatePtr;
  }
  assert(0);
}
#endif

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

