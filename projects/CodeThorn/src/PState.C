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

/*! 
  * \author Markus Schordan
  * \date 2012.
  * \brief read: regexp: '{' ( '('<varId>','<varValue>')' )* '}'
 */
void PState::fromStream(istream& is) {
  char c;
  string s;
  int __varIdCode=-1; 
  VarAbstractValue __varId; 
  AValue __varAValue; 
  if(!SPRAY::Parse::checkWord("{",is)) throw CodeThorn::Exception("Error: Syntax error PState. Expected '{'.");
  is>>c;
  // read pairs (varname,varvalue)
  while(c!='}') {
    if(c!='(') throw CodeThorn::Exception("Error: Syntax error PState. Expected '('.");
    is>>c;
    if(c!='V') throw CodeThorn::Exception("Error: Syntax error PState. Expected VariableId.");
    is>>__varIdCode;
    assert(__varIdCode>=0);
    VarAbstractValue __varId;
    __varId.setIdCode(__varIdCode);
    is>>c;
    if(c!=',') { cout << "Error: found "<<c<<"__varIdCode="<<__varIdCode<<endl; throw CodeThorn::Exception("Error: Syntax error PState. Expected ','.");}
    is>>__varAValue;
    is>>c;    
    if(c!=')' && c!=',') throw CodeThorn::Exception("Error: Syntax error PState. Expected ')' or ','.");
    is>>c;
    //cout << "DEBUG: Read from istream: ("<<__varId.toString()<<","<<__varAValue.toString()<<")"<<endl;
    (*this)[__varId]=__varAValue;
    if(c==',') is>>c;
  }
  if(c!='}') throw CodeThorn::Exception("Error: Syntax error PState. Expected '}'.");
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
    ss <<(*j).first.toString();
#if 0
    ss<<"->";
#else
    ss<<",";
#endif
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
    if(j!=begin()) ss<<", ";
    ss<<"(";
    ss <<variableIdMapping->uniqueShortVariableName((*j).first);
#if 0
    ss<<"->";
#else
    ss<<",";
#endif
    ss<<(((*j).second).toString(variableIdMapping)); // ss<<varValueToString((*j).first);
    ss<<")";
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
void PState::deleteVar(VarAbstractValue varId) {
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
bool PState::varExists(VarAbstractValue varId) const {
  PState::const_iterator i=find(varId);
  return !(i==end());
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool PState::varIsConst(VarAbstractValue varId) const {
  PState::const_iterator i=find(varId);
  if(i!=end()) {
    AValue val=(*i).second;
    return val.isConstInt();
  } else {
    // TODO: this allows variables (intentionally) not to be in PState but still to analyze
    // however, this check will have to be reinstated once this mode is fully supported
    return false; // throw CodeThorn::Exception("Error: PState::varIsConst : variable does not exist.";
  }
}
bool PState::varIsTop(VarAbstractValue varId) const {
  PState::const_iterator i=find(varId);
  if(i!=end()) {
    AValue val=(*i).second;
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
string PState::varValueToString(VarAbstractValue varId) const {
  stringstream ss;
  AValue val=varValue(varId);
  return val.toString();
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
AValue PState::varValue(VarAbstractValue varId) const {
  AValue val=((*(const_cast<PState*>(this)))[varId]);
  return val;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void PState::setAllVariablesToTop() {
  CodeThorn::AValue val=CodeThorn::Top();
  setAllVariablesToValue(val);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void PState::setAllVariablesToValue(CodeThorn::AValue val) {
  for(PState::iterator i=begin();i!=end();++i) {
    VarAbstractValue varId=(*i).first;
    setVariableToValue(varId,val);
  }
}

void PState::setVariableToTop(VarAbstractValue varId) {
  CodeThorn::AValue val=CodeThorn::Top();
  setVariableToValue(varId, val);
}

void PState::setVariableToValue(VarAbstractValue varId, CodeThorn::AValue val) {
  operator[](varId)=val;
}

VarAbstractValueSet PState::getVariableIds() const {
  VarAbstractValueSet varIdSet;
  for(PState::const_iterator i=begin();i!=end();++i) {
    VarAbstractValue varId=(*i).first;
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

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
istream& CodeThorn::operator>>(istream& is, PState& pState) {
  pState.fromStream(is);
  return is;
}

#ifdef USER_DEFINED_PSTATE_COMP
bool CodeThorn::operator<(const PState& s1, const PState& s2) {
  if(s1.size()!=s2.size())
    return s1.size()<s2.size();
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
#if 0
bool CodeThorn::operator==(const PState& c1, const PState& c2) {
  if(c1.size()==c2.size()) {
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
#endif
#endif
