/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"

#include "Analyzer.h"
#include "CommandLineOptions.h"
#include <unistd.h>
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "AnalysisAbstractionLayer.h"
#include "SpotConnection.h"

#include <boost/bind.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "boost/lexical_cast.hpp"

#include "Timer.h"

using namespace CodeThorn;
using namespace std;

#include "CollectionOperators.h"

CTIOLabeler::CTIOLabeler(SgNode* start, VariableIdMapping* variableIdMapping): SPRAY::IOLabeler(start, variableIdMapping) {
}

bool CTIOLabeler::isStdIOLabel(Label label) {
  cerr<<"Warning: deprecated function: isStdIOLabel."<<endl;
  return SPRAY::IOLabeler::isStdIOLabel(label);
}

bool CTIOLabeler::isStdInLabel(Label label, VariableId* id=0) {
  if(SPRAY::IOLabeler::isStdInLabel(label,id)) {
    return true;
  } else if(isNonDetIntFunctionCall(label,id)) {
    return true;
  } else if(isNonDetLongFunctionCall(label,id)) {
    return true;
  }
  return false;
}

// consider to use Analyzer* instead and query this information
void CTIOLabeler::setExternalNonDetIntFunctionName(std::string name) {
  _externalNonDetIntFunctionName=name;
}

void CTIOLabeler::setExternalNonDetLongFunctionName(std::string name) {
  _externalNonDetLongFunctionName=name;
}

bool CTIOLabeler::isNonDetIntFunctionCall(Label lab,VariableId* varIdPtr){
  return isFunctionCallWithName(lab,varIdPtr,_externalNonDetIntFunctionName);
}

bool CTIOLabeler::isNonDetLongFunctionCall(Label lab,VariableId* varIdPtr){
  return isFunctionCallWithName(lab,varIdPtr,_externalNonDetLongFunctionName);
}

bool CTIOLabeler::isFunctionCallWithName(Label lab,VariableId* varIdPtr,string name){
  SgNode* node=getNode(lab);
  if(isFunctionCallLabel(lab)) {
    std::pair<SgVarRefExp*,SgFunctionCallExp*> p=SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp2(node);
    if(p.first) {
      string funName=SgNodeHelper::getFunctionName(p.second);
      if(funName!=name) {
	return false;
      }
      if(varIdPtr) {
	*varIdPtr=_variableIdMapping->variableId(p.first);
      }
      return true;
    }
  }
  return false;
}

CTIOLabeler::~CTIOLabeler() {
}

bool Analyzer::isFunctionCallWithAssignment(Label lab,VariableId* varIdPtr){
  //return _labeler->getLabeler()->isFunctionCallWithAssignment(lab,varIdPtr);
  SgNode* node=getLabeler()->getNode(lab);
  if(getLabeler()->isFunctionCallLabel(lab)) {
    std::pair<SgVarRefExp*,SgFunctionCallExp*> p=SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp2(node);
    if(p.first) {
      if(varIdPtr) {
	*varIdPtr=variableIdMapping.variableId(p.first);
      }
      return true;
    }
  }
  return false;
}

bool VariableValueMonitor::isActive() {
  return _threshold!=-1;
}

VariableValueMonitor::VariableValueMonitor():_threshold(-1){
}

// in combination with adaptive-top mode
void VariableValueMonitor::setThreshold(size_t threshold) {
  _threshold=threshold;
}

void VariableValueMonitor::setVariableMode(VariableMode mode,VariableId variableId) {
  _variablesModeMap[variableId]=mode;
}

VariableValueMonitor::VariableMode VariableValueMonitor::getVariableMode(VariableId variableId) {
  return _variablesModeMap[variableId];
}

// the init function only uses the variableIds of a given estate (not its values) for initialization
void VariableValueMonitor::init(const EState* estate) {
  const PState* pstate=estate->pstate();
  init(pstate);
}

void VariableValueMonitor::init(const PState* pstate) {
  VariableIdSet varIdSet=pstate->getVariableIds();
  for(VariableIdSet::iterator i=varIdSet.begin(); i!=varIdSet.end(); ++i) {
    // to also allow reinit
    if(_variablesMap.find(*i)==_variablesMap.end()) {
      _variablesMap[*i]=new set<int>(); // initialize value set for each variable
      _variablesModeMap[*i]=VariableValueMonitor::VARMODE_PRECISE;
    }
  }
}

VariableIdSet VariableValueMonitor::getHotVariables(Analyzer* analyzer, const PState* pstate) {
  if(pstate->size()!=_variablesMap.size()) {
    // found a new variable during analysis (e.g. local variable)
    init(pstate);
  }
  VariableIdSet hotVariables;
  VariableIdSet varIdSet=pstate->getVariableIds();
  for(VariableIdSet::iterator i=varIdSet.begin(); i!=varIdSet.end(); ++i) {
    if(isHotVariable(analyzer,*i)) {
      hotVariables.insert(*i);
    }
  }
  return hotVariables;
}

VariableIdSet VariableValueMonitor::getHotVariables(Analyzer* analyzer, const EState* estate) {
  const PState* pstate=estate->pstate();
  return getHotVariables(analyzer,pstate);
}

void VariableValueMonitor::update(Analyzer* analyzer,EState* estate) {
  VariableIdSet hotVariables=getHotVariables(analyzer,estate);
  const PState* pstate=estate->pstate();
  if(pstate->size()!=_variablesMap.size()) {
    //cerr<<"WARNING: variable map size mismatch (probably local var)"<<endl;
    //cerr<<"... reinitializing."<<endl;
    init(estate);
  }
      
  VariableIdSet varIdSet=pstate->getVariableIds();
  for(VariableIdSet::iterator i=varIdSet.begin(); i!=varIdSet.end(); ++i) {
    VariableId varId=*i;
    bool isHotVariable=hotVariables.find(varId)!=hotVariables.end();
    if(!isHotVariable) {
      if(pstate->varIsConst(varId)) {
        AValue abstractVal=pstate->varValue(varId);
        ROSE_ASSERT(abstractVal.isConstInt());
        int intVal=abstractVal.getIntValue();
        _variablesMap[varId]->insert(intVal);
      }
    }
  }
}

VariableIdSet VariableValueMonitor::getVariables() {
  VariableIdSet vset;
  for(map<VariableId,VariableMode>::iterator i=_variablesModeMap.begin();
      i!=_variablesModeMap.end();
      ++i) {
    vset.insert((*i).first);
  }
  return vset;
}

bool VariableValueMonitor::isHotVariable(Analyzer* analyzer, VariableId varId) {
  // TODO: provide set of variables to ignore
  string name=SgNodeHelper::symbolToString(analyzer->getVariableIdMapping()->getSymbol(varId));
  switch(_variablesModeMap[varId]) {
  case VariableValueMonitor::VARMODE_FORCED_TOP:
    return true;
  case VariableValueMonitor::VARMODE_ADAPTIVE_TOP: {
    if(name=="input" || name=="output") 
      return false;
    else
      return _threshold!=-1 && ((long int)_variablesMap[varId]->size())>=_threshold;
  }
  case VariableValueMonitor::VARMODE_PRECISE:
    return false;
  default:
    cerr<<"Error: unknown variable monitor mode."<<endl;
    exit(1);
  }
}

#if 0
bool VariableValueMonitor::isVariableBeyondTreshold(Analyzer* analyzer, VariableId varId) {
  // TODO: provide set of variables to ignore
  string name=SgNodeHelper::symbolToString(analyzer->getVariableIdMapping()->getSymbol(varId));
  if(name=="input" || name=="output") 
    return false;
  return _threshold!=-1 && ((long int)_variablesMap[varId]->size())>=_threshold;
}
#endif

string VariableValueMonitor::toString(VariableIdMapping* variableIdMapping) {
  stringstream ss;
  for(map<VariableId,set<int>* >::iterator i=_variablesMap.begin();
      i!=_variablesMap.end();
      ++i) {
    ss<<string("VAR:")<<variableIdMapping->uniqueShortVariableName((*i).first)<<": "<<(*i).second->size()<<": ";
    set<int>* sp=(*i).second;
    for(set<int>::iterator i=sp->begin();i!=sp->end();++i) {
      ss<<*i<<" ";
    }
    ss<<endl;
  }
  ss<<endl;
  return ss.str();
}

void Analyzer::enableExternalFunctionSemantics() {
  _externalFunctionSemantics=true;
  _externalErrorFunctionName="__VERIFIER_error";
  _externalNonDetIntFunctionName="__VERIFIER_nondet_int";
  _externalNonDetLongFunctionName="__VERIFIER_nondet_long";
  _externalExitFunctionName="exit";
}

void Analyzer::disableExternalFunctionSemantics() {
  _externalFunctionSemantics=false;
  _externalErrorFunctionName="";
  _externalNonDetIntFunctionName="";
  _externalNonDetLongFunctionName="";
  _externalExitFunctionName="";
  ROSE_ASSERT(getLabeler());
  getLabeler()->setExternalNonDetIntFunctionName(_externalNonDetIntFunctionName);
  getLabeler()->setExternalNonDetLongFunctionName(_externalNonDetLongFunctionName);
}

Analyzer::Analyzer():
  startFunRoot(0),
  cfanalyzer(0),
  _globalTopifyMode(GTM_IO),
  _displayDiff(10000),
  _numberOfThreadsToUse(1),
  _ltlVerifier(2),
  _semanticFoldThreshold(5000),
  _solver(5),
  _analyzerMode(AM_ALL_STATES),
  _maxTransitions(-1),
  _maxIterations(-1),
  _maxTransitionsForcedTop(-1),
  _maxIterationsForcedTop(-1),
  _treatStdErrLikeFailedAssert(false),
  _skipSelectedFunctionCalls(false),
  _explorationMode(EXPL_BREADTH_FIRST),
  _minimizeStates(false),
  _topifyModeActive(false),
  _iterations(0),
      _approximated_iterations(0),
      _curr_iteration_cnt(0),
      _next_iteration_cnt(0),
      _externalFunctionSemantics(false)
{
  variableIdMapping.setModeVariableIdForEachArrayElement(true);
  for(int i=0;i<100;i++) {
    binaryBindingAssert.push_back(false);
  }
#ifndef USE_CUSTOM_HSET
  estateSet.max_load_factor(0.7);
  pstateSet.max_load_factor(0.7);
  constraintSetMaintainer.max_load_factor(0.7);
#endif
  resetInputSequenceIterator();
}

size_t Analyzer::getNumberOfErrorLabels() {
  return _assertNodes.size();
}

void Analyzer::setGlobalTopifyMode(GlobalTopifyMode mode) {
  _globalTopifyMode=mode;
}

void Analyzer::setExternalErrorFunctionName(std::string externalErrorFunctionName) {
  _externalErrorFunctionName=externalErrorFunctionName;
}

bool Analyzer::isPrecise() {
  return !(isActiveGlobalTopify()||variableValueMonitor.isActive());
}

bool Analyzer::isIncompleteSTGReady() {
  if(_maxTransitions==-1 && _maxIterations==-1)
    return false;
  else if ((_maxTransitions!=-1) && ((long int) transitionGraph.size()>=_maxTransitions))
    return true;
  else if ((_maxIterations!=-1) && ((long int) _iterations>_maxIterations))
    return true;
  else // at least one maximum mode is active, but the corresponding limit has not yet been reached
    return false;
}

ExprAnalyzer* Analyzer::getExprAnalyzer() {
  return &exprAnalyzer;
}

void Analyzer::runSolver() {
  switch(_solver) {
  case 1: runSolver1();break;
  case 2: runSolver2();break;
  case 3: runSolver3();break;
  case 4: runSolver4();break;
  case 5: runSolver5();break;
  case 6: runSolver6();break;
  case 7: runSolver7();break;
  case 8: runSolver8();break;
  case 9: runSolver9();break;
  case 10: runSolver10();break;
  default: assert(0);
  }
}

set<string> Analyzer::variableIdsToVariableNames(VariableIdMapping::VariableIdSet s) {
  set<string> res;
  for(VariableIdMapping::VariableIdSet::iterator i=s.begin();i!=s.end();++i) {
    res.insert(variableIdMapping.uniqueLongVariableName(*i));
  }
  return res;
}

Analyzer::VariableDeclarationList Analyzer::computeUnusedGlobalVariableDeclarationList(SgProject* root) {
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(root);
  Analyzer::VariableDeclarationList usedGlobalVars=computeUsedGlobalVariableDeclarationList(root);
  for(Analyzer::VariableDeclarationList::iterator i=usedGlobalVars.begin();i!=usedGlobalVars.end();++i) {
    globalVars.remove(*i);
  }
  return globalVars;
}

Analyzer::VariableDeclarationList Analyzer::computeUsedGlobalVariableDeclarationList(SgProject* root) {
  if(SgProject* project=isSgProject(root)) {
    Analyzer::VariableDeclarationList usedGlobalVariableDeclarationList;
    list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
    VariableIdSet setOfUsedVars=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,getVariableIdMapping());
    int filteredVars=0;
    for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
      VariableId globalVarId=variableIdMapping.variableId(*i);
      if(setOfUsedVars.find(globalVarId)!=setOfUsedVars.end()) {
        usedGlobalVariableDeclarationList.push_back(*i);
      } else {
        filteredVars++;
      }
    }
    return usedGlobalVariableDeclarationList;
  } else {
    cout << "Error: no global scope.";
    exit(1);
  }    
}
  

string Analyzer::nodeToString(SgNode* node) {
  string textual;
  if(node->attributeExists("info"))
    textual=node->getAttribute("info")->toString()+":";
  return textual+SgNodeHelper::nodeToString(node);
}

Analyzer::~Analyzer() {
  // intentionally empty, nothing to free explicitly
}

void Analyzer::recordTransition(const EState* sourceState, Edge e, const EState* targetState) {
  transitionGraph.add(Transition(sourceState,e,targetState));
  if(boolOptions["semantic-fold"]) {
    Label s=sourceState->label();
    Label t=targetState->label();
    Label stgsl=getTransitionGraph()->getStartLabel();
    if(!isLTLRelevantLabel(s) && s!=stgsl)
#pragma omp critical(NEWNODESTOFOLD)
      {
        _newNodesToFold.insert(sourceState);
      }
    if(!isLTLRelevantLabel(t) && t!=stgsl)
#pragma omp critical(NEWNODESTOFOLD)
      {
        _newNodesToFold.insert(targetState);
      }
  }
}

void Analyzer::printStatusMessage(bool forceDisplay) {
  // forceDisplay currently only turns on or off
  
  // report we are alive
  stringstream ss;
  if(forceDisplay) {
    ss <<color("white")<<"Number of pstates/estates/trans/csets/wl/iter: ";
    ss <<color("magenta")<<pstateSet.size()
       <<color("white")<<"/"
       <<color("cyan")<<estateSet.size()
       <<color("white")<<"/"
       <<color("blue")<<getTransitionGraph()->size()
       <<color("white")<<"/"
       <<color("yellow")<<constraintSetMaintainer.size()
       <<color("white")<<"/"
       <<estateWorkList.size()
       <<"/"<<getIterations()<<"-"<<getApproximatedIterations()
      ;
    ss<<endl;
    cout<<ss.str();
  }
}

bool Analyzer::isInWorkList(const EState* estate) {
  for(EStateWorkList::iterator i=estateWorkList.begin();i!=estateWorkList.end();++i) {
    if(*i==estate) return true;
  }
  return false;
}

bool Analyzer::isLoopCondLabel(Label lab) {
  SgNode* node=getLabeler()->getNode(lab);
  return SgNodeHelper::isLoopCond(node);
}

void Analyzer::addToWorkList(const EState* estate) { 
#pragma omp critical(ESTATEWL)
  {
    if(!estate) {
      cerr<<"INTERNAL ERROR: null pointer added to work list."<<endl;
      exit(1);
    }
    switch(_explorationMode) {
    case EXPL_DEPTH_FIRST: estateWorkList.push_front(estate);break;
    case EXPL_BREADTH_FIRST: estateWorkList.push_back(estate);break;
    case EXPL_RANDOM_MODE1: {
      int perc=4; // 25%-chance depth-first
      int num=rand();
      int sel=num%perc;
      if(sel==0) {
        estateWorkList.push_front(estate);
      } else {
        estateWorkList.push_back(estate);
      }
      break;
    }
    case EXPL_LOOP_AWARE: {
      if(isLoopCondLabel(estate->label())) {
        estateWorkList.push_back(estate);
        //cout<<"DEBUG: push to WorkList: "<<_curr_iteration_cnt<<","<<_next_iteration_cnt<<":"<<_iterations<<endl;
        _next_iteration_cnt++;
      } else {
        estateWorkList.push_front(estate);
      }
      break;
    }
    default:
      cerr<<"Error: unknown exploration mode."<<endl;
      exit(1);
    }
  }
}

bool Analyzer::isActiveGlobalTopify() {
  if(_maxTransitionsForcedTop==-1 && _maxIterationsForcedTop==-1)
    return false;
  if(_topifyModeActive) {
    return true;
  } else {
    if( (_maxTransitionsForcedTop!=-1 && (long int)transitionGraph.size()>=_maxTransitionsForcedTop)
        || (_maxIterationsForcedTop!=-1 && _iterations>_maxIterationsForcedTop) ) {
      _topifyModeActive=true;
      eventGlobalTopifyTurnedOn();
      boolOptions.registerOption("rers-binary",false);
      return true;
    }
  }
  return false;
}

void Analyzer::eventGlobalTopifyTurnedOn() {
  cout<<"STATUS: mode global-topify activated."<<endl;
  VariableIdSet vset=variableValueMonitor.getVariables();
  int n=0;
  int nt=0;
  for(VariableIdSet::iterator i=vset.begin();i!=vset.end();++i) {
    string name=SgNodeHelper::symbolToString(getVariableIdMapping()->getSymbol(*i));
    bool isCompoundIncVar=(_compoundIncVarsSet.find(*i)!=_compoundIncVarsSet.end());
    bool isSmallActivityVar=(_smallActivityVarsSet.find(*i)!=_smallActivityVarsSet.end());
    bool isAssertCondVar=(_assertCondVarsSet.find(*i)!=_assertCondVarsSet.end());
    // xxx
    bool topifyVar=false;
    switch(_globalTopifyMode) {
    case GTM_IO:
      if(name!="input" && name!="output") {
        topifyVar=true;
      }
      break;
    case GTM_IOCF:
      if(name!="input" && name!="output" && name!="cf") {
        topifyVar=true;
      }
      break;
    case GTM_IOCFPTR:
      if(name!="input" && name!="output" && name!="cf" && !variableIdMapping.hasPointerType(*i)) {
        topifyVar=true;
      }
      break;
    case GTM_COMPOUNDASSIGN:
      if(isCompoundIncVar) {
        topifyVar=true;
      }
      break;
    case GTM_FLAGS: {
      if(name=="input" || name=="output" || name=="cf") {
        topifyVar=false;
        break;
      }
      if(isCompoundIncVar) {
        topifyVar=true;
        break;
      }
      if(!isAssertCondVar) {
        topifyVar=true;
        break;
      }
      if(!isSmallActivityVar) {
        topifyVar=true;
        break;
      }
      topifyVar=false;
      break;
    }
    default:
      cerr<<"Error: unsupported topify mode selected. Bailing out."<<endl;
      exit(1);
    }

    //if(name!="input" && name!="output" && name!="cf" && !variableIdMapping.hasPointerType(*i)) {
    //if(name!="input" && name!="output" && name!="cf") {
    //if(isCompoundIncVar) {
    //if(name!="input") {
    //if(name!="output") {
    //if(true) {
    if(topifyVar) {
      variableValueMonitor.setVariableMode(VariableValueMonitor::VARMODE_FORCED_TOP,*i);
      n++;
    }
    nt++;
  }
  PState::setActiveGlobalTopify(true);
  PState::setVariableValueMonitor(&variableValueMonitor);
  PState::_analyzer=this;

  cout<<"STATUS: switched to static analysis (approximating "<<n<<" of "<<nt<<" variables with top-conversion)."<<endl;
  //switch to the counter for approximated loop iterations
  if (_maxTransitionsForcedTop > 1 || _maxIterationsForcedTop > 0) {
    _iterations--;
    _approximated_iterations++;
  }
}

bool Analyzer::isTopified(EState& estate) {
  const PState* pState=estate.pstate();
  return pState->isTopifiedState();
}

void Analyzer::topifyVariable(PState& pstate, ConstraintSet& cset, VariableId varId) {
  //cout<<"DEBUG: DEAD CODE (Analyzer::topifyVariale(..))." <<endl;
  //exit(1);
  pstate.setVariableToTop(varId);
  //cset.removeAllConstraintsOfVar(varId);
}

// PState and cset are assumed to exist. Pstate is assumed to be topified. Only for topify mode.
EState Analyzer::createEStateFastTopifyMode(Label label, const PState* oldPStatePtr, const ConstraintSet* oldConstraintSetPtr) {
  ROSE_ASSERT(isActiveGlobalTopify());
  EState estate=EState(label,oldPStatePtr,oldConstraintSetPtr);
  return estate;
}

EState Analyzer::createEState(Label label, PState pstate, ConstraintSet cset) {
  // here is the best location to adapt the analysis results to certain global restrictions
  if(isActiveGlobalTopify()) {
    // xxx1
#if 1
    VariableIdSet varSet=pstate.getVariableIds();
    for(VariableIdSet::iterator i=varSet.begin();i!=varSet.end();++i) {
      if(variableValueMonitor.isHotVariable(this,*i)) {
        topifyVariable(pstate, cset, *i);
      }
    }
#else
    //pstate.topifyState();
#endif
    // set cset in general to empty cset, otherwise cset can grow again arbitrarily
    ConstraintSet cset0; // xxx2
    cset=cset0;
  }
  if(variableValueMonitor.isActive()) {
    cerr<<"Error: Variable-Value-Monitor: no longer supported."<<endl;
    exit(1);
    VariableIdSet hotVarSet;
#pragma omp critical (VARIABLEVALUEMONITOR)
    hotVarSet=variableValueMonitor.getHotVariables(this,&pstate);
    for(VariableIdSet::iterator i=hotVarSet.begin();i!=hotVarSet.end();++i) {
      topifyVariable(pstate, cset, *i);
    }
  }
  const PState* newPStatePtr=processNewOrExisting(pstate);
  const ConstraintSet* newConstraintSetPtr=processNewOrExisting(cset);
  EState estate=EState(label,newPStatePtr,newConstraintSetPtr);
  return estate;
}

EState Analyzer::createEState(Label label, PState pstate, ConstraintSet cset, InputOutput io) {
  EState estate=createEState(label,pstate,cset);
  estate.io=io;
  return estate;
}

bool Analyzer::isLTLRelevantLabel(Label label) {
  bool t;
  t=isStdIOLabel(label) 
    || (getLabeler()->isStdErrLabel(label) && getLabeler()->isFunctionCallReturnLabel(label))
    //|| isTerminationRelevantLabel(label)
     || isStartLabel(label) // we keep the start state
     || isCppLabeledAssertLabel(label)
    ;
  //cout << "INFO: L"<<label<<": "<<SgNodeHelper::nodeToString(getLabeler()->getNode(label))<< "LTL: "<<t<<endl;
  return t;
}

bool Analyzer::isStartLabel(Label label) {
  return getTransitionGraph()->getStartLabel()==label;
}

bool Analyzer::isStdIOLabel(Label label) {
  bool t;
  t=
    (getLabeler()->isStdInLabel(label) && getLabeler()->isFunctionCallReturnLabel(label))
    || 
    (getLabeler()->isStdOutLabel(label) && getLabeler()->isFunctionCallReturnLabel(label))
    ;
  //cout << "INFO: L"<<label<<": "<<SgNodeHelper::nodeToString(getLabeler()->getNode(label))<< "LTL: "<<t<<endl;
  return t;
}

set<const EState*> Analyzer::nonLTLRelevantEStates() {
  set<const EState*> res;
  set<const EState*> allestates=transitionGraph.estateSet();
  for(set<const EState*>::iterator i=allestates.begin();i!=allestates.end();++i) {
    if(!isLTLRelevantLabel((*i)->label())) {
      res.insert(*i);

      // MS: deactivated this check because it impacts performance dramatically
#if 0
      if(estateSet.estateId(*i)==NO_ESTATE) {
        cerr<< "WARNING: no estate :estateId="<<estateSet.estateId(*i)<<endl;
      }
#endif
    }
  }
  return res;
}

