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
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "AType.h"
#include "VariableIdMapping.h"

using namespace std;

typedef int StateId;
typedef int EStateId;
typedef string VariableName;
typedef AType::ConstIntLattice AValue; 
typedef AType::CppCapsuleConstIntLattice CppCapsuleAValue; 
typedef AType::CppCapsuleConstIntLatticeLessComparator CppCapsuleAValueLessComp; 
typedef AType::ConstIntLatticeCmp AValueCmp; 
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

class StateSet : public set<State> {
 public:
  bool stateExists(State& s);
  const State* statePtr(State& s);
  string toString();
  StateId stateId(const State* state);
  StateId stateId(const State state);
  string stateIdString(const State* state);
};

/*
  EQ_VAR_CONST : equal (==)
  NEQ_VAR_CONST: not equal (!=)
  DEQ_VAR_CONST: disequal (both, EQ and NEQ)
*/
class Constraint {
 public:
  enum ConstraintOp {EQ_VAR_CONST,NEQ_VAR_CONST,DEQ_VAR_CONST};
  Constraint(ConstraintOp op0,VariableId lhs, AValue rhs);
  Constraint(ConstraintOp op0,VariableId lhs, CppCapsuleAValue rhs);
  ConstraintOp op;
  VariableId var;
  CppCapsuleAValue intVal;
  string toString() const;
 private:
  string opToString() const;
};

bool operator<(const Constraint& c1, const Constraint& c2);
bool operator==(const Constraint& c1, const Constraint& c2);
bool operator!=(const Constraint& c1, const Constraint& c2);

class ConstraintSet : public set<Constraint> {
 public:
  bool constraintExists(Constraint::ConstraintOp op, VariableId varId, CppCapsuleAValue intVal) const;
  bool constraintExists(Constraint::ConstraintOp op, VariableId varId, AValue intVal) const;
  bool constraintExists(const Constraint& c) const;
  ConstraintSet::iterator findSpecific(Constraint::ConstraintOp op, VariableId varId) const;
  ConstraintSet findSpecificSet(Constraint::ConstraintOp op, VariableId varId) const;
  AType::ConstIntLattice varConstIntLatticeValue(const VariableId varId) const;
  string toString() const;
  ConstraintSet& operator+=(ConstraintSet& s2);
  ConstraintSet operator+(ConstraintSet& s2);
  ConstraintSet deleteVarConstraints(VariableId varId);
  void deleteConstraints(VariableId varId);
  ConstraintSet invertedConstraints();
  void invertConstraints();
  //! duplicates constraints for par2 variable and adds them for par1 variable.
  void duplicateConstraints(VariableId lhsVarId, VariableId rhsVarId);

  // that's a tricky form of reuse (hiding the inherited function (not overloading, nor overriding)).
  void insert(Constraint c);

  bool deqConstraintExists();
};
//bool operator==(const ConstraintSet& s1, const ConstraintSet& s2);
//bool operator<(const ConstraintSet& s1, const ConstraintSet& s2);
//bool operator!=(const ConstraintSet& s1, const ConstraintSet& s2);

/* Input: a value val is read into a variable var
   Output: either a variable or a value is written
*/
class InputOutput {
 public:
 InputOutput():op(NONE),var(VariableId(0)){ val=AType::Bot();}
  enum OpType {NONE,STDIN_VAR,STDOUT_VAR,STDOUT_CONST,STDERR_VAR,STDERR_CONST};
  OpType op;
  VariableId var;
  AType::ConstIntLattice val;
  string toString() const;
  void recordVariable(OpType op, VariableId varId);
  void recordConst(OpType op, AType::ConstIntLattice val);
};

bool operator<(const InputOutput& c1, const InputOutput& c2);
bool operator==(const InputOutput& c1, const InputOutput& c2);
bool operator!=(const InputOutput& c1, const InputOutput& c2);


class EState {
 public:
 EState():label(0),state(0){}
 EState(Label label, const State* state):label(label),state(state){}
 EState(Label label, const State* state, ConstraintSet cset):label(label),state(state),constraints(cset){}
  string toString() const;
  Label label;
  const State* state;
  ConstraintSet constraints;
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
  bool eStateExists(EState& s);
  const EState* processEState(EState newEState);
  void addNewEState(EState newEState);
  string toString();
  EStateId eStateId(const EState* eState);
  EStateId eStateId(const EState eState);
  string eStateIdString(const EState* eState);
  const EState* eStatePtr(EState& s);
  int numberOfIoTypeStates(InputOutput::OpType);
};

class Transition {
 public:
 Transition(const EState* source,Edge edge, const EState* target):source(source),edge(edge),target(target){}
public:
  const EState* source; // source node
  Edge edge;
  const EState* target; // target node

};

class EStateList : public list<EState> {
 public:
  string toString();
};

class TransitionGraph : public list<Transition> {
 public:
  set<const EState*> transitionSourceEStateSetOfLabel(Label lab);
  set<const EState*> eStateSetOfLabel(Label lab);

 private:
  int numberOfNodes;
};


#endif
