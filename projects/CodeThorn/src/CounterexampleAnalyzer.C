#include "CounterexampleAnalyzer.h"

using namespace CodeThorn;

CounterexampleAnalyzer::CounterexampleAnalyzer(Analyzer& analyzer) : _analyzer(analyzer) {
}

CEAnalysisResult CounterexampleAnalyzer::analyzeCounterexample(string counterexample) {
  // OVERVIEW
  // (0) compare counterexample(CE) prefix with real trace
  // (1) initialize one hashset of seen EStates for each index of the CE cycle (bookkeeping)
  // (2) while (unknown whether or not CE is spurious)
  //   (2.1) compare one iteration of the CE's cycle part step-by-step with the original program
  //   (2.2) one step:  If (step is spurious) then determine the corresponding label in the approximated model and set "spuriousLabel". return "spurious"
  //                    Else if (step is not spurious and cycle found) then return "real"
  //                    Else store step in hashset (bookkeeping) and continue with the next step 
  // (3) return analysis result (including label for spurious transition's target, if existing)
  //cout << "DEBUG: counterexample: " << counterexample << endl;
  CEAnalysisResult result;
  result.analysisResult = CE_TYPE_UNKNOWN;
  PrefixAndCycle ceTrace = ceStringToInOutVals(counterexample);
  //save abstract model in order to be able to traverse it later (case of a spirious CE)
  _analyzer.reduceToObservableBehavior();
  _analyzer.storeStgBackup();
  int stepsToSpuriousLabel = 0;
  // (0) compare the prefix with traces on the original program
  list<CeIoVal> cePrefix = ceTrace.first;
  //run prefix on original program
  EState* startEState = const_cast<EState*>((_analyzer.getTransitionGraph())->getStartEState());
  _analyzer.resetAnalyzerToSolver8(startEState);
  setInputSequence(cePrefix);
  _analyzer.runSolver();
  _analyzer.reduceToObservableBehavior();
  TransitionGraph* traceGraph = _analyzer.getTransitionGraph();
  // compare
  const EState * compareFollowingHereNext = traceGraph->getStartEState();
  CEAnalysisStep currentResult = getSpuriousTransition(cePrefix, traceGraph, compareFollowingHereNext);
  if (currentResult.analysisResult == CE_TYPE_SPURIOUS) {
    stepsToSpuriousLabel = currentResult.spuriousIndexInCurrentPart;
  } else if (currentResult.analysisResult == CE_TYPE_UNKNOWN) { // only continue if prefix was not spurious
    list<CeIoVal> ceCycle = ceTrace.second;
    stepsToSpuriousLabel = cePrefix.size();
    // (1) initialize one hashset per index in the CE cycle (bookkeeping)
    list<boost::unordered_set<const EState*> >* statesPerCycleIndex = new list<boost::unordered_set<const EState*> >(); 
    for (unsigned int i = 0; i < ceCycle.size(); i++) {
      boost::unordered_set<const EState*> newSet;
      statesPerCycleIndex->push_back(newSet);
    }
    //(2) while (unknown if CE is spurious or not)
    while (currentResult.analysisResult == CE_TYPE_UNKNOWN) {
      //run one cycle iteration on the original program
      EState* continueTracingFromHere = const_cast<EState*>(_analyzer.getEstateBeforeMissingInput());
      setInputSequence(ceCycle);
      _analyzer.continueAnalysisFrom(continueTracingFromHere);
      _analyzer.reduceToObservableBehavior();
      traceGraph = _analyzer.getTransitionGraph();
      //compare
      compareFollowingHereNext = currentResult.mostRecentStateRealTrace;
      currentResult = getSpuriousTransition(ceCycle, traceGraph, compareFollowingHereNext, statesPerCycleIndex);
      if (currentResult.analysisResult == CE_TYPE_UNKNOWN) {
        stepsToSpuriousLabel += ceCycle.size();
      }
    }
    delete statesPerCycleIndex;
    statesPerCycleIndex = NULL;
  }
  // (3) process and return result
  _analyzer.swapStgWithBackup();
  if (currentResult.analysisResult == CE_TYPE_SPURIOUS) { 
    result.analysisResult = CE_TYPE_SPURIOUS;
    stepsToSpuriousLabel += currentResult.spuriousIndexInCurrentPart + 1;
    result.spuriousTargetLabel = getFirstSpuriousLabel(_analyzer.getTransitionGraph(), ceTrace, stepsToSpuriousLabel);
  } else if (currentResult.analysisResult == CE_TYPE_REAL) {
    result.analysisResult = CE_TYPE_REAL;
  } else {
    cout << "ERROR: counterexample should be either spurious or real." << endl;
    assert(0);
  }
  return result;
}

