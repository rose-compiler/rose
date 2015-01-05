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
  //save abstract model in order to be able to traverse it later (in the case of a spurious CE)
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
    // the number of steps to the spurious label will be updated at the end of this function
  } else if (currentResult.analysisResult == CE_TYPE_UNKNOWN) { // only continue if prefix was not spurious
    list<CeIoVal> ceCycle = ceTrace.second;
    stepsToSpuriousLabel = cePrefix.size();
    // (1) initialize one hashset per index in the cyclic part of the CE (bookkeeping)
    list<boost::unordered_set<const EState*> >* statesPerCycleIndex = new list<boost::unordered_set<const EState*> >(); 
    for (unsigned int i = 0; i < ceCycle.size(); i++) {
      boost::unordered_set<const EState*> newSet;
      statesPerCycleIndex->push_back(newSet);
    }
    //(2) while (unknown whether or not CE is spurious)
    while (currentResult.analysisResult == CE_TYPE_UNKNOWN) {
      if (currentResult.continueTracingOriginal) {
        //run one cycle iteration on the original program
        EState* continueTracingFromHere = const_cast<EState*>(_analyzer.getEstateBeforeMissingInput());
        assert(continueTracingFromHere);
        setInputSequence(ceCycle);
        _analyzer.continueAnalysisFrom(continueTracingFromHere);
        _analyzer.reduceToObservableBehavior();
        traceGraph = _analyzer.getTransitionGraph();
      }
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
    // special case for an output followed by a failing assertion in the original program 
    // (The approximated model trace does not contain the assertion. Refinement will use the assertion's guarding condition.)
    if (currentResult.assertionCausingSpuriousInput) {
      result.spuriousTargetLabel = currentResult.failingAssertionInOriginal;
    } else {
      stepsToSpuriousLabel += currentResult.spuriousIndexInCurrentPart;
      result.spuriousTargetLabel = getFirstObservableSpuriousLabel(_analyzer.getTransitionGraph(), ceTrace, stepsToSpuriousLabel);
    }
  } else if (currentResult.analysisResult == CE_TYPE_REAL) {
    result.analysisResult = CE_TYPE_REAL;
  } else {
    cout << "ERROR: counterexample should be either spurious or real." << endl;
    assert(0);
  }
  return result;
}

string CounterexampleAnalyzer::ioErrTraceToString(list<pair<int, IoType> > trace) {
  string result = "";
  for (std::list<pair<int, IoType> >::iterator i = trace.begin(); i != trace.end(); i++) {
    if (i != trace.begin()) {
      result += ",";
    }
    if (i->second == CodeThorn::IO_TYPE_ERROR) {
      result += "ERR";
    } else if (i->second == CodeThorn::IO_TYPE_INPUT) {
      result += "i";
      result = result + ((char) (i->first + ((int) 'A') - 1));
    } else if (i->second == CodeThorn::IO_TYPE_OUTPUT) {
      result += "o";
      result = result + ((char) (i->first + ((int) 'A') - 1));
    } else {
      cout << "ERROR: malformatted ioErrTrace. "<< endl; 
      assert(0); 
    }
  }
  return result;
}

CEAnalysisStep CounterexampleAnalyzer::getSpuriousTransition(list<CeIoVal> partOfCeTrace, TransitionGraph* originalTraceGraph, 
                                           const EState* compareFollowingHere, list<boost::unordered_set<const EState*> >* statesPerCycleIndex) {
  assert(compareFollowingHere);
  const EState* currentState = compareFollowingHere;
  bool matchingState;
  CeIoVal realBehavior;
  int index = 1;
  CEAnalysisStep result;
  result.assertionCausingSpuriousInput = false;
  StateSets::iterator cycleIndexStates;
  if (statesPerCycleIndex) { //bookkeeping for analyzing the cyclic part of the counterexample
    cycleIndexStates = statesPerCycleIndex->begin();
  }
  // iterate over part of the counterexample and compare its behavior step-by-step against the original program's trace
  for (list<CeIoVal>::iterator i = partOfCeTrace.begin(); i != partOfCeTrace.end(); i++) {
    matchingState = false;
    EStatePtrSet successors = originalTraceGraph->succ(currentState);
    // the trace graph should always contain enough states for the comparision
    assert(successors.size()>0);
    // retrieve the correct state at branches in the original program (branches may exist due to inherent loops)
    // note: assuming deterministic input/output programs
    for (EStatePtrSet::iterator succ = successors.begin(); succ != successors.end(); succ++) {
      realBehavior = eStateToCeIoVal(*succ);
      //cout << "DEBUG: realBehavior: " << ceIoValToString(realBehavior) << "   ceTraceItem: " << ceIoValToString(*i) << endl;
      if (realBehavior == (*i)) {  //no spurious behavior when following this path, so continue here
        currentState = (*succ);
        index++;
        matchingState = true;
        if (statesPerCycleIndex) {
          //check if the real state has already been seen
          pair<boost::unordered_set<const EState*>::iterator, bool> notYetSeen = cycleIndexStates->insert(currentState);
          if (notYetSeen.second == false) {
            //state encountered twice at the some counterexample index. cycle found --> real counterexample
            result.analysisResult = CE_TYPE_REAL;
            return result;
          } else {
            cycleIndexStates++;
          }
        }
      // special case. output-> failing assertion occurs during execution of the original program, leading to a spurious input symbol.
      // RefinementConstraints should use the failing assertion's condition to add new constraints, trying to enforce the assertion to fail
      // and thus eliminating the spurious input symbol.
      } else if (realBehavior.second == IO_TYPE_ERROR && i->second == IO_TYPE_INPUT) {
        result.assertionCausingSpuriousInput = true;
        result.failingAssertionInOriginal = (*succ)->label();
      }
    }
    // if no matching transition was found, then the counterexample trace is spurious
    if (!matchingState) {
      result.analysisResult = CE_TYPE_SPURIOUS;
      result.spuriousIndexInCurrentPart = index;
      return result;
    }
  }
  IoType mostRecentOriginalIoType = (partOfCeTrace.rbegin())->second;
  determineAnalysisStepResult(result, mostRecentOriginalIoType, currentState, originalTraceGraph, index);
  return result; // partOfCeTrace could be successfully traversed on the originalTraceGraph
}

