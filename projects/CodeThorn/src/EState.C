/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
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

CodeThorn::AbstractValue EState::determineUniqueIOValue() const {
  // this returns 1 (TODO: investigate)
  CodeThorn::AbstractValue value;
  if(io.op==InputOutput::STDIN_VAR||io.op==InputOutput::STDOUT_VAR||io.op==InputOutput::STDERR_VAR) {
    VariableId varId=io.var;
    assert(_pstate->varExists(varId));
    // case 1: check PState
    if(_pstate->varIsConst(varId)) {
      PState pstate2=*_pstate;
      AbstractValue varVal=(pstate2)[varId];
      return varVal;
    }
    // case 2: check constraint if var is top
    if(_pstate->varIsTop(varId))
      return constraints()->varAbstractValue(varId);
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
      if(!cs->varAbstractValue(varId).isConstInt()) {
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
