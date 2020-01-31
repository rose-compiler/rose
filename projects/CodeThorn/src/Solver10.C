#include "sage3basic.h"
#include "Solver10.h"
#include "IOAnalyzer.h"
#include "SpotConnection.h"
#include "RERS_empty_specialization.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility Solver10::logger;
// initialize static member flag
bool Solver10::_diagnosticsInitialized = false;

Solver10::Solver10() :
_patternSearchExplorationMode(EXPL_DEPTH_FIRST)
  {
  _patternSearchAssertTable= new PropertyValueTable(100);
  initDiagnostics();
  initSolverState();
}

int Solver10::getId() {
  return 10;
}

/*! 
 * \author Marc Jasper
 * \date 2015.
 */
void Solver10::run() {
  if(!args.getBool("rers-binary")) {
    logger[ERROR]<< "solver 10 is only compatible with the hybrid analyzer." << endl;
    exit(1);
  }

  // display initial information
  int maxInputVal = *( std::max_element(_analyzer->_inputVarValues.begin(), _analyzer->_inputVarValues.end()) ); //required for parsing to characters
  _analyzer->reachabilityResults.init(_analyzer->getNumberOfErrorLabels()); // set all reachability results to unknown
  int assertionsToFind = 0;
  for (unsigned int c = 0; c < _patternSearchAssertTable->size(); c++) {
    if (_patternSearchAssertTable->getPropertyValue(c) == PROPERTY_VALUE_YES) {
      assertionsToFind++;
    }
  }
  logger[INFO]<<"number of error labels to find: " << assertionsToFind << " (out of " << _analyzer->reachabilityResults.size() << ")" << endl;
  string expMode = "";
  if (_patternSearchExplorationMode == EXPL_BREADTH_FIRST) {
    expMode = "breadth-first";
  } else if (_patternSearchExplorationMode == EXPL_DEPTH_FIRST) {
    expMode = "depth-first";
  }
  logger[INFO]<< "pattern search exploration mode: " << expMode << endl;
  logger[INFO]<<"maximum input depth for the pattern search: "<< args.getInt("pattern-search-max-depth") << endl;
  logger[INFO]<<"following " << args.getInt("pattern-search-repetitions") << " pattern iterations before the suffix search." << endl;
  logger[INFO]<<"maximum input depth of the counterexample suffix: "<<args.getInt("pattern-search-max-suffix") << endl;
  if (args.getBool("status")) {
    cout<<"STATUS: Running parallel solver 10 (I/O-pattern search) with "<<_analyzer->_numberOfThreadsToUse<<" threads."<<endl;
    cout<<"STATUS: This may take a while. Please expect a line of output every 10.000 non-error states." << endl;
  }
  // create a new instance of the startPState
  //TODO: check why init of "output" is necessary
  PState newStartPState = _analyzer->_startPState;
  newStartPState.writeToMemoryLocation(_analyzer->globalVarIdByName("output"),CodeThorn::AbstractValue(-7));
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
  int workers=_analyzer->_numberOfThreadsToUse;
  bool earlyTermination = false;
  vector<bool> workVector(workers);
  _analyzer->set_finished(workVector,true);
  omp_set_num_threads(workers);
# pragma omp parallel shared(workVector) private(threadNum)
  {
    threadNum=omp_get_thread_num();
    while(!_analyzer->all_false(workVector)) {
      // check if all assertions have been found
      if(threadNum==0 && (processedStates >= (previousProcessedStates+checkIfFinishedEvery))) {
        bool finished = true;
        for (unsigned int c = 0; c < _patternSearchAssertTable->size(); c++) {
          if (_patternSearchAssertTable->getPropertyValue(c) == PROPERTY_VALUE_YES
              && _analyzer->reachabilityResults.getPropertyValue(c) != PROPERTY_VALUE_YES) {
            finished = false;
          }
        }
        if (finished) {
          earlyTermination = true;
        }
        //previousFinishedCheckStates = processedStates;
      }
      // display a status report every ~10.000 non-error PStates
      if(threadNum==0 && _analyzer->_displayDiff && (processedStates >= (previousProcessedStates+_analyzer->_displayDiff))) {
	if (args.getBool("status")) {
	  cout<< "STATUS: #processed PStates: " << processedStates << "   currentMaxDepth: " << currentMaxDepth << "   wl size: " << workList.size() << endl;
	}
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
	ROSE_ASSERT(threadNum>=0 && threadNum<=_analyzer->_numberOfThreadsToUse);
        continue;
      }
      // generate one new state for each input symbol and continue searching for patterns
      for (set<int>::iterator inputVal=_analyzer->_inputVarValues.begin(); inputVal!=_analyzer->_inputVarValues.end(); inputVal++) {
        // copy the state and initialize new input
        PState newPState = currentState.first;
        newPState.writeToMemoryLocation(_analyzer->globalVarIdByName("input"),CodeThorn::AbstractValue(*inputVal));
        list<int> newHistory = currentState.second;
        ROSE_ASSERT(newHistory.size() % 2 == 0);
        newHistory.push_back(*inputVal);
        // call the next-state function (a.k.a. "calculate_outputFP")
        RERS_Problem::rersGlobalVarsCallInitFP(_analyzer, newPState, omp_get_thread_num());
        (void) RERS_Problem::calculate_outputFP( omp_get_thread_num() );
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
	      if (_analyzer->reachabilityResults.getPropertyValue(index) == PROPERTY_VALUE_UNKNOWN) {
		if (args.getBool("status")) {
		  cout<< "STATUS: found a trace leading to failing assertion #" << index;
		  cout<< " (no pattern. total input length: " << ((newHistory.size()+1) / 2) << ")." << endl;
		}
		string ce = convertToCeString(newHistory, maxInputVal);
		_analyzer->reachabilityResults.setPropertyValue(index, PROPERTY_VALUE_YES);
                _analyzer->reachabilityResults.setCounterexample(index, ce);
              }
            }
          }
        } else {  // not a failed assertion, continue searching
          RERS_Problem::rersGlobalVarsCallReturnInitFP(_analyzer, newPState, omp_get_thread_num());
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
              if (args.getBool("status")) {
                cout<< "found pattern (start index " << ps << "): ";
                for (list<int>::iterator it = newHistory.begin(); it != newHistory.end(); it++) {
                  cout << *it <<",";
                }
                cout<< endl;
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
              for (int p = 0; p < (args.getInt("pattern-search-repetitions") - 2); p++) { // already found two pattern iterations
                stillAValidPath = computePStateAfterInputs(newPState, patternInputs, omp_get_thread_num(), &newHistory);
                if (!stillAValidPath) {
                  break;
                }
              }
              if (stillAValidPath) {
                int suffixDfsProcessedStates = pStateDepthFirstSearch(&newPState, args.getInt("pattern-search-max-suffix"), omp_get_thread_num(),
                                                                      &newHistory, maxInputVal, patternInputs.size(), args.getInt("pattern-search-repetitions"));
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
          if ((newHistory.size() / 2) < (unsigned int) args.getInt("pattern-search-max-depth")) {
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
            ROSE_ASSERT(newHistory.size() / 2 == (unsigned int) args.getInt("pattern-search-max-depth"));
          }
	} // end of else-case "no assertion, continue searching"
      } //end of "for each input value"-loop
    } // while
  } // omp parallel
  if (earlyTermination) {
    if (args.getBool("status"))  {
      cout<< "STATUS: solver 10 finished (found all assertions)." << endl;
    }
  } else {
    if (args.getBool("status")) {
      cout<< "STATUS: solver 10 finished (empty worklist). " << endl;
    }
 }
}