CEAnalysisStep CounterexampleAnalyzer::getSpuriousTransition(list<CeIoVal> partOfCeTrace, TransitionGraph* originalTraceGraph, 
                                           const EState* compareFollowingHere, list<boost::unordered_set<const EState*> >* statesPerCycleIndex) {
  const EState* currentState = compareFollowingHere;
  bool matchingState;
  CeIoVal realBehavior;
  int index = 0;
  CEAnalysisStep result;
  StateSets::iterator cycleIndexStates;
  if (statesPerCycleIndex) { //bookkeeping for analysing the cycle part of the counterexample
    cycleIndexStates = statesPerCycleIndex->begin();
  } 
  for (list<CeIoVal>::iterator i = partOfCeTrace.begin(); i != partOfCeTrace.end(); i++) {
    matchingState = false;
    EStatePtrSet successors = originalTraceGraph->succ(currentState);
    // retrieve the correct state at branches in the original program (branches may exist due to inherent loops)
    // note: assuming deterministic input/output programs
    for (EStatePtrSet::iterator succ = successors.begin(); succ != successors.end(); succ++) {
      realBehavior = eStateToCeIoVal(*succ);
      if (realBehavior == (*i)) {  //no spurious behavior when following this path, so follow it
        currentState = (*succ);
        index++;
        matchingState = true;
        if (statesPerCycleIndex) {
          //check if the real state was already seen
          pair<boost::unordered_set<const EState*>::iterator, bool> notYetSeen = cycleIndexStates->insert(currentState);
          if (notYetSeen.second == false) {
            //state encountered twice at the some counterexample index. cycle found --> real counterexample
            result.analysisResult = CE_TYPE_REAL;
            return result;
          } else {
            cycleIndexStates++;
          }
        }
      }
    }
    // if no matching transition was found, then the counterexample trace is spurious
    if (!matchingState) {
      result.analysisResult = CE_TYPE_SPURIOUS;
      result.spuriousIndexInCurrentPart = index;
      return result;
    }
  }
  result.analysisResult = CE_TYPE_UNKNOWN;
  result.mostRecentStateRealTrace = currentState;
  // set and return the result
  //   take care of the corner case that the last prefix index contains an input symbol and 
  //   in the original program, there is an error state following (futher tracing not possible).
  if ((partOfCeTrace.rbegin())->second == IO_TYPE_INPUT) {
    EStatePtrSet successors = originalTraceGraph->succ(currentState);
    assert(successors.size() == 1);  //(input) determinism of the original program
    const EState * nextEState = *(successors.begin());
    assert(nextEState);
    if (nextEState->io.isStdErrIO() || nextEState->io.isFailedAssertIO()) {
      // because the cycle part of the CE needs to contain at least one element in the RERS programs,
      // the CE is spurious here.
      result.analysisResult = CE_TYPE_SPURIOUS;
      result.spuriousIndexInCurrentPart = index;
    }
  }
  return result; // partOfCeTrace could be successfully traversed on the originalTraceGraph
}

CeIoVal CounterexampleAnalyzer::eStateToCeIoVal(const EState* eState) {
  PState* pstate = const_cast<PState*>( eState->pstate() ); 
  int inOutVal;
  pair<int, IoType> result;
  if (eState->io.isStdInIO()) {
    inOutVal = (*pstate)[_analyzer.globalVarIdByName("input")].getValue().getIntValue();
    result = pair<int, IoType>(inOutVal, CodeThorn::IO_TYPE_INPUT);
  } else if (eState->io.isStdOutIO()) {
    inOutVal = (*pstate)[_analyzer.globalVarIdByName("output")].getValue().getIntValue();
    result = pair<int, IoType>(inOutVal, CodeThorn::IO_TYPE_OUTPUT);
  } else if (eState->io.isStdErrIO() || eState->io.isFailedAssertIO()) {
    result= pair<int, IoType>(-1, CodeThorn::IO_TYPE_ERROR);
  } else {
    cout << "ERROR: parameter of \"eStateToCeIoVal(...)\" should only include states with observable behavior (input/output/error)" << endl; 
    assert(0); 
  } 
  return result;
}

IoType CounterexampleAnalyzer::getIoType(InputOutput io) {
  if (io.isStdInIO()) {
    return IO_TYPE_INPUT;
  } else if (io.isStdOutIO()) {
    return IO_TYPE_OUTPUT;
  } else if (io.isStdErrIO() || io.isFailedAssertIO()) {
    return IO_TYPE_ERROR;
  } else {
    return IO_TYPE_UNKNOWN;
  }
}