bool Analyzer::isTerminationRelevantLabel(Label label) {
  return SgNodeHelper::isLoopCond(getLabeler()->getNode(label));
}

// We want to avoid calling critical sections from critical sections:
// therefore all worklist functions do not use each other.
bool Analyzer::isEmptyWorkList() { 
  bool res;
#pragma omp critical(ESTATEWL)
  {
    res=estateWorkList.empty();
  }
  return res;
}
const EState* Analyzer::topWorkList() {
  const EState* estate=0;
#pragma omp critical(ESTATEWL)
  {
    if(!estateWorkList.empty())
      estate=*estateWorkList.begin();
  }
  return estate;
}
const EState* Analyzer::popWorkList() {
  const EState* estate=0;
  #pragma omp critical(ESTATEWL)
  {
    if(!estateWorkList.empty())
      estate=*estateWorkList.begin();
    if(estate) {
      estateWorkList.pop_front();
      if(getExplorationMode()==EXPL_LOOP_AWARE && isLoopCondLabel(estate->label())) {
        //cout<<"DEBUG: popFromWorkList: "<<_curr_iteration_cnt<<","<<_next_iteration_cnt<<":"<<_iterations<<endl;
        if(_curr_iteration_cnt==0) {
          _curr_iteration_cnt= (_next_iteration_cnt - 1);
          _next_iteration_cnt=0;
          incIterations();
          //cout<<"STATUS: Started analyzing loop iteration "<<_iterations<<"-"<<_approximated_iterations<<endl;
        } else {
          _curr_iteration_cnt--;
        }
      }
    }
  }
  return estate;
}

// not used anywhere
const EState* Analyzer::takeFromWorkList() {
  const EState* co=0;
#pragma omp critical(ESTATEWL)
  {
  if(estateWorkList.size()>0) {
    co=*estateWorkList.begin();
    estateWorkList.pop_front();
  }
  }
  return co;
}

#define PARALLELIZE_BRANCHES

void Analyzer::runSolver1() {
  size_t prevStateSetSize=0; // force immediate report at start
  omp_set_num_threads(_numberOfThreadsToUse);
  omp_set_dynamic(1);
  int threadNum;
  vector<const EState*> workVector(_numberOfThreadsToUse);
  printStatusMessage(true);
  while(1) {
    int workers;
    for(workers=0;workers<_numberOfThreadsToUse;++workers) {
      if(!(workVector[workers]=popWorkList()))
        break;
    }
    if(workers==0)
      break; // we are done
    
    if(_displayDiff && (estateSet.size()>(prevStateSetSize+_displayDiff))) {
      printStatusMessage(true);
      prevStateSetSize=estateSet.size();
    }
#pragma omp parallel for private(threadNum) shared(workVector)
    for(int j=0;j<workers;++j) {
      threadNum=omp_get_thread_num();
      const EState* currentEStatePtr=workVector[j];
      if(!currentEStatePtr) {
        cerr<<"Error: thread "<<threadNum<<" finished prematurely. Bailing out. "<<endl;
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
        exit(1);
      }
      assert(currentEStatePtr);
      
      Flow edgeSet=flow.outEdges(currentEStatePtr->label());
      //cerr << "DEBUG: edgeSet size:"<<edgeSet.size()<<endl;
#ifdef PARALLELIZE_BRANCHES
      // we can simplify this by adding the proper function to Flow.
      Flow::iterator i=edgeSet.begin();
      int edgeNum=edgeSet.size();
      vector<const Edge*> edgeVec(edgeNum);
      for(int edge_i=0;edge_i<edgeNum;++edge_i) {
        edgeVec[edge_i]=&(*i++);
      }
#pragma omp parallel for
      for(int edge_i=0; edge_i<edgeNum;edge_i++) {
        Edge e=*edgeVec[edge_i]; // *i
#else
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
#endif
          list<EState> newEStateList;
          newEStateList=transferFunction(e,currentEStatePtr);
          //cout << "DEBUG: transfer at edge:"<<e.toString()<<" succ="<<newEStateList.size()<< endl;
          for(list<EState>::iterator nesListIter=newEStateList.begin();
              nesListIter!=newEStateList.end();
              ++nesListIter) {
            EState newEState=*nesListIter;
            assert(newEState.label()!=Label());
            if((!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState))) {
              HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=process(newEState);
              const EState* newEStatePtr=pres.second;
              if(pres.first==true)
                addToWorkList(newEStatePtr);
              recordTransition(currentEStatePtr,e,newEStatePtr);
            }
            if((!newEState.constraints()->disequalityExists()) && (isFailedAssertEState(&newEState))) {
              // failed-assert end-state: do not add to work list but do add it to the transition graph
              const EState* newEStatePtr;
              newEStatePtr=processNewOrExisting(newEState);
              recordTransition(currentEStatePtr,e,newEStatePtr);        
              
              if(boolOptions["report-failed-assert"]) {
#pragma omp critical(OUTPUT)
                {
                  cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
                }
              }
              if(_csv_assert_live_file.size()>0) {
                string name=labelNameOfAssertLabel(currentEStatePtr->label());
                if(name.size()>0) {
                  if(name=="globalError")
                    name="error_60";
                  name=name.substr(6,name.size()-6);
                  std::ofstream fout;
                  // csv_assert_live_file is the member-variable of analyzer
#pragma omp critical(OUTPUT)
                  {
                    fout.open(_csv_assert_live_file.c_str(),ios::app);    // open file for appending
                    assert (!fout.fail( ));
                    fout << name << ",yes,9"<<endl;
                    //cout << "REACHABLE ASSERT FOUND: "<< name << ",yes,9"<<endl;
                    
                    fout.close(); 
                  }
                } // if (assert-label was found)
              } // if
            }
          } // end of loop on transfer function return-estates
#ifdef PARALLELIZE_BRANCHES
        } // edgeset-parallel for
#else
      } // just for proper auto-formatting in emacs
#endif
    } // worklist-parallel for
  } // while
  printStatusMessage(true);
  cout << "analysis finished (worklist is empty)."<<endl;
}
  
const EState* Analyzer::addToWorkListIfNew(EState estate) {
  EStateSet::ProcessingResult res=process(estate);
  if(res.first==true) {
    const EState* newEStatePtr=res.second;
    assert(newEStatePtr);
    addToWorkList(newEStatePtr);
    return newEStatePtr;
  } else {
    //cout << "DEBUG: EState already exists. Not added:"<<estate.toString()<<endl;
    const EState* existingEStatePtr=res.second;
    assert(existingEStatePtr);
    return existingEStatePtr;
  }
}

EState Analyzer::analyzeVariableDeclaration(SgVariableDeclaration* decl,EState currentEState, Label targetLabel) {
  //cout << "INFO1: we are at "<<astTermWithNullValuesToString(nextNodeToAnalyze1)<<endl;
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  if(initName0) {
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      SgSymbol* initDeclVar=initName->search_for_symbol_from_symbol_table();
      assert(initDeclVar);
      VariableId initDeclVarId=getVariableIdMapping()->variableId(initDeclVar);

      // not possible to support yet. getIntValue must succeed on declarations.
      if(false && variableValueMonitor.isHotVariable(this,initDeclVarId)) {
        PState newPState=*currentEState.pstate();
        newPState.setVariableToTop(initDeclVarId);
        ConstraintSet cset=*currentEState.constraints();
        return createEState(targetLabel,newPState,cset);
      }

      if(variableIdMapping.isConstantArray(initDeclVarId) && boolOptions["rersmode"]) {
        // in case of a constant array the array (and its members) are not added to the state.
        // they are considered to be determined from the initializer without representing them
        // in the state
        //cout<<"DEBUG: not adding array to PState."<<endl;
        PState newPState=*currentEState.pstate();
        ConstraintSet cset=*currentEState.constraints();
        return createEState(targetLabel,newPState,cset);
      }
      SgName initDeclVarName=initDeclVar->get_name();
      string initDeclVarNameString=initDeclVarName.getString();
      //cout << "INIT-DECLARATION: var:"<<initDeclVarNameString<<endl;
      //cout << "DECLARATION: var:"<<SgNodeHelper::nodeToString(decl)<<endl;
      SgInitializer* initializer=initName->get_initializer();
      //assert(initializer);
      ConstraintSet cset=*currentEState.constraints();
      SgAssignInitializer* assignInitializer=0;
      if(initializer && isSgAggregateInitializer(initializer)) {
        //cout<<"DEBUG: array-initializer found:"<<initializer->unparseToString()<<endl;
        PState newPState=*currentEState.pstate();
        int elemIndex=0;
        SgExpressionPtrList& initList=SgNodeHelper::getInitializerListOfAggregateDeclaration(decl);
        for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
          VariableId arrayElemId=variableIdMapping.variableIdOfArrayElement(initDeclVarId,elemIndex);
          SgExpression* exp=*i;
          SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
          SgIntVal* intValNode=0;
          if(assignInit && (intValNode=isSgIntVal(assignInit->get_operand_i()))) {
            int intVal=intValNode->get_value();
            //cout<<"DEBUG:initializing array element:"<<arrayElemId.toString()<<"="<<intVal<<endl;
            newPState.setVariableToValue(arrayElemId,CodeThorn::CppCapsuleAValue(AType::ConstIntLattice(intVal)));
          } else {
            cerr<<"Error: unsupported array initializer value:"<<exp->unparseToString()<<" AST:"<<SPRAY::AstTerm::astTermWithNullValuesToString(exp)<<endl;
            exit(1);
          }
          elemIndex++;
        }
        return createEState(targetLabel,newPState,cset);
      } else if(initializer && (assignInitializer=isSgAssignInitializer(initializer))) {
        //cout << "DEBUG: initializer found:"<<initializer->unparseToString()<<endl;
        SgExpression* rhs=assignInitializer->get_operand_i();
        assert(rhs);
        PState newPState=analyzeAssignRhs(*currentEState.pstate(),initDeclVarId,rhs,cset);
        return createEState(targetLabel,newPState,cset);
      } else {
        //cout << "no initializer (OK)."<<endl;
        PState newPState=*currentEState.pstate();
        //newPState[initDeclVarId]=AType::Top();
        newPState.setVariableToTop(initDeclVarId);
        return createEState(targetLabel,newPState,cset);
      }
    } else {
      cerr << "Error: in declaration (@initializedName) no variable found ... bailing out."<<endl;
      exit(1);
    }
  } else {
    cerr << "Error: in declaration: no variable found ... bailing out."<<endl;
    exit(1);
  }
}

// this function has been moved to VariableIdMapping: TODO eliminate this function here
VariableIdMapping::VariableIdSet Analyzer::determineVariableIdsOfVariableDeclarations(set<SgVariableDeclaration*> varDecls) {
  VariableIdMapping::VariableIdSet resultSet;
  for(set<SgVariableDeclaration*>::iterator i=varDecls.begin();i!=varDecls.end();++i) {
    SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(*i);
    if(sym) {
      resultSet.insert(variableIdMapping.variableId(sym));
    }
  }
  return resultSet;
}

// this function has been moved to VariableIdMapping: TODO eliminate this function here
VariableIdMapping::VariableIdSet Analyzer::determineVariableIdsOfSgInitializedNames(SgInitializedNamePtrList& namePtrList) {
  VariableIdMapping::VariableIdSet resultSet;
  for(SgInitializedNamePtrList::iterator i=namePtrList.begin();i!=namePtrList.end();++i) {
    assert(*i);
    SgSymbol* sym=SgNodeHelper::getSymbolOfInitializedName(*i);
    if(sym) {
      resultSet.insert(variableIdMapping.variableId(sym));
    }
  }
  return resultSet;
}

#if 0
bool Analyzer::isAssertExpr(SgNode* node) {
  if(isSgExprStatement(node)) {
    node=SgNodeHelper::getExprStmtChild(node);
    // TODO: refine this to also check for name, paramters, etc.
    if(isSgConditionalExp(node))
      return true;
  }
  return false;
}
#endif

bool Analyzer::isFailedAssertEState(const EState* estate) {
  if(estate->io.isFailedAssertIO())
    return true;
  if(_treatStdErrLikeFailedAssert) {
    return estate->io.isStdErrIO();
  }
  return false;
}

bool Analyzer::isVerificationErrorEState(const EState* estate) {
  if(estate->io.isVerificationError())
    return true;
  return false;
}

EState Analyzer::createFailedAssertEState(EState estate, Label target) {
  EState newEState=estate;
  newEState.io.recordFailedAssert();
  newEState.setLabel(target);
  return newEState;
}

// creates a state that represents that the verification error function was called
// the edge is the external call edge
EState Analyzer::createVerificationErrorEState(EState estate, Label target) {
  EState newEState=estate;
  newEState.io.recordVerificationError();
  newEState.setLabel(target);
  return newEState;
}

list<SgNode*> Analyzer::listOfAssertNodes(SgProject* root) {
  list<SgNode*> assertNodes;
  list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=funDefs.begin();
      i!=funDefs.end();
      ++i) {
    RoseAst ast(*i);
    for(RoseAst::iterator j=ast.begin();j!=ast.end();++j) {
      if(SgNodeHelper::Pattern::matchAssertExpr(*j)) {
        assertNodes.push_back(*j);
      }
    }
  }
  return assertNodes;
}

list<pair<SgLabelStatement*,SgNode*> > Analyzer::listOfLabeledAssertNodes(SgProject* root) {
  list<pair<SgLabelStatement*,SgNode*> > assertNodes;
  list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=funDefs.begin();
      i!=funDefs.end();
      ++i) {
    RoseAst ast(*i);
    RoseAst::iterator prev=ast.begin();
    for(RoseAst::iterator j=ast.begin();j!=ast.end();++j) {
      if(SgNodeHelper::Pattern::matchAssertExpr(*j)) {
        if(prev!=j && isSgLabelStatement(*prev)) {
          SgLabelStatement* labStmt=isSgLabelStatement(*prev);
          //string name=labStmt->get_label().getString();
          // TODO check prefix error_
          //cout<<"Found label "<<assertNodes.size()<<": "<<name<<endl;
          assertNodes.push_back(make_pair(labStmt,*j));
        }
      }
      prev=j;
    }
  }
  return assertNodes;
}

const EState* Analyzer::processCompleteNewOrExisting(const EState* es) {
  const PState* ps=es->pstate();
  const ConstraintSet* cset=es->constraints();
  const PState* ps2=pstateSet.processNewOrExisting(ps);
  // TODO: ps2 check as below
  const ConstraintSet* cset2=constraintSetMaintainer.processNewOrExisting(cset);
  // TODO: cset2 check as below
  const EState* es2=new EState(es->label(),ps2,cset2, es->io);
  const EState* es3=estateSet.processNewOrExisting(es2);
  // equivalent object (but with different address) already exists
  // discard superfluous new object and use existing object pointer.
  if(es2!=es3) {
    delete es2;
  }
  return es3;
}

InputOutput::OpType Analyzer::ioOp(const EState* estate) const {
  return estate->ioOp();
}

const PState* Analyzer::processNew(PState& s) {
  return pstateSet.processNew(s);
}
const PState* Analyzer::processNewOrExisting(PState& s) {
  return pstateSet.processNewOrExisting(s);
}

const EState* Analyzer::processNew(EState& s) {
  return estateSet.processNew(s);
}

const EState* Analyzer::processNewOrExisting(EState& estate) {
  if(boolOptions["tg-ltl-reduced"]) {
    // experimental: passing of params (we can avoid the copying)
    EStateSet::ProcessingResult res=process(estate.label(),*estate.pstate(),*estate.constraints(),estate.io);
    assert(res.second);
    return res.second;
  } else {
    return estateSet.processNewOrExisting(estate);
  }
}

const ConstraintSet* Analyzer::processNewOrExisting(ConstraintSet& cset) {
  return constraintSetMaintainer.processNewOrExisting(cset);
}

EStateSet::ProcessingResult Analyzer::process(EState& estate) {
  if(boolOptions["tg-ltl-reduced"]) {
    // experimental passing of params (we can avoid the copying)
    return process(estate.label(),*estate.pstate(),*estate.constraints(),estate.io);
  } else {
    return estateSet.process(estate);
  }
}

EStateSet::ProcessingResult Analyzer::process(Label label, PState pstate, ConstraintSet cset, InputOutput io) {
  if(isLTLRelevantLabel(label) || io.op!=InputOutput::NONE || (!boolOptions["tg-ltl-reduced"])) {
    const PState* newPStatePtr=processNewOrExisting(pstate);
    const ConstraintSet* newCSetPtr=processNewOrExisting(cset);
    EState newEState=EState(label,newPStatePtr,newCSetPtr,io);
    return estateSet.process(newEState);
  } else {
    //cout << "INFO: allocating temporary estate."<<endl;
    // the following checks are not neccessary but ensure that we reuse pstates and constraint sets
#if 0
    const PState* newPStatePtr=pstateSet.determine(pstate);
    PState* newPStatePtr2=0;
    if(!newPStatePtr) {
      newPStatePtr2=new PState();
      *newPStatePtr2=pstate;
    } else {
      newPStatePtr2=const_cast<PState*>(newPStatePtr);
    }
    ConstraintSet* newCSetPtr=constraintSetMaintainer.determine(cset);
    ConstraintSet* newCSetPtr2;
    if(!newCSetPtr) {
      newCSetPtr2=new ConstraintSet();
      *newCSetPtr2=cset;
    } else {
      newCSetPtr2=const_cast<ConstraintSet*>(newCSetPtr);
    }
#else
    // TODO: temporary states must be marked to be able to free them later (or: any non-maintained state is considered a temporary state)
    PState* newPStatePtr2=new PState();
    *newPStatePtr2=pstate;
    ConstraintSet* newCSetPtr2=new ConstraintSet();
    *newCSetPtr2=cset;
#endif
    EState newEState=EState(label,newPStatePtr2,newCSetPtr2,io);
    const EState* newEStatePtr;
    newEStatePtr=estateSet.determine(newEState);
    if(!newEStatePtr) {
      // new estate (was not stored but was not inserted (this case does not exist for maintained state)
      // therefore we handle it as : has been inserted (hence, all tmp-states are considered to be not equal)
      newEStatePtr=new EState(label,newPStatePtr2,newCSetPtr2,io);
      return make_pair(true,newEStatePtr);
    } else {
      return make_pair(false,newEStatePtr);
    }

    return estateSet.process(newEState);
  }
  throw "Error: Analyzer::processNewOrExisting: programmatic error.";
}

list<EState> elistify() {
  list<EState> resList;
  return resList;
}
list<EState> elistify(EState res) {
  //assert(res.state);
  //assert(res.constraints());
  list<EState> resList;
  resList.push_back(res);
  return resList;
}

list<EState> Analyzer::transferFunction(Edge edge, const EState* estate) {
  assert(edge.source==estate->label());
  // we do not pass information on the local edge
  if(edge.isType(EDGE_LOCAL)) {
#ifdef RERS_SPECIALIZATION
    if(boolOptions["rers-binary"]) {
	  //cout<<"DEBUG: ESTATE: "<<estate->toString(&variableIdMapping)<<endl;
      SgNode* nodeToAnalyze=getLabeler()->getNode(edge.source);
      if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nodeToAnalyze)) {
        assert(funCall);
        string funName=SgNodeHelper::getFunctionName(funCall);
        if(funName=="calculate_output") {
		  //cout<<"DEBUG: BINARY-transfer: calculate output found."<<endl;
#if 0
          SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
          SgExpressionPtrList::iterator j=actualParameters.begin();
          SgExpression* actualParameterExpr=*j;
          assert(actualParameterExpr);
          VariableId actualParameterVarId;
          if(exprAnalyzer.variable(actualParameterExpr,actualParameterVarId)) {
            PState _pstate=*estate->pstate();
            AType::CppCapsuleConstIntLattice aval_capsule=_pstate[actualParameterVarId];
            AValue aval=aval_capsule.getValue();
            int argument=aval.getIntValue();
            //cout << "DEBUG: argument:"<<argument<<endl;
#endif
            // RERS global vars binary handling
            PState _pstate=*estate->pstate();
            RERS_Problem::rersGlobalVarsCallInit(this,_pstate, omp_get_thread_num());
            //cout << "DEBUG: global vars initialized before call"<<endl;

#if 0
            //input variable passed as a parameter (obsolete since usage of script "transform_globalinputvar")
            int rers_result=RERS_Problem::calculate_output(argument); 
            (void) RERS_Problem::calculate_output(argument);
#else
            (void) RERS_Problem::calculate_output( omp_get_thread_num() );
            int rers_result=RERS_Problem::output[omp_get_thread_num()];
#endif
            //cout << "DEBUG: Called calculate_output("<<argument<<")"<<" :: result="<<rers_result<<endl;
            if(rers_result<=-100) {
              // we found a failing assert
              // = rers_result*(-1)-100 : rers error-number
              // = -160 : rers globalError (2012 only)
              int index=((rers_result+100)*(-1));
              assert(index>=0 && index <=99);
              binaryBindingAssert[index]=true;
              //reachabilityResults.reachable(index); //previous location in code
              //cout<<"DEBUG: found assert Error "<<index<<endl;
              ConstraintSet _cset=*estate->constraints();
              InputOutput _io;
              _io.recordFailedAssert();
              // error label encoded in the output value, storing it in the new failing assertion EState
              PState newPstate  = _pstate;
              //newPstate[globalVarIdByName("output")]=CodeThorn::AType::CppCapsuleConstIntLattice(rers_result);
              newPstate.setVariableToValue(globalVarIdByName("output"),
                                           CodeThorn::AType::CppCapsuleConstIntLattice(rers_result));
              EState _eState=createEState(edge.target,newPstate,_cset,_io);
              return elistify(_eState);
            }
            RERS_Problem::rersGlobalVarsCallReturnInit(this,_pstate, omp_get_thread_num());
            // TODO: _pstate[VariableId(output)]=rers_result;
            // matches special case of function call with return value, otherwise handles call without return value (function call is matched above)
            if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nodeToAnalyze)) {
              SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nodeToAnalyze));
              VariableId lhsVarId;
              bool isLhsVar=exprAnalyzer.variable(lhs,lhsVarId);
              assert(isLhsVar); // must hold
              //cout << "DEBUG: lhsvar:rers-result:"<<lhsVarId.toString()<<"="<<rers_result<<endl;
              //_pstate[lhsVarId]=AType::CppCapsuleConstIntLattice(rers_result);
              _pstate.setVariableToValue(lhsVarId,AType::CppCapsuleConstIntLattice(rers_result));
              ConstraintSet _cset=*estate->constraints();
              _cset.removeAllConstraintsOfVar(lhsVarId);
              EState _eState=createEState(edge.target,_pstate,_cset);
              return elistify(_eState);
            } else {
              ConstraintSet _cset=*estate->constraints();
              EState _eState=createEState(edge.target,_pstate,_cset);
              return elistify(_eState);
            }
            cout <<"PState:"<< _pstate<<endl;
            cerr<<"RERS-MODE: call of unknown function."<<endl;
            exit(1);
            // _pstate now contains the current state obtained from the binary
#if 0
          }
#endif
        }
        //cout << "DEBUG: @LOCAL_EDGE: function call:"<<SgNodeHelper::nodeToString(funCall)<<endl;
      }
    }
