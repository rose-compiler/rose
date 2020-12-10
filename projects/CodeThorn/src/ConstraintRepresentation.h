#ifndef CONSTRAINT_REPRESENTATION_H
#define CONSTRAINT_REPRESENTATION_H

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <set>
#include <map>
#include <utility>
#include <string>
#include <list>
#include "Labeler.h"
#include "CFAnalysis.h"
#include "AbstractValue.h"
#include "VariableIdMapping.h"
#include "EqualityMaintainer.h"
#include "HSetMaintainer.h"
#include "AbstractValue.h"

using namespace CodeThorn;

namespace CodeThorn {

  typedef std::list<AbstractValue> ListOfAValue;
  typedef AbstractValueSet SetOfVariableId;

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
  Constraint(ConstraintOp op0,CodeThorn::AbstractValue lhs, AbstractValue rhs);
  ConstraintOp op() const;
  CodeThorn::AbstractValue lhsVar() const;
  CodeThorn::AbstractValue rhsVar() const;
  AbstractValue rhsVal() const;
  std::string toString() const;
  std::string toString(VariableIdMapping*) const;
  std::string toAssertionString(VariableIdMapping*) const;
  std::string operatorStringFromStream(istream& is);
  void toStream(ostream& os);
  void toStreamAsTuple(ostream& os);
  bool isVarVarOp() const;
  bool isVarValOp() const;
  bool isEquation() const;
  bool isInequation() const;
  bool isDisequation() const;
  void negate();
  void swapVars();
  void setLhsVar(CodeThorn::AbstractValue lhs) { _lhsVar=lhs; } 
 private:
  void initialize();
  std::string opToString() const;
  ConstraintOp _op;
  CodeThorn::AbstractValue _lhsVar;
  CodeThorn::AbstractValue _rhsVar;
};

bool operator<(const Constraint& c1, const Constraint& c2);
bool operator==(const Constraint& c1, const Constraint& c2);
bool operator!=(const Constraint& c1, const Constraint& c2);

// we use only one disequality constraint to mark constraint set representing non-reachable states
#define DISEQUALITYCONSTRAINT Constraint(Constraint::DEQ,CodeThorn::AbstractValue(),AbstractValue(0))

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class ConstraintSet : public std::set<Constraint> {
 public:
  ConstraintSet constraintsOfVariable(CodeThorn::AbstractValue varId) const;
  bool constraintExists(Constraint::ConstraintOp op, CodeThorn::AbstractValue varId, AbstractValue intVal) const;
  bool constraintExists(Constraint::ConstraintOp op) const;
  ConstraintSet constraintsWithOp(Constraint::ConstraintOp op) const;
  bool constraintExists(const Constraint& c) const;

  // deprecated
  ConstraintSet::iterator findSpecific(Constraint::ConstraintOp op, CodeThorn::AbstractValue varId) const;
  // deprecated
  ConstraintSet findSpecificSet(Constraint::ConstraintOp op, CodeThorn::AbstractValue varId) const;

  std::string toString() const;
  std::string toString(VariableIdMapping* vim) const;
  std::string toStringWithoutBraces(VariableIdMapping* vim) const;
  std::string toAssertionString(VariableIdMapping* vim) const;

  //! returns concrete int-value if equality exists, otherwise Top.
  AbstractValue varAbstractValue(const CodeThorn::AbstractValue varId) const;
  //! returns set of concrete values for which an equality is stored 
  //! (there can be at most one), otherwise the set is empty. 
  //! Note that top may exist as explicit equality if it was added as such.
  ListOfAValue getEqVarConst(const CodeThorn::AbstractValue varId) const;
  //! returns set of concrete values for which an inequality exists
  ListOfAValue getNeqVarConst(const CodeThorn::AbstractValue varId) const;
  SetOfVariableId getEqVars(const CodeThorn::AbstractValue varId) const;

  //! maintains consistency of set and creates DIS if inconsistent constraints are added
  void addConstraint(Constraint c);
  void removeAllConstraintsOfVar(CodeThorn::AbstractValue varId);

#if 0
  ConstraintSet invertedConstraints(); // only correct for single constraints 
  void invertConstraints();            // only correct for single constraints 
#endif

  void addDisequality();
  bool disequalityExists() const;

  void addAssignEqVarVar(CodeThorn::AbstractValue, CodeThorn::AbstractValue);
  void addEqVarVar(CodeThorn::AbstractValue, CodeThorn::AbstractValue);
  //void removeEqualitiesOfVar(CodeThorn::AbstractValue);

  long numberOfConstConstraints(CodeThorn::AbstractValue);
  ConstraintSet& operator+=(ConstraintSet& s2);
  //ConstraintSet operator+(ConstraintSet& s2);
  long memorySize() const;

 private:
  void deleteAndMoveConstConstraints(CodeThorn::AbstractValue lhsVarId, CodeThorn::AbstractValue rhsVarId);
  void moveConstConstraints(CodeThorn::AbstractValue fromVar, CodeThorn::AbstractValue toVar);
  //! modifies internal representation
  void insertConstraint(Constraint c);
  //! modifies internal representation
  void eraseConstraint(Constraint c);
  void eraseConstraint(std::set<Constraint>::iterator i);
  //! modifies internal representation
  void eraseEqWithLhsVar(CodeThorn::AbstractValue);
  void duplicateConstConstraints(CodeThorn::AbstractValue lhsVarId, CodeThorn::AbstractValue rhsVarId);
  //! moves const-constraints from "fromVar" to "toVar". Does maintain consistency, set may be become DEQ.
  void eraseConstConstraints(CodeThorn::AbstractValue);
  EqualityMaintainer<CodeThorn::AbstractValue> equalityMaintainer;
};

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class ConstraintSetHashFun {
   public:
    ConstraintSetHashFun() {}
    long operator()(ConstraintSet* cs) const {
      unsigned int hash=1;
      for(ConstraintSet::iterator i=cs->begin();i!=cs->end();++i) {
        // use the symbol-ptr of lhsVar for hashing (we are a friend).
        if((*i).isVarValOp()) {
          hash=((hash<<8)+((long)(*i).rhsVal().hash()))^hash;
        } else if((*i).isVarVarOp()) {
          hash=((hash<<8)+((long)(*i).rhsVar().hash()))^hash;
        } else {
          hash=0; // DEQ
        }
      }
      return long(hash);
    }
   private:
};

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
  std::string toString();
 };
 
} // end of namespace CodeThorn

#endif
