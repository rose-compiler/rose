#ifndef CONSTRAINT_REPRESENTATION_H
#define CONSTRAINT_REPRESENTATION_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <set>
#include <map>
#include <utility>
#include <string>
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "AType.h"
#include "VariableIdMapping.h"

/*
  EQ_VAR_CONST : equal (==)
  NEQ_VAR_CONST: not equal (!=)
  DEQ_VAR_CONST: disequal (both, EQ and NEQ)
*/
class Constraint {
 public:
  enum ConstraintOp {EQ_VAR_CONST,NEQ_VAR_CONST,DEQ_VAR_CONST, EQ_VAR_VAR, NEQ_VAR_VAR, DEQ_VAR_VAR};
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

class ConstraintSet : public set<Constraint> {
 public:
  bool constraintExists(Constraint::ConstraintOp op, VariableId varId, CppCapsuleAValue intVal) const;
  bool constraintExists(Constraint::ConstraintOp op, VariableId varId, AValue intVal) const;
  bool constraintExists(Constraint::ConstraintOp op) const;
  ConstraintSet constraintsWithOp(Constraint::ConstraintOp op) const;
  bool constraintExists(const Constraint& c) const;
  ConstraintSet::iterator findSpecific(Constraint::ConstraintOp op, VariableId varId) const;
  ConstraintSet findSpecificSet(Constraint::ConstraintOp op, VariableId varId) const;
  AType::ConstIntLattice varConstIntLatticeValue(const VariableId varId) const;
  string toString() const;
  ConstraintSet deleteVarConstraints(VariableId varId);
  void deleteConstraints(VariableId varId);
  ConstraintSet invertedConstraints();
  void invertConstraints();
  //! duplicates constraints for par2 variable and adds them for par1 variable.
  void moveConstConstraints(VariableId lhsVarId, VariableId rhsVarId);
  void duplicateConstConstraints(VariableId lhsVarId, VariableId rhsVarId);

  void addConstraint(Constraint c);
  //! erase simply deletes the Constraint from the constraint set (in difference, removeConstraint reorganizes the constraints)
  void eraseConstraint(Constraint c);
  //! remove transfers information to other vars if the constraint to be deleted is x=y. It tries to keep information alive by transfering it to other variables if possible)
  void removeConstraint(Constraint c);
  //! implemented by method removeConstraint
  void removeConstraint(ConstraintSet::iterator i);
  bool deqConstraintExists();
  void addAssignEqVarVar(VariableId, VariableId);
  void addEqVarVar(VariableId, VariableId);
  void removeEqVarVar(VariableId, VariableId);
  ConstraintSet& operator+=(ConstraintSet& s2);
  //ConstraintSet operator+(ConstraintSet& s2);
};
ConstraintSet operator+(ConstraintSet s1, ConstraintSet s2);
//bool operator==(const ConstraintSet& s1, const ConstraintSet& s2);
//bool operator<(const ConstraintSet& s1, const ConstraintSet& s2);
//bool operator!=(const ConstraintSet& s1, const ConstraintSet& s2);

#endif