/*! 
  * \author Marc Jasper
  * \date 2015.
 */
int Solver10::pStateDepthFirstSearch(PState* startPState, int maxDepth, int thread_id, list<int>* partialTrace, int maxInputVal, int patternLength, int patternIterations) {
  // initialize worklist
  PStatePlusIOHistory startState = PStatePlusIOHistory(*startPState, list<int>());
  std::list<PStatePlusIOHistory> workList;
  workList.push_back(startState);
  // statistics and other variables
  int processedStates = 0;
  int previousProcessedStates = 0;
  unsigned int currentMaxDepth = 0; //debugging
  int displayDiff=1000;
  while(!workList.empty()) {
    if(displayDiff && (processedStates >= (previousProcessedStates+displayDiff))) {
      if (args.getBool("status")) {
	logger[TRACE]<< "STATUS: #processed PStates suffix dfs (thread_id: " << thread_id << "): " << processedStates << "   wl size: " << workList.size() << endl;
      }
      previousProcessedStates=processedStates;
    }
    // pop worklist
    PStatePlusIOHistory currentState;
    processedStates++;
    currentState=*workList.begin();
    workList.pop_front();
    // generate one new state for each input symbol and continue searching for patterns
    for (set<int>::iterator inputVal=_analyzer->_inputVarValues.begin(); inputVal!=_analyzer->_inputVarValues.end(); inputVal++) {
      // copy the state and initialize new input
      PState newPState = currentState.first;
      newPState.writeToMemoryLocation(_analyzer->globalVarIdByName("input"),CodeThorn::AbstractValue(*inputVal));
      list<int> newHistory = currentState.second;
      ROSE_ASSERT(newHistory.size() % 2 == 0);
      newHistory.push_back(*inputVal);
      // call the next-state function (a.k.a. "calculate_outputFP")
      RERS_Problem::rersGlobalVarsCallInitFP(_analyzer, newPState, thread_id);
      (void) RERS_Problem::calculate_outputFP( thread_id );
      int rers_result=RERS_Problem::output[thread_id];
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
            if (_analyzer->reachabilityResults.getPropertyValue(index) == PROPERTY_VALUE_UNKNOWN) {
              list<int> ceTrace = *partialTrace;
              for (list<int>::iterator n = newHistory.begin(); n != newHistory.end(); n++) {
                ceTrace.push_back(*n);
              }
              int prefixLength = (partialTrace->size() - (2 * patternIterations * patternLength)) / 2;
	      if (args.getBool("status")) {
	        cout<< "STATUS: found a trace leading to failing assertion #" << index << " (input lengths: reused prefix: " << prefixLength;
	        cout<< ", pattern: " <<patternLength << ", suffix: " << ((newHistory.size()+1) / 2) << ", total: " << ((ceTrace.size()+1) / 2) << ")." << endl;
              }
              string ce = convertToCeString(ceTrace, maxInputVal);
              _analyzer->reachabilityResults.setPropertyValue(index, PROPERTY_VALUE_YES);
              _analyzer->reachabilityResults.setCounterexample(index, ce);
            }
          }
        }
      } else {  // not a failed assertion, continue searching
        RERS_Problem::rersGlobalVarsCallReturnInitFP(_analyzer, newPState, thread_id);
        newHistory.push_back(rers_result);
        ROSE_ASSERT(newHistory.size() % 2 == 0);
        // continue only if the maximum depth of input symbols has not yet been reached
        if ((newHistory.size() / 2) < (unsigned int) maxDepth) {
          // add the new state to the worklist
          PStatePlusIOHistory newState = PStatePlusIOHistory(newPState, newHistory);
          currentMaxDepth = currentMaxDepth < newHistory.size() ? newHistory.size() : currentMaxDepth;
          workList.push_front(newState);  // depth-first search
        } else {
          ROSE_ASSERT(newHistory.size() / 2 == (unsigned int) maxDepth);
        }
      } // end of else-case "no assertion, continue searching"
    } //end of "for each input value"-loop
  } // while
  return processedStates;
}

