#ifndef PSTATE_H
#define PSTATE_H

#include <string>
#include <set>
#include <map>
#include <utility>
#include "Labeler.h"
#include "AbstractValue.h"
#include "VariableIdMapping.h"
#include "Lattice.h"

#define NO_STATE -3

typedef int PStateId;

#include "HSetMaintainer.h"

namespace CodeThorn {

  class VariableValueMonitor;
  class CTAnalysis;

  class PState;
  
  typedef PState* PStatePtr; // allow for in-place updates, no longer const; old version: const PState* PStatePtr;

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

    // deprecated, to be replaced with readFromMemoryLocation
    CodeThorn::AbstractValue varValue(AbstractValue varId) const;
    // deprecated, to be replaced by functio using readFromMemoryLocation
    string varValueToString(AbstractValue varId) const;

    void deleteVar(AbstractValue varname);
    long memorySize() const;

    void toStream(std::ostream& os) const;
    string toString() const;
    string toString(CodeThorn::VariableIdMapping* variableIdMapping) const;
    string toDotString(std::string prefix, CodeThorn::VariableIdMapping* variableIdMapping) const;
    std::string dotNodeIdString(std::string prefix, AbstractValue av) const;
    std::set<std::string> getDotNodeIdStrings(std::string prefix) const;
    string memoryValueToDotString(AbstractValue varId, VariableIdMapping* vim) const;

    void writeTopToAllMemoryLocations();
    void combineValueAtAllMemoryLocations(CodeThorn::AbstractValue val);
    void writeValueToAllMemoryLocations(CodeThorn::AbstractValue val);  
    void reserveMemoryLocation(AbstractValue varId);
    void writeUndefToMemoryLocation(AbstractValue varId);
    void writeTopToMemoryLocation(AbstractValue varId);
    AbstractValue readFromMemoryLocation(AbstractValue abstractMemLoc) const;
    void rawCombineAtMemoryLocation(AbstractValue abstractMemLoc,
                                 AbstractValue abstractValue);  
    // strong update overwrites (destructive), if false then it is a weak update and combines with existing values
    void writeToMemoryLocation(AbstractValue abstractMemLoc,
                               AbstractValue abstractValue,
                               bool strongUpdate=true);
    // number of elements in state
    size_t stateSize() const;
    PState::iterator begin();
    PState::iterator end();
    PState::const_iterator begin() const;
    PState::const_iterator end() const;
    bool isApproximatedBy(CodeThorn::PState& other) const;
    static void combineInPlace1st(CodeThorn::PStatePtr p1, CodeThorn::PStatePtr p2);
    static CodeThorn::PState combine(CodeThorn::PStatePtr p1, CodeThorn::PStatePtr p2);
    static CodeThorn::PState combine(CodeThorn::PState& p1, CodeThorn::PState& p2);
    AbstractValueSet getVariableIds() const;

    // additional information required for abstraction of memory regions
    // currently not used
    void registerApproximateMemRegion(VariableId memId);
    void unregisterApproximateMemRegion(VariableId memId);
    bool isApproximateMemRegion(VariableId memId) const;
    int32_t numApproximateMemRegions() const;
    bool hasEqualMemRegionApproximation(const PState& other) const;

  private:
    VariableIdSet _approximationVarIdSet;
    void conditionalApproximateRawWriteToMemoryLocation(AbstractValue abstractAddress, AbstractValue abstractValue,bool strongUpdate);
    // "raw" read does not perform any checks, it reads from the abstractAddress (no top/bot checks, reads a single value from one abstract address (sets cannot be passed))
    AbstractValue rawReadFromMemoryLocation(AbstractValue abstractAddress);
    // "raw" write does not perform any checks, it writes to the abstractAddress (no top/bot checks, writes a single value to one abstract address (sets cannot be passed))
    void rawWriteAtMemoryLocation(AbstractValue abstractAddress, AbstractValue abstractValue);
    static bool combineConsistencyCheck;
  };
  
  std::ostream& operator<<(std::ostream& os, const PState& value);
  typedef std::set<PStatePtr> PStatePtrSet;
  
class PStateHashFun {
   public:
    PStateHashFun() {}
    size_t operator()(PState* s) const {
      size_t hash=1;
      for(PState::iterator i=s->begin();i!=s->end();++i) {
        hash=((hash<<8)+((*i).second.hash()))^hash;
      }
      return hash;
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
  PStateId pstateId(PStatePtr pstate);
  PStateId pstateId(const PState pstate);
  string pstateIdString(PStatePtr pstate);
 private:
};

// define order for PState elements (necessary for PStateSet)
bool operator<(const PState& c1, const PState& c2);
bool operator==(const PState& c1, const PState& c2);
bool operator!=(const PState& c1, const PState& c2);

} // namespace CodeThorn

#endif
