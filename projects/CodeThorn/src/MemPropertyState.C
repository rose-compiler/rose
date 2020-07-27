#include "sage3basic.h"


#include "MemPropertyState.h"
#include "SetAlgo.h"
#include <iostream>
#include "AbstractValue.h"
#include "CollectionOperators.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "CodeThornException.h"

using namespace CodeThorn;
using namespace std;

// static configuration variable
bool MemPropertyState::combineConsistencyCheck=false;

CodeThorn::MemPropertyState::MemPropertyState() {
  setBot(true);
}

CodeThorn::MemPropertyState::~MemPropertyState() {
}

void CodeThorn::MemPropertyState::setBot(bool flag) {
  _bot=flag;
}

bool CodeThorn::MemPropertyState::isBot() const {
  return _bot;
}

//void CodeThorn::MemPropertyState::toStream(ostream& os, CodeThorn::VariableIdMapping* vim) {
//}

std::string CodeThorn::MemPropertyState::toString(CodeThorn::VariableIdMapping* vim) {
  return "";
}

// Lattice functions
bool MemPropertyState::approximatedBy(CodeThorn::Lattice& other0) const {
  MemPropertyState& other=dynamic_cast<MemPropertyState&>(other0);
  if(isBot()&&other.isBot())
    return true;
  if(isBot()) {
    return true;
  } else {
    if(other.isBot()) {
      return false;
    }
  }
  assert(!isBot()&&!other.isBot());

  // check if all values of 'this' are approximated by 'other'
  for(auto elem:*this) {
    auto iter=other.find(elem.first);
    if(iter!=other.end()) {
      if(!AbstractValue::approximatedBy(elem.second,(*iter).second)) {
        return false;
      }
    } else {
      // a variable of 'this' is not in state of 'other'
      return false;
    }
  }
  // all values stored in memory locations of 'this' are approximated
  // by values of the corresponding memory location in
  // 'other'. TODO: if the memory location itself is a summary.
  return true;
}

void MemPropertyState::combine(CodeThorn::Lattice& other) {
  MemPropertyState* pOther=dynamic_cast<MemPropertyState*>(&other);
  ROSE_ASSERT(pOther);
  if(!isBot()||!pOther->isBot()) {
    setBot(false);
  }
  MemPropertyState& p1=*this;
  MemPropertyState& p2=*pOther;
  MemPropertyState res;
  size_t numMatched=0;
  for(auto elem1:p1) {
    auto iter=p2.find(elem1.first);
    if(iter!=p2.end()) {
      // same memory location in both states: elem.first==(*iter).first
      // combine values elem.second and (*iter).second

      res.writeToMemoryLocation(elem1.first,AbstractValue::combine(elem1.second,(*iter).second));
      numMatched++;
    } else {
      // a variable of 'p1' is not in state of 'p2', add to result state
      res.writeToMemoryLocation(elem1.first,elem1.second);
    }
  }
  // add elements that are only in p2 to res - this can only be the
  // case if the number of matched elements above is different to p2.size()
  if(numMatched!=p2.size()) {
    for(auto elem2:p2) {
      // only add elements of p2 that are not in p1
      if(p1.find(elem2.first)==p1.end()) {
        res.writeToMemoryLocation(elem2.first,elem2.second);
      }
    }
  }
  if(MemPropertyState::combineConsistencyCheck) {
    // consistency check: all elements of p1 and p2 must be represented in res
    for(auto elem1:p1) {
      if(res.find(elem1.first)==res.end()) {
        cerr<<"Error: Element of MemPropertyState1 "<<elem1.first.toString()<<" not in combined state."<<endl;
        exit(1);
      }
    }
    for(auto elem2:p2) {
      if(res.find(elem2.first)==res.end()) {
        cerr<<"Error: Element of MemPropertyState2 "<<elem2.first.toString()<<" not in combined state."<<endl;
        exit(1);
      }
    }
  }
  *this=res;
}

void MemPropertyState::toStream(ostream& os, CodeThorn::VariableIdMapping* vim) {
  os<<toString(vim);
}

void MemPropertyState::toStream(ostream& os) {
  os<<toString();
}