#endif
    return elistify();
  }
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();
  // 1. we handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(edge.source);
  assert(nextNodeToAnalyze1);
  // handle assert(0)
  if(SgNodeHelper::Pattern::matchAssertExpr(nextNodeToAnalyze1)) {
    return elistify(createFailedAssertEState(currentEState,edge.target));
  }

  if(edge.isType(EDGE_CALL)) {
    // 1) obtain actual parameters from source
    // 2) obtain formal parameters from target
    // 3) eval each actual parameter and assign result to formal parameter in state
    // 4) create new estate

    // ad 1)
    SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(getLabeler()->getNode(edge.source));
    assert(funCall);
    string funName=SgNodeHelper::getFunctionName(funCall);
    // handling of error function (TODO: generate dedicated state (not failedAssert))
    
#ifdef RERS_SPECIALIZATION
    if(boolOptions["rers-binary"]) {
      // if rers-binary function call is selected then we skip the static analysis for this function (specific to rers)
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(funName=="calculate_output") {
        //cout << "DEBUG: rers-binary mode: skipped static-analysis call."<<endl;
        return elistify();
      }
    }
#endif

    SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    // ad 2)
    SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.target));
    SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
    assert(funDef);
    // ad 3)
    PState newPState=currentPState;
    SgInitializedNamePtrList::iterator i=formalParameters.begin();
    SgExpressionPtrList::iterator j=actualParameters.begin();
    while(i!=formalParameters.end() || j!=actualParameters.end()) {
      SgInitializedName* formalParameterName=*i;
      assert(formalParameterName);
      VariableId formalParameterVarId=variableIdMapping.variableId(formalParameterName);
      // VariableName varNameString=name->get_name();
      SgExpression* actualParameterExpr=*j;
      assert(actualParameterExpr);
      // check whether the actualy parameter is a single variable: In this case we can propagate the constraints of that variable to the formal parameter.
      // pattern: call: f(x), callee: f(int y) => constraints of x are propagated to y
      VariableId actualParameterVarId;
      assert(actualParameterExpr);
      if(exprAnalyzer.variable(actualParameterExpr,actualParameterVarId)) {
        // propagate constraint from actualParamterVarId to formalParameterVarId
        cset.addAssignEqVarVar(formalParameterVarId,actualParameterVarId);
      }
      // general case: the actual argument is an arbitrary expression (including a single variable)
      // we use for the third parameter "false": do not use constraints when extracting values.
      // Consequently, formalparam=actualparam remains top, even if constraints are available, which
      // would allow to extract a constant value (or a range (when relational constraints are added)).
      list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evalConstInt(actualParameterExpr,currentEState,false, true);
      assert(evalResultList.size()>0);
      list<SingleEvalResultConstInt>::iterator resultListIter=evalResultList.begin();
      SingleEvalResultConstInt evalResult=*resultListIter;
      if(evalResultList.size()>1) {
        cerr<<"Error: We currently do not support multi-state generating operators in function call parameters (yet)."<<endl;
        exit(1);
      }
      // above evalConstInt does not use constraints (par3==false). Therefore top vars remain top vars (which is what we want here)
      //newPState[formalParameterVarId]=evalResult.value();
      newPState.setVariableToValue(formalParameterVarId,evalResult.value());
      ++i;++j;
    }
    // assert must hold if #formal-params==#actual-params (TODO: default values)
    assert(i==formalParameters.end() && j==actualParameters.end()); 
    // ad 4
    return elistify(createEState(edge.target,newPState,cset));
  }
  // "return x;": add $return=eval() [but not for "return f();"]
  if(isSgReturnStmt(nextNodeToAnalyze1) && !SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {

    SgNode* expr=SgNodeHelper::getFirstChild(nextNodeToAnalyze1);
    ConstraintSet cset=*currentEState.constraints();
    if(isSgNullExpression(expr)) {
      // return without expr
      return elistify(createEState(edge.target,*(currentEState.pstate()),cset));
    } else {
      PState newPState=analyzeAssignRhs(*(currentEState.pstate()),
                                        variableIdMapping.createUniqueTemporaryVariableId(string("$return")),
                                        expr,
                                        cset);
      return elistify(createEState(edge.target,newPState,cset));
    }
  }

  // function exit node:
  if(getLabeler()->isFunctionExitLabel(edge.source)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.source))) {
      // 1) determine all local variables (including formal parameters) of function
      // 2) delete all local variables from state
      // 2a) remove variable from state
      // 2b) remove all constraints concerning this variable
      // 3) create new EState and return

      // ad 1)
      set<SgVariableDeclaration*> varDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
      // ad 2)
      ConstraintSet cset=*currentEState.constraints();
      PState newPState=*(currentEState.pstate());
      VariableIdMapping::VariableIdSet localVars=determineVariableIdsOfVariableDeclarations(varDecls);
      SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      VariableIdMapping::VariableIdSet formalParams=determineVariableIdsOfSgInitializedNames(formalParamInitNames);
      VariableIdMapping::VariableIdSet vars=localVars+formalParams;
      set<string> names=variableIdsToVariableNames(vars);

      for(VariableIdMapping::VariableIdSet::iterator i=vars.begin();i!=vars.end();++i) {
        VariableId varId=*i;
        newPState.deleteVar(varId);
        cset.removeAllConstraintsOfVar(varId);
      }
      // ad 3)
      return elistify(createEState(edge.target,newPState,cset));
    } else {
      cerr << "FATAL ERROR: no function definition associated with function exit label."<<endl;
      exit(1);
    }
  }
  if(getLabeler()->isFunctionCallReturnLabel(edge.source)) {
    // case 1: return f(); pass estate trough
    if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
      EState newEState=currentEState;
      newEState.setLabel(edge.target);
      return elistify(newEState);
    }
    // case 2: x=f(); bind variable x to value of $return
    if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nextNodeToAnalyze1)) {
#ifdef RERS_SPECIALIZATION
      if(boolOptions["rers-binary"]) {
        if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
          string funName=SgNodeHelper::getFunctionName(funCall);
          if(funName=="calculate_output") {
            EState newEState=currentEState;
            newEState.setLabel(edge.target);
            return elistify(newEState);
          }
        }
      }
#endif
      SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1));
      VariableId lhsVarId;
      bool isLhsVar=exprAnalyzer.variable(lhs,lhsVarId);
      assert(isLhsVar); // must hold
      PState newPState=*currentEState.pstate();
      // we only create this variable here to be able to find an existing $return variable!
      VariableId returnVarId;
      returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));

      if(newPState.find(returnVarId)!=newPState.end()) {
	AValue evalResult=newPState[returnVarId].getValue();
	//newPState[lhsVarId]=evalResult;
	newPState.setVariableToValue(lhsVarId,evalResult);

	cset.addAssignEqVarVar(lhsVarId,returnVarId);

	newPState.deleteVar(returnVarId); // remove $return from state
	cset.removeAllConstraintsOfVar(returnVarId); // remove constraints of $return

	return elistify(createEState(edge.target,newPState,cset));
      } else {
	// no $return variable found in state. This can be the case for an extern function.
	// alternatively a $return variable could be added in the external function call to
	// make this handling here uniform
	// for external functions no constraints are generated in the call-return node
	return elistify(createEState(edge.target,newPState,cset));
      }
    }
    // case 3: f(); remove $return from state (discard value)
    if(SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(nextNodeToAnalyze1)) {
      PState newPState=*currentEState.pstate();
      VariableId returnVarId;
      returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));
      // no effect if $return does not exist
      newPState.deleteVar(returnVarId);
      cset.removeAllConstraintsOfVar(returnVarId); // remove constraints of $return
      //ConstraintSet cset=*currentEState.constraints; ???
      return elistify(createEState(edge.target,newPState,cset));
    }
  }

  if(edge.isType(EDGE_EXTERNAL)) {
    InputOutput newio;
    Label lab=getLabeler()->getLabel(nextNodeToAnalyze1);
    VariableId varId;
    // TODO: check whether the following test is superfluous meanwhile, since isStdInLabel does take NonDetX functions into account
    bool isExternalNonDetXFunction=false;
    if(isFunctionCallWithAssignment(lab,&varId)) {
      if(usingExternalFunctionSemantics()) {
	if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
	  ROSE_ASSERT(funCall);
	  string externalFunctionName=SgNodeHelper::getFunctionName(funCall);
	  if(externalFunctionName==_externalNonDetIntFunctionName||externalFunctionName==_externalNonDetLongFunctionName) {
	    isExternalNonDetXFunction=true;
	  }
	}
      }
    }
    if(isExternalNonDetXFunction || getLabeler()->isStdInLabel(lab,&varId)) {
      if(_inputSequence.size()>0) {
        PState newPState=*currentEState.pstate();
        ConstraintSet newCSet=*currentEState.constraints();
        newCSet.removeAllConstraintsOfVar(varId);
        list<EState> resList;
        int newValue;
        if(_inputSequenceIterator!=_inputSequence.end()) {
          newValue=*_inputSequenceIterator;
          ++_inputSequenceIterator;
          //cout<<"INFO: input sequence value: "<<newValue<<endl;
        } else {
          return resList; // return no state (this ends the analysis)
        }
        if(boolOptions["input-values-as-constraints"]) {
          newCSet.removeAllConstraintsOfVar(varId);
          //newPState[varId]=AType::Top();
          newPState.setVariableToTop(varId);
          newCSet.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,AType::ConstIntLattice(newValue)));
          assert(newCSet.size()>0);
        } else {
          newCSet.removeAllConstraintsOfVar(varId);
          //newPState[varId]=AType::ConstIntLattice(newValue);
          newPState.setVariableToValue(varId,AType::ConstIntLattice(newValue));
        }
        newio.recordVariable(InputOutput::STDIN_VAR,varId);
        EState estate=createEState(edge.target,newPState,newCSet,newio);
        resList.push_back(estate);
        //cout << "DEBUG: created "<<_inputVarValues.size()<<" input states."<<endl;
        return resList;
      } else {
        if(_inputVarValues.size()>0) {
          // update state (remove all existing constraint on that variable and set it to top)
          PState newPState=*currentEState.pstate();
          ConstraintSet newCSet=*currentEState.constraints();
          newCSet.removeAllConstraintsOfVar(varId);
          list<EState> resList;
          for(set<int>::iterator i=_inputVarValues.begin();i!=_inputVarValues.end();++i) {
            PState newPState=*currentEState.pstate();
            if(boolOptions["input-values-as-constraints"]) {
              newCSet.removeAllConstraintsOfVar(varId);
              //newPState[varId]=AType::Top();
              newPState.setVariableToTop(varId);
              newCSet.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,AType::ConstIntLattice(*i)));
              assert(newCSet.size()>0);
            } else {
              newCSet.removeAllConstraintsOfVar(varId);
              // new input value must be const (otherwise constraints must be used)
              //newPState[varId]=AType::ConstIntLattice(*i);
              newPState.setVariableToValue(varId,AType::ConstIntLattice(*i));
            }
            newio.recordVariable(InputOutput::STDIN_VAR,varId);
            EState estate=createEState(edge.target,newPState,newCSet,newio);
            resList.push_back(estate);
          }
          //cout << "DEBUG: created "<<_inputVarValues.size()<<" input states."<<endl;
          return resList;
        } else {
          // without specified input values (default mode: analysis performed for all possible input values)
          // update state (remove all existing constraint on that variable and set it to top)
          PState newPState=*currentEState.pstate();
          ConstraintSet newCSet=*currentEState.constraints();
          if(boolOptions["abstract-interpreter"]) {
            cout<<"CodeThorn-abstract-interpreter(stdin)> ";
            AValue aval;
            SPRAY::Parse::whitespaces(cin);
            cin >> aval;
            newCSet.removeAllConstraintsOfVar(varId);
            //newPState[varId]=aval;
            newPState.setVariableToValue(varId,aval);
          } else {
            if(boolOptions["update-input-var"]) {
              newCSet.removeAllConstraintsOfVar(varId);
              //newPState[varId]=AType::Top();
              newPState.setVariableToTop(varId);
            }
          }
          newio.recordVariable(InputOutput::STDIN_VAR,varId);
          return elistify(createEState(edge.target,newPState,newCSet,newio));
        }
      }
    }
    if(getLabeler()->isStdOutVarLabel(lab,&varId)) {
      {
        newio.recordVariable(InputOutput::STDOUT_VAR,varId);
        assert(newio.var==varId);
      }
      if(boolOptions["report-stdout"]) {
        cout << "REPORT: stdout:"<<varId.toString()<<":"<<estate->toString()<<endl;
      }
      if(boolOptions["abstract-interpreter"]) {
        PState* pstate=const_cast<PState*>(estate->pstate());
        AType::ConstIntLattice aint=(*pstate)[varId].getValue();
        // TODO: to make this more specific we must parse the printf string
        cout<<"CodeThorn-abstract-interpreter(stdout)> ";
        cout<<aint.toString()<<endl;
      }
    }
    {
      int constvalue;
      if(getLabeler()->isStdOutConstLabel(lab,&constvalue)) {
        {
          newio.recordConst(InputOutput::STDOUT_CONST,constvalue);
        }
        if(boolOptions["report-stdout"]) {
          cout << "REPORT: stdoutconst:"<<constvalue<<":"<<estate->toString()<<endl;
        }
      }
    }
    if(getLabeler()->isStdErrLabel(lab,&varId)) {
      newio.recordVariable(InputOutput::STDERR_VAR,varId);
      ROSE_ASSERT(newio.var==varId);
      if(boolOptions["abstract-interpreter"]) {
        PState* pstate=const_cast<PState*>(estate->pstate());
        AType::ConstIntLattice aint=(*pstate)[varId].getValue();
        // TODO: to make this more specific we must parse the printf string
        cerr<<"CodeThorn-abstract-interpreter(stderr)> ";
        cerr<<aint.toString()<<endl;
      }
      if(boolOptions["report-stderr"]) {
        cout << "REPORT: stderr:"<<varId.toString()<<":"<<estate->toString()<<endl;
      }
    }
    /* handling of specific semantics for external function */ {
      if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
        assert(funCall);
        string funName=SgNodeHelper::getFunctionName(funCall);
        if(usingExternalFunctionSemantics()) {
          if(funName==_externalErrorFunctionName) {
            //cout<<"DETECTED error function: "<<_externalErrorFunctionName<<endl;
            return elistify(createVerificationErrorEState(currentEState,edge.target));
          } else if(funName==_externalExitFunctionName) {
            /* the exit function is modeled to terminate the program
               (therefore no successor state is generated)
            */
            return elistify();
          }
        }
      }
    }

    // for all other external functions we use identity as transfer function
    EState newEState=currentEState;
    newEState.io=newio;
    newEState.setLabel(edge.target);
    return elistify(newEState);
  }

  // special case external call
  if(SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1) 
     ||edge.isType(EDGE_EXTERNAL)
     ||edge.isType(EDGE_CALLRETURN)) {
    EState newEState=currentEState;
    newEState.setLabel(edge.target);
    return elistify(newEState);
  }
  
  //cout << "INFO1: we are at "<<astTermWithNullValuesToString(nextNodeToAnalyze1)<<endl;
  if(SgVariableDeclaration* decl=isSgVariableDeclaration(nextNodeToAnalyze1)) {
    return elistify(analyzeVariableDeclaration(decl,currentEState, edge.target));
  }

  if(isSgExprStatement(nextNodeToAnalyze1) || SgNodeHelper::isForIncExpr(nextNodeToAnalyze1)) {
    SgNode* nextNodeToAnalyze2=0;
    if(isSgExprStatement(nextNodeToAnalyze1))
      nextNodeToAnalyze2=SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1);
    if(SgNodeHelper::isForIncExpr(nextNodeToAnalyze1)) {
      nextNodeToAnalyze2=nextNodeToAnalyze1;
    }
    assert(nextNodeToAnalyze2);
    Label newLabel;
    PState newPState;
    ConstraintSet newCSet;
    if(edge.isType(EDGE_TRUE) || edge.isType(EDGE_FALSE)) {
      list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evalConstInt(nextNodeToAnalyze2,currentEState,true,true);
      //assert(evalResultList.size()==1);
      list<EState> newEStateList;
      for(list<SingleEvalResultConstInt>::iterator i=evalResultList.begin();
          i!=evalResultList.end();
          ++i) {
        SingleEvalResultConstInt evalResult=*i;
        if((evalResult.isTrue() && edge.isType(EDGE_TRUE)) || (evalResult.isFalse() && edge.isType(EDGE_FALSE)) || evalResult.isTop()) {
          // pass on EState
          newLabel=edge.target;
          newPState=*evalResult.estate.pstate();
          // merge with collected constraints of expr (exprConstraints)
          if(edge.isType(EDGE_TRUE)) {
            newCSet=*evalResult.estate.constraints()+evalResult.exprConstraints;
          } else if(edge.isType(EDGE_FALSE)) {
            ConstraintSet s1=*evalResult.estate.constraints();
            ConstraintSet s2=evalResult.exprConstraints;
            newCSet=s1+s2;
          }
          newEStateList.push_back(createEState(newLabel,newPState,newCSet));
        } else {
          // we determined not to be on an execution path, therefore do nothing (do not add any result to resultlist)
        }
      }
      // return LIST
      return newEStateList;
    }

    if(isSgConditionalExp(nextNodeToAnalyze2)) {
      // this is meanwhile modeled in the ExprAnalyzer - TODO: utilize as expr-stmt.
      cerr<<"Error: found conditional expression outside assert. We do not support this form yet."<<endl;
      exit(1);
#if 0
      // TODO
      SgExpression* cond=nextNodeToAnalyze2->get_conditional_exp();
      SgExpression* trueexp=nextNodeToAnalyze2->get_true_exp();
      SgExpression* falseexp=nextNodeToAnalyze2->get_false_exp();
#endif
      // we currently only handle ConditionalExpressions as used in asserts (handled above)
      ConstraintSet cset=*currentEState.constraints();
      PState newPState=*currentEState.pstate();
      return elistify(createEState(edge.target,newPState,cset));
    }

    if(SgNodeHelper::isPrefixIncDecOp(nextNodeToAnalyze2) || SgNodeHelper::isPostfixIncDecOp(nextNodeToAnalyze2)) {
      SgNode* nextNodeToAnalyze3=SgNodeHelper::getUnaryOpChild(nextNodeToAnalyze2);
      VariableId var;
      if(exprAnalyzer.variable(nextNodeToAnalyze3,var)) {

        list<SingleEvalResultConstInt> res=exprAnalyzer.evalConstInt(nextNodeToAnalyze3,currentEState,true,true);
        assert(res.size()==1); // must hold for currently supported limited form of ++,--
        list<SingleEvalResultConstInt>::iterator i=res.begin();
        EState estate=(*i).estate;
        PState newPState=*estate.pstate();
        ConstraintSet cset=*estate.constraints();

        AType::ConstIntLattice varVal=newPState[var].getValue();
        AType::ConstIntLattice const1=1;
        switch(nextNodeToAnalyze2->variantT()) {          
        case V_SgPlusPlusOp:
          varVal=varVal+const1; // overloaded binary + operator
          break;
        case V_SgMinusMinusOp:
          varVal=varVal-const1; // overloaded binary - operator
          break;
        default:
          cerr << "Operator-AST:"<<SPRAY::AstTerm::astTermToMultiLineString(nextNodeToAnalyze2,2)<<endl;
          cerr << "Operator:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze2)<<endl;
          cerr << "Operand:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze3)<<endl;
          cerr<<"Error: programmatic error in handling of inc/dec operators."<<endl;
          exit(1);
        }
        //newPState[var]=varVal;
        newPState.setVariableToValue(var,varVal);

        if(!(*i).result.isTop())
          cset.removeAllConstraintsOfVar(var);
        list<EState> estateList;
        estateList.push_back(createEState(edge.target,newPState,cset));
        return estateList;
      } else {
        throw "Error: currently inc/dec operators are only supported for variables.";
      }
    }

    if(isSgAssignOp(nextNodeToAnalyze2)) {
      SgNode* lhs=SgNodeHelper::getLhs(nextNodeToAnalyze2);
      SgNode* rhs=SgNodeHelper::getRhs(nextNodeToAnalyze2);
      list<SingleEvalResultConstInt> res=exprAnalyzer.evalConstInt(rhs,currentEState,true,true);
      list<EState> estateList;
      for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
        VariableId lhsVar;
        bool isLhsVar=exprAnalyzer.variable(lhs,lhsVar);
        if(isLhsVar) {
          EState estate=(*i).estate;
          PState newPState=*estate.pstate();
          ConstraintSet cset=*estate.constraints();
          // only update integer variables. Ensure values of floating-point variables are not computed
          if(variableIdMapping.hasIntegerType(lhsVar)) {
#if 0
            if(isActiveGlobalTopify()) {
              // TODO: CHECK OUTPUT-OUTPUT HERE
              string varName=variableIdMapping.variableName(lhsVar);
              if(varName=="output") {
                AType::CppCapsuleConstIntLattice  checkValCapsule=newPState[lhsVar];
                AType::ConstIntLattice checkVal=checkValCapsule.getValue();
                AValue newVal=(*i).result;
                if(!newVal.isTop()) {
                  int newInt=newVal.getIntValue();
                  if(checkVal.isConstInt()) {
                    int checkInt=checkVal.getIntValue();
                    if(checkInt!=-1 && checkInt!=-2 && newInt!=-1 && newInt!=-2) {
                      // detected 2nd assignment of output variable
                      // do not add a new state
                      //cout<<"INFO: detected output-output path."<<endl;
                      return estateList;
                    }
                  }
                }
              }
            }
#endif
            if(variableValueMonitor.isActive() && variableValueMonitor.isHotVariable(this,lhsVar)) {
              //cout<<"DEBUG: Topifying hot variable :)"<<lhsVar.toString()<<endl;
              newPState.setVariableToTop(lhsVar);
            } else {
              //newPState[lhsVar]=(*i).result;
              //cout<<"DEBUG: assign lhs var:"<<lhsVar.toString()<<endl;
              newPState.setVariableToValue(lhsVar,(*i).result);
            }
          } else if(variableIdMapping.hasPointerType(lhsVar)) {
            // we assume here that only arrays (pointers to arrays) are assigned
            // see CODE-POINT-1 in ExprAnalyzer.C
            //cout<<"DEBUG: pointer-assignment: "<<lhsVar.toString()<<"="<<(*i).result<<endl;
            //newPState[lhsVar]=(*i).result;
            if(variableValueMonitor.isActive() && variableValueMonitor.isHotVariable(this,lhsVar)) {
              newPState.setVariableToTop(lhsVar);
            } else {
              newPState.setVariableToValue(lhsVar,(*i).result);
            }
          }
          if(!(*i).result.isTop())
            cset.removeAllConstraintsOfVar(lhsVar);
          estateList.push_back(createEState(edge.target,newPState,cset));
        } else if(isSgPntrArrRefExp(lhs)) {
          // for now we ignore array refs on lhs
          // TODO: assignments in index computations of ignored array ref
          // see ExprAnalyzer.C: case V_SgPntrArrRefExp:
          // since nothing can change (because of being ignored) state remains the same
          VariableIdMapping* _variableIdMapping=&variableIdMapping;
          EState estate=(*i).estate;
          PState oldPState=*estate.pstate();
          ConstraintSet oldcset=*estate.constraints();            
          if(getSkipArrayAccesses()) {
            // TODO: remove constraints on array-element(s) [currently no constraints are computed for arrays]
            estateList.push_back(createEState(edge.target,oldPState,oldcset));            
          } else {
            cerr<<"Error: lhs array-access not supported yet."<<endl;
            exit(1);
            if(SgVarRefExp* varRefExp=isSgVarRefExp(lhs)) {
              PState pstate2=oldPState;
              VariableId arrayVarId=_variableIdMapping->variableId(varRefExp);
              // two cases
              if(_variableIdMapping->hasArrayType(arrayVarId)) {
                // has already correct id
                // nothing to do
              } else if(_variableIdMapping->hasPointerType(arrayVarId)) {
                // in case it is a pointer retrieve pointer value
                //cout<<"DEBUG: pointer-array access!"<<endl;
                if(pstate2.varExists(arrayVarId)) {
                  AValue aValuePtr=pstate2[arrayVarId].getValue();
                  // convert integer to VariableId
                  // TODO (topify mode: does read this as integer)
                  if(!aValuePtr.isConstInt()) {
                    cerr<<"Error: pointer value in array access lhs is top. Not supported yet."<<endl;
                    exit(1);
                  }
                  int aValueInt=aValuePtr.getIntValue();
                  // change arrayVarId to refered array!
                  //cout<<"DEBUG: defering pointer-to-array: ptr:"<<_variableIdMapping->variableName(arrayVarId);
                  arrayVarId=_variableIdMapping->variableIdFromCode(aValueInt);
                  //cout<<" to "<<_variableIdMapping->variableName(arrayVarId)<<endl;//DEBUG
                } else {
                  cerr<<"Error: lhs array access: pointer variable does not exist in PState."<<endl;
                  exit(1);
                }
              } else {
                cerr<<"Error: lhs array access: unkown type of array or pointer."<<endl;
                exit(1);
              }
              VariableId arrayElementId;
              AValue aValue=(*i).value();
              int index=-1;
              if(aValue.isConstInt()) {
                index=aValue.getIntValue();
                arrayElementId=_variableIdMapping->variableIdOfArrayElement(arrayVarId,index);
                //cout<<"DEBUG: arrayElementVarId:"<<arrayElementId.toString()<<":"<<_variableIdMapping->variableName(arrayVarId)<<" Index:"<<index<<endl;
              } else {
                cerr<<"Error: lhs array index cannot be evaluated to a constant. Not supported yet."<<endl;
                cerr<<"expr: "<<varRefExp->unparseToString()<<endl;
                exit(1);
              }
              ROSE_ASSERT(arrayElementId.isValid());
              // read value of variable var id (same as for VarRefExp - TODO: reuse)
              // TODO: check whether arrayElementId (or array) is a constant array (arrayVarId)
              if(pstate2.varExists(arrayElementId)) {
                // TODO: handle constraints
                pstate2[arrayElementId]=(*i).value(); // *i is assignment-rhs evaluation result
              } else {
                // check that array is constant array (it is therefore ok that it is not in the state)
                cerr<<"Error: lhs array-access index does not exist in state."<<endl;
                exit(1);
              }
            } else {
              cerr<<"Error: array-access uses expr for denoting the array. Not supported yet."<<endl;
              cerr<<"expr: "<<lhs->unparseToString()<<endl;
              cerr<<"arraySkip: "<<getSkipArrayAccesses()<<endl;
              exit(1);
            }
          }
        } else {
          cerr << "Error: transferfunction:SgAssignOp: unrecognized expression on lhs."<<endl;
          cerr << "expr: "<< lhs->unparseToString()<<endl;
          cerr << "type: "<<lhs->class_name()<<endl;
          //cerr << "performing no update of state!"<<endl;
          exit(1);
        }
      }
      return estateList;
    } else if(isSgCompoundAssignOp(nextNodeToAnalyze2)) {
      cerr<<"Error: compound assignment operators not supported. Use normalization to eliminate these operators."<<endl;
      cerr<<"expr: "<<nextNodeToAnalyze2->unparseToString()<<endl;
      exit(1);
    }
  }
  // nothing to analyze, just create new estate (from same State) with target label of edge
  // can be same state if edge is a backedge to same cfg node
  EState newEState=currentEState;
  newEState.setLabel(edge.target);
  return elistify(newEState);
}

