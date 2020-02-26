
#include "sage3basic.h"
#include "IOAnalyzer.h"
#include "CommandLineOptions.h"
#include "Solver8.h"
#include "RersCounterexample.h"
#include "CodeThornException.h"
#include "TimeMeasurement.h"
#include "CodeThornCommandLineOptions.h"

#include <unordered_set>

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility IOAnalyzer::logger;

void IOAnalyzer::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::IOAnalyzer", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

IOAnalyzer::IOAnalyzer() {
  initDiagnostics();
}

void IOAnalyzer::initializeSolver(std::string functionToStartAt,SgNode* root, bool oneFunctionOnly) {
  Analyzer::initializeSolver(functionToStartAt, root, oneFunctionOnly);
  const EState* currentEState=*(estateWorkListCurrent->begin());
  ROSE_ASSERT(currentEState);
  if(getModeLTLDriven()) {
    setStartEState(currentEState);
  }
}

void IOAnalyzer::setStartEState(const EState* estate) {
  // this function is only used in ltl-driven mode (otherwise it is not necessary)
  ROSE_ASSERT(getModeLTLDriven());
  transitionGraph.setStartEState(estate);
}

/*! 
  * \author Marc Jasper
  * \date 2014.
 */
void IOAnalyzer::extractRersIOAssertionTraces() {
  for (list<pair<int, const EState*> >::iterator i = _firstAssertionOccurences.begin(); 
       i != _firstAssertionOccurences.end(); 
       ++i ) {
    logger[TRACE]<< "STATUS: extracting trace leading to failing assertion: " << i->first << endl;
    addCounterexample(i->first, i->second);
  }
}

/*! 
  * \author Marc Jasper
  * \date 2014.
 */
void IOAnalyzer::addCounterexample(int assertCode, const EState* assertEState) {
  _counterexampleGenerator.setType(CounterexampleGenerator::TRACE_TYPE_RERS_CE);
  ExecutionTrace* trace = 
    _counterexampleGenerator.traceLeadingTo(assertEState);
  if(RersCounterexample* rersCe = dynamic_cast<RersCounterexample*>(trace)) {
    string ceString;
    if (args.getBool("counterexamples-with-output")) {
      ceString = rersCe->toRersIOString();
    } else {
      ceString = rersCe->toRersIString();
    }
    reachabilityResults.strictUpdateCounterexample(assertCode, ceString);    
    delete rersCe;
    rersCe = nullptr;
  } else {
    throw CodeThorn::Exception("Downcast to RersCounterexample unsuccessful.");
  }
}

/*! 
  * \author Marc Jasper
  * \date 2014.
 */
void IOAnalyzer::removeOutputOutputTransitions() {
  EStatePtrSet states=transitionGraph.estateSet();
  // output cannot directly follow another output in RERS programs. Erase those transitions
  for(EStatePtrSet::iterator i=states.begin(); i!=states.end(); ++i) {
    if ((*i)->io.isStdOutIO()) {
      TransitionPtrSet inEdges = transitionGraph.inEdges(*i);
      for(TransitionPtrSet::iterator k=inEdges.begin();k!=inEdges.end();++k) {
        const EState* pred = (*k)->source;
        if (pred->io.isStdOutIO()) {
          transitionGraph.erase(**k);
          logger[DEBUG]<< "erased an output -> output transition." << endl;
        }
      }
    }
  }
}

/*! 
  * \author Marc Jasper
  * \date 2014.
 */
void IOAnalyzer::removeInputInputTransitions() {
  EStatePtrSet states=transitionGraph.estateSet();
  // input cannot directly follow another input in RERS'14 programs. Erase those transitions
  for(EStatePtrSet::iterator i=states.begin();i!=states.end();++i) {
    if ((*i)->io.isStdInIO()) {
      TransitionPtrSet outEdges = transitionGraph.outEdges(*i);
      for(TransitionPtrSet::iterator k=outEdges.begin();k!=outEdges.end();++k) {
        const EState* succ = (*k)->target;
        if (succ->io.isStdInIO()) {
          transitionGraph.erase(**k);
          logger[DEBUG]<< "erased an input -> input transition." << endl;
        }
      }
    }
  }
}

