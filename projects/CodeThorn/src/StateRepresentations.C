/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"

#include <algorithm>
#include "StateRepresentations.h"
#include "ExprAnalyzer.h"
#include "AType.h"
#include "CollectionOperators.h"
#include "CommandLineOptions.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"

#include "boost/regex.hpp"

// only necessary for class VariableValueMonitor
#include "Analyzer.h"

// it is not necessary to define comparison-ops for Pstate, but
// the ordering appears to be implementation dependent (but consistent)

using namespace std;
using namespace CodeThorn;

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void InputOutput::recordVariable(OpType op0,VariableId varId) {
  switch(op0) {
  case STDIN_VAR:
  case STDOUT_VAR:
  case STDERR_VAR:
    break;
  default: cerr<<"Error: wrong IO operation type."<<endl;
    ROSE_ASSERT(0);
  }
  op=op0;
  var=varId;
  val=CodeThorn::AType::Bot();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void InputOutput::recordFailedAssert() {
  op=FAILED_ASSERT;
}

/*! 
  * \author Markus Schordan
  * \date 2015.
 */
void InputOutput::recordVerificationError() {
  op=VERIFICATION_ERROR;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void InputOutput::recordConst(OpType op0,AType::ConstIntLattice constvalue) {
  ROSE_ASSERT(op0==STDOUT_CONST || op0==STDERR_CONST);
  op=op0;
  var=VariableId();
  val=constvalue;
}
void InputOutput::recordConst(OpType op0,int value) {
  ROSE_ASSERT(op0==STDOUT_CONST || op0==STDERR_CONST);
  AType::ConstIntLattice abstractConstValue(value);
  recordConst(op0,abstractConstValue);
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string InputOutput::toString() const {
  string str;
  switch(op) {
  case NONE: str="none";break;
  case STDIN_VAR: str="stdin:"+var.toString();break;
  case STDOUT_VAR: str="stdout:"+var.toString();break;
  case STDERR_VAR: str="stderr:"+var.toString();break;
  case STDOUT_CONST: str="stdoutconst:"+val.toString();break;
  case STDERR_CONST: str="stderrconst:"+val.toString();break;
  case FAILED_ASSERT: str="failedassert";break;
  case VERIFICATION_ERROR: str="verificationerror";break;
  default:
    cerr<<"FATAL ERROR: unknown IO operation abstraction.";
    exit(1);
  }
  return str;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string InputOutput::toString(VariableIdMapping* variableIdMapping) const {
  string str;
  string varName=variableIdMapping->uniqueLongVariableName(var);
  switch(op) {
  case NONE: str="none";break;
  case STDIN_VAR: str="stdin:"+varName;break;
  case STDOUT_VAR: str="stdout:"+varName;break;
  case STDERR_VAR: str="stderr:"+varName;break;
  case STDOUT_CONST: str="stdoutconst:"+val.toString();break;
  case STDERR_CONST: str="stderrconst:"+val.toString();break;
  case FAILED_ASSERT: str="failedassert";break;
  case VERIFICATION_ERROR: str="verificationerror";break;
  default:
    cerr<<"FATAL ERROR: unknown IO operation abstraction.";
    exit(1);
  }
  return str;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator<(const InputOutput& c1, const InputOutput& c2) {
  if(c1.op!=c2.op)
    return c1.op<c2.op;
  if(!(c1.var==c2.var))
    return c1.var<c2.var;
  return AType::strictWeakOrderingIsSmaller(c1.val,c2.val);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator==(const InputOutput& c1, const InputOutput& c2) {
  return c1.op==c2.op && c1.var==c2.var && (AType::strictWeakOrderingIsEqual(c1.val,c2.val));
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator!=(const InputOutput& c1, const InputOutput& c2) {
  return !(c1==c2);
}

bool PState::_activeGlobalTopify=false;
VariableValueMonitor* PState::_variableValueMonitor=0;
Analyzer* PState::_analyzer=0;

void PState::setActiveGlobalTopify(bool val) {
  _activeGlobalTopify=val;
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
  VariableId __varId; 
  AValue __varAValue; 
  if(!SPRAY::Parse::checkWord("{",is)) throw "Error: Syntax error PState. Expected '{'.";
  is>>c;
  // read pairs (varname,varvalue)
  while(c!='}') {
    if(c!='(') throw "Error: Syntax error PState. Expected '('.";
    is>>c;
    if(c!='V') throw "Error: Syntax error PState. Expected VariableId.";
    is>>__varIdCode;
    assert(__varIdCode>=0);
    VariableId __varId;
    __varId.setIdCode(__varIdCode);
    is>>c;
    if(c!=',') { cout << "Error: found "<<c<<"__varIdCode="<<__varIdCode<<endl; throw "Error: Syntax error PState. Expected ','.";}
    is>>__varAValue;
    is>>c;    
    if(c!=')' && c!=',') throw "Error: Syntax error PState. Expected ')' or ','.";
    is>>c;
    //cout << "DEBUG: Read from istream: ("<<__varId.toString()<<","<<__varAValue.toString()<<")"<<endl;
    (*this)[__varId]=__varAValue;
    if(c==',') is>>c;
  }
  if(c!='}') throw "Error: Syntax error PState. Expected '}'.";
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
    ss <<variableIdMapping->uniqueLongVariableName((*j).first);
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

string EState::predicateToString(VariableIdMapping* variableIdMapping) const {
  string separator=",";
  string pred;
  const PState* ps=pstate();
  const ConstraintSet* cset=constraints(); 
  PState::const_iterator i=ps->begin();
  VariableIdSet varIdSet=ps->getVariableIds();
  string s;
  if(cset->disequalityExists()) {
    return "false";
  }
  bool firstPred=true;
  for(VariableIdSet::iterator i=varIdSet.begin();i!=varIdSet.end();++i) {
    VariableId varId=*i;
    string variableName=variableIdMapping->variableName(varId);
    // ignore this variable
    if(variableName=="__PRETTY_FUNCTION__")
      continue;
    //cout<<"V:"<<variableName<<":"<<endl;
    if(ps->varIsConst(varId)) {
      if(!firstPred)
        s+=separator;
      s+=variableName+"=="+ps->varValueToString(varId);
      firstPred=false;
    } else {
      ConstraintSet vcset=cset->constraintsOfVariable(varId);
      stringstream ss;
      if(vcset.size()>=0) {
        if(!firstPred)
          s+=separator;
        if(vcset.size()==0)
          s+="true"; // TODO: make this optional to not have explicit true
        else
          s+=vcset.toStringWithoutBraces(variableIdMapping);
        firstPred=false;
      }
    }
  }
  return s;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void PState::deleteVar(VariableId varId) {
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
bool PState::varExists(VariableId varId) const {
  PState::const_iterator i=find(varId);
  return !(i==end());
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool PState::varIsConst(VariableId varId) const {
  PState::const_iterator i=find(varId);
  if(i!=end()) {
    AValue val=(*i).second;
    return val.isConstInt();
  } else {
    // TODO: this allows variables (intentionally) not to be in PState but still to analyze
    // however, this check will have to be reinstated once this mode is fully supported
    return false; // throw "Error: PState::varIsConst : variable does not exist.";
  }
}
bool PState::varIsTop(VariableId varId) const {
  PState::const_iterator i=find(varId);
  if(i!=end()) {
    AValue val=(*i).second;
    return val.isTop();
  } else {
    // TODO: this allows variables (intentionally) not to be in PState but still to analyze
    // however, this check will have to be reinstated once this mode is fully supported
    return false; // throw "Error: PState::varIsConst : variable does not exist.";
  }
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string PState::varValueToString(VariableId varId) const {
  stringstream ss;
  AValue val=varValue(varId);
  return val.toString();
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
AValue PState::varValue(VariableId varId) const {
  AValue val=((*(const_cast<PState*>(this)))[varId]);
  return val;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void PState::setAllVariablesToTop() {
  CodeThorn::AValue val=AType::Top();
  setAllVariablesToValue(val);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void PState::setAllVariablesToValue(CodeThorn::AValue val) {
  for(PState::iterator i=begin();i!=end();++i) {
    VariableId varId=(*i).first;
    setVariableToValue(varId,val);
  }
}
void PState::setVariableValueMonitor(VariableValueMonitor* vvm) {
  _variableValueMonitor=vvm;
}
void PState::setVariableToTop(VariableId varId) {
  CodeThorn::AValue val=AType::Top();
  setVariableToValue(varId, val);
}

void PState::setVariableToValue(VariableId varId, CodeThorn::AValue val) {
  if(false && _activeGlobalTopify) {
    ROSE_ASSERT(_variableValueMonitor);
    if(_variableValueMonitor->isHotVariable(_analyzer,varId)) {
      setVariableToTop(varId);
    }
  } else {
    operator[](varId)=val;
  }
}

void PState::topifyState() {
  for(PState::const_iterator i=begin();i!=end();++i) {
    VariableId varId=(*i).first;
    if(_activeGlobalTopify && _variableValueMonitor->isHotVariable(_analyzer,varId)) {
      setVariableToTop(varId);
    }
  }
}

bool PState::isTopifiedState() const {
  if(!_activeGlobalTopify) {
    return false;
  }
  ROSE_ASSERT(_variableValueMonitor);
  for(PState::const_iterator i=begin();i!=end();++i) {
    VariableId varId=(*i).first;
    if(_variableValueMonitor->isHotVariable(_analyzer,varId)) {
      if(varIsTop(varId)) {
        continue;
      } else {
        cout<<"DEBUG: var is not top (but hot): "<<varId.toString(*_analyzer->getVariableIdMapping())<<":"<<(*i).second.toString()<<endl;
        cout<<"DEBUG: PState:"<<toString()<<endl;
        return false;
      }
    }
  }
  return true;
}

VariableIdSet PState::getVariableIds() const {
  VariableIdSet varIdSet;
  for(PState::const_iterator i=begin();i!=end();++i) {
    VariableId varId=(*i).first;
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

/*! 
  * \author Marc Jasper
  * \date 2016.
 */
// define order for ParProEState elements (necessary for ParProEStateSet)
bool CodeThorn::operator<(const ParProEState& e1, const ParProEState& e2) {
  return e1.getLabel()<e2.getLabel();
}

bool CodeThorn::operator==(const ParProEState& e1, const ParProEState& e2) {
  return e1.getLabel()==e2.getLabel();
}

bool CodeThorn::operator!=(const ParProEState& e1, const ParProEState& e2) {
  return !(e1==e2);
}

string ParProEState::toString() const {
  ParProLabel label = getLabel();
  bool firstEntry = true;
  stringstream ss;
  ss << "Label:";
  for (ParProLabel::iterator i=label.begin(); i!=label.end(); i++) {
    if (!firstEntry) {
      ss << ",";
    }
    firstEntry = false;
    ss << *i;
  }
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
// define order for EState elements (necessary for EStateSet)
bool CodeThorn::operator<(const EState& e1, const EState& e2) {
  if(e1.label()!=e2.label())
    return (e1.label()<e2.label());
  if(e1.pstate()!=e2.pstate())
    return (e1.pstate()<e2.pstate());
  if(e1.constraints()!=e2.constraints())
    return (e1.constraints()<e2.constraints());
  return e1.io<e2.io;
}

bool CodeThorn::operator==(const EState& c1, const EState& c2) {
  return (c1.label()==c2.label())
    && (c1.pstate()==c2.pstate())
    && (c1.constraints()==c2.constraints())
    && (c1.io==c2.io);
}

bool CodeThorn::operator!=(const EState& c1, const EState& c2) {
  return !(c1==c2);
}

EStateId EStateSet::estateId(const EState* estate) const {
  return estateId(*estate);
}

EStateId EStateSet::estateId(const EState estate) const {
  EStateId id=0;
  // MS: TODO: we may want to use the new function id(estate) here
  for(EStateSet::iterator i=begin();i!=end();++i) {
    if(estate==**i)
      return id;
    id++;
  }
  return NO_ESTATE;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string EStateSet::estateIdString(const EState* estate) const {
  stringstream ss;
  ss<<estateId(estate);
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
CodeThorn::InputOutput::OpType EState::ioOp() const {
  return io.op;
}

ConstraintSet EState::allInfoAsConstraints() const {
  ConstraintSet cset=*constraints();
  /* we use the property that state is always consistant with constraintSet
     if a variable is state(var)=top then it may have a constraint
     if a variable is state(var)=const then it cannot have a constraint
     hence, we only need to add state(var)=const as var==const to the existing constraint set
  */
  const PState* pstate=this->pstate();
  for(PState::const_iterator j=pstate->begin();j!=pstate->end();++j) {
    VariableId varId=(*j).first;
    AValue val=pstate->varValue(varId);
    if(!val.isTop()&&!val.isBot()) {
      cset.insert(Constraint(Constraint::EQ_VAR_CONST,varId,val));
    }
  }
  return cset;
}

CodeThorn::AType::ConstIntLattice EState::determineUniqueIOValue() const {
  // this returns 1 (TODO: investigate)
  CodeThorn::AType::ConstIntLattice value;
  if(io.op==InputOutput::STDIN_VAR||io.op==InputOutput::STDOUT_VAR||io.op==InputOutput::STDERR_VAR) {
    VariableId varId=io.var;
    assert(_pstate->varExists(varId));
    // case 1: check PState
    if(_pstate->varIsConst(varId)) {
      PState pstate2=*_pstate;
      AType::ConstIntLattice varVal=(pstate2)[varId];
      return varVal;
    }
    // case 2: check constraint if var is top
    if(_pstate->varIsTop(varId))
      return constraints()->varConstIntLatticeValue(varId);
  }
  if(io.op==InputOutput::STDOUT_CONST||io.op==InputOutput::STDERR_CONST) {
    value=io.val;
  }
  return value;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
int EStateSet::numberOfIoTypeEStates(InputOutput::OpType op) const {
  int counter=0;
  for(EStateSet::iterator i=begin();i!=end();++i) {
    if((*i)->io.op==op)
      counter++;
  }
  return counter;
} 

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
int EStateSet::numberOfConstEStates(VariableIdMapping* vid) const {
  int counter=0;
  for(EStateSet::iterator i=begin();i!=end();++i) {
    if((*i)->isConst(vid))
      counter++;
  }
  return counter;
} 

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string EState::toString() const {
  stringstream ss;
  ss << "EState";
  ss << "("<<label()<<", ";
  if(pstate())
    ss <<pstate()->toString();
  else
    ss <<"NULL";
  if(constraints()) {
    ss <<", constraints="<<constraints()->toString();
  } else {
    ss <<", NULL";
  }
  ss <<", io="<<io.toString();
  ss<<")";
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string EState::toString(VariableIdMapping* vim) const {
  stringstream ss;
  ss << "EState";
  ss << "("<<label()<<", ";
  if(pstate())
    ss <<pstate()->toString(vim);
  else
    ss <<"NULL";
  if(constraints()) {
    ss <<", constraints="<<constraints()->toString(vim);
  } else {
    ss <<", NULL";
  }
  ss <<", io="<<io.toString(); // TODO
  ss<<")";
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string EState::toHTML() const {
  stringstream ss;
  string nl = " <BR />\n";
  ss << "EState";
  ss << "("<<label()<<", "<<nl;
  if(pstate())
    ss <<pstate()->toString();
  else
    ss <<"NULL";
  if(constraints()) {
    ss <<","<<nl<<" constraints="<<constraints()->toString();
  } else {
    ss <<","<<nl<<" NULL";
  }
  ss <<","<<nl<<" io="<<io.toString();
  ss<<")"<<nl;
  return ss.str();
}

bool EState::isConst(VariableIdMapping* vim) const {
  const PState* ps=pstate();
  const ConstraintSet* cs=constraints();
  ROSE_ASSERT(ps);
  ROSE_ASSERT(cs);
  for(PState::const_iterator i=ps->begin();i!=ps->end();++i) {
    VariableId varId=(*i).first;
    // the following two variables are special variables that are not considered to contribute to const-ness in an EState
    if(vim->variableName(varId)=="__PRETTY_FUNCTION__"||vim->variableName(varId)=="stderr") {
      continue;
    }

    if(ps->varIsConst(varId)) {
      continue;
    } else {
      // variable non-const in PState (i.e. top/bot) -> need to investigate constraints
      if(!cs->varConstIntLatticeValue(varId).isConstInt()) {
        return false;
      }
    }
  }
  return true;
}

bool EState::isRersTopified(VariableIdMapping* vim) const {
  boost::regex re("a(.)*");
  const PState* pstate = this->pstate();
  VariableIdSet varSet=pstate->getVariableIds();
  for (VariableIdSet::iterator l=varSet.begin();l!=varSet.end();++l) {
    string varName=SgNodeHelper::symbolToString(vim->getSymbol(*l));
    if (boost::regex_match(varName, re)) { //matches internal RERS variables (e.g. "int a188")
      if (pstate->varIsConst(*l)) {  // is a concrete (therefore prefix) state
        return false;
      } else {
        return true;
      }
    }
  }
  cout << "ERROR: Could not find a matching variable to check if";
  cout << "a \"topified\" state of the analyzed RERS challenge program exists. " << endl;
  assert(0);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string EStateList::toString() {
  stringstream ss;
  ss<<"EStateWorkList=[";
  for(EStateList::iterator i=begin();
      i!=end();
      ++i) {
    ss<<(*i).toString()<<",";
  }
  ss<<"]";
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string EStateSet::toString(VariableIdMapping* variableIdMapping) const {
  stringstream ss;
  ss<<"EStateSet={";
  for(EStateSet::iterator i=begin();
      i!=end();
      ++i) {
    if(variableIdMapping)
      ss<<(*i)->toString(variableIdMapping);
    else
      ss<<(*i)->toString();
    ss<<",\n";
  }
  ss<<"}";
  return ss.str();
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
