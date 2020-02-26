#include "sage3basic.h"
#include "Solver5.h"
#include "Analyzer.h"
#include "CodeThornCommandLineOptions.h"

using namespace std;
using namespace CodeThorn;
using namespace Sawyer::Message;

#include "Analyzer.h"

Sawyer::Message::Facility Solver5::logger;
// initialize static member flag
bool Solver5::_diagnosticsInitialized = false;

Solver5::Solver5() {
  initDiagnostics();
}

int Solver5::getId() {
  return 5;
}
    
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void Solver5::run() {
  _analyzer->_analysisTimer.start();
  if(_analyzer->svCompFunctionSemantics()) {
    _analyzer->reachabilityResults.init(1); // in case of svcomp mode set single program property to unknown
  } else {
    _analyzer->reachabilityResults.init(_analyzer->getNumberOfErrorLabels()); // set all reachability results to unknown
  }
  logger[INFO]<<"number of error labels: "<<_analyzer->reachabilityResults.size()<<endl;
  size_t prevStateSetSize=0; // force immediate report at start
  int threadNum;
  int workers=_analyzer->_numberOfThreadsToUse;
  vector<bool> workVector(workers);
  _analyzer->set_finished(workVector,true);
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

  SAWYER_MESG(logger[TRACE])<<"STATUS: Running parallel solver 5 with "<<workers<<" threads."<<endl;
  _analyzer->printStatusMessage(true);
# pragma omp parallel shared(workVector) private(threadNum)
  {
    threadNum=omp_get_thread_num();
    while(!_analyzer->all_false(workVector)) {
      // logger[DEBUG]<<"running : WL:"<<estateWorkListCurrent->size()<<endl;
      if(threadNum==0 && _analyzer->_displayDiff && (_analyzer->estateSet.size()>(prevStateSetSize+_analyzer->_displayDiff))) {
        _analyzer->printStatusMessage(true);
        prevStateSetSize=_analyzer->estateSet.size();
      }
      //perform reduction to I/O/worklist states only if specified threshold was reached
      if (ioReductionActive) {
#pragma omp critical
        {
          if (_analyzer->estateSet.size() > (estatesLastReduction + ioReductionThreshold)) {
            _analyzer->reduceStgToInOutAssertWorklistStates();
            estatesLastReduction = _analyzer->estateSet.size();
            cout<< "STATUS: transition system reduced to I/O/worklist states. remaining transitions: " << _analyzer->transitionGraph.size() << endl;
          }
        }
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
      const EState* currentEStatePtr=_analyzer->popWorkList();
      // if we want to terminate early, we ensure to stop all threads and empty the worklist (e.g. verification error found).
      if(terminateEarly)
        continue;
      if(!currentEStatePtr) {
        //cerr<<"Thread "<<threadNum<<" found empty worklist. Continue without work. "<<endl;
        ROSE_ASSERT(threadNum>=0 && threadNum<=_analyzer->_numberOfThreadsToUse);
      } else {
        ROSE_ASSERT(currentEStatePtr);
        Flow edgeSet=_analyzer->flow.outEdges(currentEStatePtr->label());
        //cout << "DEBUG: out-edgeSet size:"<<edgeSet.size()<<endl;
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
          list<EState> newEStateList;
          newEStateList=_analyzer->transferEdgeEState(e,currentEStatePtr);
          for(list<EState>::iterator nesListIter=newEStateList.begin();
              nesListIter!=newEStateList.end();
              ++nesListIter) {
            // newEstate is passed by value (not created yet)
            EState newEState=*nesListIter;
            ROSE_ASSERT(newEState.label()!=Labeler::NO_LABEL);
            if(_analyzer->_stg_trace_filename.size()>0 && !newEState.constraints()->disequalityExists()) {
              std::ofstream fout;
              // _csv_stg_trace_filename is the member-variable of analyzer
#pragma omp critical
              {
                fout.open(_analyzer->_stg_trace_filename.c_str(),ios::app);    // open file for appending
                assert (!fout.fail( ));
                fout<<"ESTATE-IN :"<<currentEStatePtr->toString(_analyzer->getVariableIdMapping());
                string sourceString=_analyzer->getCFAnalyzer()->getLabeler()->getNode(currentEStatePtr->label())->unparseToString().substr(0,40);
                if(sourceString.size()==60) sourceString+="...";
                fout<<"\n==>"<<"TRANSFER:"<<sourceString;
                fout<<"==>\n"<<"ESTATE-OUT:"<<newEState.toString(_analyzer->getVariableIdMapping());
                fout<<endl;
                fout<<endl;
                fout.close();
              }
            }
            
            if((!newEState.constraints()->disequalityExists()) &&(!_analyzer->isFailedAssertEState(&newEState)&&!_analyzer->isVerificationErrorEState(&newEState))) {
              HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=_analyzer->process(newEState);
              const EState* newEStatePtr=pres.second;
              if(pres.first==true) {
                int abstractionMode=_analyzer->getAbstractionMode();
                switch(abstractionMode) {
                case 0:
                  // no abstraction
                  //cout<<"DEBUG: Adding estate to worklist."<<endl;
                  _analyzer->addToWorkList(newEStatePtr);
                  break;
                case 1: {
                  // performing merge
#pragma omp critical(SUMMARY_STATES_MAP)
                  {
                    const EState* summaryEState=_analyzer->getSummaryState(newEStatePtr->label(),newEStatePtr->callString);
                    if(_analyzer->isApproximatedBy(newEStatePtr,summaryEState)) {
                      // this is not a memory leak. newEStatePtr is
                      // stored in EStateSet and will be collected
                      // later. It may be already used in the state
                      // graph as an existing estate.
                      newEStatePtr=summaryEState; 
                    } else {
                      EState newEState2=_analyzer->combine(summaryEState,const_cast<EState*>(newEStatePtr));
                      ROSE_ASSERT(_analyzer);
                      HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=_analyzer->process(newEState2);
                      const EState* newEStatePtr2=pres.second;
                      if(pres.first==true) {
                        newEStatePtr=newEStatePtr2;
                      } else {
                        // nothing to do, EState already exists
                      }
                      ROSE_ASSERT(newEStatePtr);
                      _analyzer->setSummaryState(newEStatePtr->label(),newEStatePtr->callString,newEStatePtr);
                    }
                  }
                  _analyzer->addToWorkList(newEStatePtr);  
                  break;
                  case 2: 
                    cout<<"Mode 2 (topifying) not available for this option yet."<<endl;
                    exit(1);
                }
                default:
                  cerr<<"Error: unknown abstraction mode "<<abstractionMode<<endl;
                  exit(1);
                }
              } else {
                //cout<<"DEBUG: pres.first==false (not adding estate to worklist)"<<endl;
              }
              _analyzer->recordTransition(currentEStatePtr,e,newEStatePtr);
            }
            if((!newEState.constraints()->disequalityExists()) && ((_analyzer->isFailedAssertEState(&newEState))||_analyzer->isVerificationErrorEState(&newEState))) {
              // failed-assert end-state: do not add to work list but do add it to the transition graph
              const EState* newEStatePtr;
              newEStatePtr=_analyzer->processNewOrExisting(newEState);
              _analyzer->recordTransition(currentEStatePtr,e,newEStatePtr);

              if(_analyzer->isVerificationErrorEState(&newEState)) {
#pragma omp critical
                {
                  SAWYER_MESG(logger[TRACE]) <<"STATUS: detected verification error state ... terminating early"<<endl;
                  // set flag for terminating early
                  _analyzer->reachabilityResults.reachable(0);
		  _analyzer->_firstAssertionOccurences.push_back(pair<int, const EState*>(0, newEStatePtr));
                  terminateEarly=true;
                }
              } else if(_analyzer->isFailedAssertEState(&newEState)) {
                // record failed assert
                int assertCode;
                if(args.getBool("rers-binary")) {
                  assertCode=_analyzer->reachabilityAssertCode(newEStatePtr);
                } else {
                  assertCode=_analyzer->reachabilityAssertCode(currentEStatePtr);
                }
                if(assertCode>=0) {
#pragma omp critical
                  {
                    if(args.getBool("with-counterexamples") || args.getBool("with-assert-counterexamples")) {
                      //if this particular assertion was never reached before, compute and update counterexample
                      if (_analyzer->reachabilityResults.getPropertyValue(assertCode) != PROPERTY_VALUE_YES) {
                        _analyzer->_firstAssertionOccurences.push_back(pair<int, const EState*>(assertCode, newEStatePtr));
                      }
                    }
                    _analyzer->reachabilityResults.reachable(assertCode);
                  }
                } else {
                  // TODO: this is a workaround for isFailedAssert being true in case of rersmode for stderr (needs to be refined)
                  if(!args.getBool("rersmode")) {
                    // assert without label
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
  if (!_analyzer->isPrecise()) {
    _analyzer->_firstAssertionOccurences = list<FailedAssertion>(); //ignore found assertions if the STG is not precise
  }
  if(_analyzer->isIncompleteSTGReady()) {
    _analyzer->printStatusMessage(true);
    cout<< "STATUS: analysis finished (incomplete STG due to specified resource restriction)."<<endl;
    _analyzer->reachabilityResults.finishedReachability(_analyzer->isPrecise(),!isComplete);
    _analyzer->transitionGraph.setIsComplete(!isComplete);
  } else {
    bool tmpcomplete=true;
    _analyzer->reachabilityResults.finishedReachability(_analyzer->isPrecise(),tmpcomplete);
    _analyzer->printStatusMessage(true);
    _analyzer->transitionGraph.setIsComplete(tmpcomplete);
    cout<< "STATUS: analysis finished (worklist is empty)."<<endl;
  }
  _analyzer->transitionGraph.setIsPrecise(_analyzer->isPrecise());
}

void Solver5::initDiagnostics() {
  if (!_diagnosticsInitialized) {
    _diagnosticsInitialized = true;
    Solver::initDiagnostics(logger, getId());
  }
}
