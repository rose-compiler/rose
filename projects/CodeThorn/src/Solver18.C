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
  for(auto entry : _summaryCSStateMapMap) {
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
  return (_summaryCSStateMapMap.find(lab.getId())==_summaryCSStateMapMap.end())&&lab!=_analyzer->getFlow()->getStartLabel()&&!isPassThroughLabel(lab);
}

bool Solver18::isReachableLabel(Label lab) {
  return !isUnreachableLabel(lab);
}

void Solver18::initializeSummaryStatesFromWorkList() {
  // pop all states from worklist (can contain more than one state)
  list<EStatePtr> tmpWL;
  while(!_analyzer->isEmptyWorkList()) {
    EStatePtr estate=_analyzer->popWorkList();
    ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(estate->label()));
    tmpWL.push_back(estate);
  }
  for(auto s : tmpWL) {
    // initialize summarystate and push back to work lis
    ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(s->label()));
    setSummaryState(s->label(),s->callString,s);
    _workList->push(WorkListEntry(s->label(),s->callString));
  }
  /* the other states are not initialized here because every context
     (cs) requires its own bottom element and contexts ar computedat
     analysis run time */
}

size_t Solver18::getNumberOfStates() {
  return _numberOfStates;
}

EStatePtr Solver18::getSummaryState(CodeThorn::Label lab, CodeThorn::CallString cs) {
  EStatePtr res;
#pragma omp critical(SUMMARY_STATES)
  {
    auto iter1=_summaryCSStateMapMap.find(lab.getId());
    if(iter1==_summaryCSStateMapMap.end()) {
      res=nullptr;
    } else {
      SummaryCSStateMap& summaryCSStateMap=(*iter1).second;
      auto iter2=summaryCSStateMap.find(cs);
      if(iter2==summaryCSStateMap.end()) {
        res=nullptr;
      } else {
        res=(*iter2).second;
      }
    }
  }
  return res;
}

void Solver18::setSummaryState(CodeThorn::Label lab, CodeThorn::CallString cs, EStatePtr estate) {
  ROSE_ASSERT(lab==estate->label());
  ROSE_ASSERT(cs==estate->callString);
  ROSE_ASSERT(estate);

  if(_abstractionConsistencyCheckEnabled) {
    auto vim=_analyzer->getVariableIdMapping();
    auto nonPointerAddresses=estate->checkArrayAbstractionIndexConsistency(vim->getArrayAbstractionIndex(),vim);
    if(nonPointerAddresses>0) {
      cout<<"WARNING: number of non-pointer addresses at "<<estate->label().toString()<<": "<<nonPointerAddresses<<endl;
    }
  }
  //pair<int,CallString> p(lab.getId(),cs);
  //_summaryCSStateMap[p]=estate;
#pragma omp critical(SUMMARY_STATES)
  {
    auto iter1=_summaryCSStateMapMap.find(lab.getId());
    if(iter1==_summaryCSStateMapMap.end()) {
      // create new
      SummaryCSStateMap newSummaryCSStateMap;
      newSummaryCSStateMap[cs]=estate;
      _summaryCSStateMapMap[lab.getId()]=newSummaryCSStateMap;
      _numberOfStates++;
    } else {
      SummaryCSStateMap& summaryCSStateMap=(*iter1).second;
      auto iter2=summaryCSStateMap.find(cs);
      if(iter2==summaryCSStateMap.end()) {
        summaryCSStateMap[cs]=estate;
        //_numberOfStates++;
      } else {
        // context already exists, re-set state, no additional state added
        auto currentEState=(*iter2).second;
        if(currentEState!=estate && currentEState!=nullptr) {
          delete summaryCSStateMap[cs];
        }
        summaryCSStateMap[cs]=estate;
      }
    }
  }
}

