/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "StateRepresentation.h"
#include "ExprAnalyzer.h"
#include "AType.h"
#include <algorithm>

using namespace std;

bool operator<(const Constraint& c1, const Constraint& c2) {
  if(c1.var<c2.var)
	return true;
  if(c1.var==c2.var && c1.op<c2.op)
	return true;
  if(c1.op==c2.op && c1.intVal </*=*/ c2.intVal)
	return true;
  return false;
}

bool operator==(const Constraint& c1, const Constraint& c2) {
  return c1.intVal==c2.intVal && c1.op==c2.op && c1.var==c2.var;
}

bool operator!=(const Constraint& c1, const Constraint& c2) {
  return !(c1==c2);
}
Constraint::Constraint(ConstraintOp op0,VariableName lhs, AValue rhs):op(op0),var(lhs),intVal(rhs){} 
string Constraint::toString() const {
  stringstream ss;
  ss<<var<<(*this).opToString()<<intVal;
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

void ConstraintSet::insert(Constraint c) {
  // we have to look which version of constraint already exists (it can only be at most one)
  switch(c.op) {
  case Constraint::EQ_VAR_CONST:
	if(constraintExists(Constraint::DEQ_VAR_CONST,c.var,c.intVal))
	  return;
	if(constraintExists(Constraint::NEQ_VAR_CONST,c.var,c.intVal)) {
	  erase(Constraint(Constraint::NEQ_VAR_CONST,c.var,c.intVal));
	  set<Constraint>::insert(Constraint(Constraint::DEQ_VAR_CONST,c.var,c.intVal));
	  return;
	}
	break;
  case Constraint::NEQ_VAR_CONST:
	if(constraintExists(Constraint::DEQ_VAR_CONST,c.var,c.intVal))
	  return;
	if(constraintExists(Constraint::EQ_VAR_CONST,c.var,c.intVal)) {
	  erase(Constraint(Constraint::EQ_VAR_CONST,c.var,c.intVal));
	  set<Constraint>::insert(Constraint(Constraint::DEQ_VAR_CONST,c.var,c.intVal));
	  return;
	}
	break;
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

bool ConstraintSet::constraintExists(Constraint::ConstraintOp op, VariableName varName, AValue intVal) { 
  Constraint tmp(op,varName,intVal);
  return constraintExists(tmp);
}
bool ConstraintSet::constraintExists(Constraint& tmp) { 
  ConstraintSet::const_iterator foundElemIter=find(tmp);
  return foundElemIter!=end();
}

ConstraintSet::iterator ConstraintSet::findSpecific(Constraint::ConstraintOp op, VariableName varName) const {
  // find op-constraint for variable varname
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).var==varName && (*i).op==op)
	  return i;
  }
  return end();
}

AType::ConstIntLattice ConstraintSet::varConstIntLatticeValue(VariableName varName) const {
  AType::ConstIntLattice c;
  ConstraintSet::iterator i=findSpecific(Constraint::EQ_VAR_CONST,varName);
  if(i==end()) {
	// no EQ_VAR_CONST constraint for this variable
	return AType::ConstIntLattice(AType::Top());
  } else {
	return AType::ConstIntLattice((*i).intVal);
  }
}

ConstraintSet ConstraintSet::deleteVarConstraints(VariableName varName) {
  for(ConstraintSet::iterator i=begin();i!=end();++i) {
	if((*i).var==varName)
	  erase(i);
  }
  return *this;
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
    ss << "("<<(*j).first<<","<<varValueToString((*j).first)<<") ";
  }
  ss<<"})";
  return ss.str();
}

void State::deleteVar(VariableName varName) {
  for(State::iterator i=begin();i!=end();++i) {
	if((*i).first==varName)
	  erase(i);
  }
}

bool State::varExists(VariableName varname) const {
  State::const_iterator i=find(varname);
  return !(i==end());
}

bool State::varIsConst(VariableName varname) const {
  State::const_iterator i=find(varname);
  if(i!=end()) {
	int val=(*i).second;
	return val!=ANALYZER_INT_TOP && val!=ANALYZER_INT_BOT;
  } else {
	throw "Error: State::varIsConst : variable does not exist.";
  }
}

string State::varValueToString(string varname) const {
  stringstream ss;
  AValue val=(*(const_cast<State*>(this)))[varname];
  if(val==ANALYZER_INT_TOP) return "top";
  if(val==ANALYZER_INT_BOT) return "bot";
  ss << val;
  return ss.str();
}

const State* StateSet::statePtr(State& s) {
  StateSet::iterator i=find(s);
  if(i==end())
	return 0;
  else
	return &*i;
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
bool operator<(const EState& c1, const EState& c2) {
  return c1.label<c2.label || (c1.label==c2.label && c1.state<c2.state) || (c1.state==c2.state && c1.constraints<c2.constraints);
}

bool operator==(const EState& c1, const EState& c2) {
  return c1.label==c2.label && c1.state==c2.state && c1.constraints==c2.constraints;
}

bool operator!=(const EState& c1, const EState& c2) {
  return !(c1==c2);
}

bool EStateSet::eStateExists(EState& s) {
  return eStatePtr(s)!=0;
}

const EState* EStateSet::eStatePtr(EState& s) {
#if 1
  // we use this as the find algorithm does not properly work yet (TODO: investigate)
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
  ss << "@"<<this << " EState";
  ss << "(label="<<label<<", state=";
  if(state)
	ss <<state->toString();
  else
	ss <<"NULL";
  ss <<", constraints="<<constraints.toString();
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