/*! 
  * \author Marc Jasper
  * \date 2014.
 */
void IOAnalyzer::pruneLeaves() {
  cout<<"INFO: pruning leave nodes in transition graph."<<endl;
  size_t numPrunedNodes=0;
  EStatePtrSet states=transitionGraph.estateSet();
  unordered_set<EState*> workset;
  //insert all states into the workset
  for(EStatePtrSet::iterator i=states.begin();i!=states.end();++i) {
    workset.insert(const_cast<EState*> (*i));
  }
  //process the workset. if extracted state is a leaf, remove it and add its predecessors to the workset
  while (workset.size() != 0) {
    EState* current = (*workset.begin());
    if (transitionGraph.succ(current).size() == 0) {
      EStatePtrSet preds = transitionGraph.pred(current);
      for (EStatePtrSet::iterator iter = preds.begin(); iter != preds.end(); ++iter)  {
        workset.insert(const_cast<EState*> (*iter));
      }
      transitionGraph.reduceEState2(current);
      numPrunedNodes++;
    }
    workset.erase(current);
  }
  cout<<"INFO: pruned leave nodes: "<<numPrunedNodes<<endl;
}

/*!
 * \author Marc Jasper
 * \date 2014.
 */
void IOAnalyzer::reduceToObservableBehavior() {
  EStatePtrSet states=transitionGraph.estateSet();
  // iterate over all states, reduce those that are neither the start state nor contain input/output/error behavior
  for(EStatePtrSet::iterator i=states.begin();i!=states.end();++i) {
    if( (*i) != transitionGraph.getStartEState() ) {
      if(! ((*i)->io.isStdInIO() || (*i)->io.isStdOutIO() || (*i)->io.isStdErrIO() || (*i)->io.isFailedAssertIO()) ) {
       transitionGraph.reduceEState2(*i);
      }
    }
  }
}

/*! 
  * \author Marc Jasper
  * \date 2014, 2015.
 */
void IOAnalyzer::setAnalyzerToSolver8(EState* startEState, bool resetAnalyzerData) {
  ROSE_ASSERT(startEState);
  //set attributes specific to solver 8
  _numberOfThreadsToUse = 1;
  setSolver(new Solver8());
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
    estateWorkListCurrent = &newEStateWorkList;
    TransitionGraph newTransitionGraph;
    transitionGraph = newTransitionGraph;
    Label startLabel=cfanalyzer->getLabel(startFunRoot);
    transitionGraph.setStartLabel(startLabel);
    list<int> newInputSequence;
    _inputSequence = newInputSequence;
    resetInputSequenceIterator();
    estateSet.max_load_factor(0.7);
    pstateSet.max_load_factor(0.7);
    constraintSetMaintainer.max_load_factor(0.7);
  }
  // initialize worklist
  const EState* currentEState=processNewOrExisting(*startEState);
  ROSE_ASSERT(currentEState);
  variableValueMonitor.init(currentEState);
  addToWorkList(currentEState);
  //cout << "STATUS: start state: "<<currentEState->toString(&variableIdMapping)<<endl;
  //cout << "STATUS: reset to solver 8 finished."<<endl;
}

/*! 
  * \author Marc Jasper
  * \date 2014, 2015.
 */
void IOAnalyzer::continueAnalysisFrom(EState * newStartEState) {
  ROSE_ASSERT(newStartEState);
  addToWorkList(newStartEState);
  // connect the latest output state with the state where the analysis stopped due to missing
  // values in the input sequence
  ROSE_ASSERT(_latestOutputEState);
  ROSE_ASSERT(_estateBeforeMissingInput);
  Edge edge(_latestOutputEState->label(),EDGE_PATH,_estateBeforeMissingInput->label());
  Transition transition(_latestOutputEState,edge,_estateBeforeMissingInput);
  transitionGraph.add(transition);
  runSolver();
}

void IOAnalyzer::resetAnalysis() {
  Analyzer::resetAnalysis();
  _prevStateSetSizeDisplay = 0;
  _prevStateSetSizeResource = 0;
}
