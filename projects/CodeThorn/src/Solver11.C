#include "sage3basic.h"
#include "Solver11.h"
#include "IOAnalyzer.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility Solver11::logger;
// initialize static member flag
bool Solver11::_diagnosticsInitialized = false;

Solver11::Solver11() {
  initDiagnostics();
}

int Solver11::getId() {
  return 11;
}

/*! 
 * \author Markus Schordan
 * \date 2016.
 */
void Solver11::run() {
  if(_analyzer->svCompFunctionSemantics()) {
    _analyzer->reachabilityResults.init(1); // in case of svcomp mode set single program property to unknown
  } else {
    _analyzer->reachabilityResults.init(_analyzer->getNumberOfErrorLabels()); // set all reachability results to unknown
  }
  logger[INFO]<<"number of error labels: "<<_analyzer->reachabilityResults.size()<<endl;
  size_t prevStateSetSize=0; // force immediate report at start
  logger[TRACE]<<"STATUS: Running sequential solver 11 with 1 thread."<<endl;
  _analyzer->printStatusMessage(true);
  while(!_analyzer->isEmptyWorkList()) {
    if(_analyzer->_displayDiff && (_analyzer->estateSet.size()>(prevStateSetSize+_analyzer->_displayDiff))) {
      _analyzer->printStatusMessage(true);
      prevStateSetSize=_analyzer->estateSet.size();
    }
    const EState* currentEStatePtr=_analyzer->popWorkList();
    ROSE_ASSERT(currentEStatePtr);

    Analyzer::SubSolverResultType subSolverResult= _analyzer->subSolver(currentEStatePtr);
    EStateWorkList deferedWorkList=subSolverResult.first;
    for(EStateWorkList::iterator i=deferedWorkList.begin();i!=deferedWorkList.end();++i) {
      _analyzer->addToWorkList(*i);
    }
  } // while loop
  const bool isComplete=true;
  if (!_analyzer->isPrecise()) {
    _analyzer->_firstAssertionOccurences = list<FailedAssertion>(); //ignore found assertions if the STG is not precise
  }
  if(_analyzer->isIncompleteSTGReady()) {
    _analyzer->printStatusMessage(true);
    logger[TRACE]<< "STATUS: analysis finished (incomplete STG due to specified resource restriction)."<<endl;
    _analyzer->reachabilityResults.finishedReachability(_analyzer->isPrecise(),!isComplete);
    _analyzer->transitionGraph.setIsComplete(!isComplete);
  } else {
    bool tmpcomplete=true;
    _analyzer->reachabilityResults.finishedReachability(_analyzer->isPrecise(),tmpcomplete);
    _analyzer->printStatusMessage(true);
    _analyzer->transitionGraph.setIsComplete(tmpcomplete);
    logger[TRACE]<< "analysis finished (worklist is empty)."<<endl;
  }
  _analyzer->transitionGraph.setIsPrecise(_analyzer->isPrecise());
  _analyzer->printStatusMessage(true);
  logger[TRACE]<< "analysis with solver 11 finished (worklist is empty)."<<endl;
}


void Solver11::initDiagnostics() {
  if (!_diagnosticsInitialized) {
    _diagnosticsInitialized = true;
    Solver::initDiagnostics(logger, getId());
  }
}
