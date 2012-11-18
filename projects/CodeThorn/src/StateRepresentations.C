/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "StateRepresentations.h"
#include <algorithm>
#include "ExprAnalyzer.h"
#include "AType.h"
#include "CollectionOperators.h"
#include "CommandLineOptions.h"

// it is not necessary to define comparison-ops for Pstate, but
// the ordering appears to be implementation dependent (but consistent)

using namespace std;
using namespace CodeThorn;

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

bool CodeThorn::operator<(const InputOutput& c1, const InputOutput& c2) {
  if(c1.op!=c2.op)
	return c1.op<c2.op;
  if(!(c1.var==c2.var))
	return c1.var<c2.var;
  return AType::strictWeakOrderingIsSmaller(c1.val,c2.val);
}

bool CodeThorn::operator==(const InputOutput& c1, const InputOutput& c2) {
  return c1.op==c2.op && c1.var==c2.var && (AType::strictWeakOrderingIsEqual(c1.val,c2.val));
}

bool CodeThorn::operator!=(const InputOutput& c1, const InputOutput& c2) {
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
  PState::iterator i=begin();
  while(i!=end()) {
    if((*i).first==varId)
      erase(i++);
	else
	  ++i;
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
bool CodeThorn::operator<(const EState& e1, const EState& e2) {
  if(e1.label()!=e2.label())
	return (e1.label()<e2.label());
  if(e1.pstate()!=e2.pstate())
	return (e1.pstate()<e2.pstate());
  if(e1.constraints()!=e2.constraints())
	return (e1.constraints()<e2.constraints());
  return e1.io<e2.io;
}

bool CodeThorn::operator==(const EState& c1, const EState& c2) {
  return (c1.label()==c2.label())
	&& (c1.pstate()==c2.pstate())
	&& (c1.constraints()==c2.constraints())
	&& (c1.io==c2.io);
}

bool CodeThorn::operator!=(const EState& c1, const EState& c2) {
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
	if((*i).source->label()==getStartLabel())
	  return *i;
  }
  throw "TransitionGraph: no start transition found.";
}

string EStateSet::estateIdString(const EState* estate) const {
  stringstream ss;
  ss<<estateId(estate);
  return ss.str();
}

CodeThorn::InputOutput::OpType EState::ioOp(Labeler* labeler) const {
  Label lab=label();
  if(labeler->isStdInLabel(lab)) return InputOutput::STDIN_VAR;
  if(labeler->isStdOutLabel(lab)) return InputOutput::STDOUT_VAR;
  if(labeler->isStdErrLabel(lab)) return InputOutput::STDERR_VAR;
  return InputOutput::NONE;
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

void TransitionGraph::reduceEStates(set<const EState*> toReduce) {
  for(set<const EState*>::const_iterator i=toReduce.begin();i!=toReduce.end();++i) { 
	reduceEState(*i);
  }
}

void TransitionGraph::reduceEStates2(set<const EState*> toReduce) {
  for(set<const EState*>::const_iterator i=toReduce.begin();i!=toReduce.end();++i) { 
	reduceEState2(*i);
  }
}

// MS: we definitely need to cache all the results or use a proper graph structure
TransitionGraph::TransitionPtrSet TransitionGraph::inEdges(const EState* estate) {
  TransitionGraph::TransitionPtrSet in;
  for(TransitionGraph::const_iterator i=begin();i!=end();++i) {
	if(estate==(*i).target)
	  in.insert(&(*i));
  }
  return in;
}

// MS: we definitely need to cache all the results or use a proper graph structure
TransitionGraph::TransitionPtrSet TransitionGraph::outEdges(const EState* estate) {
  TransitionGraph::TransitionPtrSet out;
  for(TransitionGraph::const_iterator i=begin();i!=end();++i) {
	if(estate==(*i).source)
	  out.insert(&(*i));
  }
  return out;
}

void TransitionGraph::reduceEState(const EState* estate) {
  /* description of essential operations:
   *   inedges: (n_i,b)
   *   outedges: (b,n_j) 
   *   insert(ni,t,nj) where t=union(t(n_i))+union(t(n_j))+{EDGE_PATH}
   *   remove(n_i,b)
   *   remove(b,n_j)
   *   delete b
   
   * ea: (n1,cfge,n2) == ((l1,p1,c1,io1),(l1,t12,l2),(l2,p2,c2,io2))
   * eb: (n2,cfge,n3) == ((l2,p2,c2,io2),(l2,t23,l3),(l3,p3,c3,io3))
   * ==> (n1,cfge',n3) == ((l1,p1,c1,io1),(l1,{t12,t13,EDGE_PATH}},l3),(l3,p3,c3,io3))
   
   */
  TransitionGraph::TransitionPtrSet in=inEdges(estate);
  TransitionGraph::TransitionPtrSet out=outEdges(estate);
  if(in.size()!=0) {
	//cout<< "INFO: would be eliminating: node: "<<estate<<", #in="<<in.size()<<", #out="<<out.size()<<endl;
  } else {
	cout<< "INFO: not eliminating node because #in==0: node: "<<estate<<", #in="<<in.size()<<", #out="<<out.size()<<endl;
  }
}

void TransitionGraph::add(Transition trans) {
  #pragma omp critical
  {
	insert(trans);
  }
}

bool CodeThorn::operator==(const Transition& t1, const Transition& t2) {
  return t1.source==t2.source && t1.edge==t2.edge && t1.target==t2.target;
}

bool CodeThorn::operator!=(const Transition& t1, const Transition& t2) {
  return !(t1==t2);
}

bool CodeThorn::operator<(const Transition& t1, const Transition& t2) {
  if(t1.source!=t2.source)
	return t1.source<t2.source;
  if(t1.edge!=t2.edge)
	return t1.edge<t2.edge;
  return t1.target<t2.target;
}


long TransitionGraph::removeDuplicates() {
  long cnt=0;
  set<Transition> s;
  for(TransitionGraph::iterator i=begin();i!=end();) {
	if(s.find(*i)==s.end()) { 
	  s.insert(*i); 
	  ++i;
	} else {
	  erase(i++);
	  cnt++;
	}
  }
  return cnt;
}

string TransitionGraph::toString() const {
  string s;
  size_t cnt=0;
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

bool TransitionGraph::checkConsistency() {
  bool ok=true;
  size_t cnt=0;
  TransitionGraph* tg=this;
  for(TransitionGraph::const_iterator i=tg->begin();i!=tg->end();++i) {
	cnt++;
  }
  if(cnt!=tg->size()) {
	cerr<< "Error: TransitionGraph: size()==" <<tg->size()<< ", count=="<<cnt<<endl;
	ok=false;
  }
  assert(cnt==tg->size());
  cout << "checkTransitionGraph:"<<ok<<" size:"<<size()<<endl;
  return ok;
}

void TransitionGraph::reduceEState2(const EState* estate) {
  /* description of essential operations:
   *   inedges: (n_i,b)
   *   outedges: (b,n_j) 
   *   insert(ni,t,nj) where t=union(t(n_i))+union(t(n_j))+{EDGE_PATH}
   *   remove(n_i,b)
   *   remove(b,n_j)
   *   delete b
   
   * ea: (n1,cfge,n2) == ((l1,p1,c1,io1),(l1,t12,l2),(l2,p2,c2,io2))
   * eb: (n2,cfge,n3) == ((l2,p2,c2,io2),(l2,t23,l3),(l3,p3,c3,io3))
   * ==> (n1,cfge',n3) == ((l1,p1,c1,io1),(l1,{t12,t13,EDGE_PATH}},l3),(l3,p3,c3,io3))
   
   */
  assert(estate);
  TransitionGraph::TransitionPtrSet in=inEdges(estate);
  TransitionGraph::TransitionPtrSet out=outEdges(estate);
  if(in.size()!=0 /*&& out.size()!=0*/) {
	cout<< estate->toString()<<endl;
	set<Transition> newTransitions;
	for(TransitionPtrSet::iterator i=in.begin();i!=in.end();++i) {
	  for(TransitionPtrSet::iterator j=out.begin();j!=out.end();++j) {
		Edge newEdge((*i)->source->label(),(*j)->target->label());
		newEdge.addTypes((*i)->edge.types());
		newEdge.addTypes((*j)->edge.types());
		Transition t((*i)->source,newEdge,(*j)->target);
		newTransitions.insert(t);
		assert(newTransitions.find(t)!=newTransitions.end());
	  }
	}
	cout << "DEBUG: number of new transitions: "<<newTransitions.size()<<endl;

	// 2. add new transitions
	for(set<Transition>::iterator k=newTransitions.begin();k!=newTransitions.end();++k) {
	  this->add(*k);
	  assert(find(*k)!=end());
	}
	assert(newTransitions.size()<=in.size()*out.size());

#if 1
	// 1. remove all old transitions
	for(TransitionPtrSet::iterator i=in.begin();i!=in.end();++i) {
	  
	  TransitionGraph::iterator it1=find(**i);
	  //cout << "DEBUG: FOUND: "<<(*it1).toString()<<endl;
	  this->erase(*it1);
	}
#endif
#if 1
	for(TransitionPtrSet::iterator j=out.begin();j!=out.end();++j) {
	  this->erase(**j);

	}
#endif

  } else {
	cout<< "INFO: not eliminating node because #in==0 or #out==0: node: "<<estate<<", #in="<<in.size()<<", #out="<<out.size()<<endl;
  }
}

// later, we may want to maintain this set with every graph-operation (turning the linear access to constant)
set<const EState*> TransitionGraph::estateSet() {
  set<const EState*> estateSet;
  for(TransitionGraph::iterator j=begin();j!=end();++j) {
	  estateSet.insert((*j).source);
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

string EState::toHTML() const {
  stringstream ss;
  string nl = " <BR />\n";
  ss << "EState";
  ss << "("<<label()<<", "<<nl;
  if(pstate())
	ss <<pstate()->toString();
  else
	ss <<"NULL";
  if(constraints()) {
	ss <<","<<nl<<" constraints="<<constraints()->toString();
  } else {
	ss <<","<<nl<<" NULL";
  }
  ss <<","<<nl<<" io="<<io.toString();
  ss<<")"<<nl;
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
bool CodeThorn::operator<(const PState& s1, const PState& s2) {
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
bool CodeThorn::operator==(const PState& c1, const PState& c2) {
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

bool CodeThorn::operator!=(const PState& c1, const PState& c2) {
  return !(c1==c2);
}
#endif
#endif
