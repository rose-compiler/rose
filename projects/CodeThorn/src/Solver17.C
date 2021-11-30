#include "sage3basic.h"
#include "Solver17.h"
#include "CTAnalysis.h"
#include "CodeThornCommandLineOptions.h"
#include "EStateTransferFunctions.h"
#include <limits>
#include <unordered_set>

using namespace std;
using namespace CodeThorn;
using namespace Sawyer::Message;

#include "CTAnalysis.h"

Sawyer::Message::Facility Solver17::logger;
// initialize static member flag
bool Solver17::_diagnosticsInitialized = false;

Solver17::Solver17() {
  initDiagnostics();
}

int Solver17::getId() {
  return 17;
}
    
void Solver17::recordTransition(EStatePtr currentEStatePtr0,EStatePtr currentEStatePtr,Edge e, EStatePtr newEStatePtr) {
  _analyzer->recordTransition(currentEStatePtr,e,newEStatePtr);
  if(currentEStatePtr0!=currentEStatePtr) {
    // also add transition edge for the state from
    // worklist if it is different to the summary state
    // (to which an edge must exist in the STS)
    Edge e0(currentEStatePtr0->label(),e.getTypes(),newEStatePtr->label());
    e0.setAnnotation(e.getAnnotation());
    _analyzer->recordTransition(currentEStatePtr0,e0,newEStatePtr);
  }
}