void Analyzer::initializeSolver1(std::string functionToStartAt,SgNode* root, bool oneFunctionOnly) {
  ROSE_ASSERT(root);
  resetInputSequenceIterator();
  std::string funtofind=functionToStartAt;
  RoseAst completeast(root);
  startFunRoot=completeast.findFunctionByName(funtofind);
  if(startFunRoot==0) { 
    std::cout << "Function '"<<funtofind<<"' not found.\n"; 
    exit(1);
  } else {
    std::cout << "INFO: starting at function '"<<funtofind<<"'."<<endl;
  }
  cout << "INIT: Initializing AST node info."<<endl;
  initAstNodeInfo(root);

  cout << "INIT: Creating Labeler."<<endl;
  Labeler* labeler= new CTIOLabeler(root,getVariableIdMapping());
  cout << "INIT: Initializing ExprAnalyzer."<<endl;
  exprAnalyzer.setVariableIdMapping(getVariableIdMapping());
  cout << "INIT: Creating CFAnalysis."<<endl;
  cfanalyzer=new CFAnalysis(labeler,true);
  getLabeler()->setExternalNonDetIntFunctionName(_externalNonDetIntFunctionName);
  getLabeler()->setExternalNonDetLongFunctionName(_externalNonDetLongFunctionName);

  //cout<< "DEBUG: mappingLabelToLabelProperty: "<<endl<<getLabeler()->toString()<<endl;
  cout << "INIT: Building CFGs."<<endl;

  if(oneFunctionOnly)
    flow=cfanalyzer->flow(startFunRoot);
  else
    flow=cfanalyzer->flow(root);

  cout << "STATUS: Building CFGs finished."<<endl;
  if(boolOptions["reduce-cfg"]) {
    int cnt;
#if 0
    // TODO: not working yet because elimination of empty if branches can cause true and false branches to co-exist.
    cnt=cfanalyzer->optimizeFlow(flow);
    cout << "INIT: CFG reduction OK. (eliminated "<<cnt<<" block begin, block end nodes, empty cond nodes.)"<<endl;
#else
    cout << "INIT: CFG reduction is currently limited to block end nodes."<<endl;
    cnt=cfanalyzer->reduceBlockEndNodes(flow);
    cout << "INIT: CFG reduction OK. (eliminated "<<cnt<<" block end nodes)"<<endl;
#endif
  }
  int cnt=cfanalyzer->reduceBlockEndNodes(flow);
  cout << "INIT: enforced CFG reduction of block end nodes OK. (eliminated "<<cnt<<" block end nodes)"<<endl;

  cout << "INIT: Intra-Flow OK. (size: " << flow.size() << " edges)"<<endl;
  if(oneFunctionOnly) {
    cout<<"INFO: analyzing one function only. No inter-procedural flow."<<endl;
  } else {
    InterFlow interFlow=cfanalyzer->interFlow(flow);
    cout << "INIT: Inter-Flow OK. (size: " << interFlow.size()*2 << " edges)"<<endl;
    cfanalyzer->intraInterFlow(flow,interFlow);
    cout << "INIT: IntraInter-CFG OK. (size: " << flow.size() << " edges)"<<endl;
  }

  if(boolOptions["reduce-cfg"]) {
    int cnt=cfanalyzer->inlineTrivialFunctions(flow);
    cout << "INIT: CFG reduction OK. (inlined "<<cnt<<" functions; eliminated "<<cnt*4<<" nodes)"<<endl;
  }
  // create empty state
  PState emptyPState;
  // TODO1: add formal paramters of solo-function
  // SgFunctionDefinition* startFunRoot: node of function
  // estate=analyzeVariableDeclaration(SgVariableDeclaration*,estate,estate.label());
  SgInitializedNamePtrList& initNamePtrList=SgNodeHelper::getFunctionDefinitionFormalParameterList(startFunRoot);
  for(SgInitializedNamePtrList::iterator i=initNamePtrList.begin();i!=initNamePtrList.end();++i) {
    VariableId varId=variableIdMapping.variableId(*i);
    ROSE_ASSERT(varId.isValid());
    // initialize all formal parameters of function (of extremal label) with top
    //emptyPState[varId]=AType::CppCapsuleConstIntLattice(AType::Top());
    emptyPState.setVariableToTop(varId);
  }
  const PState* emptyPStateStored=processNew(emptyPState);
  assert(emptyPStateStored);
  cout << "INIT: Empty state(stored): "<<emptyPStateStored->toString()<<endl;
  assert(cfanalyzer);
  ConstraintSet cset;
  const ConstraintSet* emptycsetstored=constraintSetMaintainer.processNewOrExisting(cset);
  Label startLabel=cfanalyzer->getLabel(startFunRoot);
  transitionGraph.setStartLabel(startLabel);

  EState estate(startLabel,emptyPStateStored,emptycsetstored);
  
  if(SgProject* project=isSgProject(root)) {
    cout << "STATUS: Number of global variables: ";
    list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
    cout << globalVars.size()<<endl;

    VariableIdSet setOfUsedVars=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,&variableIdMapping);

    cout << "STATUS: Number of used variables: "<<setOfUsedVars.size()<<endl;

    int filteredVars=0;
    for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
      VariableId globalVarId=variableIdMapping.variableId(*i);
      // TODO: investigate why array variables get filtered (but should not)
      if(true || (setOfUsedVars.find(globalVarId)!=setOfUsedVars.end() && _variablesToIgnore.find(globalVarId)==_variablesToIgnore.end())) {
        globalVarName2VarIdMapping[variableIdMapping.variableName(variableIdMapping.variableId(*i))]=variableIdMapping.variableId(*i);
        estate=analyzeVariableDeclaration(*i,estate,estate.label());
      } else {
        filteredVars++;
      }
    }
    cout << "STATUS: Number of filtered variables for initial pstate: "<<filteredVars<<endl;
    if(_variablesToIgnore.size()>0)
      cout << "STATUS: Number of ignored variables for initial pstate: "<<_variablesToIgnore.size()<<endl;
  } else {
    cout << "INIT: no global scope.";
  }    

  const EState* currentEState=processNew(estate);
  assert(currentEState);
  variableValueMonitor.init(currentEState);
  //cout << "INIT: "<<eStateSet.toString()<<endl;
  addToWorkList(currentEState);
  cout << "INIT: start state: "<<currentEState->toString(&variableIdMapping)<<endl;
  cout << "INIT: finished."<<endl;
}

set<const EState*> Analyzer::transitionSourceEStateSetOfLabel(Label lab) {
  set<const EState*> estateSet;
  for(TransitionGraph::iterator j=transitionGraph.begin();j!=transitionGraph.end();++j) {
    if((*j)->source->label()==lab)
      estateSet.insert((*j)->source);
  }
  return estateSet;
}

// TODO: this function should be implemented with a call of ExprAnalyzer::evalConstInt
// TODO: currently all rhs which are not a variable are evaluated to top by this function
PState Analyzer::analyzeAssignRhs(PState currentPState,VariableId lhsVar, SgNode* rhs, ConstraintSet& cset) {
  assert(isSgExpression(rhs));
  AValue rhsIntVal=AType::Top();
  bool isRhsIntVal=false;
  bool isRhsVar=false;

  // TODO: -1 is OK, but not -(-1); yet.
  if(SgMinusOp* minusOp=isSgMinusOp(rhs)) {
    if(SgIntVal* intValNode=isSgIntVal(SgNodeHelper::getFirstChild(minusOp))) {
      // found integer on rhs
      rhsIntVal=-((int)intValNode->get_value());
      isRhsIntVal=true;
    }
  }
  // extracted info: isRhsIntVal:rhsIntVal 
  if(SgIntVal* intValNode=isSgIntVal(rhs)) {
    // found integer on rhs
    rhsIntVal=(int)intValNode->get_value();
    isRhsIntVal=true;
  }
  // allow single var on rhs
  if(SgVarRefExp* varRefExp=isSgVarRefExp(rhs)) {
    VariableId rhsVarId;
    isRhsVar=exprAnalyzer.variable(varRefExp,rhsVarId);
    ROSE_ASSERT(isRhsVar);
    ROSE_ASSERT(rhsVarId.isValid());
    // x=y: constraint propagation for var1=var2 assignments
    // we do not perform this operation on assignments yet, as the constraint set could become inconsistent.
    //cset.addEqVarVar(lhsVar, rhsVarId);

    if(currentPState.varExists(rhsVarId)) {
      rhsIntVal=currentPState[rhsVarId].getValue();
    } else {
      if(variableIdMapping.isConstantArray(rhsVarId) && boolOptions["rersmode"]) {
        // in case of an array the id itself is the pointer value
        ROSE_ASSERT(rhsVarId.isValid());
        rhsIntVal=rhsVarId.getIdCode();
      } else {
        cerr << "WARNING: access to variable "<<variableIdMapping.uniqueLongVariableName(rhsVarId)<< " id:"<<rhsVarId.toString()<<" on rhs of assignment, but variable does not exist in state. Initializing with top."<<endl;
        rhsIntVal=AType::Top();
        isRhsIntVal=true;
      }
    }
  }
  PState newPState=currentPState;

  // handle pointer assignment/initialization
  if(variableIdMapping.hasPointerType(lhsVar)) {
    //cout<<"DEBUG: "<<lhsVar.toString()<<" = "<<rhs->unparseToString()<<" : "<<astTermWithNullValuesToString(rhs)<<" : ";
    //cout<<"LHS: pointer variable :: "<<lhsVar.toString()<<endl;
    if(SgVarRefExp* rhsVarExp=isSgVarRefExp(rhs)) {
      VariableId rhsVarId=variableIdMapping.variableId(rhsVarExp);
      if(variableIdMapping.hasArrayType(rhsVarId)) {
        //cout<<" of array type.";
        // we use the id-code as int-value (points-to info)
        int idCode=rhsVarId.getIdCode();
        newPState.setVariableToValue(lhsVar,CodeThorn::CppCapsuleAValue(AType::ConstIntLattice(idCode)));
        //cout<<" id-code: "<<idCode;
        return newPState;
      } else {
        cerr<<"Errpr: RHS: unknown : type: ";
        cerr<<SPRAY::AstTerm::astTermWithNullValuesToString(isSgExpression(rhs)->get_type());
        exit(1);
      }
      cout<<endl;
    }
  }

  if(newPState.varExists(lhsVar)) {
    if(!isRhsIntVal && !isRhsVar) {
      rhsIntVal=AType::Top();
    }
    // we are using AValue here (and  operator== is overloaded for AValue==AValue)
    // for this comparison isTrue() is also false if any of the two operands is AType::Top()
    if( (newPState[lhsVar].getValue()==rhsIntVal).isTrue() ) {
      // update of existing variable with same value
      // => no state change
      return newPState;
    } else {
      // update of existing variable with new value
      //newPState[lhsVar]=rhsIntVal;
      newPState.setVariableToValue(lhsVar,rhsIntVal);
      if((!rhsIntVal.isTop() && !isRhsVar) || boolOptions["arith-top"])
        cset.removeAllConstraintsOfVar(lhsVar);
      return newPState;
    }
  } else {
    if(_variablesToIgnore.size()>0 && (_variablesToIgnore.find(lhsVar)!=_variablesToIgnore.end())) {
      // nothing to do because variable is ignored
    } else {
      // new variable with new value
      //newPState[lhsVar]=rhsIntVal;
      newPState.setVariableToValue(lhsVar,rhsIntVal);
    }
    // no update of constraints because no constraints can exist for a new variable
    return newPState;
  }
  // make sure, we only create/propagate contraints if a non-const value is assigned or if a variable is on the rhs.
  if(!rhsIntVal.isTop() && !isRhsVar)
    cset.removeAllConstraintsOfVar(lhsVar);
  return newPState;
}

void Analyzer::initAstNodeInfo(SgNode* node) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    AstNodeInfo* attr=new AstNodeInfo();
    (*i)->addNewAttribute("info",attr);
  }
}

void Analyzer::generateAstNodeInfo(SgNode* node) {
  assert(node);
  if(!cfanalyzer) {
    cerr << "Error: DFAnalyzer: no cfanalyzer found."<<endl;
    exit(1);
  }
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin().withoutNullValues();i!=ast.end();++i) {
    assert(*i);
    AstNodeInfo* attr=dynamic_cast<AstNodeInfo*>((*i)->getAttribute("info"));
    if(attr) {
      if(cfanalyzer->getLabel(*i)!=Label()) {
        attr->setLabel(cfanalyzer->getLabel(*i));
        attr->setInitialLabel(cfanalyzer->initialLabel(*i));
        attr->setFinalLabels(cfanalyzer->finalLabels(*i));
      } else {
        (*i)->removeAttribute("info");
      }
    } 
#if 0
    cout << "DEBUG:"<<(*i)->sage_class_name();
    if(attr) cout<<":"<<attr->toString();
    else cout<<": no attribute!"<<endl;
#endif
  }
}

// experimental functions
bool Analyzer::checkTransitionGraph() {
  set<const EState*> ess=transitionGraph.estateSet();
  bool ok=isConsistentEStatePtrSet(ess);
  ok=ok && getTransitionGraph()->checkConsistency();
  return ok;
}
bool Analyzer::checkEStateSet() {
  for(EStateSet::iterator i=estateSet.begin();i!=estateSet.end();++i) {
    if(estateSet.estateId(*i)==NO_ESTATE || (*i)->label()==Label()) {
      cerr<< "ERROR: estateSet inconsistent. "<<endl;
      cerr << "  label   :"<<(*i)->label()<<endl;
      cerr<< "   estateId: "<<estateSet.estateId(*i)<<endl;
      return false;
    }
  }
  cout << "INFO: estateSet of size "<<estateSet.size()<<" consistent."<<endl;
  return true;
}

bool Analyzer::isConsistentEStatePtrSet(set<const EState*> estatePtrSet)  {
  for(set<const EState*>::iterator i=estatePtrSet.begin();i!=estatePtrSet.end();++i) {
    if(estateSet.estateId(*i)==NO_ESTATE || (*i)->label()==Label()) {
      cerr<< "ERROR: estatePtrSet inconsistent. "<<endl;
      cerr << "  label   :"<<(*i)->label()<<endl;
      cerr<< "   estateId: "<<estateSet.estateId(*i)<<endl;
      return false;
    }
  }
  cout << "INFO: estatePtrSet of size "<<estatePtrSet.size()<<" consistent."<<endl;
  return true;
}

#if 0
void Analyzer::deleteNonRelevantEStates() {
  size_t numEStatesBefore=estateSet.size();
  for(EStateSet::iterator i=estateSet.begin();i!=estateSet.end();++i) {
    if(isLTLRelevantLabel((*i).label())) {
      estateSet.erase(i);
    }
  }
  size_t numEStatesAfter=estateSet.size();
  if(numEStatesBefore!=numEStatesAfter)
    cout << "STATUS: Reduced estateSet from "<<numEStatesBefore<<" to "<<numEStatesAfter<<" estates."<<endl;
}
#endif

void Analyzer::stdIOFoldingOfTransitionGraph() {
  cout << "STATUS: stdio-folding: computing states to fold."<<endl;
  assert(estateWorkList.size()==0);
  set<const EState*> toReduceSet;
  for(EStateSet::iterator i=estateSet.begin();i!=estateSet.end();++i) {
    Label lab=(*i)->label();
    if(!isStdIOLabel(lab) && !isStartLabel(lab)) {
      toReduceSet.insert(*i);
    }
  }
  cout << "STATUS: stdio-folding: "<<toReduceSet.size()<<" states to fold."<<endl;
  getTransitionGraph()->reduceEStates2(toReduceSet);
  cout << "STATUS: stdio-folding: finished."<<endl;
}

void Analyzer::semanticFoldingOfTransitionGraph() {
  //#pragma omp critical // in conflict with TransitionGraph.add ...
  {
    //cout << "STATUS: (Experimental) semantic folding of transition graph ..."<<endl;
    //assert(checkEStateSet());
    if(boolOptions["post-semantic-fold"]) {
      cout << "STATUS: post-semantic folding: computing states to fold."<<endl;
    } 
    if(boolOptions["report-semantic-fold"]) {
      cout << "STATUS: semantic folding: phase 1: computing states to fold."<<endl;
    }
    if(_newNodesToFold.size()==0) {
      _newNodesToFold=nonLTLRelevantEStates();
    }

    // filter for worklist
    // iterate over worklist and remove all elements that are in the worklist and not LTL-relevant
    int numFiltered=0;
    if(boolOptions["report-semantic-fold"]) {
      cout<<"STATUS: semantic folding: phase 2: filtering."<<endl;
    }
    for(EStateWorkList::iterator i=estateWorkList.begin();i!=estateWorkList.end();++i) {
      if(!isLTLRelevantLabel((*i)->label())) {
        _newNodesToFold.erase(*i);
        numFiltered++;
      }
    }
    if(boolOptions["report-semantic-fold"]) {
      cout << "STATUS: semantic folding: phase 3: reducing "<<_newNodesToFold.size()<< " states (excluding WL-filtered: "<<numFiltered<<")"<<endl;
    }
    int tg_size_before_folding=getTransitionGraph()->size();
    getTransitionGraph()->reduceEStates2(_newNodesToFold);
    int tg_size_after_folding=getTransitionGraph()->size();
    
    for(set<const EState*>::iterator i=_newNodesToFold.begin();i!=_newNodesToFold.end();++i) {
      bool res=estateSet.erase(const_cast<EState*>(*i));
      if(res==false) {
        cerr<< "Error: Semantic folding of transition graph: new estate could not be deleted."<<endl;
        //cerr<< (**i).toString()<<endl;
        exit(1);
      }
    }
    if(boolOptions["report-semantic-fold"]) {
      cout << "STATUS: semantic folding: phase 4: clearing "<<_newNodesToFold.size()<< " states (excluding WL-filtered: "<<numFiltered<<")"<<endl;
    }
    _newNodesToFold.clear();
    //assert(checkEStateSet());
    //assert(checkTransitionGraph());
    if(boolOptions["report-semantic-fold"] && tg_size_before_folding!=tg_size_after_folding)
      cout << "STATUS: semantic folding: finished: Folded transition graph from "<<tg_size_before_folding<<" to "<<tg_size_after_folding<<" transitions."<<endl;
    
  } // end of omp pragma
}

int Analyzer::semanticExplosionOfInputNodesFromOutputNodeConstraints() {
  set<const EState*> toExplode;
  // collect input states
  for(EStateSet::const_iterator i=estateSet.begin();
      i!=estateSet.end();
      ++i) {
    // we require that the unique value is Top; otherwise we use the existing one and do not back-propagate
    if((*i)->io.isStdInIO() && (*i)->determineUniqueIOValue().isTop()) {
      toExplode.insert(*i);
    }
  }
  // explode input states
  for(set<const EState*>::const_iterator i=toExplode.begin();
      i!=toExplode.end();
      ++i) {
    EStatePtrSet predNodes=transitionGraph.pred(*i);
    EStatePtrSet succNodes=transitionGraph.succ(*i);
    Label originalLabel=(*i)->label();
    InputOutput originalIO=(*i)->io;
    VariableId originalVar=originalIO.var;
    // eliminate original input node
    transitionGraph.eliminateEState(*i);
    estateSet.erase(const_cast<EState*>(*i));
    // create new edges to and from new input state
    for(EStatePtrSet::iterator k=succNodes.begin();k!=succNodes.end();++k) {
      // create new input state      
      EState newState=**k; // copy all information from following output state
      newState.setLabel(originalLabel); // overwrite label
      // convert IO information
      newState.io.op=InputOutput::STDIN_VAR;
      newState.io.var=originalVar;
      // register new EState now
      const EState* newEStatePtr=processNewOrExisting(newState);

      // create new edge from new input state to output state
      // since each outgoing edge produces a new input state (if constraint is different)
      // I create a set of ingoing edges for each outgoing edge (= new inputstate)
      Edge outEdge(originalLabel,EDGE_PATH,(*k)->label());
      recordTransition(newEStatePtr,outEdge,(*k)); // new outgoing edge
      for(EStatePtrSet::iterator j=predNodes.begin();j!=predNodes.end();++j) {
    //create edge: predecessor->newInputNode
    Edge inEdge((*j)->label(),EDGE_PATH,originalLabel);
    recordTransition((*j),inEdge,newEStatePtr); // new ingoing edge
      }
    }
  }
  return 0;
}

void Analyzer::pruneLeavesRec() {
  EStatePtrSet states=transitionGraph.estateSet();
  std::set<EState*> workset;
  //insert all states into the workset
  for(EStatePtrSet::iterator i=states.begin();i!=states.end();++i) {
    workset.insert(const_cast<EState*> (*i));
  }
  //process the workset. if state extracted is a leaf, remove it and add its predecessors to the workset
  while (workset.size() != 0) {
    EState* current = (*workset.begin());
    if (/*transitionGraph.succ(current) == NULL 
          || */ transitionGraph.succ(current).size() == 0) {
      EStatePtrSet preds = transitionGraph.pred(current);
      for (EStatePtrSet::iterator iter = preds.begin(); iter != preds.end(); ++iter)  {
        workset.insert(const_cast<EState*> (*iter));
      }
      transitionGraph.reduceEState2(current);
    }
    workset.erase(current);
  }
}

bool Analyzer::indegreeTimesOutdegreeLessThan(const EState* a, const EState* b) {
  return ( (transitionGraph.inEdges(a).size() * transitionGraph.outEdges(a).size()) <
             (transitionGraph.inEdges(b).size() * transitionGraph.outEdges(b).size()) );
}

void Analyzer::removeNonIOStates() {
  EStatePtrSet states=transitionGraph.estateSet();
  if (states.size() == 0) {
    cout << "STATUS: the transition system used as a model is empty, could not reduce states to I/O." << endl;
    return;
  }
  // sort EStates so that those with minimal (indegree * outdegree) come first
  std::list<const EState*>* worklist = new list<const EState*>(states.begin(), states.end());
  worklist->sort(boost::bind(&CodeThorn::Analyzer::indegreeTimesOutdegreeLessThan,this,_1,_2));
  int totalStates=states.size();
  int statesVisited =0;
  // iterate over all states, reduce those that are neither the start state nor standard input / output
  for(std::list<const EState*>::iterator i=worklist->begin();i!=worklist->end();++i) {
    if(! ((*i) == transitionGraph.getStartEState()) ) {
      if(! ((*i)->io.isStdInIO() || (*i)->io.isStdOutIO()) ) {
	transitionGraph.reduceEState2(*i);
      }
    }
    statesVisited++;
    //display current progress
    if (statesVisited % 2500 == 0) {
      double progress = ((double) statesVisited / (double) totalStates) * 100;
      cout << setiosflags(ios_base::fixed) << setiosflags(ios_base::showpoint);
      cout << "STATUS: "<<statesVisited<<" out of "<<totalStates<<" states visited for reduction to I/O. ("<<setprecision(2)<<progress<<setprecision(6)<<"%)"<<endl;
      cout << resetiosflags(ios_base::fixed) << resetiosflags(ios_base::showpoint);
    }
  }
}

