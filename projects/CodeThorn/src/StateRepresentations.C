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
    AValue val=(*i).second.getValue();
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
    AValue val=(*i).second.getValue();
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
  AValue val=((*(const_cast<PState*>(this)))[varId]).getValue();
  return val;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void PState::setAllVariablesToTop() {
  setAllVariablesToValue(CodeThorn::CppCapsuleAValue(AType::Top()));
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void PState::setAllVariablesToValue(CodeThorn::CppCapsuleAValue val) {
  for(PState::iterator i=begin();i!=end();++i) {
    VariableId varId=(*i).first;
    //operator[](varId)=val;
    setVariableToValue(varId,val);
  }
}
void PState::setVariableValueMonitor(VariableValueMonitor* vvm) {
  _variableValueMonitor=vvm;
}
void PState::setVariableToTop(VariableId varId) {
  setVariableToValue(varId, CodeThorn::CppCapsuleAValue(AType::Top()));
}

void PState::setVariableToValue(VariableId varId, CodeThorn::CppCapsuleAValue val) {
  if(_activeGlobalTopify && varIsTop(varId)) {
    return;
  }
  if(false && _activeGlobalTopify && _variableValueMonitor->isHotVariable(_analyzer,varId)) {
    setVariableToTop(varId);
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

void TransitionGraph::setIsPrecise(bool v) {
  _preciseSTG=v;
}

void TransitionGraph::setIsComplete(bool v) {
  _completeSTG=v;
}

bool TransitionGraph::isPrecise() {
  return _preciseSTG;
}

bool TransitionGraph::isComplete() {
  return _completeSTG;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
const EState* TransitionGraph::getStartEState() {
  for(TransitionGraph::iterator i=begin();i!=end();++i) {
    if((*i)->source->label()==getStartLabel()) {
      return (*i)->source;
    }
  }
  return 0;
}

Transition TransitionGraph::getStartTransition() {
  // we ensure that all start transitions share the same start label
  TransitionGraph::iterator foundElementIter=end();
  for(TransitionGraph::iterator i=begin();i!=end();++i) {
    if((*i)->source->label()==getStartLabel()) {
      if(foundElementIter!=end()) {
        cerr<< "Error: TransitionGraph: non-unique start transition."<<endl;
        exit(1);
      }
      foundElementIter=i;
    }
  }
  if(foundElementIter!=end())
    return **foundElementIter;
  else {
    throw "TransitionGraph: no start transition found.";
  }
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
      AType::CppCapsuleConstIntLattice varVal=(pstate2)[varId];
      return varVal.getValue(); // extracts ConstIntLattice from CppCapsuleConstIntLattice
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
string Transition::toString() const {
  string s1=source->toString();
  string s2=edge.toString();
  string s3=target->toString();
  return string("(")+s1+", "+s2+", "+s3+")";
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
LabelSet TransitionGraph::labelSetOfIoOperations(InputOutput::OpType op) {
  LabelSet lset;
  // the target node records the effect of the edge-operation on the source node.
  for(TransitionGraph::iterator i=begin();i!=end();++i) {
    if((*i)->target->io.op==op) {
      lset.insert((*i)->source->label());
    }
  }
  return lset;
} 

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::reduceEStates(set<const EState*> toReduce) {
  for(set<const EState*>::const_iterator i=toReduce.begin();i!=toReduce.end();++i) { 
    reduceEState(*i);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::reduceEStates2(set<const EState*> toReduce) {
  size_t todo=toReduce.size();
  if(boolOptions["post-semantic-fold"])
    cout << "STATUS: remaining states to fold: "<<todo<<endl;
  for(set<const EState*>::const_iterator i=toReduce.begin();i!=toReduce.end();++i) { 
    reduceEState2(*i);
    todo--;
    if(todo%10000==0 && boolOptions["post-semantic-fold"]) {
      cout << "STATUS: remaining states to fold: "<<todo<<endl;
    }
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */

TransitionGraph::TransitionPtrSet TransitionGraph::inEdges(const EState* estate) {
  assert(estate);
#if 1
  return _inEdges[estate];
#else
  TransitionGraph::TransitionPtrSet in;
  for(TransitionGraph::const_iterator i=begin();i!=end();++i) {
    if(estate==(*i).target)
      in.insert(&(*i)); // TODO: CHECK
  }
  TransitionGraph::TransitionPtrSet in2;
  if(!(in==_inEdges[estate])) {
    cerr<<"DEBUG: inEdges mismatch."<<endl;
    cerr << "set1:";
    for(TransitionGraph::TransitionPtrSet::iterator i=in.begin();i!=in.end();++i) {
      cerr << " "<<(*i)->toString()<<endl;
    }
    cerr <<endl;
    in2=_inEdges[estate];
    cerr << "set2:";
    for(TransitionGraph::TransitionPtrSet::iterator i=in2.begin();i!=in2.end();++i) {
      cerr << " "<<(*i)->toString()<<endl;
    }
    cerr <<endl;
    cerr<<"------------------------------------------------"<<endl;
    exit(1);
  }
  return in;
#endif
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
// MS: we definitely need to cache all the results or use a proper graph structure
TransitionGraph::TransitionPtrSet TransitionGraph::outEdges(const EState* estate) {
  assert(estate);
#if 1
  return _outEdges[estate];
#else
  TransitionGraph::TransitionPtrSet out;
  for(TransitionGraph::const_iterator i=begin();i!=end();++i) {
    if(estate==(*i).source)
      out.insert(&(*i));
  }
  assert(out==_outEdges[estate]);
  return out;
#endif
}

EStatePtrSet TransitionGraph::pred(const EState* estate) {
  EStatePtrSet predNodes;
  TransitionPtrSet tset=inEdges(estate);
  for(TransitionPtrSet::iterator i=tset.begin();i!=tset.end();++i) {
    predNodes.insert((*i)->source);
  }
  return predNodes;
}

EStatePtrSet TransitionGraph::succ(const EState* estate) {
  EStatePtrSet succNodes;
  TransitionPtrSet tset=outEdges(estate);
  for(TransitionPtrSet::iterator i=tset.begin();i!=tset.end();++i) {
    succNodes.insert((*i)->target);
  }
  return succNodes;
}



/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::reduceEState(const EState* estate) {
  assert(0);
  /* description of essential operations:
   *   inedges: (n_i,b)
   *   outedges: (b,n_j) 
   *   insert(ni,t,nj) where t=union(t(n_i))+union(t(n_j))+{EDGE_PATH}
   *   remove(n_i,b)
   *   remove(b,n_j)
   *   delete b
   
   * ea: (n1,cfge,n2) == ((l1,p1,c1,io1),(l1,t12,l2),(l2,p2,c2,io2))
   * eb: (n2,cfge,n3) == ((l2,p2,c2,io2),(l2,t23,l3),(l3,p3,c3,io3))
   * ==> (n1,cfge',n3) == ((l1,p1,c1,io1),(l1,{t12,t13,EDGE_PATH}},l3),(l3,p3,c3,io3))
   
   */
  TransitionGraph::TransitionPtrSet in=inEdges(estate);
  TransitionGraph::TransitionPtrSet out=outEdges(estate);
  if(in.size()!=0) {
    //cout<< "INFO: would be eliminating: node: "<<estate<<", #in="<<in.size()<<", #out="<<out.size()<<endl;
  } else {
    cout<< "INFO: not eliminating node because #in==0: node: "<<estate<<", #in="<<in.size()<<", #out="<<out.size()<<endl;
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator==(const Transition& t1, const Transition& t2) {
  return t1.source==t2.source && t1.edge==t2.edge && t1.target==t2.target;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator!=(const Transition& t1, const Transition& t2) {
  return !(t1==t2);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator<(const Transition& t1, const Transition& t2) {
  if(t1.source!=t2.source)
    return t1.source<t2.source;
  if(t1.edge!=t2.edge)
    return t1.edge<t2.edge;
  return t1.target<t2.target;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::add(Transition trans) {
  const Transition* transp=processNewOrExisting(trans);
  assert(transp!=0);
#pragma omp critical(TRANSGRAPH)
  {
    _outEdges[trans.source].insert(transp);
    _inEdges[trans.target].insert(transp);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::erase(TransitionGraph::iterator transiter) {
  const Transition* transp=determine(**transiter);
  assert(transp!=0);
  _outEdges[(*transiter)->source].erase(transp);
  _inEdges[(*transiter)->target].erase(transp);
  HSetMaintainer<Transition,TransitionHashFun,TransitionEqualToPred>::erase(transiter);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::erase(const Transition trans) {
  const Transition* transp=determine(trans);
  assert(transp!=0);
  _outEdges[trans.source].erase(transp);
  _inEdges[trans.target].erase(transp);
  size_t num=HSetMaintainer<Transition,TransitionHashFun,TransitionEqualToPred>::erase(const_cast<Transition*>(transp));
  assert(num==1);
}

void TransitionGraph::eliminateEState(const EState* estate) {
  TransitionGraph::TransitionPtrSet in=inEdges(estate);
  for(TransitionPtrSet::iterator i=in.begin();i!=in.end();++i)
    erase(**i);
  TransitionGraph::TransitionPtrSet out=outEdges(estate);
  for(TransitionPtrSet::iterator i=out.begin();i!=out.end();++i)
    erase(**i);
  // we only represent edges, nothing else to do
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
#if 0
long TransitionGraph::removeDuplicates() {
  long cnt=0;
  set<Transition> s;
  for(TransitionGraph::iterator i=begin();i!=end();) {
    const Transition* ii=const_cast<Transition*>(*i);
    if(s.find(ii)==s.end()) { 
      s.insert(ii); 
      ++i;
    } else {
      erase(ii);
      ++i;
      cnt++;
    }
  }
  return cnt;
}
#endif

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string TransitionGraph::toString() const {
  string s;
  size_t cnt=0;
  for(TransitionGraph::const_iterator i=begin();i!=end();++i) {
#if 0
    stringstream ss;
    ss<<cnt;
    s+="Transition["+ss.str()+"]=";
#endif
    s+=(*i)->toString()+"\n";
    cnt++;
  }
  assert(cnt==size());
  return s;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
set<const EState*> TransitionGraph::transitionSourceEStateSetOfLabel(Label lab) {
  set<const EState*> estateSet;
  for(TransitionGraph::iterator j=begin();j!=end();++j) {
    if((*j)->source->label()==lab)
      estateSet.insert((*j)->source);
  }
  return estateSet;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
set<const EState*> TransitionGraph::estateSetOfLabel(Label lab) {
  set<const EState*> estateSet;
  for(TransitionGraph::iterator j=begin();j!=end();++j) {
    if((*j)->source->label()==lab)
      estateSet.insert((*j)->source);
    if((*j)->target->label()==lab)
      estateSet.insert((*j)->target);
  }
  return estateSet;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool TransitionGraph::checkConsistency() {
  bool ok=true;
  size_t cnt=0;
  TransitionGraph* tg=this;
  for(TransitionGraph::const_iterator i=tg->begin();i!=tg->end();++i) {
    cnt++;
  }
  if(cnt!=tg->size()) {
    cerr<< "Error: TransitionGraph: size()==" <<tg->size()<< ", count=="<<cnt<<endl;
    ok=false;
  }
  assert(cnt==tg->size());
  //cout << "checkTransitionGraph:"<<ok<<" size:"<<size()<<endl;
  return ok;
}

const Transition* TransitionGraph::hasSelfEdge(const EState* estate) {
  TransitionPtrSet in=inEdges(estate);
  for(TransitionPtrSet::iterator i=in.begin();i!=in.end();++i) {
    if((*i)->source==estate)
      return *i;
  }
  // checking in edges only is sufficient because a self edge must be both
  return 0;
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::reduceEState2(const EState* estate) {
  /* description of essential operations:
   *   inedges: (n_i,b)
   *   outedges: (b,n_j) 
   *   insert(ni,t,nj) where t=union(t(n_i))+union(t(n_j))+{EDGE_PATH}
   *   remove(n_i,b)
   *   remove(b,n_j)
   *   delete b
   
   * ea: (n1,cfge,n2) == ((l1,p1,c1,io1),(l1,t12,l2),(l2,p2,c2,io2))
   * eb: (n2,cfge,n3) == ((l2,p2,c2,io2),(l2,t23,l3),(l3,p3,c3,io3))
   * ==> (n1,cfge',n3) == ((l1,p1,c1,io1),(l1,{t12,t13,EDGE_PATH}},l3),(l3,p3,c3,io3))
   
   */
  assert(estate);
  // check self-edge
  if(const Transition* trans=hasSelfEdge(estate)) {
    this->erase(*trans);
  }
  TransitionGraph::TransitionPtrSet in=inEdges(estate);
  TransitionGraph::TransitionPtrSet out=outEdges(estate);
  if(in.size()!=0 && out.size()!=0 ) {
    set<Transition> newTransitions;
    for(TransitionPtrSet::iterator i=in.begin();i!=in.end();++i) {
      for(TransitionPtrSet::iterator j=out.begin();j!=out.end();++j) {
        if((*i)->source!=estate && (*j)->target!=estate) {
          Edge newEdge((*i)->source->label(),EDGE_PATH,(*j)->target->label());
          Transition t((*i)->source,newEdge,(*j)->target);
          newTransitions.insert(t);
        //assert(newTransitions.find(t)!=newTransitions.end());
        }
      }
    }
    //cout << "DEBUG: number of new transitions: "<<newTransitions.size()<<endl;

    TransitionPtrSet all=in;
    for(TransitionPtrSet::iterator j=out.begin();j!=out.end();++j) {
      all.insert(*j);
    }
    // 1. remove all old transitions
    for(TransitionPtrSet::iterator i=all.begin();i!=all.end();++i) {
        this->erase(**i);
    }

    // 2. add new transitions
    for(set<Transition>::iterator k=newTransitions.begin();k!=newTransitions.end();++k) {
      //check if a single transition to the target already exists, delete this shorter path
      TransitionPtrSet outEdgesTransSource = outEdges((*k).source);
      for (TransitionPtrSet::iterator iter = outEdgesTransSource.begin(); iter != outEdgesTransSource.end(); ++iter) {
        if ((*iter)->target == (*k).target) {
          erase(**iter);
        }
      }
      this->add(*k);
      //assert(find(*k)!=end());
    }
    eliminateEState(estate);
    assert(newTransitions.size()<=in.size()*out.size());
  } else {
    // need to eliminate node instead
    eliminateEState(estate);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
// later, we may want to maintain this set with every graph-operation (turning the linear access to constant)
set<const EState*> TransitionGraph::estateSet() {
  _recomputedestateSet.clear();
  for(TransitionGraph::iterator j=begin();j!=end();++j) {
      _recomputedestateSet.insert((*j)->source);
      _recomputedestateSet.insert((*j)->target);
  }
  return _recomputedestateSet;
}

// author: Marc Jasper, 2015. 
long TransitionGraph::numberOfObservableStates(bool includeIn, bool includeOut, bool includeErr) {
  long result = 0;
  EStatePtrSet allStates = estateSet();
  for (EStatePtrSet::iterator i=allStates.begin(); i!=allStates.end(); ++i) {
    if ((includeIn && (*i)->io.isStdInIO()) || (includeOut && (*i)->io.isStdOutIO())
            || (includeErr && ((*i)->io.isStdErrIO()||(*i)->io.isFailedAssertIO())) ) {
      result++;
    }
  }
  return result;
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
  if(option_debug_mode) cout<<"DEBUG: PState:"<<ps<<" : "<<ps->toString(vim)<<endl;
  for(PState::const_iterator i=ps->begin();i!=ps->end();++i) {
    VariableId varId=(*i).first;
    if(option_debug_mode) cout<<"varId:"<<varId.toString()<<"/"<<vim->variableName(varId)<<":";
    // the following two variables are special variables that are not considered to contribute to const-ness in an EState
    if(vim->variableName(varId)=="__PRETTY_FUNCTION__"||vim->variableName(varId)=="stderr") {
      if(option_debug_mode) cout<<"filt-const ";
      continue;
    }

    if(ps->varIsConst(varId)) {
      if(option_debug_mode) cout<<"const ";
      continue;
    } else {
      if(option_debug_mode) cout<<"non-const ";
      // variable non-const in PState (i.e. top/bot) -> need to investigate constraints
      if(!cs->varConstIntLatticeValue(varId).isConstInt()) {
        if(option_debug_mode) cout<<" cs:non-const; \n";
        return false;
      } else {
        if(option_debug_mode) cout<<" cs:const; ";
      }
    }
  }
  if(option_debug_mode) cout<<endl;
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

int TransitionGraph::eliminateBackEdges() {
  const EState* startState=getStartEState();
  set<const EState*> visited;
  visited.insert(startState);
  TransitionPtrSet backEdges; // default empty
  determineBackEdges(startState, visited, backEdges);
  for(TransitionPtrSet::iterator i=backEdges.begin();i!=backEdges.end();++i) {
    
  }
  return backEdges.size();
}
void TransitionGraph::determineBackEdges(const EState* state, set<const EState*>& visited, TransitionPtrSet& tpSet) {
  TransitionPtrSet succPtrs=outEdges(state);
  for(TransitionPtrSet::iterator i=succPtrs.begin();i!=succPtrs.end();++i) {
    if(visited.find((*i)->target)!=visited.end()) {
      // target node exists in visited-set
      tpSet.insert(*i);
      return;
    }
    visited.insert((*i)->target);
    determineBackEdges((*i)->target,visited,tpSet);
  }
  return;
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
