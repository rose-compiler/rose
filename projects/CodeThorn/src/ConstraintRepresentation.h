#ifndef CONSTRAINT_REPRESENTATION_H
#define CONSTRAINT_REPRESENTATION_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

//#define CSET_MAINTAINER_LIST
//#define CSET_MAINTAINER_SET
//#define CSET_MAINTAINER_HSET

#include <set>
#include <map>
#include <utility>
#include <string>
#include <list>
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "AType.h"
#include "VariableIdMapping.h"
#include "EqualityMaintainer.h"

using namespace std;

typedef list<AValue> ListOfAValue;
typedef set<VariableId> SetOfVariableId;

/*
  EQ_VAR_CONST : equal (==)
  NEQ_VAR_CONST: not equal (!=)
  DEQ_VAR_CONST: disequal (both, EQ and NEQ)
*/
class Constraint {
 public:
  enum ConstraintOp {EQ_VAR_CONST,NEQ_VAR_CONST, EQ_VAR_VAR, NEQ_VAR_VAR, DEQ};
  Constraint(ConstraintOp op0,VariableId lhs, AValue rhs);
  Constraint(ConstraintOp op0,VariableId lhs, CppCapsuleAValue rhs);
  Constraint(ConstraintOp op0,VariableId lhs, VariableId rhs);
  ConstraintOp op() const;
  VariableId lhsVar() const;
  VariableId rhsVar() const;
  AValue rhsVal() const;
  CppCapsuleAValue rhsValCppCapsule() const;
  string toString() const;
  bool isVarVarOp() const;
  bool isVarValOp() const;
  bool isEquation() const;
  bool isInequation() const;
  bool isDisequation() const;
  void negate();
  void swapVars();
  void setLhsVar(VariableId lhs) { _lhsVar=lhs; } 
 private:
  string opToString() const;
  ConstraintOp _op;
  VariableId _lhsVar;
  VariableId _rhsVar;
  CppCapsuleAValue _intVal;
};

bool operator<(const Constraint& c1, const Constraint& c2);
bool operator==(const Constraint& c1, const Constraint& c2);
bool operator!=(const Constraint& c1, const Constraint& c2);

// we use only one disequality constraint to mark constraint set representing non-reachable states
#define DISEQUALITYCONSTRAINT Constraint(Constraint::DEQ,0,AType::CppCapsuleConstIntLattice(AType::ConstIntLattice(0)))

class ConstraintSet : public set<Constraint> {
 public:
  bool constraintExists(Constraint::ConstraintOp op, VariableId varId, CppCapsuleAValue intVal) const;
  bool constraintExists(Constraint::ConstraintOp op, VariableId varId, AValue intVal) const;
  bool constraintExists(Constraint::ConstraintOp op) const;
  ConstraintSet constraintsWithOp(Constraint::ConstraintOp op) const;
  bool constraintExists(const Constraint& c) const;

  // deprecated
  ConstraintSet::iterator findSpecific(Constraint::ConstraintOp op, VariableId varId) const;
  // deprecated
  ConstraintSet findSpecificSet(Constraint::ConstraintOp op, VariableId varId) const;

  string toString() const;

  //! returns concrete int-value if equality exists, otherwise Top.
  AType::ConstIntLattice varConstIntLatticeValue(const VariableId varId) const;
  //! returns set of concrete values for which an equality is stored 
  //! (there can be at most one), otherwise the set is empty. 
  //! Note that top may exist as explicit equality if it was added as such.
  ListOfAValue getEqVarConst(const VariableId varId) const;
  //! returns set of concrete values for which an inequality exists
  ListOfAValue getNeqVarConst(const VariableId varId) const;
  SetOfVariableId getEqVars(const VariableId varId) const;

  //! maintains consistency of set and creates DIS if inconsistent constraints are added
  void addConstraint(Constraint c);
  void removeAllConstraintsOfVar(VariableId varId);

  ConstraintSet invertedConstraints(); // only correct for single constraints 
  void invertConstraints();            // only correct for single constraints 

  void addDisequality();
  bool disequalityExists() const;

  void addAssignEqVarVar(VariableId, VariableId);
  void addEqVarVar(VariableId, VariableId);
  //void removeEqualitiesOfVar(VariableId);

  long numberOfConstConstraints(VariableId);
  ConstraintSet& operator+=(ConstraintSet& s2);
  //ConstraintSet operator+(ConstraintSet& s2);
  long memorySize() const;

 private:
  void deleteAndMoveConstConstraints(VariableId lhsVarId, VariableId rhsVarId);
  void moveConstConstraints(VariableId fromVar, VariableId toVar);
  //! modifies internal representation
  void insertConstraint(Constraint c);
  //! modifies internal representation
  void eraseConstraint(Constraint c);
  void eraseConstraint(set<Constraint>::iterator i);
  //! modifies internal representation
  void eraseEqWithLhsVar(VariableId);
  void duplicateConstConstraints(VariableId lhsVarId, VariableId rhsVarId);
  //! moves const-constraints from "fromVar" to "toVar". Does maintain consistency, set may be become DEQ.
  void eraseConstConstraints(VariableId);
  EqualityMaintainer<VariableId> equalityMaintainer;
};

class ConstraintSetHashFun {
   public:
    ConstraintSetHashFun(long prime=99991) : tabSize(prime) {}
    long operator()(ConstraintSet cs) const {
	  unsigned int hash=1;
	  for(ConstraintSet::iterator i=cs.begin();i!=cs.end();++i) {
		// use the symbol-ptr of lhsVar for hashing (we are a friend).
		if(!(*i).isDisequation()) {
		  hash*=(long)((*i).lhsVar().getSymbol());
		}
	  }
	  return long(hash) % tabSize;
	}
	  long tableSize() const { return tabSize;}
   private:
    long tabSize;
};

ConstraintSet operator+(ConstraintSet s1, ConstraintSet s2);
bool operator<(const ConstraintSet& s1, const ConstraintSet& s2);
//bool operator==(const ConstraintSet& s1, const ConstraintSet& s2);
//bool operator!=(const ConstraintSet& s1, const ConstraintSet& s2);

#ifdef CSET_MAINTAINER_LIST
class ConstraintSetMaintainer : public list<ConstraintSet> {
#endif
#ifdef CSET_MAINTAINER_SET
class ConstraintSetMaintainer : public set<ConstraintSet> {
#endif

#ifdef CSET_MAINTAINER_HSET
#include "HSetMaintainer.h"
class ConstraintSetMaintainer : public HSetMaintainer<ConstraintSet, ConstraintSetHashFun> {
 public:
	  typedef HSetMaintainer<ConstraintSet, ConstraintSetHashFun>::ProcessingResult ProcessingResult;
	  string toString();
};
#endif

#endif