PrefixAndCycle CounterexampleAnalyzer::ceStringToInOutVals(string counterexample) {
  list<CeIoVal> prefix;
  list<CeIoVal> cycle;
  bool isCycle = false;
  string::iterator i = counterexample.begin();
  while (i != counterexample.end()) {
    if ((*i) == 'i') {
      i++;
      if (i == counterexample.end()) {
        cout << "ERROR: counterexample in wrong format: 'i' at the end of the string." << endl; assert(0);
      }
      int value = (int) (*i) - ((int) 'A') + 1;
      if (isCycle) {
        cycle.push_back(pair<int, IoType>(value, CodeThorn::IO_TYPE_INPUT));
      } else {
        prefix.push_back(pair<int, IoType>(value, CodeThorn::IO_TYPE_INPUT));
      }
    }
    else if ((*i) == 'o') {
      i++;
      if (i == counterexample.end()) {
        cout << "ERROR: counterexample in wrong format: 'o' at the end of the string." << endl; assert(0);
      }
      int value = (int) (*i) - ((int) 'A') + 1;
       if (isCycle) {
        cycle.push_back(pair<int, IoType>(value, CodeThorn::IO_TYPE_OUTPUT));
      } else {
        prefix.push_back(pair<int, IoType>(value, CodeThorn::IO_TYPE_OUTPUT));
      }
    } else if ((*i) == '(') {
      isCycle = true;
    }
    i++;
  }
  return PrefixAndCycle(prefix, cycle);
}

void CounterexampleAnalyzer::setInputSequence(list<CeIoVal> sourceOfInputs) {
  _analyzer.resetToEmptyInputSequence();
  for (list<CeIoVal>::iterator j = sourceOfInputs.begin(); j != sourceOfInputs.end(); j++) {
    if (j->second == IO_TYPE_INPUT) {
      _analyzer.addInputSequenceValue(j->first);
    }
  } 
  _analyzer.resetInputSequenceIterator();  
}

Label CounterexampleAnalyzer::getFirstSpuriousLabel(TransitionGraph* analyzedModel, PrefixAndCycle counterexample, int numberOfSteps) {
  boost::unordered_set<const EState*>* currentDepth = new boost::unordered_set<const EState*>();
  boost::unordered_set<const EState*>* nextDepth = new boost::unordered_set<const EState*>();
  currentDepth->insert(analyzedModel->getStartEState());
  list<CeIoVal> prefix = counterexample.first;
  list<CeIoVal> cycle = counterexample.second;
  unsigned int prefixIndex = 0;
  unsigned int cycleIndex = 0;
  assert(prefix.size() > 0); // in case of an empty prefix, refine the initialization here
  list<CeIoVal>::iterator currentCeBehavior = prefix.begin();
  bool inCycle = false;
  // traverse all paths in the analyzed model that match with the counterexample's behavior
  for (int i = 0; i < numberOfSteps; i++) {
    //traverse one more level
    for (boost::unordered_set<const EState*>::iterator k = currentDepth->begin(); k != currentDepth->end(); k++) {
      EStatePtrSet successors = analyzedModel->succ(*k);
      // add those successor states to "nextDepth" that match the desired behavior
      for (EStatePtrSet::iterator m = successors.begin(); m != successors.end(); m++) {
        if (eStateToCeIoVal(*m) == *currentCeBehavior) {
          nextDepth->insert(*m);
        }
      }
    } 
    // swap currentDepth and nextDepth, then clear nextDepth
    boost::unordered_set<const EState*>* swapTemp = currentDepth;  
    currentDepth = nextDepth;
    nextDepth = swapTemp;
    nextDepth->clear();
    //update currentCeBehavior
    if (!inCycle) {
      if (prefixIndex < prefix.size() - 1) {
        currentCeBehavior++;
        prefixIndex++;
      } else { 
        currentCeBehavior = cycle.begin();
        inCycle = true;
      }
    } else {
      if (cycleIndex < cycle.size() - 1) {
        currentCeBehavior++;
        cycleIndex++;
      } else { 
        currentCeBehavior = cycle.begin();
        cycleIndex = 0;
      }
    }

  }
  // the counterexample itself comes from analyzing "anaylzedModel", there has to be at least one matching state
  assert (currentDepth->size() > 0);
  const EState* firstMatch = *(currentDepth->begin());
  delete currentDepth;
  delete nextDepth;
  return firstMatch->label();
}

