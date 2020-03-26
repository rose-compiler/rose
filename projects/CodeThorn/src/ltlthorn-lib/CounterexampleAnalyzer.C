// Author: Marc Jasper, 2014, 2015.

#include "CounterexampleAnalyzer.h"
#include "CodeThornCommandLineOptions.h"

using namespace CodeThorn;
using namespace std;

CounterexampleAnalyzer::CounterexampleAnalyzer(IOAnalyzer* analyzer) 
			: _analyzer(analyzer), _csvOutput(NULL), _maxCounterexamples(0) {}

CounterexampleAnalyzer::CounterexampleAnalyzer(IOAnalyzer* analyzer, stringstream* csvOutput) 
                        : _analyzer(analyzer), _csvOutput(csvOutput), _maxCounterexamples(-1) {}

CEAnalysisResult CounterexampleAnalyzer::analyzeCounterexample(string counterexample, const EState* startState, 
								bool returnSpuriousLabel, bool resetAnalyzerData) {
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
  int stepsToSpuriousLabel = 0;
  if (returnSpuriousLabel) {
    //save abstract model in order to be able to traverse it later (in the case of a spurious CE)
    _analyzer->reduceToObservableBehavior();
    _analyzer->storeStgBackup();
  }
  // (0) compare the prefix with traces on the original program
  list<CeIoVal> cePrefix = ceTrace.first;
  //run prefix on original program
  EState* startEState;
  if (startState) {
    //workaround for the start state not being part of the STG without having transitions 
    startEState = const_cast<EState*>(startState);
  } else {
    startEState = const_cast<EState*>((_analyzer->getTransitionGraph())->getStartEState());
  }
  ROSE_ASSERT(startEState);
  _analyzer->setAnalyzerToSolver8(startEState, resetAnalyzerData);
  setInputSequence(cePrefix);
  _analyzer->runSolver();
  _analyzer->reduceToObservableBehavior();
  TransitionGraph* traceGraph = _analyzer->getTransitionGraph(); 
  // compare
  const EState * compareFollowingHereNext = traceGraph->getStartEState();
  CEAnalysisStep currentResult = getSpuriousTransition(cePrefix, traceGraph, compareFollowingHereNext);
  if (currentResult.analysisResult == CE_TYPE_SPURIOUS) {
    // the number of steps to the spurious label will be updated at the end of this function
  } else if (currentResult.analysisResult == CE_TYPE_UNKNOWN) { // only continue if prefix was not spurious
    list<CeIoVal> ceCycle = ceTrace.second;
    stepsToSpuriousLabel = cePrefix.size();
    // (1) initialize one hashset per index in the cyclic part of the CE (bookkeeping)
    StateSets* statesPerCycleIndex = new StateSets(); 
    for (unsigned int i = 0; i < ceCycle.size(); i++) {
      boost::unordered_set<const EState*> newSet;
      statesPerCycleIndex->push_back(newSet);
    }
    //(2) while (unknown whether or not CE is spurious)
    while (currentResult.analysisResult == CE_TYPE_UNKNOWN) {
      if (currentResult.continueTracingOriginal) {
        //run one cycle iteration on the original program
        EState* continueTracingFromHere = const_cast<EState*>(_analyzer->getEstateBeforeMissingInput());
        assert(continueTracingFromHere);
        setInputSequence(ceCycle);
        _analyzer->continueAnalysisFrom(continueTracingFromHere);
        _analyzer->reduceToObservableBehavior();
        traceGraph = _analyzer->getTransitionGraph();
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
  if (returnSpuriousLabel) {
    _analyzer->swapStgWithBackup();
  }
  if (currentResult.analysisResult == CE_TYPE_SPURIOUS) { 
    result.analysisResult = CE_TYPE_SPURIOUS;
    //determine final number of steps to first observable spurious transition
    //   special case for an output followed by a failing assertion in the original program 
    //   (The approximated model trace does not contain the assertion. Refinement will use the assertion's guarding condition.)
    if (currentResult.assertionCausingSpuriousInput) {
      if (returnSpuriousLabel) {
        result.spuriousTargetLabel = currentResult.failingAssertionInOriginal;
      }
    } else {
      stepsToSpuriousLabel += currentResult.spuriousIndexInCurrentPart;
      if (returnSpuriousLabel) {
        result.spuriousTargetLabel = getFirstObservableSpuriousLabel(_analyzer->getTransitionGraph(), ceTrace, stepsToSpuriousLabel);
      }
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
                                           const EState* compareFollowingHere, StateSets* statesPerCycleIndex) {
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

PropertyValueTable* CounterexampleAnalyzer::cegarPrefixAnalysisForLtl(SpotConnection& spotConnection, 
									set<int> ltlInAlphabet, set<int> ltlOutAlphabet) {
  PropertyValueTable* currentResults = spotConnection.getLtlResults();
  // call the counterexample-guided prefix refinement for all analyzed LTL properties
  for (unsigned int i=0; i < spotConnection.getLtlResults()->size(); i++) { 
    currentResults = cegarPrefixAnalysisForLtl(i, spotConnection, ltlInAlphabet, ltlOutAlphabet);
  }
  return currentResults;
}

PropertyValueTable* CounterexampleAnalyzer::cegarPrefixAnalysisForLtl(int property, SpotConnection& spotConnection, 
									set<int> ltlInAlphabet, set<int> ltlOutAlphabet) {
  // visualizer for in-depth model outputs (.dot files)
  Visualizer visualizer(_analyzer->getLabeler(),_analyzer->getVariableIdMapping(),
                          _analyzer->getFlow(),_analyzer->getPStateSet(),_analyzer->getEStateSet(),_analyzer->getTransitionGraph());
  string vizFilenamePrefix = "";
  if(args.isDefined("viz-cegpra-detailed")) {
    vizFilenamePrefix=args.getString("viz-cegpra-detailed");
    string filename = vizFilenamePrefix + "_cegpra_init.dot";
    writeDotGraphToDisk(filename, visualizer);
  }
  // OVERVIEW
  // (0) check if the initial model already satsifies the property
  // (0.5) initialization
  // (while (property != satisfied)) do
  //   (1) Disconnect the concrete prefix (initially the start state) from the over-approx. part of the model
  //   (2) Anaylze the most recent counterexample while adding the trace of the original program to the prefix part of the model
  //       If the counterexample is real: reconnect once more to determine the model's size, ESCAPE the loop and return results 
  //   (3) Reconnect both parts of the model
  //   (4) Check the property on the now slightly refined model
  // od
  // (5) return results;
  cout << "STATUS: CEGPRA is now analyzing LTL property " << property << "..." << endl;
  if (_csvOutput) {
    (*_csvOutput) << endl << property << ",";
  }
  TransitionGraph* model = _analyzer->getTransitionGraph();
  assert(model->isComplete());
  // (0) check if the given property already holds on the initial over-approximated model
  PropertyValueTable* currentResults = spotConnection.getLtlResults();
  if (currentResults->getPropertyValue(property) != PROPERTY_VALUE_UNKNOWN) {
    cout << "STATUS: property " << property << " was already analyzed. CEGAR analysis will not be started." << endl;
    return currentResults;
  }
  spotConnection.checkSingleProperty(property, *model, ltlInAlphabet, ltlOutAlphabet, true, true);
  currentResults = spotConnection.getLtlResults();
  // (0.5) prepare for the continuous tracing of concrete states (will become the prefix of a refined abstract model)
  // store connectors in the over-approx. part of the model (single row of input states in the initial "topified" model)
  const EState* startEState = model->getStartEState();
  pair<EStatePtrSet, EStatePtrSet> concOutputAndAbstrInput = getConcreteOutputAndAbstractInput(model); 
  EStatePtrSet startAndOuputStatesPrefix = concOutputAndAbstrInput.first;
  vector<const EState*> firstInputOverApprox(ltlInAlphabet.size());
  firstInputOverApprox = sortAbstractInputStates(firstInputOverApprox, concOutputAndAbstrInput.second);
  int loopCount = 0;
  bool falsified = false;
  bool verified = true;   // the usual case for the loop below to terminate is a verified property.
  string ce = "no counterexample yet";
  // as long as the property is not satisfiable yet, refine by enlarging the prefix of concrete states according to counterexamples
  while (currentResults->getPropertyValue(property) != PROPERTY_VALUE_YES) { 
    if (_maxCounterexamples > -1 && (loopCount + 1) > _maxCounterexamples) {
      verified = false;
      spotConnection.resetLtlResults(property);
      break;
    }
    loopCount++;
    if (loopCount % 50 == 0) {
      cout << "STATUS: " << loopCount << " counterexamples analyzed. most recent counterexample: " << endl;
      cout << ce << endl;
    }
    // (1) disconnect prefix and over-approx. part of the model
    model->setIsComplete(false);
    for (unsigned int i = 0; i < firstInputOverApprox.size(); i++) {
       TransitionPtrSet connectionsToPrefix = model->inEdges(firstInputOverApprox[i]);
       for (TransitionPtrSet::iterator k=connectionsToPrefix.begin(); k!=connectionsToPrefix.end(); ++k) {
         // check if predeccesor at the source of that transition is a concrete (prefix) state
         if ( !(*k)->source->isRersTopified(_analyzer->getVariableIdMapping()) ) {
           model->erase(**k);
         }
       }
    }
    model->setIsPrecise(true);
    if(args.isDefined("viz-cegpra-detailed")) {
      stringstream filenameStream;
      filenameStream << vizFilenamePrefix << "cegpra_afterDisconnect_i" << loopCount << ".dot";
      writeDotGraphToDisk(filenameStream.str(), visualizer);
    }
    // (2) add a trace to the prefix according to the most recent counterexample. Analyze the counterexample while adding the trace.
    ce = currentResults->getCounterexample(property);
    //cout << "STATUS: counterexample: " << ce << endl;
    CEAnalysisResult ceaResult = analyzeCounterexample(ce, startEState, false, false);
    if (ceaResult.analysisResult == CE_TYPE_REAL) {
      // still reconnect the concrete prefix with the over-approx. part of the model (step (3)) in order to report the size.
      falsified = true;
      verified = false;
    } else if (ceaResult.analysisResult == CE_TYPE_SPURIOUS) {
      if(!args.getBool("keep-error-states")) {
        // remove a trace leading to an error state and mark the branches to it (do not reconnect in phase 3) 
        removeAndMarkErroneousBranches(model);
      }
      //the trace eliminating the spurious counterexample (maybe including a few extra states) was added to the prefix during analysis.
      // --> nothing to do here
    } else {
      assert(0);  //counterexample analysis not successfully completed
    }
    if(args.isDefined("viz-cegpra-detailed")) {
      stringstream filenameStream;
      filenameStream << vizFilenamePrefix << "cegpra_afterCECheck_i" << loopCount << ".dot";
      writeDotGraphToDisk(filenameStream.str(), visualizer);
    }
    // (3) reconnect both parts of the model
    model->setIsPrecise(false);
    //update set of output states (plus start state) in the precise prefix
    addAllPrefixOutputStates(startAndOuputStatesPrefix, model);
    for (set<const EState*>::iterator i=startAndOuputStatesPrefix.begin(); i!=startAndOuputStatesPrefix.end(); ++i) {
      vector<bool> inputSuccessors(ltlInAlphabet.size(), false);
      if(!args.getBool("keep-error-states")) {
        inputSuccessors = setErrorBranches(inputSuccessors, *i); 
      }
      // determine which input states exist as successors in the prefix
      inputSuccessors = hasFollowingInputStates(inputSuccessors, *i, model);
      //connect with the approx. part of the model for all input values not found among the successor states
      for (unsigned int k = 0; k < inputSuccessors.size(); k++) {
        if (!inputSuccessors[k]) {
          Edge newEdge;
          model->add(Transition((*i), newEdge, firstInputOverApprox[k]));
        }
      }
    }
    model->setIsComplete(true);
    if(args.isDefined("viz-cegpra-detailed")) {
      stringstream filenameStream;
      filenameStream << vizFilenamePrefix << "cegpra_afterReconnect_i" << loopCount << ".dot";
      writeDotGraphToDisk(filenameStream.str(), visualizer);
    }
    // if falsified: after reconnecting, leave the analysis loop, report size of the model and return the results
    if (falsified) {
      break;
    }
    // (4) check if the property holds on the refined model
    spotConnection.resetLtlResults(property);
    spotConnection.checkSingleProperty(property, *model, ltlInAlphabet, ltlOutAlphabet, true, true);
    currentResults = spotConnection.getLtlResults();
  }
  // (5) check all properties using the current model and return the result
  spotConnection.checkLtlProperties(*model, ltlInAlphabet, ltlOutAlphabet, true, false);
  currentResults = spotConnection.getLtlResults();
  printStgSizeAndCeCount(model, loopCount, property);
  if (_csvOutput) {
    if (verified && !falsified)
      (*_csvOutput) << "y,";
    if (!verified && falsified)
      (*_csvOutput) << "n,";
    if (!verified && !falsified)
      (*_csvOutput) << "?,";
    if (verified && falsified) {
      cout << "ERROR: property can not be both verified and falsified. " << endl;
      assert(0);
    }
    (*_csvOutput) << currentResults->entriesWithValue(PROPERTY_VALUE_YES)<<",";
    (*_csvOutput) << currentResults->entriesWithValue(PROPERTY_VALUE_NO)<<",";
    (*_csvOutput) << currentResults->entriesWithValue(PROPERTY_VALUE_UNKNOWN);
  }
  return currentResults;
}

void CounterexampleAnalyzer::removeAndMarkErroneousBranches(TransitionGraph* model) {
  const EState* errorState = _analyzer->getLatestErrorEState();
  if (errorState) {
    list<pair<const EState*, int> > erroneousTransitions = removeTraceLeadingToErrorState(errorState, model);
    // store entries for the newly discovered transitions to an error path (same path should never be discovered twice)
    for (list<pair<const EState*, int> >::iterator i=erroneousTransitions.begin(); i!=erroneousTransitions.end(); ++i) {
      InputsAtEState::iterator errorStateEntry = _erroneousBranches.find(i->first);
      if (errorStateEntry != _erroneousBranches.end()) {
        errorStateEntry->second.push_back(i->second);
      } else {
        list<int> newEntryTransitionList;
        newEntryTransitionList.push_back(i->second);
        pair<const EState*, list<int> > newEntry(i->first, newEntryTransitionList);
        _erroneousBranches.insert(newEntry);
      }
    }
  }
}

pair<EStatePtrSet, EStatePtrSet> CounterexampleAnalyzer::getConcreteOutputAndAbstractInput(TransitionGraph* model) {
  EStatePtrSet allEStates=model->estateSet();
  EStatePtrSet concreteOutputStates;
  EStatePtrSet abstractInputStates;
  concreteOutputStates.insert(model->getStartEState());  // the start state has following input states just like output states do.
  for(EStatePtrSet::iterator i=allEStates.begin(); i!=allEStates.end(); ++i) {
    if ((*i)->isRersTopified(_analyzer->getVariableIdMapping())) {
      if ((*i)->io.isStdInIO()) {
        abstractInputStates.insert(*i);
      }
    } else {
      if ((*i)->io.isStdOutIO()) {
        concreteOutputStates.insert(*i);
      }
    } 
  }
  return pair<EStatePtrSet, EStatePtrSet> (concreteOutputStates, abstractInputStates);
}

vector<const EState*> CounterexampleAnalyzer::sortAbstractInputStates(vector<const EState*> v, EStatePtrSet abstractInputStates) {
  for (EStatePtrSet::iterator i=abstractInputStates.begin(); i!=abstractInputStates.end(); ++i) {
    PState* pstate = const_cast<PState*>( (*i)->pstate() ); 
    int inVal = pstate->readFromMemoryLocation(_analyzer->globalVarIdByName("input")).getIntValue();
    v[inVal - 1] = (*i);
  }
  return v;
}

vector<const EState*> CounterexampleAnalyzer::getFollowingInputStates(vector<const EState*> v, const EState* startEState, TransitionGraph* model) {
  EStatePtrSet firstInputStates = model->succ(startEState);
  for (EStatePtrSet::iterator i=firstInputStates.begin(); i!=firstInputStates.end(); ++i) {
    if ((*i)->io.isStdInIO()) {
      PState* pstate = const_cast<PState*>( (*i)->pstate() ); 
      int inVal = pstate->readFromMemoryLocation(_analyzer->globalVarIdByName("input")).getIntValue();
      v[inVal - 1] = (*i);
    } else {
      cout << "ERROR: CounterexampleAnalyzer::cegarPrefixAnalysisForLtl: successor of initial model's start state is not an input state." << endl;
      assert(0); 
    }
  }
  return v;
}

vector<bool> CounterexampleAnalyzer::hasFollowingInputStates(vector<bool> v, const EState*eState, TransitionGraph* model) {
  EStatePtrSet successors = model->succ(eState);
  for (EStatePtrSet::iterator k=successors.begin(); k!=successors.end(); ++k) {
    if ((*k)->io.isStdInIO()) {
      PState* pstate = const_cast<PState*>( (*k)->pstate() ); 
      int inVal = pstate->readFromMemoryLocation(_analyzer->globalVarIdByName("input")).getIntValue();
      v[inVal - 1] = true; 
    }else {
      cout << "ERROR: CounterexampleAnalyzer::cegarPrefixAnalysisForLtl: successor of prefix output (or start) state is not an input state." << endl;
      assert(0); 
    }
  }
  return v;
}

vector<bool> CounterexampleAnalyzer::setErrorBranches(vector<bool> v, const EState* eState) {
  // do not (re-)connect in a way as to pretend the existence of a path where erroneous behavior was previously discovered
  boost::unordered_map<const EState*, list<int> >::iterator errorStateEntry = _erroneousBranches.find(eState);
  if (errorStateEntry != _erroneousBranches.end()) {
    list<int> dontFollowTheseInputs = errorStateEntry->second;
    for (list<int>::iterator k=dontFollowTheseInputs.begin(); k!=dontFollowTheseInputs.end(); ++k) {
      v[(*k) - 1] = true;
    }
  }
  return v;
}

EStatePtrSet CounterexampleAnalyzer::addAllPrefixOutputStates(EStatePtrSet& startAndOuputStatesPrefix, TransitionGraph* model) {
  EStatePtrSet allEStates = model->estateSet();
  for (EStatePtrSet::iterator i=allEStates.begin(); i!=allEStates.end(); ++i) {
    //start state has already been added
    if ((*i)->io.isStdOutIO()) {
      if (! (*i)->isRersTopified(_analyzer->getVariableIdMapping())) {
        startAndOuputStatesPrefix.insert(*i);
      }
    }  
  }
  return startAndOuputStatesPrefix;
}

bool CounterexampleAnalyzer::isPrefixState(const EState* state) {
  boost::regex re("a(.)*");
  const PState* pstate = state->pstate();
  AbstractValueSet varSet=pstate->getVariableIds();
  for (AbstractValueSet::iterator l=varSet.begin();l!=varSet.end();++l) {
    string varName=(*l).toLhsString(_analyzer->getVariableIdMapping());
    if (boost::regex_match(varName, re)) { //matches internal variables (e.g. "int a188")
      if (pstate->varIsConst(*l)) {  // is a concrete (therefore prefix) state
        return true;
      } else {
        return false;
      }
    }
  }
  cout << "ERROR: CounterexampleAnalyzer::isPrefixState could not find a matching variable to analyze. " << endl;
  assert(0);
}

list<pair<const EState*, int> > CounterexampleAnalyzer::removeTraceLeadingToErrorState(const EState* errorState, TransitionGraph* stg) {
  assert(errorState->io.isFailedAssertIO() || errorState->io.isStdErrIO() );
  list<pair<const EState*, int> > erroneousTransitions;
  PState* pstate = const_cast<PState*>( errorState->pstate() ); 
  int latestInputVal = pstate->readFromMemoryLocation(_analyzer->globalVarIdByName("input")).getIntValue();
  //eliminate the error state
  const EState* eliminateThisOne = errorState;
  EStatePtrSet preds = stg->pred(eliminateThisOne);
  assert (stg->succ(eliminateThisOne).size() == 0); // error state should have no successors.
  assert (preds.size() == 1); // error states are immediately removed after being discovered.
  const EState* currentState = *(preds.begin());
  stg->eliminateEState(eliminateThisOne);
  //in the case of new input->output->error behavior, delete the output too
  if (currentState->io.isStdOutIO()) {
    eliminateThisOne = currentState;
    preds = stg->pred(currentState);
    assert (preds.size() == 1);
    currentState = *(preds.begin());
    stg->eliminateEState(eliminateThisOne);
  }
  //also delete the input state right before
  assert (currentState->io.isStdInIO());
  eliminateThisOne = currentState;
  preds = stg->pred(currentState);
  // exclude all edges leading to the input state right before the error state for future tracing attempts
  // (due to the reduction to observable behavior, even a single execution trace can lead to multiple predecessors)
  for (EStatePtrSet::iterator i=preds.begin(); i!= preds.end(); ++i) {
    erroneousTransitions.push_back(pair<const EState*, int>(*i, latestInputVal));
  }
  stg->eliminateEState(eliminateThisOne);
  return erroneousTransitions;
}

CeIoVal CounterexampleAnalyzer::eStateToCeIoVal(const EState* eState) {
  PState* pstate = const_cast<PState*>( eState->pstate() ); 
  int inOutVal;
  pair<int, IoType> result;
  if (eState->io.isStdInIO()) {
    inOutVal = pstate->readFromMemoryLocation(_analyzer->globalVarIdByName("input")).getIntValue();
    result = pair<int, IoType>(inOutVal, CodeThorn::IO_TYPE_INPUT);
  } else if (eState->io.isStdOutIO()) {
    if (eState->io.op == InputOutput::STDOUT_VAR) {
      inOutVal = pstate->readFromMemoryLocation(_analyzer->globalVarIdByName("output")).getIntValue();
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
  _analyzer->resetToEmptyInputSequence();
  for (list<CeIoVal>::iterator j = sourceOfInputs.begin(); j != sourceOfInputs.end(); j++) {
    if (j->second == IO_TYPE_INPUT) {
      _analyzer->addInputSequenceValue(j->first);
      //cout << "DEBUG: added input value " << j->first <<  " to the analyzer" << endl;
    }
  } 
  _analyzer->resetInputSequenceIterator();  
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

void CounterexampleAnalyzer::writeDotGraphToDisk(string filename, Visualizer& visualizer) {
  cout << "generating dot IO graph file for an abstract STG:"<<filename<<endl;
  string dotFile="digraph G {\n";
  dotFile+=visualizer.abstractTransitionGraphToDot();
  dotFile+="}\n";
  write_file(filename, dotFile);
}

void CounterexampleAnalyzer::printStgSizeAndCeCount(TransitionGraph* model, int counterexampleCount, int property) {
  long inStates = model->numberOfObservableStates(true, false, false);
  long outStates = model->numberOfObservableStates(false, true, false);
  long errStates = model->numberOfObservableStates(false, false, true);
  cout << "STATUS: CEGPRA finished analyzing property " << property << ". #counterexamples analyzed: "<< counterexampleCount << endl;
  cout << "STATUS: STG size (model resulting from cegar prefix mode)"
       << ". #transitions: " << model->size()
       << ", #states: " << model->estateSet().size()
       << " (" << inStates << " in / " << outStates << " out / " << errStates << " err)" << endl;
  if (_csvOutput) {
    (*_csvOutput) << model->size() <<","<< model->estateSet().size() <<","<< inStates <<","<< outStates <<","
                  << errStates <<","<< counterexampleCount << ","; 
  }
}

