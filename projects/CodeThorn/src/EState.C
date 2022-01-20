/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"
#include "EState.h"

#include <algorithm>
#include "EState.h"
#include "EStateTransferFunctions.h"
#include "AbstractValue.h"
#include "CollectionOperators.h"
#include "CommandLineOptions.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "CodeThornException.h"

using namespace std;
using namespace CodeThorn;

bool EState::sharedPStates=true;
bool EState::fastPointerHashing=true;

EState::EState():_label(Label()) {
  if(EState::sharedPStates) {
    _pstate=0;
  } else {
    _pstate=new PState();
  }
}

EState::~EState() {
  if(EState::sharedPStates==false) {
    //cerr<<"DEBUG: Deleting estate: "<<this<<" with pstate: "<<_pstate<<endl;
    if(_pstate!=nullptr) {
      delete _pstate;
      _pstate=nullptr;
    }
  }
}

// copy
void EState::EState::copy(EState* target, ConstEStatePtr source,bool sharedPStatesFlag) {
  target->_label=source->_label;
  target->io=source->io;
  target->callString=source->callString;
  if(sharedPStatesFlag) {
    // copy pstate pointer, objects are managed and shared. Identical pointers guarantee equivalence
    target->_pstate=source->_pstate;
  } else {
    // copy entire pstate
    //_pstate=new PState(*source->_pstate);
    if(source->pstate()==nullptr) {
      target->_pstate=nullptr;
      //cout<<"DEBUG: ESTATE COPY: "<<&source-><<"=>"<<target<<": pstate: nullptr"<<" ==> nullptr"<<endl;
    } else {
      PStatePtr newPState=new PState();
      for(auto iter=source->_pstate->begin(); iter!=source->_pstate->end();++iter) {
	auto address=(*iter).first;
	auto value=(*iter).second;
	newPState->writeToMemoryLocation(address,value);
      }
      //cout<<"DEBUG: ESTATE COPY: "<<&source-><<"=>"<<target<<": pstate:"<<source->pstate()<<" ==> "<<newPState<<endl;
      target->_pstate=newPState;
    }
  }
}

EState* EState::deepClone() {
  EState* newEState=new EState();
  copy(newEState,this,false);
  return newEState;
}

// equivalent to deepClone, if sharedPStates==false
EStatePtr EState::cloneWithoutIO() {
  EStatePtr estate=clone();
  if(!estate->io.isBot())
    estate->io.recordNone(); // remove any existing IO info if not Bottom
  return estate;
}

// equivalent to deepClone, if sharedPStates==false
EStatePtr EState::clone() {
  return new EState(*this);
}

#ifdef ESTATE_PSTATE_MEM_COPY
// copy constructor
EState::EState(const EState &other) {
  copy(this,&other,sharedPStates);
}
#endif

#ifdef ESTATE_PSTATE_MEM_COPY
// assignment operator
EState& EState::operator=(const EState &other) {
  copy(this,&other,sharedPStates);
  return *this;
}
#endif

string EState::predicateToString(VariableIdMapping* variableIdMapping) const {
  string separator=",";
  string pred;
  PStatePtr ps=pstate();
  AbstractValueSet varIdSet=ps->getVariableIds();
  string s;
  bool firstPred=true;
  for(AbstractValueSet::iterator i=varIdSet.begin();i!=varIdSet.end();++i) {
    AbstractValue varId=*i;
    string variableName=varId.toLhsString(variableIdMapping);
    // ignore this variable
    if(variableName=="__PRETTY_FUNCTION__")
      continue;
    //cout<<"V:"<<variableName<<":"<<endl;
    if(ps->varIsConst(varId)) {
      if(!firstPred)
        s+=separator;
      s+=variableName+"=="+ps->varValueToString(varId);
      firstPred=false;
    }
  }
  return s;
}

/*! 
  * \author Markus Schordan
  * \date 2017.
 */