void CounterexampleAnalyzer::determineAnalysisStepResult(CEAnalysisStep& result, IoType mostRecentOriginalIoType, 
                                                const EState* currentState, TransitionGraph* originalTraceGraph, int index) {
  result.analysisResult = CE_TYPE_UNKNOWN;
  result.mostRecentStateRealTrace = currentState;
  result.continueTracingOriginal = true;
  // set and return the result
  //  Takes care of the corner case that error states after the lastly assessed input symbol render
  //  further tracing of the original program's path impossible.
  if (mostRecentOriginalIoType == IO_TYPE_INPUT) {
    EStatePtrSet successors = originalTraceGraph->succ(currentState);
    assert(successors.size() == 1);  //(input-)determinism of the original program
    const EState * nextEState = *(successors.begin());
    assert(nextEState);
    if (nextEState->io.isStdErrIO() || nextEState->io.isFailedAssertIO()) {
      // (*) see below (the first symbol of the next partOfCeTrace has to be the spurious transition here)
      result.analysisResult = CE_TYPE_SPURIOUS;
      result.spuriousIndexInCurrentPart = index;
    } else if (nextEState->io.isStdOutIO()) {
      successors = originalTraceGraph->succ(nextEState);
      for (EStatePtrSet::iterator i = successors.begin(); i != successors.end(); ++i) {
        const EState* nextEState = *i;
        if (nextEState->io.isStdErrIO() || nextEState->io.isFailedAssertIO()) {
          // (*) see below
          result.continueTracingOriginal = false;
        }
      }
    }
  } else if (mostRecentOriginalIoType == IO_TYPE_OUTPUT) {
    EStatePtrSet successors = originalTraceGraph->succ(currentState);
    for (EStatePtrSet::iterator i = successors.begin(); i != successors.end(); ++i) {
      const EState* nextEState = *i;
      if (nextEState->io.isStdErrIO() || nextEState->io.isFailedAssertIO()) {
        // (*) see below
        result.continueTracingOriginal = false;
      }
    }
  }
  // (*) there is a not yet seen error state. Because every state except from successors of the last input state of the trace have
  //     been looked at already, the cycle part of the CE contains no input state before the error state. RERS programs need to
  //     contain at least one input symbol in the cycle part of a counterexample --> this counterexample is spurious.
}

CeIoVal CounterexampleAnalyzer::eStateToCeIoVal(const EState* eState) {
  PState* pstate = const_cast<PState*>( eState->pstate() ); 
  int inOutVal;
  pair<int, IoType> result;
  if (eState->io.isStdInIO()) {
    inOutVal = (*pstate)[_analyzer.globalVarIdByName("input")].getValue().getIntValue();
    result = pair<int, IoType>(inOutVal, CodeThorn::IO_TYPE_INPUT);
  } else if (eState->io.isStdOutIO()) {
    if (eState->io.op == InputOutput::STDOUT_VAR) {
      inOutVal = (*pstate)[_analyzer.globalVarIdByName("output")].getValue().getIntValue();
    } else if (eState->io.op == InputOutput::STDOUT_CONST) {
      inOutVal = eState->io.val.getIntValue();
    } else {
      cout << "ERROR: unknown InputOutput output OpType." << endl;
      assert(0);
    }
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
      // cout << "DEBUG: added input value " << j->first <<  " to the analyzer" << endl;
    }
  } 
  _analyzer.resetInputSequenceIterator();  
}

Label CounterexampleAnalyzer::getFirstObservableSpuriousLabel(TransitionGraph* analyzedModel, PrefixAndCycle counterexample, int numberOfSteps) {
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
  assert(firstMatch);
  return firstMatch->label();
}

string CounterexampleAnalyzer::ceIoValToString(CeIoVal& ioVal) {
  string result = "";
  IoType type = ioVal.second;
  bool addValue = true;
  switch (type) {
    case IO_TYPE_INPUT: {
      result+="i"; break;
    }
    case IO_TYPE_OUTPUT: {
      result+="o"; break;
    }
    case IO_TYPE_ERROR: {
      result+="e"; addValue = false; break;
    }
    default: {
      result+="?"; addValue = false; break;
    }
  }
  if (addValue) {
    result += boost::lexical_cast<string>(ioVal.first);
  }
  return result;
}

