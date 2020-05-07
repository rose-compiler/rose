#ifndef PSTATE_H
#define PSTATE_H

#include <string>
#include <set>
#include <map>
#include <utility>
#include "Labeler.h"
#include "AbstractValue.h"
#include "VariableIdMapping.h"
#include "ConstraintRepresentation.h"
#include "Lattice.h"

typedef int PStateId;

#include "HashFun.h"
#include "HSetMaintainer.h"

using CodeThorn::ConstraintSet;
using CodeThorn::ConstraintSetMaintainer;
using CodeThorn::Edge;

//using namespace CodeThorn;

namespace CodeThorn {

  class VariableValueMonitor;
  class Analyzer;
  /*! 
   * \author Markus Schordan
   * \date 2012.
   */
  
  // private inharitance ensures PState is only used through methods defined here
  class PState : private std::map<AbstractValue,CodeThorn::AbstractValue> {
  public:
    typedef std::map<AbstractValue,CodeThorn::AbstractValue>::const_iterator const_iterator;
    typedef std::map<AbstractValue,CodeThorn::AbstractValue>::iterator iterator;
    friend std::ostream& operator<<(std::ostream& os, const PState& value);
    friend std::istream& operator>>(std::istream& os, PState& value);
    friend class PStateHashFun;
    friend class PStateEqualToPred;
    friend bool operator==(const PState& c1, const PState& c2);
    friend bool operator!=(const PState& c1, const PState& c2);
    friend bool operator<(const PState& s1, const PState& s2);
    PState();
    bool varIsConst(AbstractValue varId) const;
    // deprecated
    bool varExists(AbstractValue varId) const;
    bool memLocExists(AbstractValue memLoc) const;
    bool varIsTop(AbstractValue varId) const;
    CodeThorn::AbstractValue varValue(AbstractValue varId) const;
    string varValueToString(AbstractValue varId) const;
    void deleteVar(AbstractValue varname);
    long memorySize() const;
    void toStream(std::ostream& os) const;
    string toString() const;
    string toString(CodeThorn::VariableIdMapping* variableIdMapping) const;
    string toDotString(std::string prefix, CodeThorn::VariableIdMapping* variableIdMapping) const;
    std::string dotNodeIdString(std::string prefix, AbstractValue av) const;
    std::set<std::string> getDotNodeIdStrings(std::string prefix) const;
    void writeTopToAllMemoryLocations();
    void combineValueAtAllMemoryLocations(CodeThorn::AbstractValue val);
    void writeValueToAllMemoryLocations(CodeThorn::AbstractValue val);  
    void writeTopToMemoryLocation(AbstractValue varId);
    AbstractValue readFromMemoryLocation(AbstractValue abstractMemLoc) const;
    void combineAtMemoryLocation(AbstractValue abstractMemLoc,
                                 AbstractValue abstractValue);  
    void writeToMemoryLocation(AbstractValue abstractMemLoc,
                               AbstractValue abstractValue);
    size_t stateSize() const;
    PState::iterator begin();
    PState::iterator end();
    PState::const_iterator begin() const;
    PState::const_iterator end() const;
    bool isApproximatedBy(CodeThorn::PState& other) const;
    static CodeThorn::PState combine(CodeThorn::PState& p1, CodeThorn::PState& p2);
    AbstractValueSet getVariableIds() const;
  private:
    static bool combineConsistencyCheck;
  };
  
  std::ostream& operator<<(std::ostream& os, const PState& value);
  typedef std::set<const PState*> PStatePtrSet;
  
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

// define order for PState elements (necessary for PStateSet)
bool operator<(const PState& c1, const PState& c2);
bool operator==(const PState& c1, const PState& c2);
bool operator!=(const PState& c1, const PState& c2);

} // namespace CodeThorn

#endif
