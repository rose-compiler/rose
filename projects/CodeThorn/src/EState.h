#ifndef ESTATE_H
#define ESTATE_H

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
#include "CFAnalysis.h"
#include "AbstractValue.h"
#include "VariableIdMapping.h"
#include "ConstraintRepresentation.h"

#include "PState.h"
#include "InputOutput.h"
#include "CallString.h"

using namespace CodeThorn;

typedef int EStateId;

#include "HashFun.h"
#include "HSetMaintainer.h"

namespace CodeThorn {

  class VariableValueMonitor;
  class Analyzer;

/*! 
  * \author Markus Schordan
  * \date 2012.
 */

  class EState {
  public:
    EState():_label(Label()),_pstate(0),_constraints(0){}
    EState(Label label, const CodeThorn::PState* pstate):_label(label),_pstate(pstate),_constraints(0){}
    EState(Label label, const CodeThorn::PState* pstate, const CodeThorn::ConstraintSet* cset):_label(label),_pstate(pstate),_constraints(cset){}
    EState(Label label, const CodeThorn::PState* pstate, const CodeThorn::ConstraintSet* cset, CodeThorn::InputOutput io):_label(label),_pstate(pstate),_constraints(cset),io(io){}
  EState(Label label, CallString cs, const CodeThorn::PState* pstate, const CodeThorn::ConstraintSet* cset, CodeThorn::InputOutput io):_label(label),_pstate(pstate),_constraints(cset),io(io),callString(cs) {}
    std::string toString() const;
    std::string toString(CodeThorn::VariableIdMapping* variableIdMapping) const;
    std::string toHTML() const; /// multi-line version for dot output
    std::string labelString() const;
    long memorySize() const;
    
    void setLabel(Label lab) { _label=lab; }
    Label label() const { return _label; }
    void setPState(const CodeThorn::PState* pstate) { _pstate=pstate; }
    //void setIO(CodeThorn::InputOutput io) { io=io;} TODO: investigate
    const CodeThorn::PState* pstate() const { return _pstate; }
    const CodeThorn::ConstraintSet* constraints() const { return _constraints; }
    CodeThorn::ConstraintSet allInfoAsConstraints() const;
    CodeThorn::InputOutput::OpType ioOp() const;
    // isBot():no value, isTop(): any value (not unique), isConstInt():one concrete integer (int getIntValue())
    AbstractValue determineUniqueIOValue() const;
    /* Predicate that determines whether all variables can be determined to be bound to a constant value.
       This function uses the entire PState and all available constraints to determine constness.
    */
    bool isConst(CodeThorn::VariableIdMapping* vid) const;
    bool isRersTopified(CodeThorn::VariableIdMapping* vid) const;
    std::string predicateToString(CodeThorn::VariableIdMapping* vid) const;
    std::string programPosToString(Labeler* labeler) const;
    // uses isApproximatedBy of PState
    bool isApproximatedBy(const CodeThorn::EState* other) const;
  private:
    Label _label;
    const CodeThorn::PState* _pstate;
    const CodeThorn::ConstraintSet* _constraints;
  public:
    CodeThorn::InputOutput io;
    CallString callString;
  };

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
   std::string toString(CodeThorn::VariableIdMapping* variableIdMapping=0) const;
   EStateId estateId(const EState* estate) const;
   EStateId estateId(const EState estate) const;
   std::string estateIdString(const EState* estate) const;
   int numberOfIoTypeEStates(CodeThorn::InputOutput::OpType) const;
   int numberOfConstEStates(CodeThorn::VariableIdMapping* vid) const;
 private:
   CodeThorn::ConstraintSetMaintainer* _constraintSetMaintainer; 
 };
 
 class EStateList : public list<EState> {
 public:
   std::string toString();
 };
 
} // namespace CodeThorn

#endif
