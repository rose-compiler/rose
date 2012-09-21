#ifndef STATE_REPRESENTATION_H
#define STATE_REPRESENTATION_H

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
};

class StateSet : public list<State> {
 public:
  typedef pair<bool, const State*> ProcessingResult;
  bool stateExists(State& s);
  ProcessingResult processState(State& s);
  const State* processNewState(State& s);
  const State* processNewOrExistingState(State& s);
  string toString();
  StateId stateId(const State* state);
  StateId stateId(const State state);
  string stateIdString(const State* state);
 private:
  const State* statePtr(State& s);
};


/* Input: a value val is read into a variable var
   Output: either a variable or a value is written
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
 EState():label(0),state(0){}
 EState(Label label, const State* state):label(label),state(state){}
 EState(Label label, const State* state, ConstraintSet cset):label(label),state(state),constraints(cset){}
  Label getLabel() const { return label; }
  const State* getState() const { return state; }
  const ConstraintSet& getConstraints() const { return constraints; }
  const InputOutput& getInputOutput() const { return io; }
  string toString() const;
  // MS: following entries will be made private
  Label label;
  const State* state;
  ConstraintSet constraints;  // MS: will become a pointer to ConstraintSet
  InputOutput io;
};

// define order for State elements (necessary for StateSet)
//bool operator<(const State& c1, const State& c2);
//bool operator==(const State& c1, const State& c2);
//bool operator!=(const State& c1, const State& c2);
//bool operator<(const State::value_type& elem1, const State::value_type& elem2);
//bool operator==(const pair<VariableId,AValue>& elem1, const pair<VariableId,AValue>& elem2);

// define order for EState elements (necessary for EStateSet)
bool operator<(const EState& c1, const EState& c2);
bool operator==(const EState& c1, const EState& c2);
bool operator!=(const EState& c1, const EState& c2);

class EStateSet : public list<EState> {
 public:
  typedef pair<bool,const EState*> ProcessingResult;
  bool eStateExists(EState& s);
  ProcessingResult processEState(EState newEState);
  const EState* processNewEState(EState& s);
  const EState* processNewOrExistingEState(EState& s);
  string toString();
  EStateId eStateId(const EState* eState);
  EStateId eStateId(const EState eState);
  string eStateIdString(const EState* eState);
  int numberOfIoTypeEStates(InputOutput::OpType);
  void addNewEState(EState newEState);
 private:
  const EState* eStatePtr(EState& s);
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

class EStateList : public list<EState> {
 public:
  string toString();
};

class TransitionGraph : public list<Transition> {
 public:
  set<const EState*> transitionSourceEStateSetOfLabel(Label lab);
  set<const EState*> eStateSetOfLabel(Label lab);
  string toString() const;
 private:
  int numberOfNodes;
};


#endif
