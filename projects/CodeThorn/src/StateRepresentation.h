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
typedef int AValue; // MS: TODO: replace 'int' with 'ConstIntLattice'
class State;
typedef set<const State*> StatePtrSet;

class State : public map<VariableId,AValue> {
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
  ConstraintOp op;
  VariableId var;
  AValue intVal;
  string toString() const;
 private:
  string opToString() const;
};

bool operator<(const Constraint& c1, const Constraint& c2);
bool operator==(const Constraint& c1, const Constraint& c2);

class ConstraintSet : public set<Constraint> {
 public:
  bool constraintExists(Constraint::ConstraintOp op, VariableId varId, AValue intVal);
  bool constraintExists(Constraint& c);
  ConstraintSet::iterator findSpecific(Constraint::ConstraintOp op, VariableId varId) const;
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

/* Input: a value val is read into a variable var
   Output: either a variable or a value is written
*/
class InputOutput {
 public:
 InputOutput():op(NONE),var(VariableId(0)){ val=AType::Bot();}
  enum OpType {IN_VAR,OUT_VAR,OUT_CONST,NONE};
  OpType op;
  VariableId var;
  AType::ConstIntLattice val;
  string toString() const;
  void recordInputVariable(VariableId varId);
  void recordOutputVariable(VariableId varId);
  void recordOutputConst(AType::ConstIntLattice val);
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

// define order for EState elements (necessary for EStateSet)
bool operator<(const EState& c1, const EState& c2);
bool operator==(const EState& c1, const EState& c2);

class EStateSet : public set<EState> {
 public:
  bool eStateExists(EState& s);
  const EState* eStatePtr(EState& s);
  string toString();
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
