
#include "sage3basic.h"
#include "IOAnalyzer.h"
#include "CommandLineOptions.h"
#include "Solver8.h"
#include "RersCounterexample.h"
#include "CodeThornException.h"
#include "TimeMeasurement.h"
#include "CodeThornCommandLineOptions.h"
#include "Miscellaneous2.h"
#include "CtxCallStrings.h"
#include "CppStdUtilities.h"

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
  const EState* currentEState=estateWorkListCurrent->front();
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
    if (_ltlOpt.counterExamplesWithOutput) {
      ceString = rersCe->toRersIOString(_ltlRersMapping); // MS 8/6/20: changed to use mapping
    } else {
      ceString = rersCe->toRersIString(_ltlRersMapping); // MS 8/6/20: changed to use mapping
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
    Label startLabel=flow.getStartLabel();
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

void IOAnalyzer::printAnalyzerStatistics(double totalRunTime, string title) {
  long pstateSetSize=getPStateSet()->size();
  long pstateSetBytes=getPStateSet()->memorySize();
  long pstateSetMaxCollisions=getPStateSet()->maxCollisions();
  long pstateSetLoadFactor=getPStateSet()->loadFactor();
  long eStateSetSize=getEStateSet()->size();
  long eStateSetBytes=getEStateSet()->memorySize();
  long eStateSetMaxCollisions=getEStateSet()->maxCollisions();
  double eStateSetLoadFactor=getEStateSet()->loadFactor();
  long transitionGraphSize=getTransitionGraph()->size();
  long transitionGraphBytes=transitionGraphSize*sizeof(Transition);
  long numOfconstraintSets=getConstraintSetMaintainer()->numberOf();
  long constraintSetsBytes=getConstraintSetMaintainer()->memorySize();
  long constraintSetsMaxCollisions=getConstraintSetMaintainer()->maxCollisions();
  double constraintSetsLoadFactor=getConstraintSetMaintainer()->loadFactor();

  long numOfStdinEStates=(getEStateSet()->numberOfIoTypeEStates(InputOutput::STDIN_VAR));
  long numOfStdoutVarEStates=(getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_VAR));
  long numOfStdoutConstEStates=(getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_CONST));
  long numOfStderrEStates=(getEStateSet()->numberOfIoTypeEStates(InputOutput::STDERR_VAR));
  long numOfFailedAssertEStates=(getEStateSet()->numberOfIoTypeEStates(InputOutput::FAILED_ASSERT));
  long numOfConstEStates=(getEStateSet()->numberOfConstEStates(getVariableIdMapping()));
  //long numOfStdoutEStates=numOfStdoutVarEStates+numOfStdoutConstEStates;

  long totalMemory=pstateSetBytes+eStateSetBytes+transitionGraphBytes+constraintSetsBytes;

  stringstream ss;
  ss <<color("white");
  ss << "=============================================================="<<endl;
  ss <<color("normal")<<title<<color("white")<<endl;
  ss << "=============================================================="<<endl;
  ss << "Number of stdin-estates        : "<<color("cyan")<<numOfStdinEStates<<color("white")<<endl;
  ss << "Number of stdoutvar-estates    : "<<color("cyan")<<numOfStdoutVarEStates<<color("white")<<endl;
  ss << "Number of stdoutconst-estates  : "<<color("cyan")<<numOfStdoutConstEStates<<color("white")<<endl;
  ss << "Number of stderr-estates       : "<<color("cyan")<<numOfStderrEStates<<color("white")<<endl;
  ss << "Number of failed-assert-estates: "<<color("cyan")<<numOfFailedAssertEStates<<color("white")<<endl;
  ss << "Number of const estates        : "<<color("cyan")<<numOfConstEStates<<color("white")<<endl;
  ss << "=============================================================="<<endl;
  ss << "Number of pstates              : "<<color("magenta")<<pstateSetSize<<color("white")<<" (memory: "<<color("magenta")<<pstateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<pstateSetLoadFactor<<  "/"<<pstateSetMaxCollisions<<")"<<endl;
  ss << "Number of estates              : "<<color("cyan")<<eStateSetSize<<color("white")<<" (memory: "<<color("cyan")<<eStateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<eStateSetLoadFactor<<  "/"<<eStateSetMaxCollisions<<")"<<endl;
  ss << "Number of transitions          : "<<color("blue")<<transitionGraphSize<<color("white")<<" (memory: "<<color("blue")<<transitionGraphBytes<<color("white")<<" bytes)"<<endl;
  ss << "Number of constraint sets      : "<<color("yellow")<<numOfconstraintSets<<color("white")<<" (memory: "<<color("yellow")<<constraintSetsBytes<<color("white")<<" bytes)"<<" ("<<""<<constraintSetsLoadFactor<<  "/"<<constraintSetsMaxCollisions<<")"<<endl;
  if(getNumberOfThreadsToUse()==1 && getSolver()->getId()==5 && getExplorationMode()==EXPL_LOOP_AWARE) {
    ss << "Number of iterations           : "<<getIterations()<<"-"<<getApproximatedIterations()<<endl;
  }
  ss << "=============================================================="<<endl;
  ss << "Memory total                   : "<<color("green")<<totalMemory<<" bytes"<<color("white")<<endl;
  ss << "TimeMeasurement total          : "<<color("green")<<CodeThorn::readableruntime(totalRunTime)<<color("white")<<endl;
  ss << "=============================================================="<<endl;
  ss <<color("normal");
  printStatusMessage(ss.str());
}

