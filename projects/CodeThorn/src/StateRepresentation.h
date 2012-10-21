#ifndef ALL_STATE_REPRESENTATION_H
#define ALL_STATE_REPRESENTATION_H

#define USER_DEFINED_PSTATE_COMP

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

typedef int PStateId;
typedef int EStateId;
typedef string VariableName;
class PState;
typedef set<const PState*> PStatePtrSet;

class PState : public map<VariableId,CppCapsuleAValue> {
 public:
  bool varExists(VariableId varname) const;
  bool varIsConst(VariableId varname) const;
  string varValueToString(VariableId varname) const;
  string toString() const;
  void deleteVar(VariableId varname);
  long memorySize() const;
};

class PStateHashFun {
   public:
    PStateHashFun(long prime=99991) : tabSize(prime) {}
    long operator()(PState s) const {
	  unsigned int hash=1;
	  for(PState::iterator i=s.begin();i!=s.end();++i) {
		hash=((hash<<8)+((long)(*i).second.getValue().getIntValue()))^hash;
	  }
	  return long(hash) % tabSize;
	}
	  long tableSize() const { return tabSize;}
   private:
    long tabSize;
};

#ifdef PSTATE_MAINTAINER_LIST
class PStateSet : public list<PState> {
#endif
#ifdef PSTATE_MAINTAINER_SET
class PStateSet : public set<PState> {
#endif
#ifdef PSTATE_MAINTAINER_HSET
#include "HashFun.h"
#include "HSetMaintainer.h"
	class PStateSet : public HSetMaintainer<PState,PStateHashFun> {
	public:
	  typedef HSetMaintainer<PState,PStateHashFun>::ProcessingResult ProcessingResult;
	  string toString();
	  PStateId pstateId(const PState* pstate);
	  PStateId pstateId(const PState pstate);
	  string pstateIdString(const PState* pstate);
	private:
	  const PState* ptr(PState& s);
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
 EState():label(0),pstate(0),_constraints(0){}
 EState(Label label, const PState* pstate):label(label),pstate(pstate){}
 EState(Label label, const PState* pstate, const ConstraintSet* csetptr):label(label),pstate(pstate),_constraints(csetptr){}
  Label getLabel() const { return label; }
  const PState* getPState() const { return pstate; }
  /// \deprecated, use constraints() instead.
  const ConstraintSet& getConstraints() const { return *_constraints; }
  const InputOutput& getInputOutput() const { return io; }
  string toString() const;
  const ConstraintSet* constraints() const { return _constraints; }
  long memorySize() const;
  // MS: following entries will be made private
  Label label;
  const PState* pstate;
 private:
  const ConstraintSet* _constraints;
 public:
  InputOutput io;
};

// define order for PState elements (necessary for PStateSet)
#ifdef  USER_DEFINED_PSTATE_COMP
bool operator<(const PState& c1, const PState& c2);
#if 0
bool operator==(const PState& c1, const PState& c2);
bool operator!=(const PState& c1, const PState& c2);
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
	  hash=(long)s.getLabel()*(((long)s.getPState())+1)*(((long)s.constraints())+1);
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
  EStateId estateId(const EState* estate);
  EStateId estateId(const EState estate);
  string estateIdString(const EState* estate);
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

class TransitionHashFun {
   public:
    TransitionHashFun(long prime=99991) : tabSize(prime) {}
    long operator()(Transition s) const {
	  unsigned int hash=1;
	  hash=((((long)s.source)+1)<<8)+(long)s.target*(long)s.edge.type;
	  return long(hash) % tabSize;
	}
	  long tableSize() const { return tabSize;}
	  private:
    long tabSize;
};

bool operator==(const Transition& t1, const Transition& t2);
bool operator!=(const Transition& t1, const Transition& t2);
bool operator<(const Transition& t1, const Transition& t2);

class EStateList : public list<EState> {
 public:
  string toString();
};

class TransitionGraph : public HSet<Transition,TransitionHashFun> {
 public:
  set<const EState*> transitionSourceEStateSetOfLabel(Label lab);
  set<const EState*> estateSetOfLabel(Label lab);
  void add(Transition trans);
  string toString() const;
  LabelSet labelSetOfIoOperations(InputOutput::OpType op);
  // eliminates all duplicates of edges
  long removeDuplicates();
  Label getStartLabel() { return startLabel; }
  void setStartLabel(Label lab) { startLabel=lab; }
  Transition getStartTransition();
 private:
  int numberOfNodes;
  Label startLabel;
};

#endif
