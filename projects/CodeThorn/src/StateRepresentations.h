#ifndef STATE_REPRESENTATIONS_H
#define STATE_REPRESENTATIONS_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#define USER_DEFINED_PSTATE_COMP

#include <string>
#include <set>
#include <map>
#include <utility>
#include "Labeler.h"
#include "CFAnalysis.h"
#include "AType.h"
#include "VariableIdMapping.h"
#include "ConstraintRepresentation.h"

using namespace std;

typedef int PStateId;
typedef int EStateId;

#include "HashFun.h"
#include "HSetMaintainer.h"

using CodeThorn::AValue;
using CodeThorn::ConstraintSet;
using CodeThorn::ConstraintSetMaintainer;
using SPRAY::Edge;

using namespace SPRAY;

namespace CodeThorn {

  class VariableValueMonitor;
  class Analyzer;
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class PState : public map<VariableId,CodeThorn::AValue> {
 public:
    PState() {
    }
  friend ostream& operator<<(ostream& os, const PState& value);
  friend istream& operator>>(istream& os, PState& value);
  bool varExists(VariableId varId) const;
  bool varIsConst(VariableId varId) const;
  bool varIsTop(VariableId varId) const;
  AValue varValue(VariableId varId) const;
  string varValueToString(VariableId varId) const;
  void deleteVar(VariableId varname);
  long memorySize() const;
  void fromStream(istream& is);
  void toStream(ostream& os) const;
  string toString() const;
  string toString(VariableIdMapping* variableIdMapping) const;
  void setAllVariablesToTop();
  void setAllVariablesToValue(CodeThorn::AValue val);
  void setVariableToTop(VariableId varId);
  void setVariableToValue(VariableId varId, CodeThorn::AValue val);
  void topifyState();
  bool isTopifiedState() const;
  VariableIdSet getVariableIds() const;
  static void setActiveGlobalTopify(bool val);
  static void setVariableValueMonitor(VariableValueMonitor* vvm);
  static bool _activeGlobalTopify;
  static VariableValueMonitor* _variableValueMonitor;
  static Analyzer* _analyzer;
};

  ostream& operator<<(ostream& os, const PState& value);
  istream& operator>>(istream& os, PState& value);

  typedef set<const PState*> PStatePtrSet;

#ifdef USE_CUSTOM_HSET
class PStateHashFun {
   public:
    PStateHashFun(long prime=9999991) : tabSize(prime) {}
    long operator()(PState s) const {
      unsigned int hash=1;
      for(PState::iterator i=s.begin();i!=s.end();++i) {
        hash=((hash<<8)+((long)(*i).second.hash()))^hash;
      }
      return long(hash) % tabSize;
    }
      long tableSize() const { return tabSize;}
   private:
    long tabSize;
};
#else
class PStateHashFun {
   public:
    PStateHashFun() {}
    long operator()(PState* s) const {
      unsigned int hash=1;
      for(PState::iterator i=s->begin();i!=s->end();++i) {
        hash=((hash<<8)+((long)(*i).second.hash()))^hash;
      }
      return long(hash);
    }
   private:
};
#endif
class PStateEqualToPred {
   public:
    PStateEqualToPred() {}
    bool operator()(PState* s1, PState* s2) const {
      if(s1->size()!=s2->size()) {
        return false;
      } else {
        for(PState::iterator i1=s1->begin(), i2=s2->begin();i1!=s1->end();(++i1,++i2)) {
          if(*i1!=*i2)
            return false;
        }
      }
      return true;
    }
   private:
};

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
 class PStateSet : public HSetMaintainer<PState,PStateHashFun,PStateEqualToPred> {
 public:
  typedef HSetMaintainer<PState,PStateHashFun,PStateEqualToPred>::ProcessingResult ProcessingResult;
  string toString();
  PStateId pstateId(const PState* pstate);
  PStateId pstateId(const PState pstate);
  string pstateIdString(const PState* pstate);
 private:
};

/*! 
  * \author Markus Schordan
  * \date 2012.
 * \brief Input: a value val is read into a variable var
 * Output: either a variable or a value is written
 */
class InputOutput {
 public:
 InputOutput():op(NONE),var(VariableId()){ val=CodeThorn::AType::Bot();}
  enum OpType {NONE,STDIN_VAR,STDOUT_VAR,STDOUT_CONST,STDERR_VAR,STDERR_CONST, FAILED_ASSERT,VERIFICATION_ERROR};
  OpType op;
  VariableId var;
  CodeThorn::AType::ConstIntLattice val;
  string toString() const;
  string toString(VariableIdMapping* variableIdMapping) const;
  void recordVariable(OpType op, VariableId varId);
  void recordConst(OpType op, CodeThorn::AType::ConstIntLattice val);
  void recordConst(OpType op, int val);
  void recordFailedAssert();
  void recordVerificationError();
  bool isStdInIO() const { return op==STDIN_VAR; }
  bool isStdOutIO() const { return op==STDOUT_VAR || op==STDOUT_CONST; }
  bool isStdErrIO() const { return op==STDERR_VAR || op==STDERR_CONST; }
  bool isFailedAssertIO() const { return op==FAILED_ASSERT; }
  bool isVerificationError() const { return op==VERIFICATION_ERROR; }
  bool isNonIO() const { return op==NONE; }
};

bool operator<(const InputOutput& c1, const InputOutput& c2);
bool operator==(const InputOutput& c1, const InputOutput& c2);
bool operator!=(const InputOutput& c1, const InputOutput& c2);


/*! 
  * \author Marc Jasper
  * \date 2016.
 */

typedef std::vector<Label> ParProLabel;

class ParProEState {
  public:
  ParProEState():_parProLabel(ParProLabel()){}
  ParProEState(ParProLabel label):_parProLabel(label){}
  ParProLabel getLabel() const { return _parProLabel; }
  string toString() const;
  
