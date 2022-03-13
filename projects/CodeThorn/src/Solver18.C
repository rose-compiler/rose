#include "sage3basic.h"
#include "Solver18.h"
#include "CTAnalysis.h"
#include "CodeThornCommandLineOptions.h"
#include "EStateTransferFunctions.h"
#include <limits>
#include <unordered_set>

using namespace std;
using namespace CodeThorn;
using namespace Sawyer::Message;

#include "CTAnalysis.h"

Sawyer::Message::Facility Solver18::logger;
bool Solver18::_diagnosticsInitialized = false;

Solver18::Solver18() {
  initDiagnostics();
}

void Solver18::initDiagnostics() {
  if (!_diagnosticsInitialized) {
    _diagnosticsInitialized = true;
    Solver::initDiagnostics(logger, 18);
  }
}

void Solver18::deleteAllStates() {
  if(_analyzer->getOptionsRef().status) cout<<"STATUS: Solver18: deleting all remaining states."<<endl;
  size_t cnt=0;
  for(auto entry : _abstractCSStateMapMap) {
    auto map=entry.second;
    for(auto entry2 : map) {
      delete entry2.second;
      cnt++;
    }
  }
  EState::checkPointAllocationHistory();
  if(_analyzer->getOptionsRef().status) cout<<"STATUS: Solver18: cleanup: deleted "<<cnt<<" states."<<endl;
  if(_analyzer->getOptionsRef().status) cout<<"STATUS: Solver18: allocation history: "<<EState::allocationHistoryToString()<<endl;
}

int Solver18::getId() {
  return 18;
}

// allows to handle sequences of nodes as basic blocks
bool Solver18::isPassThroughLabel(Label lab) {
  return _analyzer->isPassThroughLabel(lab);
}

bool Solver18::isUnreachableLabel(Label lab) {
  // if code is unreachable no state is computed for it. In this case no entry is found for this label (with any callstring).
  return !isReachableLabel(lab);
}

bool Solver18::isReachableLabel(Label lab) {
  return isRegisteredTransferFunctionInvoked(lab);
}

void Solver18::dumpAbstractStateMapMap() {
  cout<<"AbstractStateMapMap:"<<endl;
  for(auto p : _abstractCSStateMapMap) {
    cout<<"Label ID"<<p.first<<": "<<endl;
  }
  cout<<"----------------------"<<endl;
}
void Solver18::initializeAbstractStatesFromWorkList() {
  // pop all states from worklist (can contain more than one state)
  list<EStatePtr> tmpWL;
  while(!_analyzer->isEmptyWorkList()) {
    EStatePtr estate=_analyzer->popWorkList();
    ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(estate->label()));
    tmpWL.push_back(estate);
  }
  for(auto s : tmpWL) {
    // initialize abstractstate and push back to work lis
    ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(s->label()));
    setAbstractState(s->label(),s->getCallString(),s);
    _workList->push(WorkListEntry(s->label(),s->getCallString()));
  }
  /* the other states are not initialized here because every context
     (cs) requires its own bottom element and contexts ar computedat
     analysis run time */
}

size_t Solver18::getNumberOfStates() {
  return _numberOfStates;
}

EStatePtr Solver18::getAbstractState(CodeThorn::Label lab, CodeThorn::CallString cs) {
  EStatePtr res;
#pragma omp critical(SUMMARY_STATES)
  {
    auto iter1=_abstractCSStateMapMap.find(lab.getId());
    if(iter1==_abstractCSStateMapMap.end()) {
      res=nullptr;
    } else {
      AbstractCSStateMap& abstractCSStateMap=(*iter1).second;
      auto iter2=abstractCSStateMap.find(cs);
      if(iter2==abstractCSStateMap.end()) {
        res=nullptr;
      } else {
        res=(*iter2).second;
      }
    }
  }
  return res;
}

void Solver18::setAbstractionConsistencyCheckFlag(bool flag) {
  _abstractionConsistencyCheckEnabled=flag;
}

bool Solver18::getAbstractionConsistencyCheckFlag() {
  return _abstractionConsistencyCheckEnabled;
}

void Solver18::setPassThroughOptimizationFlag(bool flag) {
  _passThroughOptimizationEnabled=flag;
}

bool Solver18::getPassThroughOptimizationFlag() {
  return _passThroughOptimizationEnabled;
}

