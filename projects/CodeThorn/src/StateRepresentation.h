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
		hash=((hash<<8)+((long)(*i).second.getValue().getIntValue()))^hash;
	  }
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
#include "HSetMaintainer.h"
	class StateSet : public HSetMaintainer<State,StateHashFun> {
	public:
	  typedef HSetMaintainer<State,StateHashFun>::ProcessingResult ProcessingResult;
	  string toString();
	  StateId stateId(const State* state);
	  StateId stateId(const State state);
	  string stateIdString(const State* state);
	private:
	  const State* ptr(State& s);
	};
#endif


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
bool operator<(const EState& c1, const EState& c2);
bool operator==(const EState& c1, const EState& c2);
bool operator!=(const EState& c1, const EState& c2);

struct EStateLessComp {
  bool operator()(const EState& c1, const EState& c2) {
	return c1<c2;
  }
};

class EStateHashFun {
   public:
    EStateHashFun(long prime=9999991) : tabSize(prime) {}
    long operator()(EState s) const {
	  unsigned int hash=1;
	  hash=(long)s.getLabel()*(((long)s.getState())+1)*(((long)s.constraints())+1);
	  return long(hash) % tabSize;
	}
	long tableSize() const { return tabSize;}
   private:
    long tabSize;
};

#ifdef ESTATE_MAINTAINER_HSET
#include "HSetMaintainer.h"
  class EStateSet : public HSetMaintainer<EState,EStateHashFun> {
  public:
 EStateSet():HSetMaintainer<EState,EStateHashFun>(),_constraintSetMaintainer(0){}
	public:
  typedef HSetMaintainer<EState,EStateHashFun>::ProcessingResult ProcessingResult;
  string toString();
  EStateId eStateId(const EState* eState);
  EStateId eStateId(const EState eState);
  string eStateIdString(const EState* eState);
  int numberOfIoTypeEStates(InputOutput::OpType);
 private:
  ConstraintSetMaintainer* _constraintSetMaintainer; 
};
#endif

class Transition {
 public:
 Transition(const EState* source,Edge edge, const EState* target):source(source),edge(edge),target(target){}
public:
  const EState* source; // source node
  Edge edge;
  const EState* target; // target node
  string toString() const;

};

bool operator==(const Transition& t1, const Transition& t2);
bool operator!=(const Transition& t1, const Transition& t2);
bool operator<(const Transition& t1, const Transition& t2);

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
  // eliminates all duplicates of edges
  long removeDuplicates();
 private:
  int numberOfNodes;
};


#endif