void IOAnalyzer::setup(Analyzer* analyzer, Sawyer::Message::Facility logger,
                       CodeThornOptions& ctOpt, LTLOptions& ltlOpt, ParProOptions& parProOpt) {
  analyzer->setOptionOutputWarnings(ctOpt.printWarnings);
  analyzer->setPrintDetectedViolations(ctOpt.printViolations);

  // this must be set early, as subsequent initialization depends on this flag
  if (ltlOpt.ltlDriven) {
    analyzer->setModeLTLDriven(true);
  }

  if (ltlOpt.cegpra.ltlPropertyNrIsSet() || ltlOpt.cegpra.checkAllProperties) {
    analyzer->setMaxTransitionsForcedTop(1); //initial over-approximated model
    ltlOpt.noInputInputTransitions=true;
    ltlOpt.withLTLCounterExamples=true;
    ltlOpt.counterExamplesWithOutput=true;
    cout << "STATUS: CEGPRA activated (with it LTL counterexamples that include output states)." << endl;
    cout << "STATUS: CEGPRA mode: will remove input state --> input state transitions in the approximated STG." << endl;
  }

  if (ltlOpt.counterExamplesWithOutput) {
    ltlOpt.withLTLCounterExamples=true;
  }

  if(ctOpt.stgTraceFileName.size()>0) {
    analyzer->setStgTraceFileName(ctOpt.stgTraceFileName);
  }

  if(ctOpt.analyzedProgramCLArgs.size()>0) {
    string clOptions=ctOpt.analyzedProgramCLArgs;
    vector<string> clOptionsVector=CodeThorn::Parse::commandLineArgs(clOptions);
    analyzer->setCommandLineOptions(clOptionsVector);
  }

  if(ctOpt.inputValues.size()>0) {
    cout << "STATUS: input-values="<<ctOpt.inputValues<<endl;
    set<int> intSet=Parse::integerSet(ctOpt.inputValues);
    for(set<int>::iterator i=intSet.begin();i!=intSet.end();++i) {
      analyzer->insertInputVarValue(*i);
    }
    cout << "STATUS: input-values stored."<<endl;
  }

  if(ctOpt.inputSequence.size()>0) {
    cout << "STATUS: input-sequence="<<ctOpt.inputSequence<<endl;
    list<int> intList=Parse::integerList(ctOpt.inputSequence);
    for(list<int>::iterator i=intList.begin();i!=intList.end();++i) {
      analyzer->addInputSequenceValue(*i);
    }
  }

  if(ctOpt.explorationMode.size()>0) {
    string explorationMode=ctOpt.explorationMode;
    if(explorationMode=="depth-first") {
      analyzer->setExplorationMode(EXPL_DEPTH_FIRST);
    } else if(explorationMode=="breadth-first") {
      analyzer->setExplorationMode(EXPL_BREADTH_FIRST);
    } else if(explorationMode=="loop-aware") {
      analyzer->setExplorationMode(EXPL_LOOP_AWARE);
    } else if(explorationMode=="loop-aware-sync") {
      analyzer->setExplorationMode(EXPL_LOOP_AWARE_SYNC);
    } else if(explorationMode=="random-mode1") {
      analyzer->setExplorationMode(EXPL_RANDOM_MODE1);
    } else if(explorationMode=="topologic-sort") {
      analyzer->setExplorationMode(EXPL_TOPOLOGIC_SORT);
    } else {
      logger[ERROR] <<"unknown state space exploration mode specified with option --exploration-mode."<<endl;
      exit(1);
    }
  } else {
    // default value
    analyzer->setExplorationMode(EXPL_BREADTH_FIRST);
  }

  if (ctOpt.maxIterations!=-1 || ctOpt.maxIterationsForcedTop!=-1) {
    if(ctOpt.explorationMode!="loop-aware" && ctOpt.explorationMode!="loop-aware-sync") {
      cout << "Error: \"max-iterations[-forced-top]\" modes currently require \"--exploration-mode=loop-aware[-sync]\"." << endl;
      exit(1);
    }
  }

  analyzer->setAbstractionMode(ctOpt.abstractionMode);
  analyzer->setMaxTransitions(ctOpt.maxTransitions);
  analyzer->setMaxIterations(ctOpt.maxIterations);

  if(ctOpt.maxIterationsForcedTop!=-1) {
    analyzer->setMaxIterationsForcedTop(ctOpt.maxIterationsForcedTop);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  }

  // TODO: Analyzer::GTM_IO is only mode used now, all others are deprecated
  if(ctOpt.maxTransitionsForcedTop!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  } else if(ctOpt.maxTransitionsForcedTop1!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop1);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  } else if(ctOpt.maxTransitionsForcedTop2!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop2);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IOCF);
  } else if(ctOpt.maxTransitionsForcedTop3!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop3);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IOCFPTR);
  } else if(ctOpt.maxTransitionsForcedTop4!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop4);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_COMPOUNDASSIGN);
  } else if(ctOpt.maxTransitionsForcedTop5!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop5);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_FLAGS);
  }

  if (ctOpt.maxMemory!=1) {
    analyzer->setMaxBytes(ctOpt.maxMemory);
  }
  if (ctOpt.maxTime!=1) {
    analyzer->setMaxSeconds(ctOpt.maxTime);
  }
  if (ctOpt.maxMemoryForcedTop!=-1) {
    analyzer->setMaxBytesForcedTop(ctOpt.maxMemoryForcedTop);
  }
  if (ctOpt.maxTimeForcedTop!=-1) {
    analyzer->setMaxSecondsForcedTop(ctOpt.maxTimeForcedTop);
  }

  if(ctOpt.displayDiff!=-1) {
    analyzer->setDisplayDiff(ctOpt.displayDiff);
  }
  if(ctOpt.resourceLimitDiff!=-1) {
    analyzer->setResourceLimitDiff(ctOpt.resourceLimitDiff);
  }

  // overwrite solver ID based on other options
  if(analyzer->getModeLTLDriven()) {
    ctOpt.solver=11;
  }
  int solverId=ctOpt.solver;
  // solverId sanity checks
  if(analyzer->getExplorationMode() == EXPL_LOOP_AWARE_SYNC &&
     solverId != 12) {
    logger[ERROR] <<"Exploration mode loop-aware-sync requires solver 12, but solver "<<solverId<<" was selected."<<endl;
    exit(1);
  }
  if(analyzer->getModeLTLDriven() &&
     solverId != 11) {
    logger[ERROR] <<"Ltl-driven mode requires solver 11, but solver "<<solverId<<" was selected."<<endl;
    exit(1);
  }
}

