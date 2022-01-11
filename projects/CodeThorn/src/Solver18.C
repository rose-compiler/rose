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

int Solver18::getId() {
  return 18;
}

// allows to handle sequences of nodes as basic blocks
bool Solver18::isPassThroughLabel(Label lab) {
  return _analyzer->isPassThroughLabel(lab);
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
    _analyzer->setSummaryState(s->label(),s->callString,new EState(*s)); // ensure summary states are never added to the worklist
    //_analyzer->addToWorkList(s);
    _workList->push(WorkListEntry(s->label(),s->callString));
  }
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
    //EStatePtr currentEStatePtr0=_analyzer->popWorkList();
    // terminate early, ensure to stop all threads and empty the worklist (e.g. verification error found).
    if(terminateEarly)
      continue;
    
    ROSE_ASSERT(p.label().isValid());
    ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(p.label()));
    EStatePtr currentEStatePtr=_analyzer->getSummaryState(p.label(),p.callString());
    ROSE_ASSERT(currentEStatePtr);
    
    list<EStatePtr> newEStateList0;
    size_t pathLen=0;
    while(isPassThroughLabel(currentEStatePtr->label())) {
      //cout<<"DEBUG: pass through: "<<currentEStatePtr->label().toString()<<endl;
      Flow edgeSet0=_analyzer->getFlow()->outEdges(currentEStatePtr->label());
      if(edgeSet0.size()==1) {
        Edge e=*edgeSet0.begin();
        list<EStatePtr> newEStateList0;
        if(pathLen==0) {
          newEStateList0=_analyzer->transferEdgeEState(e,currentEStatePtr);
        } else {
          newEStateList0=_analyzer->transferEdgeEStateInPlace(e,currentEStatePtr);
          pathLen++;
        }
        if(newEStateList0.size()==1) {
          currentEStatePtr=*newEStateList0.begin();
        } else {
          break;
        }
      }
    }

    Flow edgeSet=_analyzer->getFlow()->outEdges(currentEStatePtr->label());
    for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
      Edge e=*i;
      //cout<<"Transfer:"<<e.source().toString()<<"=>"<<e.target().toString()<<endl;
      list<EStatePtr> newEStateList=_analyzer->transferEdgeEState(e,currentEStatePtr);
      displayTransferCounter++;
      for(list<EStatePtr>::iterator nesListIter=newEStateList.begin();nesListIter!=newEStateList.end();++nesListIter) {
        // newEstate is passed by value (not created yet)
        EStatePtr newEStatePtr0=*nesListIter; // TEMPORARY PTR
        ROSE_ASSERT(newEStatePtr0->label()!=Labeler::NO_LABEL);
        Label lab=newEStatePtr0->label();
        CallString cs=newEStatePtr0->callString;
        if((!_analyzer->isFailedAssertEState(newEStatePtr0)&&!_analyzer->isVerificationErrorEState(newEStatePtr0))) {
          EStatePtr newEStatePtr=newEStatePtr0;
          ROSE_ASSERT(newEStatePtr);
          // performing merge
          EStatePtr summaryEStatePtr=_analyzer->getSummaryState(lab,cs);
          ROSE_ASSERT(summaryEStatePtr);
          if(_analyzer->getEStateTransferFunctions()->isApproximatedBy(newEStatePtr,summaryEStatePtr)) {
            delete newEStatePtr; // new state does not contain new information, therefore it can be deleted
            newEStatePtr=nullptr;
          } else {
            EState newCombinedSummaryEState=_analyzer->getEStateTransferFunctions()->combine(summaryEStatePtr,const_cast<EStatePtr>(newEStatePtr));
            EStatePtr newCombinedSummaryEStatePtr=new EState(newCombinedSummaryEState);
            newCombinedSummaryEStatePtr->setLabel(lab);
            newCombinedSummaryEStatePtr->setCallString(cs);
            _analyzer->setSummaryState(lab,cs,newCombinedSummaryEStatePtr);
            delete summaryEStatePtr;
            delete newEStatePtr;
            newEStatePtr=newCombinedSummaryEStatePtr;
            ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(newEStatePtr->label()));
            //_analyzer->addToWorkList(newEStatePtr);
            _workList->push(WorkListEntry(newEStatePtr->label(),newEStatePtr->callString));

          }
        }
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
            delete newEStatePtr;
            continue;
          } // end of failed assert handling
        } // end of if
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
}