/*! 
  * \author Marc Jasper
  * \date 2015.
 */
list<int> Solver10::inputsFromPatternTwoRepetitions(list<int> pattern2r) {
  ROSE_ASSERT(pattern2r.size() % 4 == 0);
  list<int> result;
  list<int>::iterator iter = pattern2r.begin();
  for (unsigned int i = 0; i < (pattern2r.size() / 4); i++) {
    result.push_back(*iter);
    iter++;
    iter++;
  }
  return result;
}

/*! 
  * \author Marc Jasper
  * \date 2015.
 */
bool Solver10::computePStateAfterInputs(PState& pState, list<int>& inputs, int thread_id, list<int>* iOSequence) {
  for (list<int>::iterator i = inputs.begin(); i !=inputs.end(); i++) {
    //pState[globalVarIdByName("input")]=CodeThorn::AbstractValue(*i);
    pState.writeToMemoryLocation(_analyzer->globalVarIdByName("input"),
                              CodeThorn::AbstractValue(*i));
    RERS_Problem::rersGlobalVarsCallInitFP(_analyzer, pState, thread_id);
    (void) RERS_Problem::calculate_outputFP(thread_id);
    RERS_Problem::rersGlobalVarsCallReturnInitFP(_analyzer, pState, thread_id);
    if (iOSequence) {
      iOSequence->push_back(*i);
      int outputVal=RERS_Problem::output[thread_id];
      // a (std)err state could was encountered, this is not a valid RERS path anymore
      if (outputVal <= 0) {
        return false;
      }
      iOSequence->push_back(outputVal);
    }
  }
  return true;
}

