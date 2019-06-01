#include "sage3basic.h"

#include "PState.h"
#include "AbstractValue.h"
#include "CollectionOperators.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "CodeThornException.h"

// only necessary for class VariableValueMonitor
#include "Analyzer.h"

using namespace std;
using namespace CodeThorn;

PState::PState() {
}

void PState::toStream(ostream& os) const {
  os<<toString();
}

string PState::toString() const {
  stringstream ss;
  //ss << "PState=";
  ss<< "{";
  for(PState::const_iterator j=begin();j!=end();++j) {
    if(j!=begin()) ss<<",";
    ss<<"(";
    ss<<"@";
    ss <<(*j).first.toString();
    ss<<":";
    ss<<varValueToString((*j).first);
    ss<<")";
  }
  ss<<"}";
  return ss.str();
}

string PState::toString(VariableIdMapping* variableIdMapping) const {
  stringstream ss;
  //ss << "PState=";
  ss<< "{";
  for(PState::const_iterator j=begin();j!=end();++j) {
    if(j!=begin()) 
      ss<<", ";
    //ss<<"(";
    ss<<"@";
    ss <<((*j).first).toString(variableIdMapping);
    ss<<":";
    ss<<(((*j).second).toString(variableIdMapping)); // ss<<varValueToString((*j).first);
    //ss<<")";
  }
  ss<<"}";
  return ss.str();
}

long PState::memorySize() const {
  long mem=0;
  for(PState::const_iterator i=begin();i!=end();++i) {
    mem+=sizeof(*i);
  }
  return mem+sizeof(*this);
}
long EState::memorySize() const {
  return sizeof(*this);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void PState::deleteVar(AbstractValue varId) {
  PState::iterator i=begin();
  while(i!=end()) {
    if((*i).first==varId)
      erase(i++);
    else
      ++i;
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool PState::varExists(AbstractValue varId) const {
  PState::const_iterator i=find(varId);
  return !(i==end());
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool PState::varIsConst(AbstractValue varId) const {
  PState::const_iterator i=find(varId);
  if(i!=end()) {
    AbstractValue val=(*i).second;
    return val.isConstInt()||val.isConstPtr();
  } else {
    // assume that a variable not in state is non-const (has any value)
    // this is currently used in some modes to reduce state size
    return false; // throw CodeThorn::Exception("Error: PState::varIsConst : variable does not exist.";
  }
}
bool PState::varIsTop(AbstractValue varId) const {
  PState::const_iterator i=find(varId);
  if(i!=end()) {
    AbstractValue val=(*i).second;
    return val.isTop();
  } else {
    // TODO: this allows variables (intentionally) not to be in PState but still to analyze
    // however, this check will have to be reinstated once this mode is fully supported
    return false; // throw CodeThorn::Exception("Error: PState::varIsConst : variable does not exist.";
  }
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string PState::varValueToString(AbstractValue varId) const {
  stringstream ss;
  AbstractValue val=varValue(varId);
  return val.toString();
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
AbstractValue PState::varValue(AbstractValue varId) const {
  AbstractValue val=((*(const_cast<PState*>(this)))[varId]);
  return val;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void PState::writeTopToAllMemoryLocations() {
  CodeThorn::AbstractValue val=CodeThorn::Top();
  writeValueToAllMemoryLocations(val);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void PState::writeValueToAllMemoryLocations(CodeThorn::AbstractValue val) {
  for(PState::iterator i=begin();i!=end();++i) {
    AbstractValue varId=(*i).first;
    writeToMemoryLocation(varId,val);
  }
}

void PState::writeTopToMemoryLocation(AbstractValue varId) {
  CodeThorn::AbstractValue val=CodeThorn::Top();
  writeToMemoryLocation(varId, val);
}

AbstractValueSet PState::getVariableIds() const {
  AbstractValueSet varIdSet;
  for(PState::const_iterator i=begin();i!=end();++i) {
    AbstractValue varId=(*i).first;
    varIdSet.insert(varId);
  }
  return varIdSet;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
PStateId PStateSet::pstateId(const PState* pstate) {
  return pstateId(*pstate);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
PStateId PStateSet::pstateId(const PState pstate) {
  PStateId xid=0;
  // MS: TODO: we may want to use the new function id(pstate) here
  for(PStateSet::iterator i=begin();i!=end();++i) {
    if(pstate==**i) {
      return xid;
    }
    xid++;
  }
  return NO_STATE;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */string PStateSet::pstateIdString(const PState* pstate) {
  stringstream ss;
  ss<<pstateId(pstate);
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string PStateSet::toString() {
  stringstream ss;
  ss << "@"<<this<<": PStateSet={";
  int si=0;
  for(PStateSet::iterator i=begin();i!=end();++i) {
    if(i!=begin())
      ss<<", ";
    ss << "S"<<si++<<": "<<(*i)->toString();
  }
  ss << "}";
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
ostream& CodeThorn::operator<<(ostream& os, const PState& pState) {
  pState.toStream(os);
  return os;
}

bool CodeThorn::operator<(const PState& s1, const PState& s2) {
  if(s1.stateSize()!=s2.stateSize())
    return s1.stateSize()<s2.stateSize();
  PState::const_iterator i=s1.begin();
  PState::const_iterator j=s2.begin();
  while(i!=s1.end() && j!=s2.end()) {
    if(*i!=*j) {
      return *i<*j;
    } else {
      ++i;++j;
    }
  }
  assert(i==s1.end() && j==s2.end());
  return false; // both are equal
}

bool CodeThorn::operator==(const PState& c1, const PState& c2) {
  if(c1.stateSize()==c2.stateSize()) {
    PState::const_iterator i=c1.begin();
    PState::const_iterator j=c2.begin();
    while(i!=c1.end()) {
      if(!((*i).first==(*j).first))
        return false;
      if(!((*i).second==(*j).second))
        return false;
      ++i;++j;
    }
    return true;
  } else {
    return false;
  }
}

bool CodeThorn::operator!=(const PState& c1, const PState& c2) {
  return !(c1==c2);
}

AbstractValue PState::readFromMemoryLocation(AbstractValue abstractMemLoc) const {
  if(abstractMemLoc.isTop()) {
    // result can be any value
    return AbstractValue(CodeThorn::Top());
  }
  return this->varValue(abstractMemLoc);
}

void PState::writeToMemoryLocation(AbstractValue abstractMemLoc,
                                   AbstractValue abstractValue) {
  if(abstractValue.isBot()) {
    //cout<<"INFO: writing bot to memory (bot->top conversion)."<<endl;
    abstractValue=AbstractValue(CodeThorn::Top());
  }
  operator[](abstractMemLoc)=abstractValue;
}

size_t PState::stateSize() const {
  return this->size();
}

PState::iterator PState::begin() {
  return map<AbstractValue,CodeThorn::AbstractValue>::begin();
}

PState::iterator PState::end() {
  return map<AbstractValue,CodeThorn::AbstractValue>::end();
}

PState::const_iterator PState::begin() const {
  return map<AbstractValue,CodeThorn::AbstractValue>::begin();
}

PState::const_iterator PState::end() const {
  return map<AbstractValue,CodeThorn::AbstractValue>::end();
}