void Solver17::initializeSummaryStatesFromWorkList() {
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
    _analyzer->addToWorkList(s);
  }
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void Solver17::run() {
  SAWYER_MESG(logger[INFO])<<"Running solver "<<getId()<<endl;
  ROSE_ASSERT(_analyzer);
  if(_analyzer->getOptionsRef().abstractionMode==0) {
    cerr<<"Error: abstraction mode is 0, but >= 1 required."<<endl;
    exit(1);
  }
  if(_analyzer->getOptionsRef().explorationMode!="topologic-sort") {
    cerr<<"Error: topologic-sort required for exploration mode, but it is "<<_analyzer->getOptionsRef().explorationMode<<endl;
    exit(1);
  }

  initializeSummaryStatesFromWorkList();

  size_t prevStateSetSize=0; // force immediate report at start
  if(_analyzer->getOptionsRef().getInterProceduralFlag()==false) {
    prevStateSetSize+=_analyzer->getOptionsRef().displayDiff; // skip report at start
  }

  int threadNum=0;
  int workers=_analyzer->getOptionsRef().threads;
  vector<bool> workVector(workers);
  _analyzer->set_finished(workVector,true);
  bool terminateEarly=false;
  //omp_set_dynamic(0);     // Explicitly disable dynamic teams
  omp_set_num_threads(workers);

  SAWYER_MESG(logger[TRACE])<<"STATUS: Running parallel solver "<<getId()<<" with "<<workers<<" threads."<<endl;
  _analyzer->printStatusMessage(true);
# pragma omp parallel shared(workVector) private(threadNum)
  {
    threadNum=omp_get_thread_num();
    while(!_analyzer->all_false(workVector)) {
      // SAWYER_MESG(logger[DEBUG])<<"running : WL:"<<estateWorkListCurrent->size()<<endl;
      if(threadNum==0 && _analyzer->getOptionsRef().displayDiff && (_analyzer->getEStateSetSize()>(prevStateSetSize+_analyzer->getOptionsRef().displayDiff))) {
        _analyzer->printStatusMessage(true);
        prevStateSetSize=_analyzer->getEStateSetSize();
      }
      if(_analyzer->isEmptyWorkList()||_analyzer->isIncompleteSTGReady()) {
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
      // currentEStatePtr0 is not merged, because it must already be present in a summary state. Here only the (label,callstring) is used to obtain the summary state.
      // the worklist could be reduced to (label,callstring) pairs, but since it's also used for explicit model checking, it uses pointers to estates, which include some more info.
      // note: initial summary states are set in initializeSummaryStatesFromWorkList()
      if(!_analyzer->isEmptyWorkList()) {
        EStatePtr currentEStatePtr0=_analyzer->popWorkList();
        // terminate early, ensure to stop all threads and empty the worklist (e.g. verification error found).
        if(terminateEarly)
          continue;
        if(!currentEStatePtr0) {
          // empty worklist. Continue without work.
          ROSE_ASSERT(threadNum>=0 && threadNum<=_analyzer->getOptionsRef().threads);
        } else {
          ROSE_ASSERT(currentEStatePtr0);
          ROSE_ASSERT(currentEStatePtr0->label().isValid());
          ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(currentEStatePtr0->label()));
          //cout<<"DEBUG: getSummaryState: lab:"<<currentEStatePtr0->label().toString()<<" cs:"<<currentEStatePtr0->callString.toString()<<endl;
          EStatePtr currentEStatePtr=_analyzer->getSummaryState(currentEStatePtr0->label(),currentEStatePtr0->callString);
          ROSE_ASSERT(currentEStatePtr);
          
          //cout<<"DEBUG: get out edges at: "<<currentEStatePtr->label().toString()<<endl;
          Flow edgeSet=_analyzer->getFlow()->outEdges(currentEStatePtr->label());
          //cout << "DEBUG: out-edgeSet size:"<<edgeSet.size()<<endl;
          for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
            Edge e=*i;
            list<EStatePtr> newEStateList=_analyzer->transferEdgeEState(e,currentEStatePtr);
            for(list<EStatePtr>::iterator nesListIter=newEStateList.begin();
                nesListIter!=newEStateList.end();
                ++nesListIter) {
              // newEstate is passed by value (not created yet)
              EStatePtr newEStatePtr0=*nesListIter; // TEMPORARY PTR
              ROSE_ASSERT(newEStatePtr0->label()!=Labeler::NO_LABEL);
              if((!_analyzer->isFailedAssertEState(newEStatePtr0)&&!_analyzer->isVerificationErrorEState(newEStatePtr0))) {
                EStatePtr newEStatePtr=newEStatePtr0;
                ROSE_ASSERT(newEStatePtr);
                if(true) {
                  int abstractionMode=_analyzer->getAbstractionMode();
                  switch(abstractionMode) {
                  case 1: {
                    // performing merge
                    bool addToWorkListFlag=false;
#pragma omp critical(SUMMARY_STATES_MAP)
                    {
                      Label lab=newEStatePtr->label();
                      CallString cs=newEStatePtr->callString;
                      EStatePtr summaryEStatePtr=_analyzer->getSummaryState(lab,cs);
                      ROSE_ASSERT(summaryEStatePtr);
                      if(_analyzer->getEStateTransferFunctions()->isApproximatedBy(newEStatePtr,summaryEStatePtr)) {
                        delete newEStatePtr; // new state does not contain new information, therefore it can be deleted
                        addToWorkListFlag=false; // nothing to do (flag required because of OpenMP block, continue not allowed)
                        newEStatePtr=nullptr;
                      } else {
                        EState newCombinedSummaryEState=_analyzer->getEStateTransferFunctions()->combine(summaryEStatePtr,const_cast<EStatePtr>(newEStatePtr));
                        EStatePtr newCombinedSummaryEStatePtr=new EState(newCombinedSummaryEState);
                        newCombinedSummaryEStatePtr->setLabel(lab);
                        newCombinedSummaryEStatePtr->setCallString(cs);
                        _analyzer->setSummaryState(lab,cs,newCombinedSummaryEStatePtr);
                        delete summaryEStatePtr;
                        delete newEStatePtr;
                        addToWorkListFlag=true;
                        newEStatePtr=new EState(*newCombinedSummaryEStatePtr); // ensure summary state ptrs are not added to the work list (avoid aliasing)
                      }
                    }
                    ROSE_ASSERT(((addToWorkListFlag==true && newEStatePtr!=nullptr)||(addToWorkListFlag==false&&newEStatePtr==nullptr)));
                    if(addToWorkListFlag) {
                      ROSE_ASSERT(_analyzer->getLabeler()->isValidLabelIdRange(newEStatePtr->label()));
                      _analyzer->addToWorkList(newEStatePtr);  // uses its own omp synchronization, do not mix with above
                    }
                    break;
                  }
                  default:
                    cerr<<"Error: unknown abstraction mode in Solver 17: "<<abstractionMode<<endl;
                    exit(1);
                  }
                } else {
                  //cout<<"DEBUG: pres.first==false (not adding estate to worklist)"<<endl;
                }
              }
              if(((_analyzer->isFailedAssertEState(newEStatePtr0
                                                   ))||_analyzer->isVerificationErrorEState(newEStatePtr0))) {
                // failed-assert end-state: do not add to work list but do add it to the transition graph
                EStatePtr newEStatePtr=newEStatePtr0;
                if(_analyzer->isVerificationErrorEState(newEStatePtr)) {
#pragma omp critical
                  {
                    SAWYER_MESG(logger[TRACE]) <<"STATUS: detected verification error state ... terminating early"<<endl;
                    // set flag for terminating early
                    _analyzer->reachabilityResults.reachable(0);
                    _analyzer->_firstAssertionOccurences.push_back(pair<int, EStatePtr>(0, newEStatePtr));
                    terminateEarly=true;
                  }
                } else if(_analyzer->isFailedAssertEState(newEStatePtr)) {
                  delete newEStatePtr;
                  continue;
                } // end of failed assert handling
              } // end of if
            } // end of loop on transfer function return-estates
          } // edge set iterator
        } // conditional: test if work is available
      } // while
    } // omp parallel
  }
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

void Solver17::initDiagnostics() {
  if (!_diagnosticsInitialized) {
    _diagnosticsInitialized = true;
    Solver::initDiagnostics(logger, 16);
  }
}