// creates bottom state
EStatePtr Solver18::createBottomSummaryState(Label lab, CallString cs) {
  InputOutput io;
  io.recordBot();
  EState estate(lab,cs,new PState(),io);
  ROSE_ASSERT(!createsTransitionSystem());
  return new EState(estate);
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

void Solver18::run() {
  SAWYER_MESG(logger[INFO])<<"Running solver "<<getId()<<" (sharedpstates:"<<_analyzer->getOptionsRef().sharedPStates<<")"<<endl;
  ROSE_ASSERT(_analyzer);
  if(_analyzer->getOptionsRef().abstractionMode==0) {
    cerr<<"Error: abstraction mode is 0, but >= 1 required."<<endl;
    exit(1);
  }
  if(_analyzer->getOptionsRef().explorationMode!="topologic-sort") {
    cerr<<"Error: topologic-sort required for exploration mode, but it is "<<_analyzer->getOptionsRef().explorationMode<<endl;
    exit(1);
  }
  ROSE_ASSERT(_analyzer->getTopologicalSort());
  if(_workList==nullptr) {
    _workList=new GeneralPriorityWorkList<Solver18::WorkListEntry>(_analyzer->getTopologicalSort()->labelToPriorityMap());
  }

  initializeSummaryStatesFromWorkList();

  size_t displayTransferCounter=0;
  bool terminateEarly=false;
  _analyzer->printStatusMessage(true);
  while(!_workList->empty()) {
    auto p=_workList->top();
    _workList->pop();
    // terminate early, ensure to stop all threads and empty the worklist (e.g. verification error found).
    if(terminateEarly)
      continue;
    
    ROSE_ASSERT(p.label().isValid());
    ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(p.label()));
    EStatePtr currentEStatePtr=getSummaryState(p.label(),p.callString());
    if(currentEStatePtr==nullptr) {
      currentEStatePtr=createBottomSummaryState(p.label(),p.callString());
    }
    ROSE_ASSERT(currentEStatePtr);
    
    // basic block optimization
    list<EStatePtr> newEStateList0;
    size_t pathLen=0;
    bool endStateFound=false;
    bool bbClonedState=false;
    //cout<<"DEBUG: at: "<<currentEStatePtr->label().toString()<<endl;
#if 1
    if(_analyzer->getFlow()->singleSuccessorIsPassThroughLabel(currentEStatePtr->label(),_analyzer->getLabeler())) {
      // transfer to successor
      EStatePtr newEStatePtr=currentEStatePtr->clone();
      currentEStatePtr=newEStatePtr;
      bbClonedState=true;
      Flow outEdges=_analyzer->getFlow()->outEdges(currentEStatePtr->label());
      ROSE_ASSERT(outEdges.size()==1);
      Edge e=*outEdges.begin();
      
      auto newEStateList0=_analyzer->transferEdgeEStateInPlace(e,currentEStatePtr);
      ROSE_ASSERT(newEStateList0.size()<=1);
      if(newEStateList0.size()==0) {
        delete currentEStatePtr;
        continue;
      }
      ROSE_ASSERT(newEStateList0.size()==1);
      currentEStatePtr=*newEStateList0.begin();
      while(true && isPassThroughLabel(currentEStatePtr->label())) {
        //cout<<"DEBUG: pass through: "<<currentEStatePtr->label().toString()<<endl;
        Flow edgeSet0=_analyzer->getFlow()->outEdges(currentEStatePtr->label());
        if(edgeSet0.size()==1) {
          Edge e=*edgeSet0.begin();
          list<EStatePtr> newEStateList0;
          newEStateList0=_analyzer->transferEdgeEStateInPlace(e,currentEStatePtr);
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
#endif
    Flow edgeSet=_analyzer->getFlow()->outEdges(currentEStatePtr->label());
    for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
      Edge e=*i;
      //cout<<"Transfer:"<<e.source().toString()<<"=>"<<e.target().toString()<<endl;
      printAllocationStats("P1:");

      ROSE_ASSERT(currentEStatePtr);
      ROSE_ASSERT(currentEStatePtr->pstate());

      // check if state was already cloned in preceding basic-block section
      EStatePtr newEState=nullptr;
      if(bbClonedState) {
        newEState=currentEStatePtr->clone();
        delete currentEStatePtr;
        
      } else
        newEState=currentEStatePtr->clone();

      ROSE_ASSERT(newEState);
      ROSE_ASSERT(newEState->pstate());

      printAllocationStats("P2:");
      //auto checkDiff1=checkDiff();
      list<EStatePtr> newEStateList=_analyzer->transferEdgeEStateInPlace(e,newEState);
      if(newEStateList.size()==0) {
        delete newEState;
        printAllocationStats("P2b:");
        continue;
      }
      printAllocationStats("P3:");
      //auto checkDiff2=checkDiff();
      //if(checkDiff1!=checkDiff2 && newEStateList.size()<=1) {
      //  cerr<<"Solver18: FAIL1: "<<checkDiff1<<":"<<checkDiff2<<" :"<< newEStateList.size()<<endl;
      //  exit(1);
      //}
      displayTransferCounter++;
      for(list<EStatePtr>::iterator nesListIter=newEStateList.begin();nesListIter!=newEStateList.end();++nesListIter) {
        EStatePtr newEStatePtr0=*nesListIter; // TEMPORARY PTR
        ROSE_ASSERT(newEStatePtr0);
        ROSE_ASSERT(newEStatePtr0->pstate());
        ROSE_ASSERT(newEStatePtr0->label()!=Labeler::NO_LABEL);
        Label lab=newEStatePtr0->label();
        CallString cs=newEStatePtr0->callString;
        printAllocationStats("P4:");
        if((!_analyzer->isFailedAssertEState(newEStatePtr0)&&!_analyzer->isVerificationErrorEState(newEStatePtr0))) {
          EStatePtr newEStatePtr=newEStatePtr0;
          ROSE_ASSERT(newEStatePtr);
          // performing merge
          EStatePtr summaryEStatePtr=getSummaryState(lab,cs);
          if(summaryEStatePtr==nullptr)
            summaryEStatePtr=createBottomSummaryState(lab,cs);

          ROSE_ASSERT(summaryEStatePtr);
          ROSE_ASSERT(summaryEStatePtr->pstate());
          if(_analyzer->getEStateTransferFunctions()->isApproximatedBy(newEStatePtr,summaryEStatePtr)) {
            printAllocationStats("P5a:");
            delete newEStatePtr; // new state does not contain new information, therefore it can be deleted
            printAllocationStats("P5b:");
            newEStatePtr=nullptr;
          } else {
            printAllocationStats("P6a:");
            ROSE_ASSERT(summaryEStatePtr);
            ROSE_ASSERT(summaryEStatePtr->pstate());
            ROSE_ASSERT(newEStatePtr);
            ROSE_ASSERT(newEStatePtr->pstate());
            _analyzer->getEStateTransferFunctions()->combineInPlace1st(summaryEStatePtr,const_cast<EStatePtr>(newEStatePtr));
            ROSE_ASSERT(summaryEStatePtr);
            ROSE_ASSERT(summaryEStatePtr->pstate());

            printAllocationStats("P6b:");
            setSummaryState(lab,cs,summaryEStatePtr);
            printAllocationStats("P6c:");
            delete newEStatePtr;
            printAllocationStats("P6d:");
            ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(summaryEStatePtr->label()));
            _workList->push(WorkListEntry(summaryEStatePtr->label(),summaryEStatePtr->callString));

          }
        }
        printAllocationStats("P7:");
        /*
        if(((_analyzer->isFailedAssertEState(newEStatePtr0))||_analyzer->isVerificationErrorEState(newEStatePtr0))) {
          // failed-assert end-state: do not add to work list but do add it to the transition graph
          EStatePtr newEStatePtr=newEStatePtr0;
          if(_analyzer->isVerificationErrorEState(newEStatePtr)) {
            SAWYER_MESG(logger[TRACE]) <<"STATUS: detected verification error state ... terminating early"<<endl;
            // set flag for terminating early
            _analyzer->reachabilityResults.reachable(0);
            _analyzer->_firstAssertionOccurences.push_back(pair<int, EStatePtr>(0, newEStatePtr));
            terminateEarly=true;
          } else if(_analyzer->isFailedAssertEState(newEStatePtr)) {
            printAllocationStats("P8a:");
            delete newEStatePtr;
            printAllocationStats("P8b:");
            continue;
          } // end of failed assert handling
        } // end of if
        */
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
  EState::checkPointAllocationHistory();
  _analyzer->printStatusMessage("STATUS: number of states: "+std::to_string(getNumberOfStates()),true);
  _analyzer->printStatusMessage("STATUS: "+EState::allocationStatsToString(),true);
}
