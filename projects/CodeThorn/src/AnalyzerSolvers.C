#include "sage3basic.h"
#include "Analyzer.h"

#include "CommandLineOptions.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "AnalysisAbstractionLayer.h"
#include "SpotConnection.h"
#include "CodeThornException.h"

#include <boost/bind.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "boost/lexical_cast.hpp"

#include "Timer.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

#include "CollectionOperators.h"

// MS 2016: Solvers are supposed to become separate objects

// algorithm 4 also records reachability for incomplete STGs (analyzer::reachabilityResults)
// this solver support semantic folding (solver 5 does not)
void Analyzer::runSolver4() {
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
  logger[TRACE]<<"STATUS: Running parallel solver 4 with "<<workers<<" threads."<<endl;
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
        ROSE_ASSERT(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
      } else {
        ROSE_ASSERT(currentEStatePtr);

        Flow edgeSet=flow.outEdges(currentEStatePtr->label());
        // logger[DEBUG]<< "edgeSet size:"<<edgeSet.size()<<endl;
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
          list<EState> newEStateList;
          newEStateList=transferEdgeEState(e,currentEStatePtr);
          if(isTerminationRelevantLabel(e.source())) {
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

void Analyzer::runSolver5() {
  if(isUsingExternalFunctionSemantics()) {
    reachabilityResults.init(1); // in case of svcomp mode set single program property to unknown
  } else {
    reachabilityResults.init(getNumberOfErrorLabels()); // set all reachability results to unknown
  }
  logger[INFO]<<"number of error labels: "<<reachabilityResults.size()<<endl;
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

  if(boolOptions["rers-binary"]) {
    //initialize the global variable arrays in the linked binary version of the RERS problem
    logger[DEBUG]<< "init of globals with arrays for "<< workers << " threads. " << endl;
    RERS_Problem::rersGlobalVarsArrayInit(workers);
    RERS_Problem::createGlobalVarAddressMaps(this);
  }

  logger[TRACE]<<"STATUS: Running parallel solver 5 with "<<workers<<" threads."<<endl;
  printStatusMessage(true);
# pragma omp parallel shared(workVector) private(threadNum)
  {
    threadNum=omp_get_thread_num();
    while(!all_false(workVector)) {
      // logger[DEBUG]<<"running : WL:"<<estateWorkListCurrent->size()<<endl;
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
            logger[TRACE]<< "STATUS: transition system reduced to I/O/worklist states. remaining transitions: " << transitionGraph.size() << endl;
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
        ROSE_ASSERT(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
      } else {
        ROSE_ASSERT(currentEStatePtr);
        Flow edgeSet=flow.outEdges(currentEStatePtr->label());
        // logger[DEBUG] << "out-edgeSet size:"<<edgeSet.size()<<endl;
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
            if(_stg_trace_filename.size()>0 && !newEState.constraints()->disequalityExists()) {
              std::ofstream fout;
              // _csv_stg_trace_filename is the member-variable of analyzer
#pragma omp critical
              {
#if 1
                fout.open(_stg_trace_filename.c_str(),ios::app);    // open file for appending
                assert (!fout.fail( ));
                fout<<"PSTATE-IN :"<<currentEStatePtr->pstate()->toString(&variableIdMapping);
                string sourceString=getCFAnalyzer()->getLabeler()->getNode(currentEStatePtr->label())->unparseToString().substr(0,40);
                if(sourceString.size()==40) sourceString+="...";
                fout<<"\n==>"<<"TRANSFER:"<<sourceString;
                fout<<"==>\n"<<"PSTATE-OUT:"<<newEState.pstate()->toString(&variableIdMapping);
                fout<<endl;
                fout<<endl;
                fout.close();
                // logger[DEBUG] <<"generate STG-edge:"<<"ICFG-EDGE:"<<e.toString()<<endl;
#else
                cout<<"PSTATE-IN :"<<currentEStatePtr->pstate()->toString(&variableIdMapping)<<endl;
                string sourceString=getCFAnalyzer()->getLabeler()->getNode(currentEStatePtr->label())->unparseToString().substr(0,40);
                if(sourceString.size()==40) sourceString+="...";
                cout<<sourceString<<endl;
                cout<<"PSTATE-OUT:"<<newEState.pstate()->toString(&variableIdMapping)<<endl;
                cout<<endl;
#endif                

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
                  logger[TRACE] <<"STATUS: detected verification error state ... terminating early"<<endl;
                  // set flag for terminating early
                  reachabilityResults.reachable(0);
                  terminateEarly=true;
                }
              } else if(isFailedAssertEState(&newEState)) {
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
    logger[TRACE]<< "STATUS: analysis finished (incomplete STG due to specified resource restriction)."<<endl;
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
    logger[TRACE]<< "analysis finished (worklist is empty)."<<endl;
  }
  transitionGraph.setIsPrecise(isPrecise());
}

// solver 8 is used to analyze traces of consecutively added input sequences
void Analyzer::runSolver8() {
  int workers = 1; //only one thread
  if(boolOptions["rers-binary"]) {
    //initialize the global variable arrays in the linked binary version of the RERS problem
    // logger[DEBUG]<< "init of globals with arrays for "<< workers << " threads. " << endl;
    RERS_Problem::rersGlobalVarsArrayInit(workers);
  }
  while(!isEmptyWorkList()) {
    const EState* currentEStatePtr;
    //solver 8
    ROSE_ASSERT(estateWorkListCurrent->size() == 1);
    if (!isEmptyWorkList()) {
      currentEStatePtr=popWorkList();
    } else {
      ROSE_ASSERT(0); // there should always be exactly one element in the worklist at this point
    }
    ROSE_ASSERT(currentEStatePtr);

    Flow edgeSet=flow.outEdges(currentEStatePtr->label());
    for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
      Edge e=*i;
      list<EState> newEStateList;
      newEStateList=transferEdgeEState(e,currentEStatePtr);
      // solver 8: keep track of the input state where the input sequence ran out of elements (where solver8 stops)
      if (newEStateList.size()== 0) {
        if(e.isType(EDGE_EXTERNAL)) {
          SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(e.source());
          InputOutput newio;
          Label lab=getLabeler()->getLabel(nextNodeToAnalyze1);
          VariableId varId;
          if(getLabeler()->isStdInLabel(lab,&varId)) {
            _estateBeforeMissingInput = currentEStatePtr; //store the state where input was missing in member variable
          }
        }
      }
      // solver 8: only single traces allowed
      ROSE_ASSERT(newEStateList.size()<=1);
      for(list<EState>::iterator nesListIter=newEStateList.begin();
          nesListIter!=newEStateList.end();
          ++nesListIter) {
        // newEstate is passed by value (not created yet)
        EState newEState=*nesListIter;
        ROSE_ASSERT(newEState.label()!=Labeler::NO_LABEL);
        if((!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState))) {
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
  if(boolOptions["rers-binary"]) {
    //initialize the global variable arrays in the linked binary version of the RERS problem
    RERS_Problem::rersGlobalVarsArrayInit(_numberOfThreadsToUse);
    RERS_Problem::createGlobalVarAddressMaps(this);
  } else {
    logger[ERROR]<< "solver 9 is only compatible with the hybrid analyzer." << endl;
    exit(1);
  }
  reachabilityResults.init(getNumberOfErrorLabels()); // set all reachability results to unknown
  logger[INFO]<<"INFO: number of error labels: "<<reachabilityResults.size()<<endl;
  int maxInputVal = *( std::max_element(_inputVarValues.begin(), _inputVarValues.end()) ); //required for parsing to characters
  logger[INFO]<<"INFO: maximum length of input patterns: "<< (_reconstructMaxInputDepth / 2) <<endl;
  logger[INFO]<<"INFO: maximum number of pattern repetitions: "<<_reconstructMaxRepetitions<<endl;
  logger[TRACE]<<"STATUS: Running parallel solver 9 (reconstruct assertion traces) with "<<_numberOfThreadsToUse<<" threads."<<endl;
  logger[TRACE]<<"STATUS: This may take a while. Please expect a line of output every 10.000 non-error states. (counter resets itself everytime the prefix is expanded)" << endl;
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
        logger[TRACE]<< "STATUS: found a trace leading to failing assertion #" << i << " (input lengths: reused prefix: " << ceIndex;
        logger[TRACE]<< ", pattern: " << * inputPatternLength << ", total: " << ((realTrace.size()+1) /2) << ")." << endl;
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
            logger[TRACE]<< "STATUS: found a trace leading to failing assertion #" << i << " (input lengths: reused prefix: " << ceIndex;
            logger[TRACE]<< ", pattern: " << * inputPatternLength << ", total: " << ((realTrace.size()+1) /2) << ")." << endl;
            string ce = convertToCeString(realTrace, maxInputVal);
            reachabilityResults.setCounterexample(i, ce);
          }
        }
        ceIndex++;
        ceIter++;
      }
      if (!foundAssertion) {
        logger[INFO]<< "no trace to assertion #" << i << " could be found. Maybe try again with greater thresholds." << endl;
      }
    }
  }
}

void Analyzer::runSolver10() {
  if(boolOptions["rers-binary"]) {
    //initialize the global variable arrays in the linked binary version of the RERS problem
    RERS_Problem::rersGlobalVarsArrayInit(_numberOfThreadsToUse);
    RERS_Problem::createGlobalVarAddressMaps(this);
  } else {
    logger[ERROR]<< "solver 10 is only compatible with the hybrid analyzer." << endl;
    exit(1);
  }
  // display initial information
  int maxInputVal = *( std::max_element(_inputVarValues.begin(), _inputVarValues.end()) ); //required for parsing to characters
  reachabilityResults.init(getNumberOfErrorLabels()); // set all reachability results to unknown
  int assertionsToFind = 0;
  for (unsigned int c = 0; c < _patternSearchAssertTable->size(); c++) {
    if (_patternSearchAssertTable->getPropertyValue(c) == PROPERTY_VALUE_YES) {
      assertionsToFind++;
    }
  }
  logger[INFO]<<"number of error labels to find: " << assertionsToFind << " (out of " << reachabilityResults.size() << ")" << endl;
  string expMode = "";
  if (_patternSearchExplorationMode == EXPL_BREADTH_FIRST) {
    expMode = "breadth-first";
  } else if (_patternSearchExplorationMode == EXPL_DEPTH_FIRST) {
    expMode = "depth-first";
  }
  logger[INFO]<< "pattern search exploration mode: " << expMode << endl;
  logger[INFO]<<"maximum input depth for the pattern search: "<< _patternSearchMaxDepth << endl;
  logger[INFO]<<"following " << _patternSearchRepetitions << " pattern iterations before the suffix search." << endl;
  logger[INFO]<<"maximum input depth of the counterexample suffix: "<<_patternSearchMaxSuffixDepth << endl;
  logger[TRACE]<<"STATUS: Running parallel solver 10 (I/O-pattern search) with "<<_numberOfThreadsToUse<<" threads."<<endl;
  logger[TRACE]<<"STATUS: This may take a while. Please expect a line of output every 10.000 non-error states." << endl;
  // create a new instance of the startPState
  //TODO: check why init of "output" is necessary
  PState newStartPState = _startPState;
  newStartPState[globalVarIdByName("output")]=CodeThorn::AbstractValue(-7);
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
        logger[TRACE]<< "STATUS: #processed PStates: " << processedStates << "   currentMaxDepth: " << currentMaxDepth << "   wl size: " << workList.size() << endl;
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
	ROSE_ASSERT(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
        continue;
      }
      // generate one new state for each input symbol and continue searching for patterns
      for (set<int>::iterator inputVal=_inputVarValues.begin(); inputVal!=_inputVarValues.end(); inputVal++) {
        // copy the state and initialize new input
        PState newPState = currentState.first;
        newPState[globalVarIdByName("input")]=CodeThorn::AbstractValue(*inputVal);
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
          ROSE_ASSERT(index>=0 && index <=99);
          if (_patternSearchAssertTable->getPropertyValue(index) == PROPERTY_VALUE_YES) {
            // report the result and add it to the results table
#pragma omp critical(CSV_ASSERT_RESULTS)
            {
	      if (reachabilityResults.getPropertyValue(index) == PROPERTY_VALUE_UNKNOWN) {
		logger[TRACE]<< "STATUS: found a trace leading to failing assertion #" << index;
		logger[TRACE]<< " (no pattern. total input length: " << ((newHistory.size()+1) / 2) << ")." << endl;
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
              if (logger[DEBUG]) {
                logger[DEBUG]<< "found pattern (start index " << ps << "): ";
                for (list<int>::iterator it = newHistory.begin(); it != newHistory.end(); it++) {
                  logger[DEBUG] << *it <<",";
                }
                logger[DEBUG]<< endl;
              }
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
                logger[ERROR]<< "requested pattern search exploration mode currently not supported." << endl;
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
    logger[TRACE]<< "STATUS: solver 10 finished (found all assertions)." << endl;
  } else {
  logger[TRACE]<< "STATUS: solver 10 finished (empty worklist). " << endl;
 }
}