void Analyzer::reduceGraphInOutWorklistOnly(bool includeIn, bool includeOut, bool includeErr) {
  // 1.) worklist_reduce <- list of startState and all input/output states
  std::list<const EState*> worklist_reduce;
  EStatePtrSet states=transitionGraph.estateSet();
  if (states.size() == 0) {
    cout << "STATUS: the transition system used as a model is empty, could not reduce states to I/O." << endl;
    return;
  }
  worklist_reduce.push_back(transitionGraph.getStartEState());
  for(EStatePtrSet::iterator i=states.begin();i!=states.end();++i) {
    if ( (includeIn&&(*i)->io.isStdInIO()) || (includeOut&&(*i)->io.isStdOutIO()) || (includeErr&&(*i)->io.isFailedAssertIO())) {
      worklist_reduce.push_back(*i);
    }
  }
  // 2.) for each state in worklist_reduce: insert startState/I/O/worklist transitions into list of new transitions ("newTransitions").
  std::list<Transition*> newTransitions;
  for(std::list<const EState*>::iterator i=worklist_reduce.begin();i!=worklist_reduce.end();++i) {
    boost::unordered_set<Transition*>* someNewTransitions = transitionsToInOutErrAndWorklist(*i, includeIn, includeOut, includeErr);
    newTransitions.insert(newTransitions.begin(), someNewTransitions->begin(), someNewTransitions->end());
  }
  // 3.) remove all old transitions
  for(EStatePtrSet::iterator i=states.begin();i!=states.end();++i) {
    transitionGraph.eliminateEState(*i);
  }
  // 4.) add newTransitions
  //cout << "DEBUG: number of new shortcut transitions to be added: " << newTransitions.size() << endl;
  for(std::list<Transition*>::iterator i=newTransitions.begin();i!=newTransitions.end();++i) {
    transitionGraph.add(**i);
    //cout << "DEBUG: added " << (*i)->toString() << endl;
  }
}

boost::unordered_set<Transition*>* Analyzer::transitionsToInOutErrAndWorklist( const EState* startState,
									    bool includeIn, bool includeOut, bool includeErr) {
  // initialize result set and visited set (the latter for cycle checks)
  boost::unordered_set<Transition*>* result = new boost::unordered_set<Transition*>();
  boost::unordered_set<const EState*>* visited = new boost::unordered_set<const EState*>();
  // start the recursive function from initState's successors, therefore with a minimum distance of one from initState. 
  // Simplifies the detection of allowed self-edges.
  EStatePtrSet succOfInitState = transitionGraph.succ(startState);
  if (startState->io.isFailedAssertIO()) {
    assert (succOfInitState.size() == 0);
  }
  for(EStatePtrSet::iterator i=succOfInitState.begin();i!=succOfInitState.end();++i) {
    boost::unordered_set<Transition*>* tempResult = new boost::unordered_set<Transition*>();
    tempResult = transitionsToInOutErrAndWorklist((*i), startState, result, visited, includeIn, includeOut, includeErr);
    result->insert(tempResult->begin(), tempResult->end());
    tempResult = NULL;
  }
  delete visited;
  visited = NULL;
  return result;
}
                                                            

boost::unordered_set<Transition*>* Analyzer::transitionsToInOutErrAndWorklist( const EState* currentState, const EState* startState,
                                                                            boost::unordered_set<Transition*>* results, boost::unordered_set<const EState*>* visited,
									    bool includeIn, bool includeOut, bool includeErr) {
  //cycle check
  if (visited->count(currentState)) {
    return results;  //currentState already visited, do nothing
  } 
  visited->insert(currentState);
  //base case
  // add a new transition depending on what type of states are to be included in the reduced STG. Looks at input, 
  // output (as long as the start state is not output), failed assertion and worklist states.
  if( (includeIn && currentState->io.isStdInIO()) 
      || (!startState->io.isStdOutIO() &&  includeIn && currentState->io.isStdOutIO())  
      || (includeErr && currentState->io.isFailedAssertIO()) 
      // currently disabled 
      //|| (transitionGraph.succ(currentState).size() == 0 && !currentState->io.isFailedAssertIO()) //defines a worklist state
  ) { 
    Edge* newEdge = new Edge(startState->label(),EDGE_PATH,currentState->label());
    Transition* newTransition = new Transition(startState, *newEdge, currentState);
    results->insert(newTransition);
  } else {
    //recursively collect the resulting transitions
    EStatePtrSet nextStates = transitionGraph.succ(currentState);
    for(EStatePtrSet::iterator i=nextStates.begin();i!=nextStates.end();++i) {
      transitionsToInOutErrAndWorklist((*i), startState, results, visited, includeIn, includeOut, includeErr);
    }
  }
  return results;
}

void Analyzer::generateSpotTransition(stringstream& ss, const Transition& t) {
  ss<<"S"<<estateSet.estateIdString(t.source);
  ss<<",";
  ss<<"S"<<estateSet.estateIdString(t.target);
  const EState* myTarget=t.target;
  AType::ConstIntLattice myIOVal=myTarget->determineUniqueIOValue();
  ss<<",\""; // dquote reqired for condition
  // generate transition condition
  if(myTarget->io.isStdInIO()||myTarget->io.isStdOutIO()) {
    if(!myIOVal.isConstInt()) {
      //assert(myIOVal.isConstInt());
      cerr<<"Error: IOVal is NOT const.\n"<<"EState: "<<myTarget->toString()<<endl;
      exit(1);
    }
  }
  // myIOVal.isTop(): this only means that any value *may* be read/written. This cannot be modeled here.
  // if it represents "any of A..F" or any of "U..Z" it could be handled.
  for(int i=1;i<=6;i++) {
    if(i!=1)
      ss<<" & ";
    if(myTarget->io.isStdInIO() && myIOVal.isConstInt() && myIOVal.getIntValue()==i) {
      ss<<"  ";
    } else {
      ss<<"! ";
    }
    ss<<"i"<<(char)(i+'A'-1);
  }
  for(int i=21;i<=26;i++) {
    ss<<" & ";
    if(myTarget->io.isStdOutIO() && myIOVal.isConstInt() && myIOVal.getIntValue()==i) {
      ss<<"  ";
      } else {
      ss<<"! ";
    }
    ss<<"o"<<(char)(i+'A'-1);
  }
  ss<<"\""; // dquote reqired for condition
  ss<<",;"; // no accepting states specified
  ss<<endl;
}

string Analyzer::generateSpotSTG() {
  stringstream ss;
  // (1) generate accepting states
#if 0
  EStatePtrSet states=transitionGraph.estateSet();
  cout<<"Generating accepting states."<<endl;
  ss<<"acc=";
  for(EStatePtrSet::iterator i=states.begin();i!=states.end();++i) {
    if(!((*i)->io.isStdErrIO()||(*i)->io.isFailedAssertIO())) {
      ss<<"S"<<estateSet.estateIdString(*i)<<" ";
    }
  }
  ss<<";"<<endl;
#else
  cout<<"All states are accepting."<<endl;
#endif
  // (2) generate state transition graph
  // the start state is identified by the first transition. Therefore I generate all transitions of the
  // start state first, and exclude them from all the others.
  const EState* startState=transitionGraph.getStartEState();
  TransitionPtrSet startTransitions=transitionGraph.outEdges(startState);
  for(TransitionPtrSet::iterator i=startTransitions.begin();i!=startTransitions.end();++i) {
    generateSpotTransition(ss,**i);
  }
  int num=0;
  for(TransitionGraph::iterator i=transitionGraph.begin();i!=transitionGraph.end();++i) {
    if((*i)->source!=startState)
      generateSpotTransition(ss,**i);
    if(num%1000==0 && num>0)
      cout<<"Generated "<<num<<" of "<<transitionGraph.size()<<" transitions."<<endl;
    num++;
  }
  cout<<"SPOT STG: start state: "<<"S"<<estateSet.estateIdString(startState)<<endl;
  return ss.str();
}

int Analyzer::semanticEliminationOfSelfInInTransitions() {
  //cout<<"STATUS: eliminating In-In-Self Transitions."<<endl;
  set<const Transition*> transitionsToEliminate;
  int eliminated;
  transitionsToEliminate.clear();
  eliminated=0;
  for(TransitionGraph::iterator i=transitionGraph.begin();
      i!=transitionGraph.end();
      ++i) {
    const Transition* t=*i;
    if((t->source==t->target)
       &&
       (getLabeler()->isStdInLabel(t->source->label()))
       &&
       (getLabeler()->isStdInLabel(t->target->label())) ) {
      // found in-in edge
      transitionsToEliminate.insert(t);
    }
  }
  for(set<const Transition*>::iterator i=transitionsToEliminate.begin();
      i!=transitionsToEliminate.end();
      ++i) {
    transitionGraph.erase(**i);
    eliminated++;
  }
  //cout<<"STATUS: eliminated "<<eliminated<<" In-In-Self Transitions."<<endl;
  return eliminated;
}

int Analyzer::semanticEliminationOfDeadStates() {
  set<const EState*> toEliminate;
  for(EStateSet::const_iterator i=estateSet.begin();
      i!=estateSet.end();
      ++i) {
    if(transitionGraph.outEdges(*i).size()==0 && (*i)->io.isStdInIO()) {
      toEliminate.insert(*i);
    }
  }
  for(set<const EState*>::const_iterator i=toEliminate.begin();
    i!=toEliminate.end();
      ++i) {
    // eliminate node in estateSet (only because LTL is using it)
    transitionGraph.eliminateEState(*i);
    estateSet.erase(const_cast<EState*>(*i));
  }
  return toEliminate.size();
}
int Analyzer::semanticFusionOfInInTransitions() {
  set<const EState*> toReduce;
  for(TransitionGraph::iterator i=transitionGraph.begin();
      i!=transitionGraph.end();
      ++i) {
    if(((*i)->source->io.isStdInIO())
       &&
       ((*i)->target->io.isStdInIO())
       &&
       ((*i)->source!=(*i)->target)
       ) {
      // found in-in edge; fuse source and target state into target state (VERY different to reduction!)
      // 1) all in edges of source become in-edges of target
      // 2) all out edges of source become out-edges of target
      // 3) eliminate source
      set<Transition> newTransitions;
      const EState* remapped=(*i)->target;
      TransitionPtrSet in=transitionGraph.inEdges((*i)->source);
      for(TransitionPtrSet::iterator j=in.begin();j!=in.end();++j) {
        newTransitions.insert(Transition((*j)->source,
                                         Edge((*j)->source->label(),EDGE_PATH,remapped->label()),
                                         remapped));
      }
      TransitionPtrSet out=transitionGraph.outEdges((*i)->source);
      for(TransitionPtrSet::iterator j=out.begin();j!=out.end();++j) {
        newTransitions.insert(Transition(remapped,
                                         Edge(remapped->label(),EDGE_PATH,(*j)->target->label()),
                                         (*j)->target));
      }
      for(set<Transition>::iterator k=newTransitions.begin();k!=newTransitions.end();++k) {
        transitionGraph.add(*k);
        //assert(find(*k)!=end());
      }
      transitionGraph.eliminateEState((*i)->source);
      return 1;
    }
  }
  //cout<<"STATUS: Eliminated "<<elim<<" in-in transitions."<<endl;
  //return toReduce.size();
  return 0;
}

void Analyzer::semanticEliminationOfTransitions() {
  cout << "STATUS: (Experimental) semantic elimination of transitions ... ";
  int elim;
  do {
    elim=0;
    assert(transitionGraph.checkConsistency());
    elim+=semanticEliminationOfSelfInInTransitions();
    elim+=semanticEliminationOfDeadStates();
    // this function does not work in general yet
    // probably because of self-edges
    elim+=semanticFusionOfInInTransitions();
    assert(transitionGraph.checkConsistency());
  } while (elim>0);
  cout << "done."<<endl;
  return;
}

void Analyzer::runSolver2() {
  size_t prevStateSetSize=_displayDiff+1; // force immediate report at start
#ifdef _OPENMP
  omp_set_num_threads(_numberOfThreadsToUse);
  omp_set_dynamic(1);
#endif
  int threadNum;
  vector<const EState*> workVector(_numberOfThreadsToUse);
  int analyzedSemanticFoldingNode=0;
  while(1) {
    int workers;
    for(workers=0;workers<_numberOfThreadsToUse;++workers) {
      if(!(workVector[workers]=popWorkList()))
        break;
    }
    if(workers==0)
      break; // we are done

#pragma omp parallel for private(threadNum) shared(workVector,analyzedSemanticFoldingNode)
    for(int j=0;j<workers;++j) {
#ifdef _OPENMP
      threadNum=omp_get_thread_num();
#endif
      const EState* currentEStatePtr=workVector[j];
      if(!currentEStatePtr) {
        cerr<<"Error: thread "<<threadNum<<" finished prematurely. Bailing out. "<<endl;
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
        exit(1);
      }
      assert(currentEStatePtr);
      
      Flow edgeSet=flow.outEdges(currentEStatePtr->label());
#ifdef PARALLELIZE_BRANCHES
      // we can simplify this by adding the proper function to Flow.
      Flow::iterator i=edgeSet.begin();
      int edgeNum=edgeSet.size();
      vector<const Edge*> edgeVec(edgeNum);
      for(int edge_i=0;edge_i<edgeNum;++edge_i) {
        edgeVec[edge_i]=&(*i++);
      }
#pragma omp parallel for
      for(int edge_i=0; edge_i<edgeNum;edge_i++) {
        Edge e=*edgeVec[edge_i]; // *i
#else
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
#endif
          list<EState> newEStateList;
          newEStateList=transferFunction(e,currentEStatePtr);
          if(isTerminationRelevantLabel(e.source)) {
            #pragma omp atomic
            analyzedSemanticFoldingNode++;
          }
          for(list<EState>::iterator nesListIter=newEStateList.begin();
              nesListIter!=newEStateList.end();
              ++nesListIter) {
            EState newEState=*nesListIter;
            if(newEState.label()!=Labeler::NO_LABEL && (!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState))) {
              HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=process(newEState);
              const EState* newEStatePtr=pres.second;
              if(pres.first==true)
                addToWorkList(newEStatePtr);
              recordTransition(currentEStatePtr,e,newEStatePtr);
            }
            if(newEState.label()!=Labeler::NO_LABEL && (!newEState.constraints()->disequalityExists()) && (isFailedAssertEState(&newEState))) {
              // failed-assert end-state: do not add to work list but do add it to the transition graph
              const EState* newEStatePtr;
              newEStatePtr=processNewOrExisting(newEState);
              recordTransition(currentEStatePtr,e,newEStatePtr);        
              
              if(boolOptions["report-failed-assert"]) {
#pragma omp critical(OUTPUT)
                {
                  cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
                }
              }
              if(boolOptions["abstract-interpreter"]) {
                cerr<<"CodeThorn-abstract-interpreter> failed assert";
                string name=labelNameOfAssertLabel(currentEStatePtr->label());
                if(name!="") { cout << " @ Label: "<<name;}
                cout <<endl;
              }
              if(_csv_assert_live_file.size()>0) {
                string name=labelNameOfAssertLabel(currentEStatePtr->label());
                if(name=="globalError")
                  name="error_60";
                name=name.substr(6,name.size()-6);
                std::ofstream fout;
                // csv_assert_live_file is the member-variable of analyzer
#pragma omp critical(OUTPUT)
                {
                  fout.open(_csv_assert_live_file.c_str(),ios::app);    // open file for appending
                  assert (!fout.fail( ));
                  fout << name << ",yes,9"<<endl;
                  //cout << "REACHABLE ASSERT FOUND: "<< name << ",yes,9"<<endl;
                  
                  fout.close(); 
                }
              } // if
            }
            if(newEState.label()==Labeler::NO_LABEL) {
              //cerr << "INFO: found final state."<<endl;
            }
          } // end of loop on transfer function return-estates
#ifdef PARALLELIZE_BRANCHES
        } // edgeset-parallel for
#else
      } // just for proper auto-formatting in emacs
#endif
    } // worklist-parallel for
    if(boolOptions["semantic-fold"]) {
      if(analyzedSemanticFoldingNode>_semanticFoldThreshold) {
        semanticFoldingOfTransitionGraph();
        analyzedSemanticFoldingNode=0;
        prevStateSetSize=estateSet.size();
      }
    }
    if(_displayDiff && (estateSet.size()>(prevStateSetSize+_displayDiff))) {
      printStatusMessage(true);
      prevStateSetSize=estateSet.size();
    }
  } // while
  if(boolOptions["semantic-fold"]) {
    semanticFoldingOfTransitionGraph(); // final fold
  }
  printStatusMessage(true);
  cout << "analysis finished (worklist is empty)."<<endl;
  assert(checkTransitionGraph());
}

void Analyzer::runSolver3() {
  flow.boostify();
  reachabilityResults.init(getNumberOfErrorLabels()); // set all reachability results to unknown
  size_t prevStateSetSize=0; // force immediate report at start
  int threadNum;
  vector<const EState*> workVector(_numberOfThreadsToUse);
  int workers=_numberOfThreadsToUse;
  omp_set_dynamic(0);     // Explicitly disable dynamic teams
  omp_set_num_threads(workers);
  cout <<"STATUS: Running parallel solver 3 with "<<workers<<" threads."<<endl;
  printStatusMessage(true);
  while(1) {
    if(_displayDiff && (estateSet.size()>(prevStateSetSize+_displayDiff))) {
      printStatusMessage(true);
      prevStateSetSize=estateSet.size();
    }
    if(isEmptyWorkList())
      break;
#pragma omp parallel for private(threadNum)
    for(int j=0;j<workers;++j) {
      threadNum=omp_get_thread_num();
      const EState* currentEStatePtr=popWorkList();
      if(!currentEStatePtr) {
        //cerr<<"Thread "<<threadNum<<" found empty worklist. Continue without work. "<<endl;
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
      } else {
        assert(currentEStatePtr);
      
        Flow edgeSet=flow.outEdges(currentEStatePtr->label());
        //cerr << "DEBUG: out-edgeSet size:"<<edgeSet.size()<<endl;
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
          list<EState> newEStateList;
          //#pragma omp critical(TRANSFER) 
          newEStateList=transferFunction(e,currentEStatePtr);

          //cout << "DEBUG: transfer at edge:"<<e.toString()<<" succ="<<newEStateList.size()<< endl;
          for(list<EState>::iterator nesListIter=newEStateList.begin();
              nesListIter!=newEStateList.end();
              ++nesListIter) {
            // newEstate is passed by value (not created yet)
            EState newEState=*nesListIter;
            assert(newEState.label()!=Labeler::NO_LABEL);
            if((!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState))) {
              HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=process(newEState);
              const EState* newEStatePtr=pres.second;
              if(pres.first==true)
                addToWorkList(newEStatePtr);            
              recordTransition(currentEStatePtr,e,newEStatePtr);
            }
            if((!newEState.constraints()->disequalityExists()) && (isFailedAssertEState(&newEState))) {
              // failed-assert end-state: do not add to work list but do add it to the transition graph
              const EState* newEStatePtr;
              newEStatePtr=processNewOrExisting(newEState);
              recordTransition(currentEStatePtr,e,newEStatePtr);        
              
              if(boolOptions["report-failed-assert"]) {
#pragma omp critical(OUTPUT)
                {
                  cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
                }
              }
              
              // record reachability
              int assertCode=reachabilityAssertCode(currentEStatePtr);
              if(assertCode>=0) {
#pragma omp critical(REACHABILITY)
                {
                  reachabilityResults.reachable(assertCode);
                }
              } else {
                // TODO: this is a workaround for isFailedAssert being true in case of rersmode for stderr (needs to be refined)
                if(!boolOptions["rersmode"]) {
                  // assert without label
                }
              }
              
              if(_csv_assert_live_file.size()>0) {
                string name=labelNameOfAssertLabel(currentEStatePtr->label());
                if(name=="globalError")
                  name="error_60";
                name=name.substr(6,name.size()-6);
                std::ofstream fout;
                // csv_assert_live_file is the member-variable of analyzer
#pragma omp critical(OUTPUT)
                {
                  fout.open(_csv_assert_live_file.c_str(),ios::app);    // open file for appending
                  assert (!fout.fail( ));
                  fout << name << ",yes,9"<<endl;
                  //cout << "REACHABLE ASSERT FOUND: "<< name << ",yes,9"<<endl;
                  
                  fout.close(); 
                }
              } // if
            }
          } // end of loop on transfer function return-estates
        } // just for proper auto-formatting in emacs
      } // conditional: test if work is available
    } // worklist-parallel for
    if(isIncompleteSTGReady()) {
      transitionGraph.setIsComplete(false);
      transitionGraph.setIsPrecise(isActiveGlobalTopify());
      // we report some information and finish the algorithm with an incomplete STG
      cout << "-------------------------------------------------"<<endl;
      cout << "STATUS: finished with incomplete STG (as planned)"<<endl;
      cout << "-------------------------------------------------"<<endl;
      return;
    }
  } // while
  reachabilityResults.finished(); // sets all unknown entries to NO.
  transitionGraph.setIsComplete(true);
  transitionGraph.setIsPrecise(isActiveGlobalTopify());
  printStatusMessage(true);
  cout << "analysis finished (worklist is empty)."<<endl;
}

int Analyzer::reachabilityAssertCode(const EState* currentEStatePtr) {
#ifdef RERS_SPECIALIZATION
  if(boolOptions["rers-binary"]) {
    PState* pstate = const_cast<PState*>( (currentEStatePtr)->pstate() ); 
    int outputVal = (*pstate)[globalVarIdByName("output")].getValue().getIntValue();
    if (outputVal > -100) {  //either not a failing assertion or a stderr output treated as a failing assertion)
      return -1;
    }
    int assertCode = ((outputVal+100)*(-1));
    assert(assertCode>=0 && assertCode <=99); 
    return assertCode;
  }
#endif
  string name=labelNameOfAssertLabel(currentEStatePtr->label());
  if(name.size()==0)
    return -1;
  if(name=="globalError")
    name="error_60";
  name=name.substr(6,name.size()-6);
  std::istringstream ss(name);
  int num;
  ss>>num;
  return num;
}


