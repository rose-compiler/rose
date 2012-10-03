#ifndef STATE_REPRESENTATION_H
#define STATE_REPRESENTATION_H

#define USER_DEFINED_STATE_COMP

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <string>
#include <set>
#include <map>
#include <utility>
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "AType.h"
#include "VariableIdMapping.h"
#include "ConstraintRepresentation.h"

using namespace std;

typedef int StateId;
typedef int EStateId;
typedef string VariableName;
class State;
typedef set<const State*> StatePtrSet;

class State : public map<VariableId,CppCapsuleAValue> {
 public:
  bool varExists(VariableId varname) const;
  bool varIsConst(VariableId varname) const;
  string varValueToString(VariableId varname) const;
  string toString() const;
  void deleteVar(VariableId varname);
  long memorySize() const;
};

class StateHashFun {
   public:
    StateHashFun(long prime=99991) : tabSize(prime) {}
    long operator()(State s) const {
	  unsigned int hash=1;
	  for(State::iterator i=s.begin();i!=s.end();++i) {
		hash*=(long)((*i).first.getSymbol());
	  }
	  if(hash<0)
		hash=-hash;
	  return long(hash) % tabSize;
	}
	  long tableSize() const { return tabSize;}
   private:
    long tabSize;
};

#ifdef STATE_MAINTAINER_LIST
class StateSet : public list<State> {
#endif
#ifdef STATE_MAINTAINER_SET
class StateSet : public set<State> {
#endif
#ifdef STATE_MAINTAINER_HSET
#include "HashFun.h"
#include "HSet.h"
using namespace br_stl;
	class StateSet : public HSet<State,StateHashFun> {
#endif
 public:
  typedef pair<bool, const State*> ProcessingResult;
  bool stateExists(State& s);
  ProcessingResult process(State& s);
  const State* processNew(State& s);
  const State* processNewOrExisting(State& s);
  string toString();
  StateId stateId(const State* state);
  StateId stateId(const State state);
  string stateIdString(const State* state);
  long memorySize() const;
 private:
  const State* ptr(State& s);
};

/**
 * Input: a value val is read into a variable var
 * Output: either a variable or a value is written
 */
class InputOutput {
 public:
 InputOutput():op(NONE),var(VariableId(0)){ val=AType::Bot();}
  enum OpType {NONE,STDIN_VAR,STDOUT_VAR,STDOUT_CONST,STDERR_VAR,STDERR_CONST, FAILED_ASSERT};
  OpType op;
  VariableId var;
  AType::ConstIntLattice val;
  string toString() const;
  void recordVariable(OpType op, VariableId varId);
  void recordConst(OpType op, AType::ConstIntLattice val);
  void recordFailedAssert();
};

bool operator<(const InputOutput& c1, const InputOutput& c2);
bool operator==(const InputOutput& c1, const InputOutput& c2);
bool operator!=(const InputOutput& c1, const InputOutput& c2);

class EState {
 public:
 EState():label(0),state(0),_constraints(0){}
 EState(Label label, const State* state):label(label),state(state){}
 EState(Label label, const State* state, const ConstraintSet* csetptr):label(label),state(state),_constraints(csetptr){}
  Label getLabel() const { return label; }
  const State* getState() const { return state; }
  /// \deprecated, use constraints() instead.
  const ConstraintSet& getConstraints() const { return *_constraints; }
  const InputOutput& getInputOutput() const { return io; }
  string toString() const;
  const ConstraintSet* constraints() const { return _constraints; }
  long memorySize() const;
  // MS: following entries will be made private
  Label label;
  const State* state;
 private:
  const ConstraintSet* _constraints;
 public:
  InputOutput io;
};

// define order for State elements (necessary for StateSet)
#ifdef  USER_DEFINED_STATE_COMP
bool operator<(const State& c1, const State& c2);
#if 0
bool operator==(const State& c1, const State& c2);
bool operator!=(const State& c1, const State& c2);
#endif
#endif

// define order for EState elements (necessary for EStateSet)
//#ifndef ESTATE_MAINTAINER_LIST
bool operator<(const EState& c1, const EState& c2);
bool operator==(const EState& c1, const EState& c2);
bool operator!=(const EState& c1, const EState& c2);
//#endif

//#ifndef ESTATE_MAINTAINER_LIST
struct EStateLessComp {
  bool operator()(const EState& c1, const EState& c2) {
	return c1<c2;
  }
};
//#endif

class EStateHashFun {
   public:
    EStateHashFun(long prime=9999991) : tabSize(prime) {}
    long operator()(EState s) const {
	  unsigned int hash=1;
	  hash*=(long)s.getLabel();
	  hash*=(long)s.getState();
#if 0
	  for(ConstraintSet::iterator i=cs.begin();i!=cs.end();++i) {
		// use the symbol-ptr of lhsVar for hashing (we are a friend).
		if(!(*i).isDisequation()) {
		  hash*=(long)((*i).lhsVar().getSymbol());
		}
	  }
#endif
	  if(hash<0)
		hash=-hash;
	  return long(hash) % tabSize;
	}
	long tableSize() const { return tabSize;}
   private:
    long tabSize;
};

#ifdef ESTATE_MAINTAINER_LIST
class EStateSet : public list<EState> {
#endif
#ifdef ESTATE_MAINTAINER_SET
  class EStateSet : public set<EState> {
#endif
#ifdef ESTATE_MAINTAINER_HSET
#include "HSet.h"
using namespace br_stl;
	class EStateSet : public HSet<EState,EStateHashFun> {
#endif
 public:
 EStateSet():_constraintSetMaintainer(0){}
  typedef pair<bool,const EState*> ProcessingResult;
  bool eStateExists(EState& s);
  ProcessingResult process(EState newEState);
  const EState* processNew(EState& s);
  const EState* processNewOrExisting(EState& s);
  string toString();
  EStateId eStateId(const EState* eState);
  EStateId eStateId(const EState eState);
  string eStateIdString(const EState* eState);
  int numberOfIoTypeEStates(InputOutput::OpType);
  long memorySize() const;
 private:
  const EState* ptr(EState& s);
  ConstraintSetMaintainer* _constraintSetMaintainer; 
};

class Transition {
 public:
 Transition(const EState* source,Edge edge, const EState* target):source(source),edge(edge),target(target){}
public:
  const EState* source; // source node
  Edge edge;
  const EState* target; // target node
  string toString() const;

};

bool operator==(Transition& t1, Transition& t2);

class EStateList : public list<EState> {
 public:
  string toString();
};

class TransitionGraph : public list<Transition> {
 public:
  set<const EState*> transitionSourceEStateSetOfLabel(Label lab);
  set<const EState*> eStateSetOfLabel(Label lab);
  void add(Transition trans);
  string toString() const;
  LabelSet labelSetOfIoOperations(InputOutput::OpType op);
 private:
  int numberOfNodes;
};


#endif