string MemPropertyState::toString() const {
  stringstream ss;
  //ss << "MemPropertyState=";
  ss<< "{";
  for(MemPropertyState::const_iterator j=begin();j!=end();++j) {
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

string MemPropertyState::toString(VariableIdMapping* variableIdMapping) const {
  stringstream ss;
  //ss << "MemPropertyState=";
  ss<< "{";
  for(MemPropertyState::const_iterator j=begin();j!=end();++j) {
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

std::set<std::string> MemPropertyState::getDotNodeIdStrings(std::string prefix) const {
  std::set<std::string> nodeIds;
  for(MemPropertyState::const_iterator j=begin();j!=end();++j) {
    nodeIds.insert(dotNodeIdString(prefix,(*j).first));
    if((*j).first.isPtr()) {
      // need to insert also target if pointer value. Using set ensures no duplicates for shared targets.
      if((*j).second.isPtr())
        nodeIds.insert(dotNodeIdString(prefix,(*j).second)); 
    }
  }
  return nodeIds;
}

std::string MemPropertyState::dotNodeIdString(std::string prefix, AbstractValue av) const {
  stringstream ss;
  ss<<prefix<<string("n")<<this<<av.toString();
  return ss.str();
}

string MemPropertyState::toDotString(std::string prefix, VariableIdMapping* variableIdMapping) const {
  stringstream ss;
  for(MemPropertyState::const_iterator j=begin();j!=end();++j) {
    //    AbstractValue v1=(*j).first;
    AbstractValue v2=(*j).second;
    // this pointer is used to get unique names for all elements of a MemPropertyState
    if(v2.isPtr()) {
      // nodes
      ss<<"\""<<dotNodeIdString(prefix,(*j).first)<<"\"" << " [label=\""<<(*j).first.toString(variableIdMapping)<<"\"];"<<endl;
      ss<<"\""<<dotNodeIdString(prefix,(*j).second)<<"\""<< " [label=\""<<(*j).second.toString(variableIdMapping)<<"\"];"<<endl;
      //endl; // target label intentionally not generated
      // edge
      ss <<"\""<<dotNodeIdString(prefix,(*j).first)<<"\"";
      ss<<"->";
      ss<<"\""<<dotNodeIdString(prefix,(*j).second)<<"\" [weight=\"0.0\"]";
      ss<<";"<<endl;
    } else {
      ss<<"\""<<dotNodeIdString(prefix,(*j).first)<<"\"" << " [label=\""<<(*j).first.toString(variableIdMapping)<<":"<<(*j).second.toString(variableIdMapping)<<"\"];"<<endl;
    }
  }  
  return ss.str();
}

long MemPropertyState::memorySize() const {
  long mem=0;
  for(MemPropertyState::const_iterator i=begin();i!=end();++i) {
    mem+=sizeof(*i);
  }
  return mem+sizeof(*this);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void MemPropertyState::deleteVar(AbstractValue varId) {
  MemPropertyState::iterator i=begin();
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
bool MemPropertyState::varExists(AbstractValue varId) const {
  MemPropertyState::const_iterator i=find(varId);
  return !(i==end());
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool MemPropertyState::memLocExists(AbstractValue memLoc) const {
  MemPropertyState::const_iterator i=find(memLoc);
  return !(i==end());
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool MemPropertyState::varIsConst(AbstractValue varId) const {
  MemPropertyState::const_iterator i=find(varId);
  if(i!=end()) {
    AbstractValue val=(*i).second;
    return val.isConstInt()||val.isConstPtr();
  } else {
    // assume that a variable not in state is non-const (has any value)
    // this is currently used in some modes to reduce state size
    return false; // throw CodeThorn::Exception("Error: MemPropertyState::varIsConst : variable does not exist.";
  }
}
bool MemPropertyState::varIsTop(AbstractValue varId) const {
  MemPropertyState::const_iterator i=find(varId);
  if(i!=end()) {
    AbstractValue val=(*i).second;
    return val.isTop();
  } else {
    // TODO: this allows variables (intentionally) not to be in MemPropertyState but still to analyze
    // however, this check will have to be reinstated once this mode is fully supported
    return false; // throw CodeThorn::Exception("Error: MemPropertyState::varIsConst : variable does not exist.";
  }
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string MemPropertyState::varValueToString(AbstractValue varId) const {
  stringstream ss;
  AbstractValue val=varValue(varId);
  return val.toString();
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
AbstractValue MemPropertyState::varValue(AbstractValue varId) const {
  AbstractValue val=((*(const_cast<MemPropertyState*>(this)))[varId]);
  return val;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void MemPropertyState::writeTopToAllMemoryLocations() {
  CodeThorn::AbstractValue val=CodeThorn::Top();
  writeValueToAllMemoryLocations(val);
}

/*! 
  * \author Markus Schordan
  * \date 2019.
 */
void MemPropertyState::combineValueAtAllMemoryLocations(AbstractValue val) {
  for(MemPropertyState::iterator i=begin();i!=end();++i) {
    AbstractValue memLoc=(*i).first;
    combineAtMemoryLocation(memLoc,val);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void MemPropertyState::writeValueToAllMemoryLocations(CodeThorn::AbstractValue val) {
  for(MemPropertyState::iterator i=begin();i!=end();++i) {
    AbstractValue varId=(*i).first;
    writeToMemoryLocation(varId,val);
  }
}

void MemPropertyState::reserveMemoryLocation(AbstractValue varId) {
  writeUndefToMemoryLocation(varId);
}

void MemPropertyState::writeUndefToMemoryLocation(AbstractValue varId) {
  AbstractValue undefValue=AbstractValue::createUndefined();
  writeToMemoryLocation(varId, undefValue);
}

void MemPropertyState::writeTopToMemoryLocation(AbstractValue varId) {
  CodeThorn::AbstractValue val=CodeThorn::Top();
  writeToMemoryLocation(varId, val);
}

AbstractValueSet MemPropertyState::getVariableIds() const {
  AbstractValueSet varIdSet;
  for(MemPropertyState::const_iterator i=begin();i!=end();++i) {
    AbstractValue varId=(*i).first;
    varIdSet.insert(varId);
  }
  return varIdSet;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
MemPropertyStateId MemPropertyStateSet::pstateId(const MemPropertyState* pstate) {
  return pstateId(*pstate);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
MemPropertyStateId MemPropertyStateSet::pstateId(const MemPropertyState pstate) {
  MemPropertyStateId xid=0;
  // MS: TODO: we may want to use the new function id(pstate) here
  for(MemPropertyStateSet::iterator i=begin();i!=end();++i) {
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
 */string MemPropertyStateSet::pstateIdString(const MemPropertyState* pstate) {
  stringstream ss;
  ss<<pstateId(pstate);
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string MemPropertyStateSet::toString() {
  stringstream ss;
  ss << "@"<<this<<": MemPropertyStateSet={";
  int si=0;
  for(MemPropertyStateSet::iterator i=begin();i!=end();++i) {
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
ostream& CodeThorn::operator<<(ostream& os, const MemPropertyState& pState) {
  const_cast<MemPropertyState&>(pState).toStream(os);
  return os;
}

bool CodeThorn::operator<(const MemPropertyState& s1, const MemPropertyState& s2) {
  if(s1.stateSize()!=s2.stateSize())
    return s1.stateSize()<s2.stateSize();
  MemPropertyState::const_iterator i=s1.begin();
  MemPropertyState::const_iterator j=s2.begin();
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

bool CodeThorn::operator==(const MemPropertyState& c1, const MemPropertyState& c2) {
  if(c1.stateSize()==c2.stateSize()) {
    MemPropertyState::const_iterator i=c1.begin();
    MemPropertyState::const_iterator j=c2.begin();
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

bool CodeThorn::operator!=(const MemPropertyState& c1, const MemPropertyState& c2) {
  return !(c1==c2);
}

AbstractValue MemPropertyState::readFromMemoryLocation(AbstractValue abstractMemLoc) const {
  if(abstractMemLoc.isTop()) {
    // result can be any value
    return AbstractValue(CodeThorn::Top());
  }
  return this->varValue(abstractMemLoc);
}

void MemPropertyState::writeToMemoryLocation(AbstractValue abstractMemLoc,
                                   AbstractValue abstractValue) {
  if(abstractValue.isBot()) {
    // writing bot to memory (bot->top conversion)
    abstractValue=AbstractValue(CodeThorn::Top());
  }
  if(abstractMemLoc.isTop()) {
    //combineValueAtAllMemoryLocations(abstractValue); // BUG: leads to infinite loop in DOM029
  } else {
    operator[](abstractMemLoc)=abstractValue;
  }
}

void MemPropertyState::combineAtMemoryLocation(AbstractValue abstractMemLoc,
                                     AbstractValue abstractValue) {
  AbstractValue currentValue=operator[](abstractMemLoc);
  AbstractValue newValue=AbstractValue::combine(currentValue,abstractValue);
  operator[](abstractMemLoc)=newValue;
}

size_t MemPropertyState::stateSize() const {
  return this->size();
}

MemPropertyState::iterator MemPropertyState::begin() {
  return map<AbstractValue,CodeThorn::AbstractValue>::begin();
}

MemPropertyState::iterator MemPropertyState::end() {
  return map<AbstractValue,CodeThorn::AbstractValue>::end();
}

MemPropertyState::const_iterator MemPropertyState::begin() const {
  return map<AbstractValue,CodeThorn::AbstractValue>::begin();
}

MemPropertyState::const_iterator MemPropertyState::end() const {
  return map<AbstractValue,CodeThorn::AbstractValue>::end();
}