// algorithm 4 also records reachability for incomplete STGs (analyzer::reachabilityResults)
void Analyzer::runSolver4() {
  //flow.boostify();
  reachabilityResults.init(getNumberOfErrorLabels()); // set all reachability results to unknown
  size_t prevStateSetSize=0; // force immediate report at start
  int analyzedSemanticFoldingNode=0;
  int threadNum;
  vector<const EState*> workVector(_numberOfThreadsToUse);
  int workers=_numberOfThreadsToUse;
#ifdef _OPENMP
  omp_set_dynamic(0);     // Explicitly disable dynamic teams
  omp_set_num_threads(workers);
#endif
  cout <<"STATUS: Running parallel solver 4 with "<<workers<<" threads."<<endl;
  printStatusMessage(true);
  while(1) {
    if(_displayDiff && (estateSet.size()>(prevStateSetSize+_displayDiff))) {
      printStatusMessage(true);
      prevStateSetSize=estateSet.size();
    }
    if(isEmptyWorkList())
      break;
#pragma omp parallel for private(threadNum)
    for(int j=0;j<workers;++j) {
#ifdef _OPENMP
      threadNum=omp_get_thread_num();
#endif
      const EState* currentEStatePtr=popWorkList();
      if(!currentEStatePtr) {
        //cerr<<"Thread "<<threadNum<<" found empty worklist. Continue without work. "<<endl;
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
      } else {
        assert(currentEStatePtr);
      
        Flow edgeSet=flow.outEdges(currentEStatePtr->label());
        //cerr << "DEBUG: edgeSet size:"<<edgeSet.size()<<endl;
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
          list<EState> newEStateList;
          newEStateList=transferFunction(e,currentEStatePtr);
          if(isTerminationRelevantLabel(e.source)) {
            #pragma omp atomic
            analyzedSemanticFoldingNode++;
          }

          //cout << "DEBUG: transfer at edge:"<<e.toString()<<" succ="<<newEStateList.size()<< endl;
          for(list<EState>::iterator nesListIter=newEStateList.begin();
              nesListIter!=newEStateList.end();
              ++nesListIter) {
            // newEstate is passed by value (not created yet)
            EState newEState=*nesListIter;
            assert(newEState.label()!=Labeler::NO_LABEL);
            if((!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState))&&(!isVerificationErrorEState(&newEState))) {
              HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=process(newEState);
              const EState* newEStatePtr=pres.second;
              if(pres.first==true)
                addToWorkList(newEStatePtr);
              recordTransition(currentEStatePtr,e,newEStatePtr);
            }
	    if((!newEState.constraints()->disequalityExists()) && isVerificationErrorEState(&newEState)) {
	      // verification error is handled as property 0
              const EState* newEStatePtr;
#pragma omp critical(REACHABILITY)
	      {
		newEStatePtr=processNewOrExisting(newEState);
		recordTransition(currentEStatePtr,e,newEStatePtr);        
		reachabilityResults.reachable(0);
	      }
	    } else if((!newEState.constraints()->disequalityExists()) && isFailedAssertEState(&newEState)) {
              // failed-assert end-state: do not add to work list but do add it to the transition graph
              const EState* newEStatePtr;
#pragma omp critical(REACHABILITY)
                {
		  newEStatePtr=processNewOrExisting(newEState);
              recordTransition(currentEStatePtr,e,newEStatePtr);        
		}              
              // record reachability
              int assertCode=reachabilityAssertCode(currentEStatePtr);
              if(assertCode>=0) {
#pragma omp critical(REACHABILITY)
                {
                  reachabilityResults.reachable(assertCode);
                }
	      } else {
                // assert without label
	      }
              
              if(boolOptions["report-failed-assert"]) {
#pragma omp critical(OUTPUT)
                {
                  cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
                }
              }
              if(_csv_assert_live_file.size()>0) {
                string name=labelNameOfAssertLabel(currentEStatePtr->label());
                if(name.size()>0) {
                  if(name=="globalError")
                    name="error_60";
                  name=name.substr(6,name.size()-6);
                  std::ofstream fout;
                  // csv_assert_live_file is the member-variable of analyzer
#pragma omp critical(OUTPUT)
                  {
                    fout.open(_csv_assert_live_file.c_str(),ios::app);    // open file for appending
                    assert (!fout.fail( ));
                    fout << name << ",yes,9"<<endl;
                    //cout << "REACHABLE ASSERT FOUND: "<< name << ",yes,9"<<endl;
                    
                    fout.close(); 
                  }
                }// if label of assert was found (name.size()>0)
              } // if
            }
          } // end of loop on transfer function return-estates
        } // just for proper auto-formatting in emacs
      } // conditional: test if work is available
    } // worklist-parallel for
    if(boolOptions["semantic-fold"]) {
      if(analyzedSemanticFoldingNode>_semanticFoldThreshold) {
        semanticFoldingOfTransitionGraph();
        analyzedSemanticFoldingNode=0;
        prevStateSetSize=estateSet.size();
      }
    }
    if(_displayDiff && (estateSet.size()>(prevStateSetSize+_displayDiff))) {
      printStatusMessage(true);
      prevStateSetSize=estateSet.size();
    }
    if(isIncompleteSTGReady()) {
      // ensure that the STG is folded properly when finished
      if(boolOptions["semantic-fold"]) {
        semanticFoldingOfTransitionGraph();
      }  
      // we report some information and finish the algorithm with an incomplete STG
      cout << "-------------------------------------------------"<<endl;
      cout << "STATUS: finished with incomplete STG (as planned)"<<endl;
      cout << "-------------------------------------------------"<<endl;
      return;
    }
  } // while
  // ensure that the STG is folded properly when finished
  if(boolOptions["semantic-fold"]) {
    semanticFoldingOfTransitionGraph();
  }  
  reachabilityResults.finished(); // sets all unknown entries to NO.
  printStatusMessage(true);
  cout << "analysis finished (worklist is empty)."<<endl;
}

void set_finished(vector<bool>& v, bool val) {
  ROSE_ASSERT(v.size()>0);
  for(vector<bool>::iterator i=v.begin();i!=v.end();++i) {
    *i=val;
  }
}

bool all_false(vector<bool>& v) {
  ROSE_ASSERT(v.size()>0);
  bool res=false;
#pragma omp critical
  {
  for(vector<bool>::iterator i=v.begin();i!=v.end();++i) {
    res=res||(*i);
  }
  }
  return !res;
}

void Analyzer::runSolver5() {
  //flow.boostify();
  if(usingExternalFunctionSemantics()) {
    reachabilityResults.init(1); // in case of svcomp mode set single program property to unknown
  } else {
    reachabilityResults.init(getNumberOfErrorLabels()); // set all reachability results to unknown
  }
  cout<<"INFO: number of error labels: "<<reachabilityResults.size()<<endl;
  size_t prevStateSetSize=0; // force immediate report at start
  int threadNum;
  int workers=_numberOfThreadsToUse;
  vector<bool> workVector(_numberOfThreadsToUse);
  set_finished(workVector,true);
  bool terminateEarly=false;
  //omp_set_dynamic(0);     // Explicitly disable dynamic teams
  omp_set_num_threads(workers);

  bool ioReductionActive = false;
  unsigned int ioReductionThreshold = 0;
  unsigned int estatesLastReduction = 0;
  if(args.count("io-reduction")) {
    ioReductionActive = true;
    ioReductionThreshold = args["io-reduction"].as<int>(); 
  }

#ifdef RERS_SPECIALIZATION  
  //initialize the global variable arrays in the linked binary version of the RERS problem
  if(boolOptions["rers-binary"]) {
    cout << "DEBUG: init of globals with arrays for "<< workers << " threads. " << endl;
    RERS_Problem::rersGlobalVarsArrayInit(workers);
    RERS_Problem::createGlobalVarAddressMaps(this);
  }
#endif
  cout <<"STATUS: Running parallel solver 5 with "<<workers<<" threads."<<endl;
  printStatusMessage(true);
# pragma omp parallel shared(workVector) private(threadNum)
  {
    threadNum=omp_get_thread_num();
    while(!all_false(workVector)) {
      //cout<<"DEBUG: running : WL:"<<estateWorkList.size()<<endl;
      if(threadNum==0 && _displayDiff && (estateSet.size()>(prevStateSetSize+_displayDiff))) {
        printStatusMessage(true);
        prevStateSetSize=estateSet.size();
      }
      //perform reduction to I/O/worklist states only if specified threshold was reached
      if (ioReductionActive) {
#pragma omp critical
        {
          if (estateSet.size() > (estatesLastReduction + ioReductionThreshold)) {
            //int beforeReduction = estateSet.size();
            reduceGraphInOutWorklistOnly();
            estatesLastReduction = estateSet.size();
            cout << "STATUS: transition system reduced to I/O/worklist states. remaining transitions: " << transitionGraph.size() << endl;
          }
        }
      }
      if(isEmptyWorkList()||isIncompleteSTGReady()) {
#pragma omp critical
        {
          workVector[threadNum]=false;
        }
        continue;
      } else {
#pragma omp critical
        {
          if(terminateEarly)
            workVector[threadNum]=false;
          else
            workVector[threadNum]=true;
        }
      }
      const EState* currentEStatePtr=popWorkList();
      // if we want to terminate early, we ensure to stop all threads and empty the worklist (e.g. verification error found).
      if(terminateEarly)
        continue;
      if(!currentEStatePtr) {
        //cerr<<"Thread "<<threadNum<<" found empty worklist. Continue without work. "<<endl;
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
      } else {
        assert(currentEStatePtr);
        if(variableValueMonitor.isActive()) {
          variableValueMonitor.update(this,const_cast<EState*>(currentEStatePtr));
        }
        
        Flow edgeSet=flow.outEdges(currentEStatePtr->label());
        //cerr << "DEBUG: out-edgeSet size:"<<edgeSet.size()<<endl;
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
          list<EState> newEStateList;
          newEStateList=transferFunction(e,currentEStatePtr);
          for(list<EState>::iterator nesListIter=newEStateList.begin();
              nesListIter!=newEStateList.end();
              ++nesListIter) {
            // newEstate is passed by value (not created yet)
            EState newEState=*nesListIter;
            ROSE_ASSERT(newEState.label()!=Labeler::NO_LABEL);
            if(_stg_trace_filename.size()>0 && !newEState.constraints()->disequalityExists()) {
              std::ofstream fout;
              // _csv_stg_trace_filename is the member-variable of analyzer
#pragma omp critical
              {
                fout.open(_stg_trace_filename.c_str(),ios::app);    // open file for appending
                assert (!fout.fail( ));
                fout<<"PSTATE-IN:"<<currentEStatePtr->pstate()->toString(&variableIdMapping);
                string sourceString=getCFAnalyzer()->getLabeler()->getNode(currentEStatePtr->label())->unparseToString().substr(0,20);
                if(sourceString.size()==20) sourceString+="...";
                fout<<" ==>"<<"TRANSFER:"<<sourceString;
                fout<<"==> "<<"PSTATE-OUT:"<<newEState.pstate()->toString(&variableIdMapping);
                fout<<endl;
                fout.close(); 
                //cout<<"DEBUG:generate STG-edge:"<<"ICFG-EDGE:"<<e.toString()<<endl;
              }
            }

            if((!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState)&&!isVerificationErrorEState(&newEState))) {
              HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=process(newEState);
              const EState* newEStatePtr=pres.second;
              if(pres.first==true)
                addToWorkList(newEStatePtr);
              recordTransition(currentEStatePtr,e,newEStatePtr);
            }
              if((!newEState.constraints()->disequalityExists()) && ((isFailedAssertEState(&newEState))||isVerificationErrorEState(&newEState))) {
                // failed-assert end-state: do not add to work list but do add it to the transition graph
                const EState* newEStatePtr;
                newEStatePtr=processNewOrExisting(newEState);
                recordTransition(currentEStatePtr,e,newEStatePtr);        
              
                if(isVerificationErrorEState(&newEState)) {
#pragma omp critical
                  {
                  cout<<"STATUS: detected verification error state ... terminating early"<<endl;
                  // set flag for terminating early
		  reachabilityResults.reachable(0);
                  terminateEarly=true;
                  }
                } else if(isFailedAssertEState(&newEState)) {
                  if(boolOptions["report-failed-assert"]) {
#pragma omp critical
                    {
                      cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
                    }
                  }
                  // record failed assert
                  int assertCode;
                  if(boolOptions["rers-binary"]) {
                    assertCode=reachabilityAssertCode(newEStatePtr);
                  } else {
                    assertCode=reachabilityAssertCode(currentEStatePtr);
                  }  
                  if(assertCode>=0) {
#pragma omp critical
                    {
                      if(boolOptions["with-counterexamples"] || boolOptions["with-assert-counterexamples"]) { 
                        //if this particular assertion was never reached before, compute and update counterexample
                        if (reachabilityResults.getPropertyValue(assertCode) != PROPERTY_VALUE_YES) {
                          _firstAssertionOccurences.push_back(pair<int, const EState*>(assertCode, newEStatePtr));
                        } 
                      }
                      reachabilityResults.reachable(assertCode);
                    }
                  } else {
                    // TODO: this is a workaround for isFailedAssert being true in case of rersmode for stderr (needs to be refined)
                    if(!boolOptions["rersmode"]) {
                      // assert without label
                    }
                  }
                  if(_csv_assert_live_file.size()>0) {
                    string name=labelNameOfAssertLabel(currentEStatePtr->label());
                    if(name=="globalError")
                      name="error_60";
                    name=name.substr(6,name.size()-6);
                    std::ofstream fout;
                    // csv_assert_live_file is the member-variable of analyzer
#pragma omp critical
                    {
                      fout.open(_csv_assert_live_file.c_str(),ios::app);    // open file for appending
                      assert (!fout.fail( ));
                      fout << name << ",yes,9"<<endl;
                      //cout << "REACHABLE ASSERT FOUND: "<< name << ",yes,9"<<endl;
                      
                      fout.close(); 
                    }
                  }
                } // end of failed assert handling
              } // end of if (no disequality (= no infeasable path))
          } // end of loop on transfer function return-estates
        } // edge set iterator
      } // conditional: test if work is available
    } // while
  } // omp parallel
  const bool isComplete=true;
  if (!isPrecise()) {
    _firstAssertionOccurences = list<FailedAssertion>(); //ignore found assertions if the STG is not precise
  }
  if(isIncompleteSTGReady()) {
    printStatusMessage(true);
    cout << "STATUS: analysis finished (incomplete STG due to specified resource restriction)."<<endl;
    reachabilityResults.finishedReachability(isPrecise(),!isComplete);
    transitionGraph.setIsComplete(!isComplete);
  } else {
    bool complete;
    if(boolOptions["set-stg-incomplete"]) {
      complete=false;
    } else {
      complete=true;
    }
    reachabilityResults.finishedReachability(isPrecise(),complete);
    printStatusMessage(true);
    transitionGraph.setIsComplete(complete);
    cout << "analysis finished (worklist is empty)."<<endl;
  }
  transitionGraph.setIsPrecise(isPrecise());
}

// solver 6 uses a different parallelization scheme (condition is not omp compliant yet)
void Analyzer::runSolver6() {
  flow.boostify();
  reachabilityResults.init(getNumberOfErrorLabels()); // set all reachability results to unknown
  size_t prevStateSetSize=0; // force immediate report at start
  int threadNum;
  int workers=_numberOfThreadsToUse;
  vector<bool> workVector(_numberOfThreadsToUse);
  set_finished(workVector,false);
  //omp_set_dynamic(0);     // Explicitly disable dynamic teams
  omp_set_num_threads(workers);
  cout <<"STATUS: Running parallel solver 6 with "<<workers<<" threads."<<endl;
  printStatusMessage(true);
  int numActiveThreads=0; //_numberOfThreadsToUse;
  int numIter=0;
# pragma omp parallel shared(numActiveThreads,numIter) private(threadNum) 
  {
    threadNum=omp_get_thread_num();
    bool isActive=false; // private
    do {
      //cout << numActiveThreads<< " ";
    numIter++;
    if(numIter>1000000) {
      stringstream ss;
      ss<<"["<<threadNum<<":"<<isActive<<":"<<numActiveThreads<<":W"<<estateWorkList.size()<<"]";
      //cout<<ss.str();
    }
    if(threadNum==0 && _displayDiff && (estateSet.size()>(prevStateSetSize+_displayDiff))) {
      printStatusMessage(true);
      prevStateSetSize=estateSet.size();
    }
    if(isEmptyWorkList()) {
      if(isActive==true) {
        isActive=false;
#pragma omp atomic
        numActiveThreads-=1;
      }
      continue;
    } else {
      if(isActive==false) {
        isActive=true;
#pragma omp atomic
        numActiveThreads+=1;
      }
    }
      const EState* currentEStatePtr=popWorkList();
      if(!currentEStatePtr) {
        //cerr<<"Thread "<<threadNum<<" found empty worklist. Continue without work. "<<endl;
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
      } else {
        assert(currentEStatePtr);
      
        Flow edgeSet=flow.outEdges(currentEStatePtr->label());
        //cerr << "DEBUG: out-edgeSet size:"<<edgeSet.size()<<endl;
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
          list<EState> newEStateList;
          newEStateList=transferFunction(e,currentEStatePtr);
          //cout << "DEBUG: transfer at edge:"<<e.toString()<<" succ="<<newEStateList.size()<< endl;
          for(list<EState>::iterator nesListIter=newEStateList.begin();
              nesListIter!=newEStateList.end();
              ++nesListIter) {
            // newEstate is passed by value (not created yet)
            EState newEState=*nesListIter;
            assert(newEState.label()!=Labeler::NO_LABEL);
            if((!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState))) {
              HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=process(newEState);
              const EState* newEStatePtr=pres.second;
              if(pres.first==true)
                addToWorkList(newEStatePtr);
              recordTransition(currentEStatePtr,e,newEStatePtr);
            }
            if((!newEState.constraints()->disequalityExists()) && (isFailedAssertEState(&newEState))) {
              // failed-assert end-state: do not add to work list but do add it to the transition graph
              const EState* newEStatePtr;
              newEStatePtr=processNewOrExisting(newEState);
              recordTransition(currentEStatePtr,e,newEStatePtr);        
              
              if(boolOptions["report-failed-assert"]) {
#pragma omp critical
                {
                  cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
                }
              }
              
              // record reachability
              int assertCode=reachabilityAssertCode(currentEStatePtr);
              if(assertCode>=0) {
#pragma omp critical
                {
                  reachabilityResults.reachable(assertCode);
                }
              } else {
                // TODO: this is a workaround for isFailedAssert being true in case of rersmode for stderr (needs to be refined)
                if(!boolOptions["rersmode"]) {
                  // assert without label
                }
              }
              
              if(_csv_assert_live_file.size()>0) {
                string name=labelNameOfAssertLabel(currentEStatePtr->label());
                if(name=="globalError")
                  name="error_60";
                name=name.substr(6,name.size()-6);
                std::ofstream fout;
                // csv_assert_live_file is the member-variable of analyzer
#pragma omp critical
                {
                  fout.open(_csv_assert_live_file.c_str(),ios::app);    // open file for appending
                  assert (!fout.fail( ));
                  fout << name << ",yes,9"<<endl;
                  //cout << "REACHABLE ASSERT FOUND: "<< name << ",yes,9"<<endl;
                  
                  fout.close(); 
                }
              } // if
            }
          } // end of loop on transfer function return-estates
        } // just for proper auto-formatting in emacs
      } // conditional: test if work is available
    } while(numActiveThreads>0); // do-while
  } // omp parallel
  reachabilityResults.finished(); // sets all unknown entries to NO.
  printStatusMessage(true);
  cout << "analysis finished (worklist is empty)."<<endl;
}

void Analyzer::runSolver7() {
  flow.boostify();
  reachabilityResults.init(getNumberOfErrorLabels()); // set all reachability results to unknown
  size_t prevStateSetSize=0; // force immediate report at start
  int threadNum;
  int workers=_numberOfThreadsToUse;
  vector<bool> workVector(_numberOfThreadsToUse);
  set_finished(workVector,true);
  //omp_set_dynamic(0);     // Explicitly disable dynamic teams
  omp_set_num_threads(workers);
  cout <<"STATUS: Running parallel solver 7 with "<<workers<<" threads."<<endl;
  printStatusMessage(true);
# pragma omp parallel shared(workVector) private(threadNum)
  {
    threadNum=omp_get_thread_num();
    while(!all_false(workVector)) {
      if(threadNum==0 && _displayDiff && (estateSet.size()>(prevStateSetSize+_displayDiff))) {
        printStatusMessage(true);
        prevStateSetSize=estateSet.size();
      }
      //cerr<<threadNum<<";";
      if(isEmptyWorkList()) {
        //      if(workVector[threadNum]==true) {
#pragma omp critical
        {
          workVector[threadNum]=false;
        }
        //}
        continue;
        //break;
      } else {
        //if(workVector[threadNum]==false) {
#pragma omp critical
        {
          workVector[threadNum]=true;
        }
        //}
      }
      const EState* currentEStatePtr=popWorkList();
      if(!currentEStatePtr) {
        //cerr<<"Thread "<<threadNum<<" found empty worklist. Continue without work. "<<endl;
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
      } else {
        assert(currentEStatePtr);
      
        if(variableValueMonitor.isActive()) {
          variableValueMonitor.update(this,const_cast<EState*>(currentEStatePtr));
        }
        
        Flow edgeSet=flow.outEdges(currentEStatePtr->label());
        //cerr << "DEBUG: out-edgeSet size:"<<edgeSet.size()<<endl;
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
          list<EState> newEStateList;
          newEStateList=transferFunction(e,currentEStatePtr);
#if 0
          cout << "DEBUG: transfer at edge:"<<e.toString()<<" succ="<<newEStateList.size()<< endl;
          cout << "DEBUG: newEStateList.size()"<<newEStateList.size()<<endl;
          string sourceString=getCFAnalyzer()->getLabeler()->getNode(currentEStatePtr->label())->unparseToString().substr(0,20);
          if(sourceString.size()==20) sourceString+="...";
          cout << "DEBUG: source:"<<sourceString<<endl;
#endif
          for(list<EState>::iterator nesListIter=newEStateList.begin();
              nesListIter!=newEStateList.end();
              ++nesListIter) {
            // newEstate is passed by value (not created yet)
            EState newEState=*nesListIter;
            assert(newEState.label()!=Labeler::NO_LABEL);
            if(_stg_trace_filename.size()>0 && !newEState.constraints()->disequalityExists()) {
              std::ofstream fout;
              // _csv_stg_trace_filename is the member-variable of analyzer
#pragma omp critical
              {
                fout.open(_stg_trace_filename.c_str(),ios::app);    // open file for appending
                assert (!fout.fail( ));
                fout<<"PSTATE-IN:"<<currentEStatePtr->pstate()->toString(&variableIdMapping);
                string sourceString=getCFAnalyzer()->getLabeler()->getNode(currentEStatePtr->label())->unparseToString().substr(0,20);
                if(sourceString.size()==20) sourceString+="...";
                fout<<" ==>"<<"TRANSFER:"<<sourceString;
                fout<<"==> "<<"PSTATE-OUT:"<<newEState.pstate()->toString(&variableIdMapping);
                fout<<endl;
                fout.close(); 
                //cout<<"DEBUG:generate STG-edge:"<<"ICFG-EDGE:"<<e.toString()<<endl;
              }
            }

            if((!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState))) {
              HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=process(newEState);
              const EState* newEStatePtr=pres.second;
              if(pres.first==true)
                addToWorkList(newEStatePtr);
              recordTransition(currentEStatePtr,e,newEStatePtr);
            }
            if((!newEState.constraints()->disequalityExists()) && (isFailedAssertEState(&newEState))) {
              // failed-assert end-state: do not add to work list but do add it to the transition graph
              const EState* newEStatePtr;
              newEStatePtr=processNewOrExisting(newEState);
              recordTransition(currentEStatePtr,e,newEStatePtr);        
              if(boolOptions["report-failed-assert"]) {
#pragma omp critical
                {
                  cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
                }
              }
              
              // record reachability
              int assertCode=reachabilityAssertCode(currentEStatePtr);
              if(assertCode>=0) {
#pragma omp critical
                {
                    reachabilityResults.reachable(assertCode);
                }
              } else {
                // TODO: this is a workaround for isFailedAssert being true in case of rersmode for stderr (needs to be refined)
                if(!boolOptions["rersmode"]) {
                  // assert without label
                }
              }
              
              if(_csv_assert_live_file.size()>0) {
                string name=labelNameOfAssertLabel(currentEStatePtr->label());
                if(name=="globalError")
                  name="error_60";
                name=name.substr(6,name.size()-6);
                std::ofstream fout;
                // csv_assert_live_file is the member-variable of analyzer
#pragma omp critical
                {
                  fout.open(_csv_assert_live_file.c_str(),ios::app);    // open file for appending
                  assert (!fout.fail( ));
                  fout << name << ",yes,9"<<endl;
                  //cout << "REACHABLE ASSERT FOUND: "<< name << ",yes,9"<<endl;
                  
                  fout.close(); 
                }
              } // if
            }
          } // end of loop on transfer function return-estates
        } // just for proper auto-formatting in emacs
      } // conditional: test if work is available
      //    } // worklist-parallel for
  } // while
  } // omp parallel
  reachabilityResults.finished(); // sets all unknown entries to NO.
  printStatusMessage(true);
  cout << "analysis finished (worklist is empty)."<<endl;
}

