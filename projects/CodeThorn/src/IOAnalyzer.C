
#include "sage3basic.h"
#include "IOAnalyzer.h"
#include "CommandLineOptions.h"
#include "Solver8.h"
#include "RersCounterexample.h"
#include "CodeThornException.h"
#include "Timer.h"

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

bool IOAnalyzer::isLTLRelevantEState(const EState* estate) {
  ROSE_ASSERT(estate);
  return ((estate)->io.isStdInIO()
          || (estate)->io.isStdOutIO()
          || (estate)->io.isStdErrIO()
          || (estate)->io.isFailedAssertIO());
}

IOAnalyzer::SubSolverResultType IOAnalyzer::subSolver(const EState* currentEStatePtr) {
  // start the timer if not yet done
  if (!_timerRunning) {
    _analysisTimer.start();
    _timerRunning=true;
  }
  // first, check size of global EStateSet and print status or switch to topify/terminate analysis accordingly.
  unsigned long estateSetSize;
  bool earlyTermination = false;
  int threadNum = 0; //subSolver currently does not support multiple threads.
  // print status message if required
  if (args.getBool("status") && _displayDiff) {
#pragma omp critical(HASHSET)
    {
      estateSetSize = estateSet.size();
    }
    if(threadNum==0 && (estateSetSize>(_prevStateSetSizeDisplay+_displayDiff))) {
      printStatusMessage(true);
      _prevStateSetSizeDisplay=estateSetSize;
    }
  }
  // switch to topify mode or terminate analysis if resource limits are exceeded
  if (_maxBytes != -1 || _maxBytesForcedTop != -1 || _maxSeconds != -1 || _maxSecondsForcedTop != -1
      || _maxTransitions != -1 || _maxTransitionsForcedTop != -1 || _maxIterations != -1 || _maxIterationsForcedTop != -1) {
#pragma omp critical(HASHSET)
    {
      estateSetSize = estateSet.size();
    }
    if(threadNum==0 && _resourceLimitDiff && (estateSetSize>(_prevStateSetSizeResource+_resourceLimitDiff))) {
      if (isIncompleteSTGReady()) {
#pragma omp critical(ESTATEWL)
	{
	  earlyTermination = true;
	}	  
      }
      isActiveGlobalTopify(); // Checks if a switch to topify is necessary. If yes, it changes the analyzer state.
      _prevStateSetSizeResource=estateSetSize;
    }
  } 
  EStateWorkList deferedWorkList;
  std::set<const EState*> existingEStateSet;
  if (earlyTermination) {
    if(args.getBool("status")) {
      cout << "STATUS: Early termination within subSolver (resource limit reached)." << endl;
    }
    transitionGraph.setForceQuitExploration(true);
  } else {
    // run the actual sub-solver
    EStateWorkList localWorkList;
    localWorkList.push_back(currentEStatePtr);
    while(!localWorkList.empty()) {
      // logger[DEBUG]<<"local work list size: "<<localWorkList.size()<<endl;
      const EState* currentEStatePtr=*localWorkList.begin();
      localWorkList.pop_front();
      if(isFailedAssertEState(currentEStatePtr)) {
	// ensure we do not compute any successors of a failed assert state
	continue;
      }
      Flow edgeSet=flow.outEdges(currentEStatePtr->label());
      for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
	Edge e=*i;
	list<EState> newEStateList;
	newEStateList=transferEdgeEState(e,currentEStatePtr);
	for(list<EState>::iterator nesListIter=newEStateList.begin();
	    nesListIter!=newEStateList.end();
	    ++nesListIter) {
	  // newEstate is passed by value (not created yet)
	  EState newEState=*nesListIter;
	  ROSE_ASSERT(newEState.label()!=Labeler::NO_LABEL);

	  if((!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState)&&!isVerificationErrorEState(&newEState))) {
	    HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=process(newEState);
	    const EState* newEStatePtr=pres.second;
	    ROSE_ASSERT(newEStatePtr);
	    if(pres.first==true) {
	      if(isLTLRelevantEState(newEStatePtr)) {
		deferedWorkList.push_back(newEStatePtr);
	      } else {
		localWorkList.push_back(newEStatePtr);
	      }
	    } else {
	      // we have found an existing state, but need to make also sure it's a relevent one
	      if(isLTLRelevantEState(newEStatePtr)) {
		ROSE_ASSERT(newEStatePtr!=nullptr);
		existingEStateSet.insert(const_cast<EState*>(newEStatePtr));
	      } else {
		// TODO: use a unique list
		localWorkList.push_back(newEStatePtr);
	      }
	    }
	    // TODO: create reduced transition set at end of this function
	    if(!getModeLTLDriven()) {
	      recordTransition(currentEStatePtr,e,newEStatePtr);
	    }
	  }
	  if((!newEState.constraints()->disequalityExists()) && ((isFailedAssertEState(&newEState))||isVerificationErrorEState(&newEState))) {
	    // failed-assert end-state: do not add to work list but do add it to the transition graph
	    const EState* newEStatePtr;
	    newEStatePtr=processNewOrExisting(newEState);
	    // TODO: create reduced transition set at end of this function
	    if(!getModeLTLDriven()) {
	      recordTransition(currentEStatePtr,e,newEStatePtr);
	    }
	    deferedWorkList.push_back(newEStatePtr);
	    if(isVerificationErrorEState(&newEState)) {
	      logger[TRACE]<<"STATUS: detected verification error state ... terminating early"<<endl;
	      // set flag for terminating early
	      reachabilityResults.reachable(0);
	      _firstAssertionOccurences.push_back(pair<int, const EState*>(0, newEStatePtr));
	      EStateWorkList emptyWorkList;
	      EStatePtrSet emptyExistingStateSet;
	      return make_pair(emptyWorkList,emptyExistingStateSet);
	    } else if(isFailedAssertEState(&newEState)) {
	      // record failed assert
	      int assertCode;
	      if(args.getBool("rers-binary")) {
		assertCode=reachabilityAssertCode(newEStatePtr);
	      } else {
		assertCode=reachabilityAssertCode(currentEStatePtr);
	      }
	      /* if a property table is created for reachability we can also
		 collect on the fly reachability results in LTL-driven mode
		 but for now, we don't
	      */
	      if(!getModeLTLDriven()) {
		if(assertCode>=0) {
		  if(args.getBool("with-counterexamples") || args.getBool("with-assert-counterexamples")) {
		    //if this particular assertion was never reached before, compute and update counterexample
		    if (reachabilityResults.getPropertyValue(assertCode) != PROPERTY_VALUE_YES) {
		      _firstAssertionOccurences.push_back(pair<int, const EState*>(assertCode, newEStatePtr));
		    }
		  }
		  reachabilityResults.reachable(assertCode);
		}	    // record failed assert
	      }
	    } // end of failed assert handling
	  } // end of if (no disequality (= no infeasable path))
	} // end of loop on transfer function return-estates
      } // edge set iterator
    }
  }
  return make_pair(deferedWorkList,existingEStateSet);
}

void IOAnalyzer::resetAnalysis() {
  Analyzer::resetAnalysis();
  _prevStateSetSizeDisplay = 0;
  _prevStateSetSizeResource = 0;
}