/*! 
  * \author Marc Jasper
  * \date 2015.
 */
bool Solver10::containsPatternTwoRepetitions(std::list<int>& sequence, int startIndex, int endIndex) {
  // copy the sublist to an array
  int patternLength = endIndex - startIndex + 1;
  ROSE_ASSERT(patternLength % 2 == 0);
  vector<int> ceSymbolsVec(patternLength);
  list<int>::iterator it = sequence.begin();
  for (int i = 0; i < startIndex; i++) {
    it++;
  }
  for (int k = 0; k < patternLength; k++) {
    ceSymbolsVec[k] = *it;
    it++;
  }
  // check if the subsequence contains a pattern
  bool mismatch = false;
  for (int j = 0; j < (patternLength / 2); j++) {
    if (ceSymbolsVec[j] != ceSymbolsVec[j + (patternLength / 2)]) {
      mismatch = true;
      break;
    }
  }
  return !mismatch;
}

/*! 
  * \author Marc Jasper
  * \date 2014.
 */
string Solver10::convertToCeString(list<int>& ceAsIntegers, int maxInputVal) {
  SpotConnection spotConnection;
  stringstream ss;
  ss << "[";
  bool firstElem = true;
  for (list<int>::iterator i=ceAsIntegers.begin(); i!=ceAsIntegers.end(); i++) {
    if (!firstElem) {
      ss << ";";
    }
    ss << spotConnection.int2PropName(*i, maxInputVal);
    firstElem = false;
  }
  ss << "]";
  return ss.str();
}
 
void Solver10::initSolverState() {
  if (args.isDefined("pattern-search-exploration")) {
    string chosenMode = args.getString("pattern-search-exploration");
    if (chosenMode == "depth-first") {
      _patternSearchExplorationMode = EXPL_DEPTH_FIRST;
    } else if (chosenMode == "breadth-first") {
      _patternSearchExplorationMode = EXPL_BREADTH_FIRST;
    } else {
      throw CodeThorn::Exception("Unknown exploration mode \"" + chosenMode + "\" selected (pattern search).");
    }
  } else { // default if no option selected
    _patternSearchExplorationMode = EXPL_DEPTH_FIRST;
  }
}
 
void Solver10::initDiagnostics() {
  if (!_diagnosticsInitialized) {
    _diagnosticsInitialized = true;
    Solver::initDiagnostics(logger, getId());
  }
}