// solver 8 is used to analyze traces of consecutively added input sequences 
void Analyzer::runSolver8() {
  //flow.boostify();
  int workers = 1; //only one thread
#ifdef RERS_SPECIALIZATION  
  //initialize the global variable arrays in the linked binary version of the RERS problem
  if(boolOptions["rers-binary"]) {
    //cout << "DEBUG: init of globals with arrays for "<< workers << " threads. " << endl;
    RERS_Problem::rersGlobalVarsArrayInit(workers);
  }
#endif
  while(!isEmptyWorkList()) {
    bool oneSuccessorOnly=false;
    EState newEStateBackupForOneSuccessorOnly;
    const EState* currentEStatePtr;
    //solver 8
    assert(estateWorkList.size() == 1);
    if (!isEmptyWorkList()) {
      currentEStatePtr=popWorkList();
    } else {
      assert(0); // there should always be exactly one element in the worklist at this point
    }
    assert(currentEStatePtr);

    shortcut:      
      if(variableValueMonitor.isActive()) {
        variableValueMonitor.update(this,const_cast<EState*>(currentEStatePtr));
      }
    
    Flow edgeSet=flow.outEdges(currentEStatePtr->label());
    oneSuccessorOnly=(edgeSet.size()==1);
    for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
      Edge e=*i;
      list<EState> newEStateList;
      newEStateList=transferFunction(e,currentEStatePtr);
      // solver 8: keep track of the input state where the input sequence ran out of elements (where solver8 stops)
      if (newEStateList.size()== 0) {
        if(e.isType(EDGE_EXTERNAL)) {
          SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(e.source);
          InputOutput newio;
          Label lab=getLabeler()->getLabel(nextNodeToAnalyze1);
          VariableId varId;
          if(getLabeler()->isStdInLabel(lab,&varId)) {
            _estateBeforeMissingInput = currentEStatePtr; //store the state where input was missing in member variable
          }
        }
      }
      oneSuccessorOnly=oneSuccessorOnly&&(newEStateList.size()==1);
      // solver 8: only single traces allowed
      assert(newEStateList.size()<=1);
      for(list<EState>::iterator nesListIter=newEStateList.begin();
          nesListIter!=newEStateList.end();
          ++nesListIter) {
        // newEstate is passed by value (not created yet)
        EState newEState=*nesListIter;
        assert(newEState.label()!=Labeler::NO_LABEL);
        if((!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState))) {
          if(oneSuccessorOnly && _minimizeStates && (newEState.io.isNonIO())) {
            newEStateBackupForOneSuccessorOnly=newEState;
            currentEStatePtr=&newEStateBackupForOneSuccessorOnly;
            goto shortcut;
          }
          HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=process(newEState);
          const EState* newEStatePtr=pres.second;
          // maintain the most recent output state. It can be connected with _estateBeforeMissingInput to facilitate
          // further tracing of an STG that is reduced to input/output/error states.
          if (newEStatePtr->io.isStdOutIO()) {
            _latestOutputEState = newEStatePtr;
          }
          if (true)//simply continue analysing until the input sequence runs out
            addToWorkList(newEStatePtr);          
          recordTransition(currentEStatePtr,e,newEStatePtr);
        }
        if((!newEState.constraints()->disequalityExists()) && (isFailedAssertEState(&newEState))) {
          // failed-assert end-state: do not add to work list but do add it to the transition graph
          const EState* newEStatePtr;
          newEStatePtr=processNewOrExisting(newEState);
          _latestErrorEState = newEStatePtr;
          recordTransition(currentEStatePtr,e,newEStatePtr);
        }
      }  // all successor states of transfer function
    } // all outgoing edges in CFG
  } // while worklist is not empty
  //the result of the analysis is just a concrete trace on the original program
  transitionGraph.setIsPrecise(true);
  transitionGraph.setIsComplete(false);
}

typedef std::pair<PState,  std::list<int> > PStatePlusIOHistory;

void Analyzer::runSolver9() {
#ifndef RERS_SPECIALIZATION
  cout << "ERROR: solver 9 is only compatible with the hybrid analyzer." << endl;
  ROSE_ASSERT(0);
#endif
#ifdef RERS_SPECIALIZATION
  //initialize the global variable arrays in the linked binary version of the RERS problem
  if(boolOptions["rers-binary"]) {
    RERS_Problem::rersGlobalVarsArrayInit(_numberOfThreadsToUse);
    RERS_Problem::createGlobalVarAddressMaps(this);
  }
#endif
  flow.boostify();
  reachabilityResults.init(getNumberOfErrorLabels()); // set all reachability results to unknown
  cout<<"INFO: number of error labels: "<<reachabilityResults.size()<<endl;
  int maxInputVal = *( std::max_element(_inputVarValues.begin(), _inputVarValues.end()) ); //required for parsing to characters
  cout<<"INFO: maximum length of input patterns: "<< (_reconstructMaxInputDepth / 2) <<endl;
  cout<<"INFO: maximum number of pattern repetitions: "<<_reconstructMaxRepetitions<<endl;
  cout <<"STATUS: Running parallel solver 9 (reconstruct assertion traces) with "<<_numberOfThreadsToUse<<" threads."<<endl;
  cout <<"STATUS: This may take a while. Please expect a line of output every 10.000 non-error states. (counter resets itself everytime the prefix is expanded)" << endl;
  for (unsigned int i = 0; i < _reconstructPreviousResults->size(); i++) {
    if (_reconstructPreviousResults->getPropertyValue(i) == PROPERTY_VALUE_YES) {
      bool foundAssertion = false;
      list<int> realTrace;
      list<int> suffixRealTrace;
      int* inputPatternLength = new int();
      PState startPStateCopy = _startPState;
      list<int> ceInputs;
      string ce = _reconstructPreviousResults->getCounterexample(i);
      //extract list of input values
      ce = ce.substr(1, (ce.size()-2)); //eliminate outer square brackets
      std::vector<std::string> symbols;
      boost::algorithm::split(symbols, ce, boost::algorithm::is_any_of(";"));
      for (vector<string>::iterator k=symbols.begin(); k!=symbols.end(); k++) {
        if ((*k)[0]=='i') { //input symbol
          int inputVal = ((int) (*k)[1]) - ((int) 'A') + 1;
          ceInputs.push_back(inputVal);
        }
      }
      // try the search with all prefix/suffix combinations of the loaded counterexample
      list<int> inputSuffix = ceInputs;
      int ceIndex = 0;
      foundAssertion = searchForIOPatterns(&startPStateCopy, i, inputSuffix, &suffixRealTrace, inputPatternLength);
      if (foundAssertion) {
        reachabilityResults.setPropertyValue(i, PROPERTY_VALUE_YES);
        realTrace.splice(realTrace.end(), suffixRealTrace); //append the suffix
        cout << "STATUS: found a trace leading to failing assertion #" << i << " (input lengths: reused prefix: " << ceIndex;
        cout << ", pattern: " << * inputPatternLength << ", total: " << ((realTrace.size()+1) /2) << ")." << endl;
        string ce = convertToCeString(realTrace, maxInputVal);
        reachabilityResults.setCounterexample(i, ce);
      }
      ceIndex++;
      list<int>::iterator ceIter=ceInputs.begin();
      while (!foundAssertion && ceIter!=ceInputs.end()) {
        bool validPath = computePStateAfterInputs(startPStateCopy, *ceIter, 0, &realTrace);
        if (validPath) {
          inputSuffix.pop_front();
          suffixRealTrace = list<int>(); //reset the real suffix before searching with a different prefix
          foundAssertion = searchForIOPatterns(&startPStateCopy, i, inputSuffix, &suffixRealTrace, inputPatternLength);  
          if (foundAssertion) {
            reachabilityResults.setPropertyValue(i, PROPERTY_VALUE_YES);
            realTrace.splice(realTrace.end(), suffixRealTrace); //append the suffix 
            cout << "STATUS: found a trace leading to failing assertion #" << i << " (input lengths: reused prefix: " << ceIndex;
            cout << ", pattern: " << * inputPatternLength << ", total: " << ((realTrace.size()+1) /2) << ")." << endl;
            string ce = convertToCeString(realTrace, maxInputVal);
            reachabilityResults.setCounterexample(i, ce);
          }
        }    
        ceIndex++;
        ceIter++;
      }
      if (!foundAssertion) {
        cout << "INFO: no trace to assertion #" << i << " could be found. Maybe try again with greater thresholds." << endl;
      }
    }
  }
}

void Analyzer::runSolver10() {
#ifndef RERS_SPECIALIZATION
  cout << "ERROR: solver 10 is only compatible with the hybrid analyzer." << endl;
  ROSE_ASSERT(0);
#endif
#ifdef RERS_SPECIALIZATION
  //initialize the global variable arrays in the linked binary version of the RERS problem
  if(boolOptions["rers-binary"]) {
    RERS_Problem::rersGlobalVarsArrayInit(_numberOfThreadsToUse);
    RERS_Problem::createGlobalVarAddressMaps(this);
  }
#endif
  // display initial information
  int maxInputVal = *( std::max_element(_inputVarValues.begin(), _inputVarValues.end()) ); //required for parsing to characters
  flow.boostify();
  reachabilityResults.init(getNumberOfErrorLabels()); // set all reachability results to unknown
  int assertionsToFind = 0;
  for (unsigned int c = 0; c < _patternSearchAssertTable->size(); c++) {
    if (_patternSearchAssertTable->getPropertyValue(c) == PROPERTY_VALUE_YES) {
      assertionsToFind++;
    }
  }
  cout<<"INFO: number of error labels to find: " << assertionsToFind << " (out of " << reachabilityResults.size() << ")" << endl;
  string expMode = "";
  if (_patternSearchExplorationMode == EXPL_BREADTH_FIRST) {
    expMode = "breadth-first";
  } else if (_patternSearchExplorationMode == EXPL_DEPTH_FIRST) {
    expMode = "depth-first";
  }
  cout << "INFO: pattern search exploration mode: " << expMode << endl;
  cout<<"INFO: maximum input depth for the pattern search: "<< _patternSearchMaxDepth << endl;
  cout<<"INFO: following " << _patternSearchRepetitions << " pattern iterations before the suffix search." << endl;
  cout<<"INFO: maximum input depth of the counterexample suffix: "<<_patternSearchMaxSuffixDepth << endl;
  cout <<"STATUS: Running parallel solver 10 (I/O-pattern search) with "<<_numberOfThreadsToUse<<" threads."<<endl;
  cout <<"STATUS: This may take a while. Please expect a line of output every 10.000 non-error states." << endl;
  // create a new instance of the startPState
  //TODO: check why init of "output" is necessary
  PState newStartPState = _startPState;
  newStartPState[globalVarIdByName("output")]=CodeThorn::AType::CppCapsuleConstIntLattice(-7);
  // initialize worklist
  PStatePlusIOHistory startState = PStatePlusIOHistory(newStartPState, list<int>());
  std::list<PStatePlusIOHistory> workList;
  workList.push_back(startState);
  // statistics and other variables
  int processedStates = 0;
  int previousProcessedStates = 0;
  int checkIfFinishedEvery = 500;
  //int previousFinishedCheckStates = 0;
  unsigned int currentMaxDepth = 0; //debugging
  int threadNum;
  int workers=_numberOfThreadsToUse;
  bool earlyTermination = false;
  vector<bool> workVector(_numberOfThreadsToUse);
  set_finished(workVector,true);
  omp_set_num_threads(workers);
# pragma omp parallel shared(workVector) private(threadNum)
  {
    threadNum=omp_get_thread_num();
    while(!all_false(workVector)) {
      // check if all assertions have been found
      if(threadNum==0 && (processedStates >= (previousProcessedStates+checkIfFinishedEvery))) {
        bool finished = true;
        for (unsigned int c = 0; c < _patternSearchAssertTable->size(); c++) {
          if (_patternSearchAssertTable->getPropertyValue(c) == PROPERTY_VALUE_YES 
              && reachabilityResults.getPropertyValue(c) != PROPERTY_VALUE_YES) {
            finished = false;
          }
        }
        if (finished) {
          earlyTermination = true;
        }
        //previousFinishedCheckStates = processedStates;
      }
      // display a status report every ~10.000 non-error PStates
      if(threadNum==0 && _displayDiff && (processedStates >= (previousProcessedStates+_displayDiff))) {
        cout << "STATUS: #processed PStates: " << processedStates << "   currentMaxDepth: " << currentMaxDepth << "   wl size: " << workList.size() << endl;
        previousProcessedStates=processedStates;
      }
      // updated workVector
      bool isEmptyWorkList;
      #pragma omp critical(SOLVERNINEWL)
      {
        isEmptyWorkList = (workList.empty());
      }
      if(isEmptyWorkList || earlyTermination) {
#pragma omp critical(SOLVERNINEWV)
        {
          workVector[threadNum]=false;
        }
        continue;
      } else {
#pragma omp critical(SOLVERNINEWV)
        {
          workVector[threadNum]=true;
        }
      }
      // pop worklist
      PStatePlusIOHistory currentState;
      bool nextElement;
      #pragma omp critical(SOLVERNINEWL)
      {
        if(!workList.empty()) {
          processedStates++;
          currentState=*workList.begin();
          workList.pop_front();
          nextElement=true;
        } else {
          nextElement=false;
        }
      }
      if (!nextElement) {
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
        continue;
      }
      // generate one new state for each input symbol and continue searching for patterns
      for (set<int>::iterator inputVal=_inputVarValues.begin(); inputVal!=_inputVarValues.end(); inputVal++) {
        // copy the state and initialize new input
        PState newPState = currentState.first;
        newPState[globalVarIdByName("input")]=CodeThorn::AType::CppCapsuleConstIntLattice(*inputVal);
        list<int> newHistory = currentState.second;
        ROSE_ASSERT(newHistory.size() % 2 == 0);
        newHistory.push_back(*inputVal);
        // call the next-state function (a.k.a. "calculate_output")
        RERS_Problem::rersGlobalVarsCallInit(this, newPState, omp_get_thread_num());
        (void) RERS_Problem::calculate_output( omp_get_thread_num() );
        int rers_result=RERS_Problem::output[omp_get_thread_num()];
        // handle assertions found to be reachable
        if (rers_result==-2) {
          //Stderr state, do not continue (rers mode)
        } else if(rers_result<=-100) {
          // we found a failing assert
          int index=((rers_result+100)*(-1));
          assert(index>=0 && index <=99);
          if (_patternSearchAssertTable->getPropertyValue(index) == PROPERTY_VALUE_YES) {
            // report the result and add it to the results table
            #pragma omp critical(CSV_ASSERT_RESULTS)
            {
              if (reachabilityResults.getPropertyValue(index) == PROPERTY_VALUE_UNKNOWN) {
                cout << "STATUS: found a trace leading to failing assertion #" << index; 
                cout << " (no pattern. total input length: " << ((newHistory.size()+1) / 2) << ")." << endl;
                string ce = convertToCeString(newHistory, maxInputVal);
                reachabilityResults.setPropertyValue(index, PROPERTY_VALUE_YES);
                reachabilityResults.setCounterexample(index, ce);
              }
            }
          }
        } else {  // not a failed assertion, continue searching
          RERS_Problem::rersGlobalVarsCallReturnInit(this, newPState, omp_get_thread_num());
          newHistory.push_back(rers_result);
          ROSE_ASSERT(newHistory.size() % 2 == 0);
          // check for all possible patterns in the current I/O-trace "newHistory"
          for (unsigned int ps = 0; ps < newHistory.size(); ps +=2) { //"ps" = pattern start
            //every input is followed by an output & pattern needs to occur twice --> modulo 4
            if ((newHistory.size() - ps) % 4 != 0) {
              ps+=2;
              continue;
            }
            // check for a cyclic pattern
            bool containsPattern = containsPatternTwoRepetitions(newHistory, ps, (newHistory.size() - 1));
            if (containsPattern) {
              // modulo 4: sets of input and output symbols are distinct & the system always alternates between input / ouput 
              ROSE_ASSERT( (newHistory.size() - ps)  % 4 == 0 );
#if 0
              cout << "DEBUG: found pattern (start index " << ps << "): ";
              for (list<int>::iterator it = newHistory.begin(); it != newHistory.end(); it++) {
                cout << *it <<",";
              }
              cout << endl;
#endif
              PState backupPState = PState(newPState);
              list<int> backupHistory = list<int>(newHistory);
              list<int>::iterator patternStart = newHistory.begin();
              for (unsigned int i = 0; i < ps; i++) {
                patternStart++;
              }
              list<int> patternStates = list<int>(patternStart, newHistory.end());
              list<int> patternInputs = inputsFromPatternTwoRepetitions(patternStates);
              bool stillAValidPath = true;
              // follow the cyclic I/O-pattern
              for (int p = 0; p < (_patternSearchRepetitions - 2); p++) { // already found two pattern iterations
                stillAValidPath = computePStateAfterInputs(newPState, patternInputs, omp_get_thread_num(), &newHistory);
                if (!stillAValidPath) {
                  break;
                }
              }
              if (stillAValidPath) {
                int suffixDfsProcessedStates = pStateDepthFirstSearch(&newPState, _patternSearchMaxSuffixDepth, omp_get_thread_num(), 
                                                                      &newHistory, maxInputVal, patternInputs.size(), _patternSearchRepetitions);
                #pragma omp critical(SOLVERNINEWL)
                {
                  processedStates += suffixDfsProcessedStates;
                }
              }
              // continue searching for patterns
              newPState = backupPState;
              newHistory = backupHistory;
            }
            ps +=2;
          } // end of "check for all possible patterns"
          // continue only if the maximum depth of input symbols has not yet been reached
          if ((newHistory.size() / 2) < (unsigned int) _patternSearchMaxDepth) {
            // add the new state to the worklist
            PStatePlusIOHistory newState = PStatePlusIOHistory(newPState, newHistory);
            #pragma omp critical(SOLVERNINEWL)
            {
              currentMaxDepth = currentMaxDepth < newHistory.size() ? newHistory.size() : currentMaxDepth;
              if (_patternSearchExplorationMode == EXPL_DEPTH_FIRST) {
                workList.push_front(newState);
              } else if (_patternSearchExplorationMode == EXPL_BREADTH_FIRST) {
                workList.push_back(newState); 
              } else {
                cout << "ERROR: requested pattern search exploration mode currently not supported." << endl;
                ROSE_ASSERT(0);
              }
            }
          } else {
            ROSE_ASSERT(newHistory.size() / 2 == (unsigned int) _patternSearchMaxDepth);
          }
        } // end of else-case "no assertion, continue searching"  
      } //end of "for each input value"-loop
    } // while
  } // omp parallel
  if (earlyTermination) {
    cout << "STATUS: solver 10 finished (found all assertions)." << endl;
  } else {
    cout << "STATUS: solver 10 finished (empty worklist). " << endl;
  }
}

bool Analyzer::searchForIOPatterns(PState* startPState, int assertion_id, list<int>& inputSuffix, list<int>* partialTrace,
                                   int* inputPatternLength) {
  // create a new instance of the startPState
  //TODO: check why init of "output" is necessary
  (*startPState).setVariableToValue(globalVarIdByName("output"),
                                    CodeThorn::AType::CppCapsuleConstIntLattice(-7));
  PState newStartPState = *startPState;
  // initialize worklist
  PStatePlusIOHistory startState = PStatePlusIOHistory(newStartPState, list<int>());
  std::list<PStatePlusIOHistory> workList;
  workList.push_back(startState);
  // statistics and other variables
  bool foundTrace = false;
  int processedStates = 0;
  int previousProcessedStates = 0;
  unsigned int currentMaxDepth = 0; //debugging
  int threadNum;
  int workers=_numberOfThreadsToUse;
  bool foundTheAssertion = false; //only access through omp_critical(SOLVERNINEWV)
  vector<bool> workVector(_numberOfThreadsToUse);
  set_finished(workVector,true);
  omp_set_num_threads(workers);
# pragma omp parallel shared(workVector) private(threadNum)
  {
    threadNum=omp_get_thread_num();
    while(!all_false(workVector)) {
      if(threadNum==0 && _displayDiff && (processedStates >= (previousProcessedStates+_displayDiff))) {
        cout << "DEBUG: #processed PStates: " << processedStates << "   currentMaxDepth: " << currentMaxDepth << "   wl size: " << workList.size() << endl;
        previousProcessedStates=processedStates;
      }
      bool sppResult = false;
      bool isEmptyWorkList;
      #pragma omp critical(SOLVERNINEWL)
      {
        isEmptyWorkList = (workList.empty());
      }
      if(isEmptyWorkList || foundTheAssertion) {
#pragma omp critical(SOLVERNINEWV)
        {
          workVector[threadNum]=false;
        }
        continue;
      } else {
#pragma omp critical(SOLVERNINEWV)
        {
          workVector[threadNum]=true;
        }
      }
      // pop worklist
      PStatePlusIOHistory currentState;
      bool nextElement;
      #pragma omp critical(SOLVERNINEWL)
      {
        if(!workList.empty()) {
          processedStates++;
          currentState=*workList.begin();
          workList.pop_front();
          nextElement=true;
        } else {
          nextElement=false;
        }
      }
      if (!nextElement) {
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
        continue;
      }
      // generate one new state for each input symbol and continue searching for patterns
      for (set<int>::iterator inputVal=_inputVarValues.begin(); inputVal!=_inputVarValues.end(); inputVal++) {
        // copy the state and initialize new input
        PState newPState = currentState.first;
        newPState.setVariableToValue(globalVarIdByName("input"),
                                     CodeThorn::AType::CppCapsuleConstIntLattice(*inputVal));
        list<int> newHistory = currentState.second;
        ROSE_ASSERT(newHistory.size() % 2 == 0);
        newHistory.push_back(*inputVal);
        // call the next-state function (a.k.a. "calculate_output")
        RERS_Problem::rersGlobalVarsCallInit(this, newPState, omp_get_thread_num());
        (void) RERS_Problem::calculate_output( omp_get_thread_num() );
        int rers_result=RERS_Problem::output[omp_get_thread_num()];
        // handle assertions found to be reachable
        if (rers_result==-2) {
          //Stderr state, do not continue (rers mode)
        } else if(rers_result<=-100) {
          // we found a failing assert
          int index=((rers_result+100)*(-1));
          assert(index>=0 && index <=99);
        } else {  // not a failed assertion, continue searching
          RERS_Problem::rersGlobalVarsCallReturnInit(this, newPState, omp_get_thread_num());
          newHistory.push_back(rers_result);
          ROSE_ASSERT(newHistory.size() % 2 == 0);
          // check for a cyclic pattern
          bool containsPattern = containsPatternTwoRepetitions(newHistory);
          if (containsPattern) {
            // modulo 4: sets of input and output symbols are distinct & the system always alternates between input / ouput 
            ROSE_ASSERT(newHistory.size() % 4 == 0);
            //call "follow path"-funtion
            PState backupPState = PState(newPState);
            list<int> backupHistory = list<int>(newHistory);
            list<int> patternInputs = inputsFromPatternTwoRepetitions(newHistory);
            sppResult = searchPatternPath(assertion_id, newPState, patternInputs, inputSuffix, omp_get_thread_num(), &newHistory);
            if (sppResult) { // the assertion was found
              #pragma omp critical(SOLVERNINEWV)
              {
                foundTheAssertion=true;
              }
              //
              if (partialTrace) { // update the real counterexample trace
                #pragma omp critical(SOLVERNINETRACE)
                {
                  if (!foundTrace) {
                    partialTrace->splice(partialTrace->end(), newHistory); //append the real trace suffix
                    if (inputPatternLength) {
                      *inputPatternLength = (backupHistory.size() / 4);
                    }
                    foundTrace=true;
                  }
                }
              }
            } else {
              // search for specific assertion was unsuccessful, continue searching for patterns
              newPState = backupPState;
              newHistory = backupHistory;
            }
          }
          // continue only if the maximum depth of input symbols has not yet been reached
          if (!sppResult) {
            if ((newHistory.size() / 2) < (unsigned int) _reconstructMaxInputDepth) {
              // add the new state to the worklist
              PStatePlusIOHistory newState = PStatePlusIOHistory(newPState, newHistory);
              #pragma omp critical(SOLVERNINEWL)
              {
                currentMaxDepth = currentMaxDepth < newHistory.size() ? newHistory.size() : currentMaxDepth;
                workList.push_front(newState);
              }
            } else {
              ROSE_ASSERT(newHistory.size() / 2 == (unsigned int) _reconstructMaxInputDepth);
            }
          }
        } // end of else-case "no assertion, continue searching"  
      } //end of "for each input value"-loop
    } // while
  } // omp parallel
  if (foundTheAssertion) {
    //cout << "analysis finished (found assertion #" << assertion_id <<")."<<endl;
    return true;
  } else {
    //cout << "analysis finished (worklist is empty)."<<endl;
    return false;
  }
}