void Solver18::setAbstractState(CodeThorn::Label lab, CodeThorn::CallString cs, EStatePtr estate) {
  ROSE_ASSERT(lab==estate->label());
  ROSE_ASSERT(cs==estate->getCallString());
  ROSE_ASSERT(estate);

  if(_abstractionConsistencyCheckEnabled) {
    auto vim=_analyzer->getVariableIdMapping();
    auto nonPointerAddresses=estate->checkArrayAbstractionIndexConsistency(vim->getArrayAbstractionIndex(),vim);
    if(nonPointerAddresses>0) {
      cout<<"WARNING: number of non-pointer addresses greater 0 at "<<estate->label().toString()<<": "<<nonPointerAddresses<<endl;
    }
  }
  //pair<int,CallString> p(lab.getId(),cs);
  //_summaryCSStateMap[p]=estate;
#pragma omp critical(SUMMARY_STATES)
  {
    auto iter1=_abstractCSStateMapMap.find(lab.getId());
    if(iter1==_abstractCSStateMapMap.end()) {
      // create new
      AbstractCSStateMap newAbstractCSStateMap;
      newAbstractCSStateMap[cs]=estate;
      _abstractCSStateMapMap[lab.getId()]=newAbstractCSStateMap;
      _numberOfStates++;
    } else {
      AbstractCSStateMap& abstractCSStateMap=(*iter1).second;
      auto iter2=abstractCSStateMap.find(cs);
      if(iter2==abstractCSStateMap.end()) {
        abstractCSStateMap[cs]=estate;
        _numberOfStates++;
      } else {
        // context already exists, re-set state, no additional state added
        auto currentEState=(*iter2).second;
        if(currentEState!=estate && currentEState!=nullptr) {
          delete abstractCSStateMap[cs];
        }
        abstractCSStateMap[cs]=estate;
      }
    }
  }
}

// creates bottom state
EStatePtr Solver18::createBottomAbstractState(Label lab, CallString cs) {
  ROSE_ASSERT(!createsTransitionSystem());
  return EState::createBottomEState(lab,cs);
}

bool Solver18::isBottomAbstractState(EStatePtr estate) {
  return estate->isBottomEState();
}

size_t Solver18::checkDiff() {
  ROSE_ASSERT(EState::getDestructCount()<=EState::getConstructCount());
  ROSE_ASSERT(getNumberOfStates()<=(EState::getConstructCount()-EState::getDestructCount()));
  return EState::getConstructCount()-EState::getDestructCount()-getNumberOfStates();
}

static bool debugFlag=false;
void Solver18::printAllocationStats(string text) {
  if(debugFlag) cout<<text<<EState::allocationStatsToString()<<": num states:"<<getNumberOfStates()<<endl;
}

bool Solver18::callStringExistsAtLabel(CallString& cs, Label lab) {
  ROSE_ASSERT(lab.isValid());
  auto iter=_abstractCSStateMapMap.find(lab.getId());
  if(iter==_abstractCSStateMapMap.end()) {
    // call site has never been analyzed before
    //dumpAbstractStateMapMap();
    return false;
  } else { 
    AbstractCSStateMap& map=_abstractCSStateMapMap.at(lab.getId());
    return map[cs]!=nullptr;
  }
}

void Solver18::registerTransferFunctionInvoked(Label lab) {
  _transferFunctionInvoked.insert(lab);
}
bool Solver18::isRegisteredTransferFunctionInvoked(Label lab) {
  return _transferFunctionInvoked.find(lab)!=_transferFunctionInvoked.end();
}

std::list<EStatePtr> Solver18::transferEdgeEStateInPlace(Edge e,EStatePtr currentEStatePtr) {
  registerTransferFunctionInvoked(currentEStatePtr->label()); // in presence of pass-through optimization this is necessary to "remember" pass-through labels for which no state is stored
  std::list<EStatePtr> list=_analyzer->transferEdgeEStateInPlace(e,currentEStatePtr);
  return list;
}

bool Solver18::isJoinLabel(Label lab) {
  return _analyzer->getFlow()->inEdges(lab).size()>1;
}

