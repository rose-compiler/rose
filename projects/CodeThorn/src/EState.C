/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"
#include "EState.h"

#include <algorithm>
#include "EState.h"
#include "ExprAnalyzer.h"
#include "AbstractValue.h"
#include "CollectionOperators.h"
#include "CommandLineOptions.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "CodeThornException.h"

using namespace std;
using namespace CodeThorn;

string EState::predicateToString(VariableIdMapping* variableIdMapping) const {
  string separator=",";
  string pred;
  const PState* ps=pstate();
  const ConstraintSet* cset=constraints(); 
  AbstractValueSet varIdSet=ps->getVariableIds();
  string s;
  if(cset->disequalityExists()) {
    return "false";
  }
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
#define USE_CALLSTRINGS
#ifdef USE_CALLSTRINGS
bool CodeThorn::operator<(const EState& e1, const EState& e2) {
  if(e1.label()!=e2.label())
    return (e1.label()<e2.label());
  if(e1.pstate()!=e2.pstate())
    return (e1.pstate()<e2.pstate());
  if(e1.constraints()!=e2.constraints())
    return (e1.constraints()<e2.constraints());
  if(e1.io!=e2.io) {
    return e1.io<e2.io;
  }
  return e1.callString<e2.callString;
}
#else
bool CodeThorn::operator<(const EState& e1, const EState& e2) {
  if(e1.label()!=e2.label())
    return (e1.label()<e2.label());
  if(e1.pstate()!=e2.pstate())
    return (e1.pstate()<e2.pstate());
  if(e1.constraints()!=e2.constraints())
    return (e1.constraints()<e2.constraints());
  return e1.io<e2.io;
}
#endif

bool CodeThorn::operator==(const EState& c1, const EState& c2) {
  return (c1.label()==c2.label())
    && (c1.pstate()==c2.pstate())
    && (c1.constraints()==c2.constraints())
    && (c1.io==c2.io)
    //#ifdef USE_CALLSTRINGS
    && (c1.callString==c2.callString)
    //#endif
    ;
}

bool CodeThorn::operator!=(const EState& c1, const EState& c2) {
  return !(c1==c2);
}

EStateId EStateSet::estateId(const EState* estate) const {
  return estateId(*estate);
}

long EState::memorySize() const {
  return sizeof(*this);
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
    AbstractValue varId=(*j).first;
    AbstractValue val=pstate->varValue(varId);
    if(!val.isTop()&&!val.isBot()) {
      cset.insert(Constraint(Constraint::EQ_VAR_CONST,varId,val));
    }
  }
  return cset;
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
    // case 2: check constraint if var is top
    if(_pstate->varIsTop(varId)) {
      return constraints()->varAbstractValue(varId);
    } else {
      cerr<<"Error: could not determine constant value from constraints."<<endl;
      exit(1);
    }
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
  ss << "("
     <<label()<<", "
     <<callString.toString()<<", "
    ;
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
  ss <<callString.toString()<<", ";
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
  const PState* pstate = this->pstate();
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

std::string EState::labelString() const {
  return "L"+label().toString();
}

bool EState::isApproximatedBy(const EState* other) const {
  ROSE_ASSERT(label()==other->label()); // ensure same location
  ROSE_ASSERT(constraints()==other->constraints()); // pointer equality
  if(callString!=other->callString) {
    return false;
  }
  // it only remains to check the pstate
  return pstate()->isApproximatedBy(*const_cast<PState*>(other->pstate())) && (io.isBot()||(io==other->io));
}

// required for PropertyState
bool EState::approximatedBy(PropertyState& other) const {
  // This function is always read only
  // EStates are used in hash sets and therefore only const pointers exist
  return isApproximatedBy(const_cast<const EState*>(&dynamic_cast<EState&>(other)));
}
// required for PropertyState
bool EState::isBot() const {
  // dummy implementation - TODO
  return false;
}
// required for PropertyState
void EState::combine(PropertyState& other0) {
  // see Amalyzer::combine for original implementation
  EState& other=dynamic_cast<EState&>(other0);
  ROSE_ASSERT(label()==other.label());
  ROSE_ASSERT(constraints()==other.constraints()); // pointer equality
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
  *(const_cast<PState*>(this->pstate()))=newPState; 

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