void CodeThorn::IOAnalyzer::configureOptions(CodeThornOptions ctOpt, LTLOptions ltlOpt, ParProOptions parProOpt) {
  setOptions(ctOpt);
  setLtlOptions(ltlOpt);
  AbstractValue::byteMode=ctOpt.byteMode;
  AbstractValue::strictChecking=ctOpt.strictChecking;

  SgNodeHelper::WITH_EXTENDED_NORMALIZED_CALL=ctOpt.extendedNormalizedCppFunctionCalls;
  if (ctOpt.callStringLength >= 2) 
    setFiniteCallStringMaxLength(ctOpt.callStringLength);

  configureOptionSets(ctOpt);

  optionStringLiteralsInState=ctOpt.inStateStringLiterals;
  setSkipUnknownFunctionCalls(ctOpt.ignoreUnknownFunctions);
  setIgnoreFunctionPointers(ctOpt.ignoreFunctionPointers);
  setStdFunctionSemantics(ctOpt.stdFunctions);

  setup(this, logger, ctOpt, ltlOpt, parProOpt);
  //setSolver(createSolver(ctOpt));
    
  switch(int mode=ctOpt.interpreterMode) {
  case 0: setInterpreterMode(IM_DISABLED); break;
  case 1: setInterpreterMode(IM_ENABLED); break;
  default:
    cerr<<"Unknown interpreter mode "<<mode<<" provided on command line (supported: 0..1)."<<endl;
    exit(1);
  }
  string outFileName=ctOpt.interpreterModeOuputFileName;
  if(outFileName!="") {
    setInterpreterModeOutputFileName(outFileName);
    CppStdUtilities::writeFile(outFileName,""); // touch file
  }

  setFunctionResolutionModeInCFAnalysis(ctOpt);

  setNumberOfThreadsToUse(ctOpt.threads);

  // handle RERS mode: reconfigure options
  if(ctOpt.rers.rersMode) {
    SAWYER_MESG(logger[TRACE]) <<"RERS MODE activated [stderr output is treated like a failed assert]"<<endl;
    ctOpt.rers.stdErrLikeFailedAssert=true;
  }
  setTreatStdErrLikeFailedAssert(ctOpt.rers.stdErrLikeFailedAssert);

  if(ctOpt.svcomp.svcompMode) {
    enableSVCompFunctionSemantics();
    string errorFunctionName="__VERIFIER_error";
    setExternalErrorFunctionName(errorFunctionName);
  }

  if(ctOpt.svcomp.detectedErrorFunctionName.size()>0) {
    setExternalErrorFunctionName(ctOpt.svcomp.detectedErrorFunctionName);
  }


  // Build the AST used by ROSE
  if(ctOpt.status) {
    cout<< "STATUS: Parsing and creating AST started."<<endl;
  }
}