void Solver18::run() {
  ROSE_ASSERT(_analyzer);
  if(_analyzer->getOptionsRef().status)
    cout<<"Running solver "<<getId()
        <<" (pass-through states:"<<_passThroughOptimizationEnabled
        <<" domain abstr. variant:"<<AbstractValue::domainAbstractionVariant
        <<" normalization level:"<<_analyzer->getOptionsRef().normalizeLevel
        <<" abstraction check:"<<_abstractionConsistencyCheckEnabled
        <<" sharedpstates:"<<_analyzer->getOptionsRef().sharedPStates
        <<")"<<endl;
  if(_analyzer->getOptionsRef().abstractionMode==0) {
    cerr<<"Error: Solver18: abstraction mode is 0, but >= 1 required."<<endl;
    exit(1);
  }
  if(_analyzer->getOptionsRef().explorationMode!="topologic-sort") {
    cerr<<"Error: topologic-sort required for exploration mode, but it is "<<_analyzer->getOptionsRef().explorationMode<<endl;
    exit(1);
  }
  ROSE_ASSERT(_analyzer->getTopologicalSort());
  if(_workList==nullptr) {
    _workList=new GeneralPriorityWorkList<Solver18::WorkListEntry>(_analyzer->getTopologicalSort()->labelToPriorityMap());
    _analyzer->ensureToplogicSortFlowConsistency();
  }

  initializeAbstractStatesFromWorkList();

  size_t displayTransferCounter=0;
  bool terminateEarly=false;
  _analyzer->printStatusMessage(true);
  while(!_workList->empty()) {
    //_workList->print();
    auto p=_workList->top();
     _workList->pop();
    // terminate early, ensure to stop all threads and empty the worklist (e.g. verification error found).
    if(terminateEarly)
      continue;
    
    ROSE_ASSERT(p.label().isValid());
    ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(p.label()));
    EStatePtr currentEStatePtr=getAbstractState(p.label(),p.callString());
    if(currentEStatePtr==nullptr) {
      currentEStatePtr=createBottomAbstractState(p.label(),p.callString());
    }
    ROSE_ASSERT(currentEStatePtr);
    
    // basic block optimization
    list<EStatePtr> newEStateList0;
    size_t pathLen=0;
    bool endStateFound=false;
    bool bbClonedState=false;
    //cout<<"DEBUG: at: "<<currentEStatePtr->label().toString()<<endl;
#if 1
    if(_passThroughOptimizationEnabled && _analyzer->getFlow()->singleSuccessorIsPassThroughLabel(currentEStatePtr->label(),_analyzer->getLabeler())) {
      // transfer to successor
      EStatePtr newEStatePtr=currentEStatePtr->cloneWithoutIO();
      currentEStatePtr=newEStatePtr;
      bbClonedState=true;
      Flow outEdges=_analyzer->getFlow()->outEdges(currentEStatePtr->label());
      ROSE_ASSERT(outEdges.size()==1);
      Edge e=*outEdges.begin();
      
      auto newEStateList0=transferEdgeEStateInPlace(e,currentEStatePtr);
      ROSE_ASSERT(newEStateList0.size()<=1);
      if(newEStateList0.size()==0) {
        delete currentEStatePtr;
        continue;
      }
      ROSE_ASSERT(newEStateList0.size()==1);
      currentEStatePtr=*newEStateList0.begin();
      while(_passThroughOptimizationEnabled && isPassThroughLabel(currentEStatePtr->label())) {
        //cout<<"DEBUG: pass through: "<<currentEStatePtr->label().toString()<<endl;
        Flow edgeSet0=_analyzer->getFlow()->outEdges(currentEStatePtr->label());
        if(edgeSet0.size()==1) {
          Edge e=*edgeSet0.begin();
          list<EStatePtr> newEStateList0;
          newEStateList0=transferEdgeEStateInPlace(e,currentEStatePtr);
          pathLen++;
          ROSE_ASSERT(newEStateList0.size()<=1);
          if(newEStateList0.size()==1) {
            currentEStatePtr=*newEStateList0.begin();
          } else {
            endStateFound=true;
            break;
          }
          ROSE_ASSERT(currentEStatePtr);
          ROSE_ASSERT(currentEStatePtr->pstate());
          ROSE_ASSERT(currentEStatePtr->label()==e.target());
        }
      }
      if(endStateFound) {
        delete currentEStatePtr;
        continue;
      }
      ROSE_ASSERT(currentEStatePtr);
      ROSE_ASSERT(currentEStatePtr->pstate());
    }
    // last state of BB must be stored
#endif

    if(bbClonedState) {
      // was cloned and modified in BB, create a clone, and store the current one
      EStatePtr oldEStatePtr=currentEStatePtr;
      setAbstractState(oldEStatePtr->label(),oldEStatePtr->getCallString(),oldEStatePtr);
      // store oldEStatePtr
      //currentEStatePtr=currentEStatePtr->cloneWithoutIO();
      _workList->push(WorkListEntry(currentEStatePtr->label(),currentEStatePtr->getCallString()));
      continue;
    }

    Flow edgeSet=_analyzer->getFlow()->outEdges(currentEStatePtr->label());
    for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
      Edge e=*i;
      //cout<<"Transfer:"<<e.source().toString()<<"=>"<<e.target().toString()<<endl;
      ROSE_ASSERT(currentEStatePtr);
      ROSE_ASSERT(currentEStatePtr->pstate());
      // check if state was already cloned in preceding basic-block section, if yes, don't clone again.
      EStatePtr newEState=nullptr;
      if(bbClonedState) {
        newEState=currentEStatePtr;
        bbClonedState=false;
      } else {
        newEState=currentEStatePtr->cloneWithoutIO();
      }
      ROSE_ASSERT(newEState);
      ROSE_ASSERT(newEState->pstate());

      list<EStatePtr> newEStateList=transferEdgeEStateInPlace(e,newEState);
      if(newEStateList.size()==0) {
        delete newEState;
        continue;
      }

      displayTransferCounter++;
      for(list<EStatePtr>::iterator nesListIter=newEStateList.begin();nesListIter!=newEStateList.end();++nesListIter) {
        EStatePtr newEStatePtr0=*nesListIter; // TEMPORARY PTR
        ROSE_ASSERT(newEStatePtr0);
        ROSE_ASSERT(newEStatePtr0->pstate());
        ROSE_ASSERT(newEStatePtr0->label()!=Labeler::NO_LABEL);
        Label lab=newEStatePtr0->label();
        CallString cs=newEStatePtr0->getCallString();
        if((true || (!_analyzer->isFailedAssertEState(newEStatePtr0)&&!_analyzer->isVerificationErrorEState(newEStatePtr0)))) {
          EStatePtr newEStatePtr=newEStatePtr0;
          ROSE_ASSERT(newEStatePtr);

          // performing merge
          EStatePtr abstractEStatePtr=getAbstractState(lab,cs);
          if(abstractEStatePtr==nullptr)
            abstractEStatePtr=createBottomAbstractState(lab,cs);

          ROSE_ASSERT(abstractEStatePtr);
          ROSE_ASSERT(abstractEStatePtr->pstate());
          if(!isBottomAbstractState(abstractEStatePtr) && _analyzer->getEStateTransferFunctions()->isApproximatedBy(newEStatePtr,abstractEStatePtr)) {
            delete newEStatePtr; // new state does not contain new information, therefore it can be deleted
            newEStatePtr=nullptr;
          } else {
            if(isJoinLabel(lab)) {
              ROSE_ASSERT(abstractEStatePtr);
              ROSE_ASSERT(abstractEStatePtr->pstate());
              ROSE_ASSERT(newEStatePtr);
              ROSE_ASSERT(newEStatePtr->pstate());
              _analyzer->getEStateTransferFunctions()->combineInPlace1st(abstractEStatePtr,const_cast<EStatePtr>(newEStatePtr));
              ROSE_ASSERT(abstractEStatePtr);
              ROSE_ASSERT(abstractEStatePtr->pstate());
              
              setAbstractState(lab,cs,abstractEStatePtr);
              delete newEStatePtr;
              ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(abstractEStatePtr->label()));
              _workList->push(WorkListEntry(abstractEStatePtr->label(),abstractEStatePtr->getCallString()));
            } else {
              setAbstractState(lab,cs,newEStatePtr);
              if(isBottomAbstractState(abstractEStatePtr)) {
                // only needs to be deleted if it is bottom estate (which was created above), otherwise it is deleted by setAbstractState
                delete abstractEStatePtr;
              }
              _workList->push(WorkListEntry(newEStatePtr->label(),newEStatePtr->getCallString()));              
            }

          }
        }
      } // end of loop on transfer function return-estates
    } // edge set iterator
    if(_analyzer->getOptionsRef().displayDiff && displayTransferCounter>=(size_t)_analyzer->getOptionsRef().displayDiff) {
      _analyzer->printStatusMessage(true);
      displayTransferCounter=0; // reset counter
    }
  } // fixpoint loop
  if (!_analyzer->isPrecise()) {
    _analyzer->_firstAssertionOccurences = list<FailedAssertion>(); //ignore found assertions if the STG is not precise
  }
  if(_analyzer->isIncompleteSTGReady()) {
    _analyzer->printStatusMessage(true);
    _analyzer->printStatusMessage("STATUS: analysis finished (incomplete STG due to specified resource restriction).",true);
  } else {
    _analyzer->printStatusMessage(true);
    _analyzer->printStatusMessage("STATUS: analysis finished (worklist is empty).",true);
  }
  //EState::checkPointAllocationHistory();
  _analyzer->printStatusMessage("STATUS: number of states stored: "+std::to_string(getNumberOfStates()),true);
}