int Analyzer::pStateDepthFirstSearch(PState* startPState, int maxDepth, int thread_id, list<int>* partialTrace, int maxInputVal, int patternLength, int patternIterations) {
  // initialize worklist
  PStatePlusIOHistory startState = PStatePlusIOHistory(*startPState, list<int>());
  std::list<PStatePlusIOHistory> workList;
  workList.push_back(startState);
  // statistics and other variables
  int processedStates = 0;
  int previousProcessedStates = 0;
  unsigned int currentMaxDepth = 0; //debugging
  int displayDiff=1000; 
  while(!workList.empty()) {
    if(displayDiff && (processedStates >= (previousProcessedStates+displayDiff))) {
      cout << "STATUS: #processed PStates suffix dfs (thread_id: " << thread_id << "): " << processedStates << "   wl size: " << workList.size() << endl;
      previousProcessedStates=processedStates;
    }
    // pop worklist
    PStatePlusIOHistory currentState;
    processedStates++;
    currentState=*workList.begin();
    workList.pop_front();
    // generate one new state for each input symbol and continue searching for patterns
    for (set<int>::iterator inputVal=_inputVarValues.begin(); inputVal!=_inputVarValues.end(); inputVal++) {
      // copy the state and initialize new input
      PState newPState = currentState.first;
      newPState[globalVarIdByName("input")]=CodeThorn::AType::CppCapsuleConstIntLattice(*inputVal);
      list<int> newHistory = currentState.second;
      ROSE_ASSERT(newHistory.size() % 2 == 0);
      newHistory.push_back(*inputVal);
      // call the next-state function (a.k.a. "calculate_output")
      RERS_Problem::rersGlobalVarsCallInit(this, newPState, thread_id);
      (void) RERS_Problem::calculate_output( thread_id );
      int rers_result=RERS_Problem::output[thread_id];
      // handle assertions found to be reachable
      if (rers_result==-2) {
        //Stderr state, do not continue (rers mode)
      } else if(rers_result<=-100) {
        // we found a failing assert
        int index=((rers_result+100)*(-1));
        assert(index>=0 && index <=99);
        if (_patternSearchAssertTable->getPropertyValue(index) == PROPERTY_VALUE_YES) {
          // report the result and add it to the results table
          #pragma omp critical(CSV_ASSERT_RESULTS)
          {
            if (reachabilityResults.getPropertyValue(index) == PROPERTY_VALUE_UNKNOWN) {
              list<int> ceTrace = *partialTrace;
              for (list<int>::iterator n = newHistory.begin(); n != newHistory.end(); n++) {
                ceTrace.push_back(*n);
              }
              int prefixLength = (partialTrace->size() - (2 * patternIterations * patternLength)) / 2; 
              cout << "STATUS: found a trace leading to failing assertion #" << index << " (input lengths: reused prefix: " << prefixLength;
              cout << ", pattern: " <<patternLength << ", suffix: " << ((newHistory.size()+1) / 2) << ", total: " << ((ceTrace.size()+1) / 2) << ")." << endl;
              string ce = convertToCeString(ceTrace, maxInputVal);
              reachabilityResults.setPropertyValue(index, PROPERTY_VALUE_YES);
              reachabilityResults.setCounterexample(index, ce);
            }
          }
        }
      } else {  // not a failed assertion, continue searching
        RERS_Problem::rersGlobalVarsCallReturnInit(this, newPState, thread_id);
        newHistory.push_back(rers_result);
        ROSE_ASSERT(newHistory.size() % 2 == 0);
        // continue only if the maximum depth of input symbols has not yet been reached
        if ((newHistory.size() / 2) < (unsigned int) maxDepth) {
          // add the new state to the worklist
          PStatePlusIOHistory newState = PStatePlusIOHistory(newPState, newHistory);
          currentMaxDepth = currentMaxDepth < newHistory.size() ? newHistory.size() : currentMaxDepth;
          workList.push_front(newState);  // depth-first search
        } else {
          ROSE_ASSERT(newHistory.size() / 2 == (unsigned int) maxDepth);
        }
      } // end of else-case "no assertion, continue searching"  
    } //end of "for each input value"-loop
  } // while
  return processedStates;
}

list<int> Analyzer::inputsFromPatternTwoRepetitions(list<int> pattern2r) {
  ROSE_ASSERT(pattern2r.size() % 4 == 0);
  list<int> result;
  list<int>::iterator iter = pattern2r.begin();
  for (unsigned int i = 0; i < (pattern2r.size() / 4); i++) {
    result.push_back(*iter);
    iter++;
    iter++;
  }
  return result;
}

bool Analyzer::computePStateAfterInputs(PState& pState, int input, int thread_id, list<int>* iOSequence) {    
  //pState[globalVarIdByName("input")]=CodeThorn::AType::CppCapsuleConstIntLattice(input);
  pState.setVariableToValue(globalVarIdByName("input"),
                            CodeThorn::AType::CppCapsuleConstIntLattice(input));
  RERS_Problem::rersGlobalVarsCallInit(this, pState, thread_id);
  (void) RERS_Problem::calculate_output(thread_id);
  RERS_Problem::rersGlobalVarsCallReturnInit(this, pState, thread_id);
  if (iOSequence) {
    iOSequence->push_back(input);
    int outputVal=RERS_Problem::output[thread_id];
    // a (std)err state could was encountered, this is not a valid RERS path anymore
    if (outputVal <= 0) {
      return false;
    }
    iOSequence->push_back(outputVal);
  }
  return true;
}

bool Analyzer::computePStateAfterInputs(PState& pState, list<int>& inputs, int thread_id, list<int>* iOSequence) {
  for (list<int>::iterator i = inputs.begin(); i !=inputs.end(); i++) {
    //pState[globalVarIdByName("input")]=CodeThorn::AType::CppCapsuleConstIntLattice(*i);
    pState.setVariableToValue(globalVarIdByName("input"),
                              CodeThorn::AType::CppCapsuleConstIntLattice(*i));
    RERS_Problem::rersGlobalVarsCallInit(this, pState, thread_id);
    (void) RERS_Problem::calculate_output(thread_id);
    RERS_Problem::rersGlobalVarsCallReturnInit(this, pState, thread_id);
    if (iOSequence) {
      iOSequence->push_back(*i);
      int outputVal=RERS_Problem::output[thread_id];
      // a (std)err state could was encountered, this is not a valid RERS path anymore
      if (outputVal <= 0) {
        return false;
      }
      iOSequence->push_back(outputVal);
    }
  }
  return true;
}

bool Analyzer::searchPatternPath(int assertion_id, PState& pState, list<int>& inputPattern, list<int>& inputSuffix, int thread_id, list<int>* iOSequence) {
  bool validPath = true;
  int i = 2; // the pattern was found previously, therefore two pattern iterations exist already in "iOSequence"
  while (validPath && i < _reconstructMaxRepetitions) {
    // check if the assertion can be found after the current number of pattern iterations
    if (!inputSuffix.empty()) {
      PState branchToCheckForAssertion = pState;
      list<int> iOSquenceStartingAtBranch;
      computePStateAfterInputs(branchToCheckForAssertion, inputSuffix, thread_id, &iOSquenceStartingAtBranch);
      int rers_result=RERS_Problem::output[thread_id];
      if(rers_result<=-100) {
        // we found a failing assert
        int index=((rers_result+100)*(-1));
        assert(index>=0 && index <=99);
        if(index == assertion_id) {
          //cout << "DEBUG: found assertion #" << assertion_id << " after " << i << " pattern iterations." << endl; 
          if (iOSequence) {
            iOSequence->splice(iOSequence->end(), iOSquenceStartingAtBranch); // append the new I/O symbols to the current trace 
          }
          return true;
        }
      }  
    } else {
      //TODO: try all input symbols
      cout << "ERROR: reached an unhandled case (empty spurious suffix)." << endl;
      ROSE_ASSERT(0);
    }
    // follow the pattern
    validPath = computePStateAfterInputs(pState, inputPattern, thread_id, iOSequence);
    i++;
  }
  //cout << "DEBUG: could NOT find assertion #" << assertion_id << " after " << _reconstructMaxRepetitions << " pattern iterations." << endl;  
  return false;
}

bool Analyzer::containsPatternTwoRepetitions(std::list<int>& sequence) {
  if (sequence.size() % 2 != 0) {
    return false;
  }
  bool mismatch = false;
  list<int>::iterator firstHalf = sequence.begin();
  //get a pointer to the beginning of the second half of the list
  list<int>::iterator secondHalf = sequence.begin();
  for (unsigned int i = 0; i < (sequence.size() / 2); i++) {
    secondHalf++;
  }
  // check for two consecutive repetitions of the same subsequence
  while (!mismatch && secondHalf != sequence.end()) {
    if (*firstHalf != *secondHalf) {
      mismatch = true;
    } else {
      firstHalf++;
      secondHalf++;
    }
  }
  return (!mismatch);
}

bool Analyzer::containsPatternTwoRepetitions(std::list<int>& sequence, int startIndex, int endIndex) {
  // copy the sublist to an array
  int patternLength = endIndex - startIndex + 1;
  ROSE_ASSERT(patternLength % 2 == 0);
  vector<int> ceSymbolsVec(patternLength);
  list<int>::iterator it = sequence.begin();
  for (int i = 0; i < startIndex; i++) {
    it++;
  }
  for (int k = 0; k < patternLength; k++) {
    ceSymbolsVec[k] = *it;
    it++;
  }
  // check if the subsequence contains a pattern
  bool mismatch = false;
  for (int j = 0; j < (patternLength / 2); j++) {
    if (ceSymbolsVec[j] != ceSymbolsVec[j + (patternLength / 2)]) {
      mismatch = true;
      break;
    }
  }
  return !mismatch;
}

string Analyzer::convertToCeString(list<int>& ceAsIntegers, int maxInputVal) {
  SpotConnection spotConnection;
  stringstream ss;
  ss << "[";
  bool firstElem = true;
  for (list<int>::iterator i=ceAsIntegers.begin(); i!=ceAsIntegers.end(); i++) {
    if (!firstElem) {
      ss << ";";
    }
    ss << spotConnection.int2PropName(*i, maxInputVal);
    firstElem = false;
  }
  ss << "]";
  return ss.str();
}

PropertyValueTable* Analyzer::loadAssertionsToReconstruct(string filePath) {
  PropertyValueTable* result = new PropertyValueTable(100);
  ifstream assert_input(filePath.c_str());
  if (assert_input.is_open()) {
    //load the containing counterexamples
    std::string line;
    while (std::getline(assert_input, line)){
      std::vector<std::string> entries;
      boost::algorithm::split(entries, line, boost::algorithm::is_any_of(","));
      if (entries[1] == "yes") {
        int property_id = boost::lexical_cast<int>(entries[0]);
        if (args.count("reconstruct-assert-paths")) {
          assert(entries.size() == 3);
          assert(entries[2] != "");
        }
        result->setPropertyValue(property_id, PROPERTY_VALUE_YES);
        result->setCounterexample(property_id, entries[2]);
      }
    }
  }
  assert_input.close();
  return result;
}

int Analyzer::extractAssertionTraces() {
  int maxInputTraceLength = -1;
  for (list<pair<int, const EState*> >::iterator i = _firstAssertionOccurences.begin(); i != _firstAssertionOccurences.end(); ++i ) {
    cout << "STATUS: extracting trace leading to assertion: " << i->first << endl;
    int ceLength = addCounterexample(i->first, i->second);
    if (ceLength > maxInputTraceLength) {maxInputTraceLength = ceLength;}
  }
  if(boolOptions["rers-binary"]) {
    if ((getExplorationMode() == EXPL_BREADTH_FIRST) && transitionGraph.isPrecise()) {
      return maxInputTraceLength;
    }
  }
  return -1;
}

list<const EState*> Analyzer::reverseInOutSequenceBreadthFirst(const EState* source, const EState* target, bool counterexampleWithOutput) {
  // 1.) init: list wl , hashset predecessor, hashset visited
  list<const EState*> worklist;
  worklist.push_back(source);
  boost::unordered_map <const EState*, const EState*> predecessor;
  boost::unordered_set<const EState*> visited;
  // 2.) while (elem in worklist) {s <-- pop wl; if (s not yet visited) {update predecessor map; 
  //                                check if s==target: yes --> break, no --> add all pred to wl }}
  bool targetFound = false;
  while (worklist.size() > 0 && !targetFound) {
    const EState* vertex = worklist.front();
    worklist.pop_front();
    if (visited.find(vertex) == visited.end()) {  //avoid cycles
      visited.insert(vertex);
      EStatePtrSet predsOfVertex = transitionGraph.pred(vertex);
      for(EStatePtrSet::iterator i=predsOfVertex.begin();i!=predsOfVertex.end();++i) {
        predecessor.insert(pair<const EState*, const EState*>((*i), vertex));
        if ((*i) == target) {
          targetFound=true;
          break;
        } else {
          worklist.push_back((*i));
        }
      }
    }
  }
  if (!targetFound) {
    cout << "ERROR: target state not connected to source while generating reversed trace source --> target." << endl;
    assert(0);
  }
  // 3.) reconstruct trace. filter list of only input ( & output) states and return it
  list<const EState*> run;
  run.push_front(target);
  boost::unordered_map <const EState*, const EState*>::iterator nextPred = predecessor.find(target);
  while (nextPred != predecessor.end()) {
    run.push_front(nextPred->second);
    nextPred = predecessor.find(nextPred->second);
  }
  list<const EState*> result = filterStdInOutOnly(run, counterexampleWithOutput);
  return result;
}

list<const EState*> Analyzer::reverseInOutSequenceDijkstra(const EState* source, const EState* target, bool counterexampleWithOutput) {
  EStatePtrSet states = transitionGraph.estateSet();
  boost::unordered_set<const EState*> worklist;
  map <const EState*, int> distance;
  map <const EState*, const EState*> predecessor;
  //initialize distances and worklist
  for(EStatePtrSet::iterator i=states.begin();i!=states.end();++i) {
    worklist.insert(*i);
    if ((*i) == source) {
      distance.insert(pair<const EState*, int>((*i), 0));
    } else {
      distance.insert(pair<const EState*, int>((*i), (std::numeric_limits<int>::max() - 1)));
    }
  }
  assert( distance.size() == worklist.size() );

  //process worklist
  while (worklist.size() > 0 ) {   
    //extract vertex with shortest distance to source
    int minDist = std::numeric_limits<int>::max();
    const EState* vertex = NULL;
    for (map<const EState*, int>::iterator i=distance.begin(); i != distance.end(); ++i) {
      if ( (worklist.find(i->first) != worklist.end()) ) {
        if ( (i->second < minDist)) {
          minDist = i->second;
          vertex = i->first;
        }
      }
    }
    //check for all predecessors if a shorter path leading to them was found
    EStatePtrSet predsOfVertex = transitionGraph.pred(vertex);
    for(EStatePtrSet::iterator i=predsOfVertex.begin();i!=predsOfVertex.end();++i) {
      int altDist;
      if( (*i)->io.isStdInIO() ) { 
        altDist = distance[vertex] + 1; 
      } else {
        altDist = distance[vertex]; //we only count the input sequence length
      }
      if (altDist < distance[*i]) {
        distance[*i] = altDist;
        //update predecessor
        map <const EState*, const EState*>::iterator predListIndex = predecessor.find((*i));
        if (predListIndex != predecessor.end()) {
          predListIndex->second = vertex;
        } else {
          predecessor.insert(pair<const EState*, const EState*>((*i), vertex));
        }
        //optimization: stop if the target state was found
        if ((*i) == target) {break;}
      } 
    }
    int worklistReducedBy = worklist.erase(vertex); 
    assert(worklistReducedBy == 1);
  }

  //extract and return input run from source to target (backwards in the STG)
  list<const EState*> run;
  run.push_front(target);
  map <const EState*, const EState*>::iterator nextPred = predecessor.find(target);
  while (nextPred != predecessor.end()) {
    run.push_front(nextPred->second);
    nextPred = predecessor.find(nextPred->second);
  }
  assert ((*run.begin()) == source);
  list<const EState*> result = filterStdInOutOnly(run, counterexampleWithOutput);
  return result;
}

list<const EState*> Analyzer::filterStdInOutOnly(list<const EState*>& states, bool counterexampleWithOutput) const {
  list<const EState*> result;
  for (list<const EState*>::iterator i = states.begin(); i != states.end(); i++ ) {
    if( (*i)->io.isStdInIO() || (counterexampleWithOutput && (*i)->io.isStdOutIO())) { 
      result.push_back(*i);
    }
  }
  return result;
} 

string Analyzer::reversedInOutRunToString(list<const EState*>& run) {
  string result = "[";
  for (list<const EState*>::reverse_iterator i = run.rbegin(); i != run.rend(); i++ ) {
    if (i != run.rbegin()) {
      result += ";";
    }
    //get input or output value
    PState* pstate = const_cast<PState*>( (*i)->pstate() ); 
    int inOutVal;
    if ((*i)->io.isStdInIO()) {
      inOutVal = (*pstate)[globalVarIdByName("input")].getValue().getIntValue();
      result += "i";
    } else if ((*i)->io.isStdOutIO()) {
      inOutVal = (*pstate)[globalVarIdByName("output")].getValue().getIntValue();
      result += "o";
    } else {
      assert(0);  //function is supposed to handle list of stdIn and stdOut states only
    }
    //transform into string representation and add to result
    char inOutValChar = (char) (inOutVal + ((int) 'A') - 1);
    string ltlInOutVar = boost::lexical_cast<string>(inOutValChar);
    result += ltlInOutVar; 
  }
  result += "]";
  return result;
}

int Analyzer::addCounterexample(int assertCode, const EState* assertEState) {
  list<const EState*> counterexampleRun;
  // TODO: fix the reported minimum depth to reach an assertion for the first time
  if(true) { //boolOptions["rers-binary"] && (getExplorationMode() == EXPL_BREADTH_FIRST) ) {

    counterexampleRun = reverseInOutSequenceBreadthFirst(assertEState, transitionGraph.getStartEState(), 
                                                         boolOptions["counterexamples-with-output"]);
  } else {
    counterexampleRun = reverseInOutSequenceDijkstra(assertEState, transitionGraph.getStartEState(), 
                                                     boolOptions["counterexamples-with-output"]);
  }
  int ceRunLength = counterexampleRun.size();
  string counterexample = reversedInOutRunToString(counterexampleRun);
  //cout << "DEBUG: adding assert counterexample " << counterexample << endl;
  reachabilityResults.strictUpdateCounterexample(assertCode, counterexample);
  return ceRunLength;
}

int Analyzer::inputSequenceLength(const EState* target) {
  list<const EState*> run;
  if(boolOptions["rers-binary"] && (getExplorationMode() == EXPL_BREADTH_FIRST) ) {
    run = reverseInOutSequenceBreadthFirst(target, transitionGraph.getStartEState());
  } else {
    run = reverseInOutSequenceDijkstra(target, transitionGraph.getStartEState());
  }
  return run.size();
}

void Analyzer::reduceToObservableBehavior() {
  EStatePtrSet states=transitionGraph.estateSet();
  std::list<const EState*>* worklist = new list<const EState*>(states.begin(), states.end());  
  // iterate over all states, reduce those that are neither the start state nor contain input/output/error behavior
  for(std::list<const EState*>::iterator i=worklist->begin();i!=worklist->end();++i) {
    if( (*i) != transitionGraph.getStartEState() ) {
      if(! ((*i)->io.isStdInIO() || (*i)->io.isStdOutIO() || (*i)->io.isStdErrIO() || (*i)->io.isFailedAssertIO()) ) {
	transitionGraph.reduceEState2(*i);
      }
    }
  }
}

void Analyzer::removeOutputOutputTransitions() {
  EStatePtrSet states=transitionGraph.estateSet();
  std::list<const EState*>* worklist = new list<const EState*>(states.begin(), states.end());  
  // output cannot directly follow another output in RERS programs. Erase those transitions
  for(std::list<const EState*>::iterator i=worklist->begin();i!=worklist->end();++i) {
    if ((*i)->io.isStdOutIO()) {
      TransitionPtrSet inEdges = transitionGraph.inEdges(*i);
      for(TransitionPtrSet::iterator k=inEdges.begin();k!=inEdges.end();++k) {
        const EState* pred = (*k)->source;
        if (pred->io.isStdOutIO()) {
          transitionGraph.erase(**k);
          cout << "DEBUG: erased an output -> output transition." << endl;
        }
      }
    }
  }
}

void Analyzer::removeInputInputTransitions() {
  EStatePtrSet states=transitionGraph.estateSet();
  // input cannot directly follow another input in RERS'14 programs. Erase those transitions
  for(EStatePtrSet::iterator i=states.begin();i!=states.end();++i) {
    if ((*i)->io.isStdInIO()) {
      TransitionPtrSet outEdges = transitionGraph.outEdges(*i);
      for(TransitionPtrSet::iterator k=outEdges.begin();k!=outEdges.end();++k) {
        const EState* succ = (*k)->target;
        if (succ->io.isStdInIO()) {
          transitionGraph.erase(**k);
          //cout << "DEBUG: erased an input -> input transition." << endl;
        }
      }
    }
  }
}

void Analyzer::storeStgBackup() {
  backupTransitionGraph = transitionGraph;
}

void Analyzer::swapStgWithBackup() {
  TransitionGraph tTemp = transitionGraph;
  transitionGraph = backupTransitionGraph;
  backupTransitionGraph = tTemp;
}

void Analyzer::setAnalyzerToSolver8(EState* startEState, bool resetAnalyzerData) {
  assert(startEState);
  //set attributes specific to solver 8
  _numberOfThreadsToUse = 1;
  _solver = 8;
  _maxTransitions = -1,
  _maxIterations = -1,
  _maxTransitionsForcedTop = -1;
  _maxIterationsForcedTop = -1;
  _topifyModeActive = false;
  _numberOfThreadsToUse = 1;
  _latestOutputEState = NULL;
  _latestErrorEState = NULL;

  if (resetAnalyzerData) {
    //reset internal data structures
    EStateSet newEStateSet;
    estateSet = newEStateSet;
    PStateSet newPStateSet;
    pstateSet = newPStateSet;
    EStateWorkList newEStateWorkList;
    estateWorkList = newEStateWorkList;
    TransitionGraph newTransitionGraph;
    transitionGraph = newTransitionGraph;
    Label startLabel=cfanalyzer->getLabel(startFunRoot);
    transitionGraph.setStartLabel(startLabel);
    list<int> newInputSequence;
    _inputSequence = newInputSequence;
    resetInputSequenceIterator();
#ifndef USE_CUSTOM_HSET
    estateSet.max_load_factor(0.7);
    pstateSet.max_load_factor(0.7);
    constraintSetMaintainer.max_load_factor(0.7);
#endif
  }
  // initialize worklist
  const EState* currentEState=processNewOrExisting(*startEState);
  assert(currentEState);
  variableValueMonitor.init(currentEState);
  addToWorkList(currentEState);
  //cout << "STATUS: start state: "<<currentEState->toString(&variableIdMapping)<<endl;
  //cout << "STATUS: reset to solver 8 finished."<<endl;
}

void Analyzer::continueAnalysisFrom(EState * newStartEState) {
  assert(newStartEState);
  addToWorkList(newStartEState);
  // connect the latest output state with the state where the analysis stopped due to missing 
  // values in the input sequence
  assert(_latestOutputEState);
  assert(_estateBeforeMissingInput);
  Edge edge(_latestOutputEState->label(),EDGE_PATH,_estateBeforeMissingInput->label());
  Transition transition(_latestOutputEState,edge,_estateBeforeMissingInput); 
  transitionGraph.add(transition);
  runSolver();
}

void Analyzer::mapGlobalVarInsert(std::string name, int* addr) {
  mapGlobalVarAddress[name]=addr;
  mapAddressGlobalVar[addr]=name;
}

 void Analyzer::setCompoundIncVarsSet(set<VariableId> ciVars) {
   _compoundIncVarsSet=ciVars;
 }

 void Analyzer::setSmallActivityVarsSet(set<VariableId> saVars) {
   _smallActivityVarsSet=saVars;
 }

 void Analyzer::setAssertCondVarsSet(set<VariableId> acVars) {
   _assertCondVarsSet=acVars;
 }