std::string EState::programPosToString(Labeler* labeler) const {
  Label lab=this->label();
  SgNode* node=labeler->getNode(lab);
  return node->unparseToString();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
// define order for EState elements (necessary for EStateSet)
// only used in SpotState
bool CodeThorn::operator<(const EState& e1, const EState& e2) {
  if(EState::sharedPStates) {
    if(e1.label()!=e2.label())
      return (e1.label()<e2.label());
    if(e1.pstate()!=e2.pstate())
      return (e1.pstate()<e2.pstate());
    if(e1.io!=e2.io) {
      return e1.io<e2.io;
    }
    return e1.callString<e2.callString;
  } else {
    if(e1.label()!=e2.label())
      return (e1.label()<e2.label());
    if(*e1.pstate()!=*e2.pstate())
      return (e1.pstate()<e2.pstate());
    if(e1.io!=e2.io) {
      return e1.io<e2.io;
    }
    return e1.callString<e2.callString;
  }

}

bool CodeThorn::operator==(const EState& c1, const EState& c2) {
  if(EState::sharedPStates) {
    return (c1.label()==c2.label())
      && (c1.pstate()==c2.pstate())
      && (c1.io==c2.io)
      && (c1.callString==c2.callString)
    ;
  } else {
    return (c1.label()==c2.label())
      && (*c1.pstate()==*c2.pstate())
      && (c1.io==c2.io)
      && (c1.callString==c2.callString)
    ;
  }
}

bool CodeThorn::operator!=(const EState& c1, const EState& c2) {
  return !(c1==c2);
}

EStateId EStateSet::estateId(EStatePtr estate) const {
  return estateId(*estate);
}

long EState::memorySize() const {
  return sizeof(*this);
}

EStateId EStateSet::estateId(const EState estate) const {
  EStateId id=0;
  // MS: TODO: we may want to use the new function id(estate) here
  for(EStateSet::const_iterator i=begin();i!=end();++i) {
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
string EStateSet::estateIdString(EStatePtr estate) const {
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

CodeThorn::AbstractValue EState::determineUniqueIOValue() const {
  // this returns 1 (TODO: investigate)
  CodeThorn::AbstractValue value;
  if(io.op==InputOutput::STDIN_VAR||io.op==InputOutput::STDOUT_VAR||io.op==InputOutput::STDERR_VAR) {
    AbstractValue varId=io.var;
    ROSE_ASSERT(_pstate);
    ROSE_ASSERT(_pstate->varExists(varId));
    // case 1: check PState
    if(_pstate->varIsConst(varId)) {
      PState pstate2=*_pstate;
      AbstractValue varVal=pstate2.readFromMemoryLocation(varId);
      return varVal;
    }
    cerr<<"Error: determineUniqueIOValue:: could not determine constant value."<<endl;
      exit(1);
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
  for(EStateSet::const_iterator i=begin();i!=end();++i) {
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
  for(EStateSet::const_iterator i=begin();i!=end();++i) {
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
  ss << "("
     <<label()<<", "
     <<callString.toString()<<", "
    ;
  if(pstate())
    ss <<pstate()->toString();
  else
    ss <<"NULL";
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
  ss <<callString.toString()<<", ";
  if(pstate())
    ss <<pstate()->toString(vim);
  else
    ss <<"NULL";
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
  ss <<","<<nl<<" io="<<io.toString();
  ss<<")"<<nl;
  return ss.str();
}

bool EState::isConst(VariableIdMapping* vim) const {
  PStatePtr ps=pstate();
  ROSE_ASSERT(ps);
  //ROSE_ASSERT(cs);
  for(PState::const_iterator i=ps->begin();i!=ps->end();++i) {
    AbstractValue varId=(*i).first;
    // the following two variables are special variables that are not considered to contribute to const-ness in an EState
    if(varId.toString(vim)=="__PRETTY_FUNCTION__"||varId.toString(vim)=="stderr") {
      continue;
    } else if(!ps->varIsConst(varId)) {
      return false;
    }
  }
  return true;
}

// TODO: remove this function
bool EState::isRersTopified(VariableIdMapping* vim) const {
  boost::regex re("a(.)*");
  PStatePtr pstate = this->pstate();
  AbstractValueSet varSet=pstate->getVariableIds();
  for (AbstractValueSet::iterator l=varSet.begin();l!=varSet.end();++l) {
    string varName=(*l).toLhsString(vim);
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
  for(EStateSet::const_iterator i=begin();
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

std::string EState::labelString() const {
  return "L"+label().toString();
}

bool EState::isApproximatedBy(EStatePtr other) const {
  ROSE_ASSERT(label()==other->label()); // ensure same location
  if(callString!=other->callString) {
    return false;
  }
  // it only remains to check the pstate
  return pstate()->isApproximatedBy(*const_cast<PStatePtr>(other->pstate())) && (io.isBot()||(io==other->io));
}

// required for PropertyState
bool EState::approximatedBy(PropertyState& other) const {
  // This function is always read only
  // EStates are used in hash sets and therefore only const pointers exist
  return isApproximatedBy(const_cast<EStatePtr>(&dynamic_cast<EState&>(other)));
}
// required for PropertyState
bool EState::isBot() const {
  // io field is used to indicate bottom element
  return io.isBot();
}
// required for PropertyState
void EState::combine(PropertyState& other0) {
  // see Amalyzer::combine for original implementation
  EState& other=dynamic_cast<EState&>(other0);

  // special cases if one of the two arguments is bot. An EState bot
  // element is not a valid state and only remains if the associated
  // code cannot be executed (i.e. is dead code)
  if(other.isBot()) {
    return;
  } else if(this->isBot()) {
    *this=other;
  }

  ROSE_ASSERT(label()==other.label());
  if(callString!=other.callString) {
    cerr<<"combining estates with different callstrings at label:"<<this->label().toString()<<endl;
    cerr<<"cs1: "<<this->callString.toString()<<endl;
    cerr<<"cs2: "<<other.callString.toString()<<endl;
  }

  // updates of four entries: label,callstring,pstate,io:

  // (1) this->label remains unchanged

  // (2) this->callString remains unchanged

  // (3) updated pstate entry
  PState ps1=*this->pstate();
  PState ps2=*other.pstate();
  PState newPState=PState::combine(ps1,ps2);
  // allowing in-place update for framework not maintaining a state set, not compatible with use in sorted containers
  *(this->pstate())=newPState; 

  // (4) update IO entry
  InputOutput newIO;
  if(this->io.isBot()) {
    this->io=other.io;
  } else if(other.io.isBot()) {
    // this->io remains unchanged
  } else {
    ROSE_ASSERT(this->io==other.io);
    // this->io remains unchanged
  }
  
}

void EState::setCallString(CallString cs) {
  callString=cs;
}

CallString EState::getCallString() const {
  return callString;
}

size_t EState::getCallStringLength() const {
  return callString.getLength();
}

void EState::setPState(PStatePtr pstate) {
  if(!EState::sharedPStates) {
    if(_pstate!=nullptr && _pstate!=pstate)
      delete _pstate;
    _pstate=pstate;
  } else {
    _pstate=pstate;
  }
}
