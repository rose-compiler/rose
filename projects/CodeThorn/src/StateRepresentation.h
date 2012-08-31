#ifndef STATE_REPRESENTATION_H
#define STATE_REPRESENTATION_H

#include <string>
#include <set>
#include <map>
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "AType.h"

using namespace std;

typedef int StateId;
typedef int EStateId;
typedef string VariableName;
typedef int AValue; // MS: TODO: replace 'int' with 'ConstIntLattice'
class State;
typedef set<const State*> StatePtrSet;

class State : public map<VariableName,AValue> {
 public:
  bool varExists(VariableName varname) const;
  bool varIsConst(VariableName varname) const;
  string varValueToString(string varname) const;
  string toString() const;
  void deleteVar(VariableName varname);
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
  Constraint(ConstraintOp op0,VariableName lhs, AValue rhs);
  ConstraintOp op;
  VariableName var;
  AValue intVal;
  string toString() const;
 private:
  string opToString() const;
};

bool operator<(const Constraint& c1, const Constraint& c2);
bool operator==(const Constraint& c1, const Constraint& c2);

class ConstraintSet : public set<Constraint> {
 public:
  bool constraintExists(Constraint::ConstraintOp op, VariableName varName, AValue intVal);
  bool constraintExists(Constraint& c);
  ConstraintSet::iterator findSpecific(Constraint::ConstraintOp op, VariableName varName) const;
  AType::ConstIntLattice varConstIntLatticeValue(VariableName varName) const;
  string toString() const;
  ConstraintSet& operator+=(ConstraintSet& s2);
  ConstraintSet operator+(ConstraintSet& s2);
  ConstraintSet deleteVarConstraints(VariableName varName);
  ConstraintSet invertedConstraints();

  // that's a tricky form of reuse (hiding the inherited function (not overloading, nor overriding)).
  void insert(Constraint c);

  bool deqConstraintExists();
};

/* Input: a value val is read into a variable var
   Output: either a variable or a value is written
*/
class InputOutput {
 public:
 InputOutput():op(NONE){ val=AType::Bot();}
  enum OpType {IN_VAR,OUT_VAR,OUT_CONST,NONE};
  OpType op;
  VariableName var;
  AType::ConstIntLattice val;
};

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
  const EState* source;
  Edge edge;
  const EState* target; 

};

class EStateList : public list<EState> {
 public:
  string toString();
};

class TransitionGraph : public list<Transition> {
 public:
  set<const EState*> transitionSourceEStateSetOfLabel(Label lab);
  set<const EState*> eStateSetOfLabel(Label lab);
};

#endif
