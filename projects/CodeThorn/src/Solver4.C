#include "sage3basic.h"
#include "Solver4.h"
#include "Analyzer.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility Solver4::logger;
// initialize static member flag
bool Solver4::_diagnosticsInitialized = false;

Solver4::Solver4() {
  initDiagnostics();
}

int Solver4::getId() {
  return 4;
}
    
/*! 
  * \author Markus Schordan
  * \date 2012.
 */

// algorithm 4 also records reachability for incomplete STGs (analyzer::reachabilityResults)
// this solver support semantic folding (solver 5 does not)
void Solver4::run() {
  _analyzer->reachabilityResults.init(_analyzer->getNumberOfErrorLabels()); // set all reachability results to unknown
  size_t prevStateSetSize=0; // force immediate report at start
  int analyzedSemanticFoldingNode=0;
  int threadNum;
  vector<const EState*> workVector(_analyzer->_numberOfThreadsToUse);
  int workers=_analyzer->_numberOfThreadsToUse;
#ifdef _OPENMP
  omp_set_dynamic(0);     // Explicitly disable dynamic teams
  omp_set_num_threads(workers);
#endif
  logger[TRACE]<<"STATUS: Running parallel solver 4 with "<<workers<<" threads."<<endl;
  _analyzer->printStatusMessage(true);
  while(1) {
    if(_analyzer->_displayDiff && (_analyzer->estateSet.size()>(prevStateSetSize+_analyzer->_displayDiff))) {
      _analyzer->printStatusMessage(true);
      prevStateSetSize=_analyzer->estateSet.size();
    }
    if(_analyzer->isEmptyWorkList())
      break;
#pragma omp parallel for private(threadNum)
    for(int j=0;j<workers;++j) {
#ifdef _OPENMP
      threadNum=omp_get_thread_num();
#endif
      const EState* currentEStatePtr=_analyzer->popWorkList();
      if(!currentEStatePtr) {
        //cerr<<"Thread "<<threadNum<<" found empty worklist. Continue without work. "<<endl;
        ROSE_ASSERT(threadNum>=0 && threadNum<=_analyzer->_numberOfThreadsToUse);
      } else {
        ROSE_ASSERT(currentEStatePtr);

        Flow edgeSet=_analyzer->flow.outEdges(currentEStatePtr->label());
        // logger[DEBUG]<< "edgeSet size:"<<edgeSet.size()<<endl;
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
          list<EState> newEStateList;
          newEStateList=_analyzer->transferEdgeEState(e,currentEStatePtr);
          if(_analyzer->isTerminationRelevantLabel(e.source())) {
            #pragma omp atomic
            analyzedSemanticFoldingNode++;
          }

          // logger[DEBUG]<< "transfer at edge:"<<e.toString()<<" succ="<<newEStateList.size()<< endl;
          for(list<EState>::iterator nesListIter=newEStateList.begin();
              nesListIter!=newEStateList.end();
              ++nesListIter) {
            // newEstate is passed by value (not created yet)
            EState newEState=*nesListIter;
            ROSE_ASSERT(newEState.label()!=Labeler::NO_LABEL);
            if((!newEState.constraints()->disequalityExists()) &&(!_analyzer->isFailedAssertEState(&newEState))&&(!_analyzer->isVerificationErrorEState(&newEState))) {
              HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=_analyzer->process(newEState);
              const EState* newEStatePtr=pres.second;
              if(pres.first==true)
                _analyzer->addToWorkList(newEStatePtr);
              _analyzer->recordTransition(currentEStatePtr,e,newEStatePtr);
            }
	    if((!newEState.constraints()->disequalityExists()) && _analyzer->isVerificationErrorEState(&newEState)) {
	      // verification error is handled as property 0
              const EState* newEStatePtr;
#pragma omp critical(REACHABILITY)
	      {
		newEStatePtr=_analyzer->processNewOrExisting(newEState);
		_analyzer->recordTransition(currentEStatePtr,e,newEStatePtr);
		_analyzer->reachabilityResults.reachable(0);
	      }
	    } else if((!newEState.constraints()->disequalityExists()) && _analyzer->isFailedAssertEState(&newEState)) {
              // failed-assert end-state: do not add to work list but do add it to the transition graph
              const EState* newEStatePtr;
#pragma omp critical(REACHABILITY)
                {
		  newEStatePtr=_analyzer->processNewOrExisting(newEState);
              _analyzer->recordTransition(currentEStatePtr,e,newEStatePtr);
		}
              // record reachability
              int assertCode=_analyzer->reachabilityAssertCode(currentEStatePtr);
              if(assertCode>=0) {
#pragma omp critical(REACHABILITY)
                {
                  _analyzer->reachabilityResults.reachable(assertCode);
                }
	      } else {
                // assert without label
	      }
            }
          } // end of loop on transfer function return-estates
        } // just for proper auto-formatting in emacs
      } // conditional: test if work is available
    } // worklist-parallel for
    if(args.getBool("semantic-fold")) {
      if(analyzedSemanticFoldingNode>_analyzer->_semanticFoldThreshold) {
        _analyzer->semanticFoldingOfTransitionGraph();
        analyzedSemanticFoldingNode=0;
        prevStateSetSize=_analyzer->estateSet.size();
      }
    }
    if(_analyzer->_displayDiff && (_analyzer->estateSet.size()>(prevStateSetSize+_analyzer->_displayDiff))) {
      _analyzer->printStatusMessage(true);
      prevStateSetSize=_analyzer->estateSet.size();
    }
    if(_analyzer->isIncompleteSTGReady()) {
      // ensure that the STG is folded properly when finished
      if(args.getBool("semantic-fold")) {
        _analyzer->semanticFoldingOfTransitionGraph();
      }
      // we report some information and finish the algorithm with an incomplete STG
      cout << "-------------------------------------------------"<<endl;
      cout << "STATUS: finished with incomplete STG (as planned)"<<endl;
      cout << "-------------------------------------------------"<<endl;
      return;
    }
  } // while
  // ensure that the STG is folded properly when finished
  if(args.getBool("semantic-fold")) {
    _analyzer->semanticFoldingOfTransitionGraph();
  }
  _analyzer->reachabilityResults.finished(); // sets all unknown entries to NO.
  _analyzer->printStatusMessage(true);
  cout << "analysis finished (worklist is empty)."<<endl;
}

void Solver4::initDiagnostics() {
  if (!_diagnosticsInitialized) {
    _diagnosticsInitialized = true;
    Solver::initDiagnostics(logger, getId());
  }
}
