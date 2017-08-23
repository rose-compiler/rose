#include "sage3basic.h"
#include "Solver10.h"
#include "Analyzer.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility Solver10::logger;
// initialize static member flag
bool Solver10::_diagnosticsInitialized = false;

Solver10::Solver10() {
  initDiagnostics();
}

int Solver10::getId() {
  return 10;
}

/*! 
 * \author Marc Jasper
 * \date 2015.
 */
// solver 10 is the black-box pattern search solver (used for the Reachability Track of RERS)
void Solver10::run() {
  if(args.isSet("rers-binary")) {
    //initialize the global variable arrays in the linked binary version of the RERS problem
    RERS_Problem::rersGlobalVarsArrayInit(_analyzer->_numberOfThreadsToUse);
    RERS_Problem::createGlobalVarAddressMaps(_analyzer);
  } else {
    logger[ERROR]<< "solver 10 is only compatible with the hybrid analyzer." << endl;
    exit(1);
  }
  // display initial information
  int maxInputVal = *( std::max_element(_analyzer->_inputVarValues.begin(), _analyzer->_inputVarValues.end()) ); //required for parsing to characters
  _analyzer->reachabilityResults.init(_analyzer->getNumberOfErrorLabels()); // set all reachability results to unknown
  int assertionsToFind = 0;
  for (unsigned int c = 0; c < _analyzer->_patternSearchAssertTable->size(); c++) {
    if (_analyzer->_patternSearchAssertTable->getPropertyValue(c) == PROPERTY_VALUE_YES) {
      assertionsToFind++;
    }
  }
  logger[INFO]<<"number of error labels to find: " << assertionsToFind << " (out of " << _analyzer->reachabilityResults.size() << ")" << endl;
  string expMode = "";
  if (_analyzer->_patternSearchExplorationMode == Analyzer::EXPL_BREADTH_FIRST) {
    expMode = "breadth-first";
  } else if (_analyzer->_patternSearchExplorationMode == Analyzer::EXPL_DEPTH_FIRST) {
    expMode = "depth-first";
  }
  logger[INFO]<< "pattern search exploration mode: " << expMode << endl;
  logger[INFO]<<"maximum input depth for the pattern search: "<< _analyzer->_patternSearchMaxDepth << endl;
  logger[INFO]<<"following " << _analyzer->_patternSearchRepetitions << " pattern iterations before the suffix search." << endl;
  logger[INFO]<<"maximum input depth of the counterexample suffix: "<<_analyzer->_patternSearchMaxSuffixDepth << endl;
  if (_analyzer->getOptionStatusMessages()) {
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
        for (unsigned int c = 0; c < _analyzer->_patternSearchAssertTable->size(); c++) {
          if (_analyzer->_patternSearchAssertTable->getPropertyValue(c) == PROPERTY_VALUE_YES
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
	if (_analyzer->getOptionStatusMessages()) {
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
        // call the next-state function (a.k.a. "calculate_output")
        RERS_Problem::rersGlobalVarsCallInit(_analyzer, newPState, omp_get_thread_num());
        (void) RERS_Problem::calculate_output( omp_get_thread_num() );
        int rers_result=RERS_Problem::output[omp_get_thread_num()];
        // handle assertions found to be reachable
        if (rers_result==-2) {
          //Stderr state, do not continue (rers mode)
        } else if(rers_result<=-100) {
          // we found a failing assert
          int index=((rers_result+100)*(-1));
          ROSE_ASSERT(index>=0 && index <=99);
          if (_analyzer->_patternSearchAssertTable->getPropertyValue(index) == PROPERTY_VALUE_YES) {
            // report the result and add it to the results table
#pragma omp critical(CSV_ASSERT_RESULTS)
            {
	      if (_analyzer->reachabilityResults.getPropertyValue(index) == PROPERTY_VALUE_UNKNOWN) {
		if (_analyzer->getOptionStatusMessages()) {
		  cout<< "STATUS: found a trace leading to failing assertion #" << index;
		  cout<< " (no pattern. total input length: " << ((newHistory.size()+1) / 2) << ")." << endl;
		}
		string ce = _analyzer->convertToCeString(newHistory, maxInputVal);
		_analyzer->reachabilityResults.setPropertyValue(index, PROPERTY_VALUE_YES);
                _analyzer->reachabilityResults.setCounterexample(index, ce);
              }
            }
          }
        } else {  // not a failed assertion, continue searching
          RERS_Problem::rersGlobalVarsCallReturnInit(_analyzer, newPState, omp_get_thread_num());
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
            bool containsPattern = _analyzer->containsPatternTwoRepetitions(newHistory, ps, (newHistory.size() - 1));
            if (containsPattern) {
              // modulo 4: sets of input and output symbols are distinct & the system always alternates between input / ouput
              ROSE_ASSERT( (newHistory.size() - ps)  % 4 == 0 );
              if (_analyzer->getOptionStatusMessages()) {
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
              list<int> patternInputs = _analyzer->inputsFromPatternTwoRepetitions(patternStates);
              bool stillAValidPath = true;
              // follow the cyclic I/O-pattern
              for (int p = 0; p < (_analyzer->_patternSearchRepetitions - 2); p++) { // already found two pattern iterations
                stillAValidPath = _analyzer->computePStateAfterInputs(newPState, patternInputs, omp_get_thread_num(), &newHistory);
                if (!stillAValidPath) {
                  break;
                }
              }
              if (stillAValidPath) {
                int suffixDfsProcessedStates = _analyzer->pStateDepthFirstSearch(&newPState, _analyzer->_patternSearchMaxSuffixDepth, omp_get_thread_num(),
                                                                      &newHistory, maxInputVal, patternInputs.size(), _analyzer->_patternSearchRepetitions);
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
          if ((newHistory.size() / 2) < (unsigned int) _analyzer->_patternSearchMaxDepth) {
            // add the new state to the worklist
            PStatePlusIOHistory newState = PStatePlusIOHistory(newPState, newHistory);
#pragma omp critical(SOLVERNINEWL)
            {
              currentMaxDepth = currentMaxDepth < newHistory.size() ? newHistory.size() : currentMaxDepth;
              if (_analyzer->_patternSearchExplorationMode == Analyzer::EXPL_DEPTH_FIRST) {
                workList.push_front(newState);
              } else if (_analyzer->_patternSearchExplorationMode == Analyzer::EXPL_BREADTH_FIRST) {
                workList.push_back(newState);
              } else {
                logger[ERROR]<< "requested pattern search exploration mode currently not supported." << endl;
                ROSE_ASSERT(0);
              }
            }
          } else {
            ROSE_ASSERT(newHistory.size() / 2 == (unsigned int) _analyzer->_patternSearchMaxDepth);
          }
	} // end of else-case "no assertion, continue searching"
      } //end of "for each input value"-loop
    } // while
  } // omp parallel
  if (earlyTermination) {
    if (_analyzer->getOptionStatusMessages())  {
      cout<< "STATUS: solver 10 finished (found all assertions)." << endl;
    }
  } else {
    if (_analyzer->getOptionStatusMessages()) {
      cout<< "STATUS: solver 10 finished (empty worklist). " << endl;
    }
 }
}

void Solver10::initDiagnostics() {
  if (!_diagnosticsInitialized) {
    _diagnosticsInitialized = true;
    Solver::initDiagnostics(logger, getId());
  }
}
