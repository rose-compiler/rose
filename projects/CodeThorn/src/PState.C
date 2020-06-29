#include "sage3basic.h"

#include "PState.h"
#include "AbstractValue.h"
#include "CollectionOperators.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "CodeThornException.h"

// only necessary for class VariableValueMonitor
//#include "Analyzer.h"

using namespace std;
using namespace CodeThorn;

// static configuration variable
bool PState::combineConsistencyCheck=false;

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

std::set<std::string> PState::getDotNodeIdStrings(std::string prefix) const {
  std::set<std::string> nodeIds;
  for(PState::const_iterator j=begin();j!=end();++j) {
    nodeIds.insert(dotNodeIdString(prefix,(*j).first));
    if((*j).first.isPtr()) {
      // need to insert also target if pointer value. Using set ensures no duplicates for shared targets.
      if((*j).second.isPtr())
        nodeIds.insert(dotNodeIdString(prefix,(*j).second)); 
    }
  }
  return nodeIds;
}

std::string PState::dotNodeIdString(std::string prefix, AbstractValue av) const {
  stringstream ss;
  ss<<prefix<<string("n")<<this<<av.toString();
  return ss.str();
}

string PState::toDotString(std::string prefix, VariableIdMapping* variableIdMapping) const {
  stringstream ss;
  for(PState::const_iterator j=begin();j!=end();++j) {
    //    AbstractValue v1=(*j).first;
    AbstractValue v2=(*j).second;
    // this pointer is used to get unique names for all elements of a PState
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

long PState::memorySize() const {
  long mem=0;
  for(PState::const_iterator i=begin();i!=end();++i) {
    mem+=sizeof(*i);
  }
  return mem+sizeof(*this);
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
bool PState::memLocExists(AbstractValue memLoc) const {
  if(AbstractValue::byteMode) {
    AlignedMemLoc aMemLoc=memLoc.alignedMemLoc();
    return find(aMemLoc.memLoc)!=end();
  }
  return find(memLoc)!=end();
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
string PState::varValueToString(AbstractValue av) const {
  stringstream ss;
  AbstractValue val=varValue(av);
  return val.toString();
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
  * \date 2019.
 */
void PState::combineValueAtAllMemoryLocations(AbstractValue val) {
  for(PState::iterator i=begin();i!=end();++i) {
    AbstractValue memLoc=(*i).first;
    if(!memLoc.isRef()) {
      combineAtMemoryLocation(memLoc,val);
    }
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void PState::writeValueToAllMemoryLocations(CodeThorn::AbstractValue val) {
  for(PState::iterator i=begin();i!=end();++i) {
    AbstractValue av=(*i).first;
    writeToMemoryLocation(av,val);
  }
}

void PState::reserveMemoryLocation(AbstractValue av) {
  writeUndefToMemoryLocation(av);
}

void PState::writeUndefToMemoryLocation(AbstractValue av) {
  AbstractValue undefValue=AbstractValue::createUndefined();
  writeToMemoryLocation(av, undefValue);
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

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
AbstractValue PState::varValue(AbstractValue av) const {
  AbstractValue val=((*(const_cast<PState*>(this)))[av]);
  return val;
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
    // writing bot to memory (bot->top conversion)
    abstractValue=AbstractValue(CodeThorn::Top());
  }
  if(abstractMemLoc.isTop()) {
    combineValueAtAllMemoryLocations(abstractValue); // BUG: leads to infinite loop in DOM029
    return;
  } else {
    if(AbstractValue::byteMode) {
      VariableId varId=abstractMemLoc.getVariableId();
      long int offset=abstractMemLoc.getIndexIntValue();
      ROSE_ASSERT(AbstractValue::_variableIdMapping);
      long int pointerValueElemSize=abstractMemLoc.getElementTypeSize();
      long int inStateElemSize=(long int)AbstractValue::_variableIdMapping->getElementSize(varId);
      abstractMemLoc.setElementTypeSize(inStateElemSize); // adapt element size when storing in state
      if(pointerValueElemSize!=inStateElemSize) {
        // create new abstractMemLoc
        //cout<<"memloc:"<<abstractMemLoc.toString(AbstractValue::_variableIdMapping)<<endl;
        //cout<<"elemSize: "<<elemSize<<endl;
        if(inStateElemSize!=0) {
          //cout<<"DEBUG: offset     : "<<offset<<endl;
          long int withinElementOffset=offset%inStateElemSize;
          //cout<<"DEBUG: withinElementOffset: "<<withinElementOffset<<endl;
          if(withinElementOffset!=0) {
            // TODO: access within element with mod as byte offset
            // need to know the element size of the pointer to mask it properly
            offset-=withinElementOffset;
            //cout<<"DEBUG: adj. offset: "<<offset;
            abstractMemLoc.setValue(offset); // adjustment to element-aligned offset
            operator[](abstractMemLoc)=abstractValue; // TODO: write target size and value must be masked!
          }
          operator[](abstractMemLoc)=abstractValue; // TODO: write value must be masked!
        } else {
          operator[](abstractMemLoc)=abstractValue; // should not happen (elemsize=0)
        }
      } else {
        operator[](abstractMemLoc)=abstractValue; // elem size is the same
      }
    } else {
      operator[](abstractMemLoc)=abstractValue; // not in bytemode
    }
  }
}

void PState::combineAtMemoryLocation(AbstractValue abstractMemLoc,
                                     AbstractValue abstractValue) {
  AbstractValue currentValue=readFromMemoryLocation(abstractMemLoc);
  AbstractValue newValue=AbstractValue::combine(currentValue,abstractValue);
  if(!abstractMemLoc.isTop()&&!abstractMemLoc.isBot()) 
    writeToMemoryLocation(abstractMemLoc,newValue);
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

// Lattice functions
bool PState::isApproximatedBy(CodeThorn::PState& other) const {
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

CodeThorn::PState PState::combine(CodeThorn::PState& p1, CodeThorn::PState& p2) {
  CodeThorn::PState res;
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
  if(PState::combineConsistencyCheck) {
    // consistency check: all elements of p1 and p2 must be represented in res
    for(auto elem1:p1) {
      if(res.find(elem1.first)==res.end()) {
        cerr<<"Error: Element of PState1 "<<elem1.first.toString()<<" not in combined state."<<endl;
        exit(1);
      }
    }
    for(auto elem2:p2) {
      if(res.find(elem2.first)==res.end()) {
        cerr<<"Error: Element of PState2 "<<elem2.first.toString()<<" not in combined state."<<endl;
        exit(1);
      }
    }
  }
  return res;
}