  private:
  ParProLabel _parProLabel; 
};

// define order for ParProEState elements (necessary for ParProEStateSet)
bool operator<(const ParProEState& c1, const ParProEState& c2);
bool operator==(const ParProEState& c1, const ParProEState& c2);
bool operator!=(const ParProEState& c1, const ParProEState& c2);

struct ParProEStateLessComp {
  bool operator()(const ParProEState& c1, const ParProEState& c2) {
    return c1<c2;
  }
};

#ifdef USE_CUSTOM_HSET
class ParProEStateHashFun {
   public:
    ParProEStateHashFun(long prime=9999991) : tabSize(prime) {}
    long operator()(ParProEState s) const {
      unsigned int hash=1;
      ParProLabel label=s->getLabel();
      for (ParProLabel::iterator i=label.begin(); i!=label.end(); i++) {
	hash*=(1 + *i.getId());
      }
      return long(hash) % tabSize;
    }
    long tableSize() const { return tabSize;}
   private:
    long tabSize;
};
#else
class ParProEStateHashFun {
   public:
    ParProEStateHashFun() {}
    long operator()(ParProEState* s) const {
      unsigned int hash=1;
      ParProLabel label=s->getLabel();
      for (ParProLabel::iterator i=label.begin(); i!=label.end(); i++) {
	hash*=(1 + i->getId());
      }
      return long(hash);
    }
   private:
};
#endif
class ParProEStateEqualToPred {
   public:
    ParProEStateEqualToPred() {}
    long operator()(ParProEState* s1, ParProEState* s2) const {
      return *s1==*s2;
    }
   private:
};

class ParProEStateSet : public HSetMaintainer<ParProEState,ParProEStateHashFun,ParProEStateEqualToPred> {
  public:
  ParProEStateSet():HSetMaintainer<ParProEState,ParProEStateHashFun,ParProEStateEqualToPred>(),_constraintSetMaintainer(0){}
  public:
   typedef HSetMaintainer<ParProEState,ParProEStateHashFun,ParProEStateEqualToPred>::ProcessingResult ProcessingResult;
  private:
   ConstraintSetMaintainer* _constraintSetMaintainer; 
};


/*! 
  * \author Markus Schordan
  * \date 2012.
 */

class EState {
 public:
 EState():_label(Label()),_pstate(0),_constraints(0){}
 EState(Label label, const PState* pstate):_label(label),_pstate(pstate),_constraints(0){}
 EState(Label label, const PState* pstate, const ConstraintSet* cset):_label(label),_pstate(pstate),_constraints(cset){}
 EState(Label label, const PState* pstate, const ConstraintSet* cset, InputOutput io):_label(label),_pstate(pstate),_constraints(cset),io(io){}

  string toString() const;
  string toString(VariableIdMapping* variableIdMapping) const;
  string toHTML() const; /// multi-line version for dot output
  long memorySize() const;

  void setLabel(Label lab) { _label=lab; }
  Label label() const { return _label; }
  void setPState(const PState* pstate) { _pstate=pstate; }
  //void setIO(InputOutput io) { io=io;} TODO: investigate
  const PState* pstate() const { return _pstate; }
  const ConstraintSet* constraints() const { return _constraints; }
  ConstraintSet allInfoAsConstraints() const;
  InputOutput::OpType ioOp() const;
  // isBot():no value, isTop(): any value (not unique), isConstInt():one concrete integer (int getIntValue())
  AType::ConstIntLattice determineUniqueIOValue() const;
  /* Predicate that determines whether all variables can be determined to be bound to a constant value.
     This function uses the entire PState and all available constraints to determine constness.
   */
  bool isConst(VariableIdMapping* vid) const;
  bool isRersTopified(VariableIdMapping* vid) const;
  string predicateToString(VariableIdMapping* vid) const;
 private:
  Label _label;
  const PState* _pstate;
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

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
#ifdef USE_CUSTOM_HSET
class EStateHashFun {
   public:
    EStateHashFun(long prime=9999991) : tabSize(prime) {}
    long operator()(EState s) const {
      unsigned int hash=1;
      hash=(long)s.label()*(((long)s.pstate())+1)*(((long)s.constraints())+1);
      return long(hash) % tabSize;
    }
    long tableSize() const { return tabSize;}
   private:
    long tabSize;
};
#else
class EStateHashFun {
   public:
    EStateHashFun() {}
    long operator()(EState* s) const {
      unsigned int hash=1;
      hash=(long)s->label().getId()*(((long)s->pstate())+1)*(((long)s->constraints())+1);
      return long(hash);
    }
   private:
};
#endif
class EStateEqualToPred {
   public:
    EStateEqualToPred() {}
    long operator()(EState* s1, EState* s2) const {
      return *s1==*s2;
    }
   private:
};
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
 class EStateSet : public HSetMaintainer<EState,EStateHashFun,EStateEqualToPred> {
 public:
 EStateSet():HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>(),_constraintSetMaintainer(0){}
 public:
  typedef HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult ProcessingResult;
  string toString(VariableIdMapping* variableIdMapping=0) const;
  EStateId estateId(const EState* estate) const;
  EStateId estateId(const EState estate) const;
  string estateIdString(const EState* estate) const;
  int numberOfIoTypeEStates(InputOutput::OpType) const;
  int numberOfConstEStates(VariableIdMapping* vid) const;
 private:
  ConstraintSetMaintainer* _constraintSetMaintainer; 
};

class EStateList : public list<EState> {
 public:
  string toString();
};

} // namespace CodeThorn

#endif
