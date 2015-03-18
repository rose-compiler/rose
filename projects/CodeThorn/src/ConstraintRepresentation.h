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
#include <list>
#include "Labeler.h"
#include "CFAnalysis.h"
#include "AType.h"
#include "VariableIdMapping.h"
#include "EqualityMaintainer.h"
#include "HSetMaintainer.h"

using namespace std;
using namespace SPRAY;

namespace CodeThorn {

typedef list<AValue> ListOfAValue;
 typedef VariableIdMapping::VariableIdSet SetOfVariableId;

/*
  EQ_VAR_CONST : equal (==)
  NEQ_VAR_CONST: not equal (!=)
  EQ_VAR_VAR   : equal (==)
  NEQ_VAR_VAR  : not equal (==) [not supported yet]
  DEQ          : (##)
*/
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class Constraint {
 public:
  enum ConstraintOp {UNDEFINED,EQ_VAR_CONST,NEQ_VAR_CONST, EQ_VAR_VAR, NEQ_VAR_VAR, DEQ};
  Constraint();
  Constraint(ConstraintOp op0,VariableId lhs, AValue rhs);
  Constraint(ConstraintOp op0,VariableId lhs, CppCapsuleAValue rhs);
  Constraint(ConstraintOp op0,VariableId lhs, VariableId rhs);
  ConstraintOp op() const;
  VariableId lhsVar() const;
  VariableId rhsVar() const;
  AValue rhsVal() const;
  CppCapsuleAValue rhsValCppCapsule() const;
  string toString() const;
  string toString(VariableIdMapping*) const;
  string toAssertionString(VariableIdMapping*) const;
  string operatorStringFromStream(istream& is);
  void fromStream(istream& is);
  void toStream(ostream& os);
  void toStreamAsTuple(ostream& os);
  bool isVarVarOp() const;
  bool isVarValOp() const;
  bool isEquation() const;
  bool isInequation() const;
  bool isDisequation() const;
  void negate();
  void swapVars();
  void setLhsVar(VariableId lhs) { _lhsVar=lhs; } 
 private:
  void initialize();
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
#define DISEQUALITYCONSTRAINT Constraint(Constraint::DEQ,VariableId(),AType::CppCapsuleConstIntLattice(AType::ConstIntLattice(0)))

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class ConstraintSet : public set<Constraint> {
 public:
  ConstraintSet constraintsOfVariable(VariableId varId) const;
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
  string toString(VariableIdMapping* vim) const;
  string toStringWithoutBraces(VariableIdMapping* vim) const;
  string toAssertionString(VariableIdMapping* vim) const;

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

#if 0
  ConstraintSet invertedConstraints(); // only correct for single constraints 
  void invertConstraints();            // only correct for single constraints 
#endif

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

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
#ifdef USE_CUSTOM_HSET
class ConstraintSetHashFun {
   public:
    ConstraintSetHashFun(long prime=99991) : tabSize(prime) {}
    long operator()(ConstraintSet cs) const {
      unsigned int hash=1;
      for(ConstraintSet::iterator i=cs.begin();i!=cs.end();++i) {
        // use the symbol-ptr of lhsVar for hashing (we are a friend).
        if((*i).isVarValOp())
          hash=((hash<<8)+((long)(*i).rhsValCppCapsule().getValue().hash()))^hash;
        else if((*i).isVarVarOp()) {
          hash=((hash<<8)+((long)((*i).rhsVar().getIdCode())))^hash;
        } else {
          hash=0; // DEQ
        }
      }
      return long(hash) % tabSize;
    }
      long tableSize() const { return tabSize;}
   private:
    long tabSize;
};
#else
class ConstraintSetHashFun {
   public:
    ConstraintSetHashFun() {}
    long operator()(ConstraintSet* cs) const {
      unsigned int hash=1;
      for(ConstraintSet::iterator i=cs->begin();i!=cs->end();++i) {
        // use the symbol-ptr of lhsVar for hashing (we are a friend).
        if((*i).isVarValOp())
          hash=((hash<<8)+((long)(*i).rhsValCppCapsule().getValue().hash()))^hash;
        else if((*i).isVarVarOp()) {
          hash=((hash<<8)+((long)((*i).rhsVar().getIdCode())))^hash;
        } else {
          hash=0; // DEQ
        }
      }
      return long(hash);
    }
   private:
};
#endif

ConstraintSet operator+(ConstraintSet s1, ConstraintSet s2);
bool operator<(const ConstraintSet& s1, const ConstraintSet& s2);
//bool operator==(const ConstraintSet& s1, const ConstraintSet& s2); // std operator is implemented for set
//bool operator!=(const ConstraintSet& s1, const ConstraintSet& s2);
class ConstraintSetEqualToPred {
   public:
    ConstraintSetEqualToPred() {}
    bool operator()(ConstraintSet* s1, ConstraintSet* s2) const {
      //return *s1==*s2;
      if(s1->disequalityExists()&&s2->disequalityExists())
        return true;
      if(s1->size()!=s2->size()) {
        return false;
      } else {
        return *s1==*s2;
        for(ConstraintSet::iterator i=s1->begin(),j=s2->begin();
            i!=s1->end();
            (++i,++j)) {
          if(!(*i==*j))
            return false;
        }
        return true;
      }
    }
   private:
};

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
 class ConstraintSetMaintainer : public HSetMaintainer<ConstraintSet, ConstraintSetHashFun,ConstraintSetEqualToPred> {
 public:
  typedef HSetMaintainer<ConstraintSet, ConstraintSetHashFun,ConstraintSetEqualToPred>::ProcessingResult ProcessingResult;
  string toString();
 };
 
} // end of namespace CodeThorn

#endif
