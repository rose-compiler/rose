/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "StateRepresentations.h"
#include "ExprAnalyzer.h"
#include "AType.h"
#include <algorithm>
#include "CollectionOperators.h"
#include "CommandLineOptions.h"

// it is not necessary to define comparison-ops for Pstate, but
// the ordering appears to be implementation dependent (but consistent)

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
  if(c1.op!=c2.op)
	return c1.op<c2.op;
  if(!(c1.var==c2.var))
	return c1.var<c2.var;
  return AType::strictWeakOrderingIsSmaller(c1.val,c2.val);
}

bool operator==(const InputOutput& c1, const InputOutput& c2) {
  return c1.op==c2.op && c1.var==c2.var && (AType::strictWeakOrderingIsEqual(c1.val,c2.val));
}

bool operator!=(const InputOutput& c1, const InputOutput& c2) {
  return !(c1==c2);
}

string PState::toString() const {
  stringstream ss;
  ss << "State="<< "{";
  for(PState::const_iterator j=begin();j!=end();++j) {
	if(j!=begin()) ss<<", ";
	ss<<"(";
    ss <<(*j).first.longVariableName();
#if 0
	ss<<"->";
#else
	ss<<",";
#endif
	ss<<varValueToString((*j).first);
	ss<<")";
  }
  ss<<"}";
  return ss.str();
}

long PState::memorySize() const {
  long mem=0;
  for(PState::const_iterator i=begin();i!=end();++i) {
	mem+=sizeof(*i);
  }
  return mem+sizeof(*this);
}
long EState::memorySize() const {
  return sizeof(*this);
}

void PState::deleteVar(VariableId varId) {
  for(PState::iterator i=begin();i!=end();++i) {
	if((*i).first==varId)
	  erase(i);
  }
}

bool PState::varExists(VariableId varId) const {
  PState::const_iterator i=find(varId);
  return !(i==end());
}

bool PState::varIsConst(VariableId varId) const {
  PState::const_iterator i=find(varId);
  if(i!=end()) {
	AValue val=(*i).second.getValue();
	return val.isConstInt();
  } else {
	throw "Error: PState::varIsConst : variable does not exist.";
  }
}

string PState::varValueToString(VariableId varId) const {
  stringstream ss;
  AValue val=((*(const_cast<PState*>(this)))[varId]).getValue();
  return val.toString();
}

PStateId PStateSet::pstateId(const PState* pstate) {
  return pstateId(*pstate);
}

PStateId PStateSet::pstateId(const PState pstate) {
  PStateId id=0;
  for(PStateSet::iterator i=begin();i!=end();++i) {
	if(pstate==*i)
	  return id;
	id++;
  }
  return NO_STATE;
}

string PStateSet::pstateIdString(const PState* pstate) {
  stringstream ss;
  ss<<pstateId(pstate);
  return ss.str();
}

string PStateSet::toString() {
  stringstream ss;
  ss << "@"<<this<<": PStateSet={";
  int si=0;
  for(PStateSet::iterator i=begin();i!=end();++i) {
	if(i!=begin())
	  ss<<", ";
    ss << "S"<<si++<<": "<<(*i).toString();
  }
  ss << "}";
  return ss.str();
}

// define order for EState elements (necessary for EStateSet)
bool operator<(const EState& e1, const EState& e2) {
  if(e1.label()!=e2.label())
	return (e1.label()<e2.label());
  if(e1.pstate()!=e2.pstate())
	return (e1.pstate()<e2.pstate());
  if(e1.constraints()!=e2.constraints())
	return (e1.constraints()<e2.constraints());
  return e1.io<e2.io;
}

bool operator==(const EState& c1, const EState& c2) {
  bool result=((c1.label()==c2.label()) && (c1.pstate()==c2.pstate()));
  if(boolOptions["precision-equality-constraints"])
	result = result && (c1.constraints()==c2.constraints());
  if(boolOptions["precision-equality-io"])
	result = result && (c1.io==c2.io);
  return result;
}

bool operator!=(const EState& c1, const EState& c2) {
  return !(c1==c2);
}

EStateId EStateSet::estateId(const EState* estate) const {
  return estateId(*estate);
}

EStateId EStateSet::estateId(const EState estate) const {
  EStateId id=0;
  for(EStateSet::iterator i=begin();i!=end();++i) {
	if(estate==*i)
	  return id;
	id++;
  }
  return NO_ESTATE;
}

Transition TransitionGraph::getStartTransition() {
  for(TransitionGraph::iterator i=begin();i!=end();++i) {
	if((*i).source->label()==_startLabel)
	  return *i;
  }
}

string EStateSet::estateIdString(const EState* estate) const {
  stringstream ss;
  ss<<estateId(estate);
  return ss.str();
}

