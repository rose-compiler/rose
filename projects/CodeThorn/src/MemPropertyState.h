#ifndef MemPROPERTYSTATE_H
#define MemPROPERTYSTATE_H

#include "PropertyState.h"

#include <string>
#include <set>
#include <map>
#include <utility>
#include "Labeler.h"
#include "VariableIdMapping.h"
#include "AbstractValue.h"
#include "Lattice.h"

typedef int MemPropertyStateId;

#include "HashFun.h"
#include "HSetMaintainer.h"
#include "CFAnalysis.h"

using CodeThorn::Edge;

namespace CodeThorn {

  /*!
   * \author Markus Schordan
   */
  class MemPropertyState : public Lattice, private std::map<AbstractValue,CodeThorn::AbstractValue> {
  public:
    MemPropertyState();
    ~MemPropertyState();
    void toStream(std::ostream& os, CodeThorn::VariableIdMapping* vim) override;
    virtual std::string toString(CodeThorn::VariableIdMapping* vim=0) override;
    virtual bool approximatedBy(Lattice& other) const override;
    virtual void combine(Lattice& other) override;
    void setBot(bool val);
    bool isBot() const override;

    typedef MemPropertyState MemLattice;

    // private inharitance ensures MemPropertyState is only used through methods defined here
  public:
    typedef std::map<AbstractValue,CodeThorn::AbstractValue>::const_iterator const_iterator;
    typedef std::map<AbstractValue,CodeThorn::AbstractValue>::iterator iterator;
    friend std::ostream& operator<<(std::ostream& os, const MemPropertyState& value);
    friend std::istream& operator>>(std::istream& os, MemPropertyState& value);
    friend class MemPropertyStateHashFun;
    friend class MemPropertyStateEqualToPred;
    friend bool operator==(const MemPropertyState& c1, const MemPropertyState& c2);
    friend bool operator!=(const MemPropertyState& c1, const MemPropertyState& c2);
    friend bool operator<(const MemPropertyState& s1, const MemPropertyState& s2);
    bool varIsConst(AbstractValue varId) const;
    // deprecated
    bool varExists(AbstractValue varId) const;
    bool memLocExists(AbstractValue memLoc) const;
    bool varIsTop(AbstractValue varId) const;
    CodeThorn::AbstractValue varValue(AbstractValue varId) const;
    string varValueToString(AbstractValue varId) const;
    void deleteVar(AbstractValue varname);
    long memorySize() const;
    void toStream(std::ostream& os);
    string toString() const;
    string toString(CodeThorn::VariableIdMapping* variableIdMapping) const;
    string toDotString(std::string prefix, CodeThorn::VariableIdMapping* variableIdMapping) const;
    std::string dotNodeIdString(std::string prefix, AbstractValue av) const;
    std::set<std::string> getDotNodeIdStrings(std::string prefix) const;
    void writeTopToAllMemoryLocations();
    void combineValueAtAllMemoryLocations(CodeThorn::AbstractValue val);
    void writeValueToAllMemoryLocations(CodeThorn::AbstractValue val);  
    void reserveMemoryLocation(AbstractValue varId);
    void writeUndefToMemoryLocation(AbstractValue varId);
    void writeTopToMemoryLocation(AbstractValue varId);
    AbstractValue readFromMemoryLocation(AbstractValue abstractMemLoc) const;
    void combineAtMemoryLocation(AbstractValue abstractMemLoc,
                                 AbstractValue abstractValue);  
    void writeToMemoryLocation(AbstractValue abstractMemLoc,
                               AbstractValue abstractValue);
    size_t stateSize() const;
    MemPropertyState::iterator begin();
    MemPropertyState::iterator end();
    MemPropertyState::const_iterator begin() const;
    MemPropertyState::const_iterator end() const;
    //static CodeThorn::MemPropertyState combine(CodeThorn::MemPropertyState& p1, CodeThorn::MemPropertyState& p2);
    AbstractValueSet getVariableIds() const;
  private:
    bool _bot=true;
  private:
    static bool combineConsistencyCheck;
  };
  
  std::ostream& operator<<(std::ostream& os, const MemPropertyState& value);
  typedef std::set<const MemPropertyState*> MemPropertyStatePtrSet;
  
  class MemPropertyStateHashFun {
  public:
    MemPropertyStateHashFun() {}
    long operator()(MemPropertyState* s) const {
      unsigned int hash=1;
      for(MemPropertyState::iterator i=s->begin();i!=s->end();++i) {
        hash=((hash<<8)+((long)(*i).second.hash()))^hash;
      }
      return long(hash);
    }
  private:
  };

  class MemPropertyStateEqualToPred {
  public:
    MemPropertyStateEqualToPred() {}
    bool operator()(MemPropertyState* s1, MemPropertyState* s2) const {
      if(s1->size()!=s2->size()) {
        return false;
      } else {
        for(MemPropertyState::iterator i1=s1->begin(), i2=s2->begin();i1!=s1->end();(++i1,++i2)) {
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
  class MemPropertyStateSet : public HSetMaintainer<MemPropertyState,MemPropertyStateHashFun,MemPropertyStateEqualToPred> {
  public:
    typedef HSetMaintainer<MemPropertyState,MemPropertyStateHashFun,MemPropertyStateEqualToPred>::ProcessingResult ProcessingResult;
    string toString();
    MemPropertyStateId pstateId(const MemPropertyState* pstate);
    MemPropertyStateId pstateId(const MemPropertyState pstate);
    string pstateIdString(const MemPropertyState* pstate);
  private:
  };

  // define order for MemPropertyState elements (necessary for MemPropertyStateSet)
  bool operator<(const MemPropertyState& c1, const MemPropertyState& c2);
  bool operator==(const MemPropertyState& c1, const MemPropertyState& c2);
  bool operator!=(const MemPropertyState& c1, const MemPropertyState& c2);

} // namespace CodeThorn

#endif