int EStateSet::numberOfIoTypeEStates(InputOutput::OpType op) const {
  int counter=0;
  for(EStateSet::iterator i=begin();i!=end();++i) {
	if((*i).io.op==op)
	  counter++;
  }
  return counter;
} 

string Transition::toString() const {
  string s1=source->toString();
  string s2=edge.toString();
  string s3=target->toString();
  return string("(")+s1+", "+s2+", "+s3+")";
}

LabelSet TransitionGraph::labelSetOfIoOperations(InputOutput::OpType op) {
  LabelSet lset;
  // the target node records the effect of the edge-operation on the source node.
  for(TransitionGraph::iterator i=begin();i!=end();++i) {
	if((*i).target->io.op==op) {
	  lset.insert((*i).source->label());
	}
  }
  return lset;
} 

void TransitionGraph::add(Transition trans) {
  #pragma omp critical
  {
	insert(trans);
  }
}

bool operator==(const Transition& t1, const Transition& t2) {
  return t1.source==t2.source && t1.edge==t2.edge && t1.target==t2.target;
}

bool operator!=(const Transition& t1, const Transition& t2) {
  return !(t1==t2);
}

bool operator<(const Transition& t1, const Transition& t2) {
  if(t1.source!=t2.source)
	return t1.source<t2.source;
  if(t1.edge!=t2.edge)
	return t1.edge<t2.edge;
  return t1.target<t2.target;
}


long TransitionGraph::removeDuplicates() {
  long cnt=0;
  set<Transition> s;
#if 0
  // lambda expression is not supported by g++4.4
  remove_if([&](Transition n) {
	  return (s.find(n) == s.end()) ? (s.insert(n), false) : true;
	});
#else
  for(TransitionGraph::iterator i=begin();i!=end();) {
	if(s.find(*i)==s.end()) { 
	  s.insert(*i); 
	  ++i;
	} else {
	  TransitionGraph::iterator i2=i;
	  ++i;
	  erase(i2);
	  cnt++;
	}
  }
#endif
  return cnt;
}


string TransitionGraph::toString() const {
  string s;
  int cnt=0;
  for(TransitionGraph::const_iterator i=begin();i!=end();++i) {
#if 0
	stringstream ss;
	ss<<cnt;
	s+="Transition["+ss.str()+"]=";
#endif
	s+=(*i).toString()+"\n";
	cnt++;
  }
  assert(cnt==size());
  return s;
}

set<const EState*> TransitionGraph::transitionSourceEStateSetOfLabel(Label lab) {
  set<const EState*> estateSet;
  for(TransitionGraph::iterator j=begin();j!=end();++j) {
	if((*j).source->label()==lab)
	  estateSet.insert((*j).source);
  }
  return estateSet;
}

set<const EState*> TransitionGraph::estateSetOfLabel(Label lab) {
  set<const EState*> estateSet;
  for(TransitionGraph::iterator j=begin();j!=end();++j) {
	if((*j).source->label()==lab)
	  estateSet.insert((*j).source);
	if((*j).target->label()==lab)
	  estateSet.insert((*j).target);
  }
  return estateSet;
}

string EState::toString() const {
  stringstream ss;
  ss << "EState";
  ss << "("<<label()<<", ";
  if(pstate())
	ss <<pstate()->toString();
  else
	ss <<"NULL";
  if(constraints()) {
	ss <<", constraints="<<constraints()->toString();
  } else {
	ss <<", NULL";
  }
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

string EStateSet::toString() const {
  stringstream ss;
  ss<<"EStateSet={";
  for(EStateSet::iterator i=begin();
	  i!=end();
	  ++i) {
	ss<<(*i).toString()<<",\n";
  }
  ss<<"}";
  return ss.str();
}

#ifdef USER_DEFINED_PSTATE_COMP
bool operator<(const PState& s1, const PState& s2) {
  if(s1.size()!=s2.size())
	return s1.size()<s2.size();
  PState::const_iterator i=s1.begin();
  PState::const_iterator j=s2.begin();
  while(i!=s1.end() && j!=s2.end()) {
	if(*i!=*j) {
	  return *i<*j;
	} else {
	  ++i;++j;
	}
  }
  assert(i==s1.end() && j==s2.end());
  return false; // both are equal
}
#if 0
bool operator==(const PState& c1, const PState& c2) {
  if(c1.size()==c2.size()) {
	PState::const_iterator i=c1.begin();
	PState::const_iterator j=c2.begin();
	while(i!=c1.end()) {
	  if(!((*i).first==(*j).first))
		return false;
	  if(!((*i).second==(*j).second))
		return false;
	  ++i;++j;
	}
	return true;
  } else {
	return false;
  }
}

bool operator!=(const PState& c1, const PState& c2) {
  return !(c1==c2);
}
#endif
#endif
