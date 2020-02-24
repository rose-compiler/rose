/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"
#include "Labeler.h"
#include "Analyzer.h"
#include "CodeThornCommandLineOptions.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "AnalysisAbstractionLayer.h"
#include "SvcompWitness.h"
#include "CodeThornException.h"

#include <unordered_set>
#include <boost/bind.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "TimeMeasurement.h"
#include "CollectionOperators.h"
#include "RersSpecialization.h"
#include "RERS_empty_specialization.h"
#include "CodeThornLib.h"

using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility CodeThorn::Analyzer::logger;

CodeThorn::Analyzer::Analyzer():
  startFunRoot(0),
  cfanalyzer(0),
  _globalTopifyMode(GTM_IO),
  _stgReducer(&estateSet, &transitionGraph),
  _counterexampleGenerator(&transitionGraph),
  _displayDiff(10000),
  _resourceLimitDiff(10000),
  _numberOfThreadsToUse(1),
  _solver(nullptr),
  _analyzerMode(AM_ALL_STATES),
  _maxTransitions(-1),
  _maxIterations(-1),
  _maxBytes(-1),
  _maxSeconds(-1),
  _maxTransitionsForcedTop(-1),
  _maxIterationsForcedTop(-1),
  _maxBytesForcedTop(-1),
  _maxSecondsForcedTop(-1),
  _treatStdErrLikeFailedAssert(false),
  _skipSelectedFunctionCalls(false),
  _explorationMode(EXPL_BREADTH_FIRST),
  _topifyModeActive(false),
  _iterations(0),
  _approximated_iterations(0),
  _curr_iteration_cnt(0),
  _next_iteration_cnt(0),
  _svCompFunctionSemantics(false),
  _contextSensitiveAnalysis(false)
{
  initDiagnostics();
  _analysisTimer.start();
  _analysisTimer.stop();
  variableIdMapping=new VariableIdMappingExtended();
  variableIdMapping->setModeVariableIdForEachArrayElement(true);
  for(int i=0;i<100;i++) {
    binaryBindingAssert.push_back(false);
  }
  estateWorkListCurrent = &estateWorkListOne;
  estateWorkListNext = &estateWorkListTwo;
  estateSet.max_load_factor(0.7);
  pstateSet.max_load_factor(0.7);
  constraintSetMaintainer.max_load_factor(0.7);
  resetInputSequenceIterator();
  exprAnalyzer.setAnalyzer(this);
 }

CodeThorn::Analyzer::~Analyzer() {
  if(cfanalyzer) {
    if(FunctionIdMapping* fim=cfanalyzer->getFunctionIdMapping()) {
      delete fim;
    }
    delete cfanalyzer;
  }
  if(variableIdMapping)
    delete variableIdMapping;
}

CodeThorn::Analyzer::SubSolverResultType CodeThorn::Analyzer::subSolver(const CodeThorn::EState* currentEStatePtr) {
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
  if (CodeThorn::args.getBool("status") && _displayDiff) {
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
    if(CodeThorn::args.getBool("status")) {
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
	      SAWYER_MESG(logger[TRACE])<<"STATUS: detected verification error state ... terminating early"<<endl;
	      // set flag for terminating early
	      reachabilityResults.reachable(0);
	      _firstAssertionOccurences.push_back(pair<int, const EState*>(0, newEStatePtr));
	      EStateWorkList emptyWorkList;
	      EStatePtrSet emptyExistingStateSet;
	      return make_pair(emptyWorkList,emptyExistingStateSet);
	    } else if(isFailedAssertEState(&newEState)) {
	      // record failed assert
	      int assertCode;
	      if(CodeThorn::args.getBool("rers-binary")) {
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
		  if(CodeThorn::args.getBool("with-counterexamples") || CodeThorn::args.getBool("with-assert-counterexamples")) {
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

std::string CodeThorn::Analyzer::programPositionInfo(CodeThorn::Label lab) {
  SgNode* node=getLabeler()->getNode(lab);
  return SgNodeHelper::lineColumnNodeToString(node);
}

bool CodeThorn::Analyzer::isApproximatedBy(const EState* es1, const EState* es2) {
  return es1->isApproximatedBy(es2);
}

void CodeThorn::Analyzer::setOptionOutputWarnings(bool flag) {
  exprAnalyzer.setOptionOutputWarnings(flag);
}
bool CodeThorn::Analyzer::getOptionOutputWarnings() {
  return exprAnalyzer.getOptionOutputWarnings();
}

EState CodeThorn::Analyzer::combine(const EState* es1, const EState* es2) {
  ROSE_ASSERT(es1->label()==es2->label());
  ROSE_ASSERT(es1->constraints()==es2->constraints()); // pointer equality
  if(es1->callString!=es2->callString) {
    if(getOptionOutputWarnings()) {
      SAWYER_MESG(logger[WARN])<<"combining estates with different callstrings at label:"<<es1->label().toString()<<endl;
      SAWYER_MESG(logger[WARN])<<"cs1: "<<es1->callString.toString()<<endl;
      SAWYER_MESG(logger[WARN])<<"cs2: "<<es2->callString.toString()<<endl;
    }
  }
  PState ps1=*es1->pstate();
  PState ps2=*es2->pstate();

  InputOutput io;
  if(es1->io.isBot()) {
    io=es2->io;
  } else if(es2->io.isBot()) {
    io=es1->io;
  } else {
    ROSE_ASSERT(es1->io==es2->io);
    io=es1->io;
  }

  return createEState(es1->label(),es1->callString,PState::combine(ps1,ps2),*es1->constraints(),io);
}

size_t CodeThorn::Analyzer::getSummaryStateMapSize() {
  return _summaryCSStateMap.size();
}

const CodeThorn::EState* CodeThorn::Analyzer::getSummaryState(CodeThorn::Label lab, CodeThorn::CallString cs) {
  // cs not used yet
  //return _summaryStateMap[lab.getId()];
  pair<int,CallString> p(lab.getId(),cs);
  auto iter=_summaryCSStateMap.find(p);
  if(iter==_summaryCSStateMap.end()) {
    return getBottomSummaryState(lab,cs);
  } else {
    return (*iter).second;
  }
}

void CodeThorn::Analyzer::setSummaryState(CodeThorn::Label lab, CodeThorn::CallString cs, CodeThorn::EState const* estate) {
  ROSE_ASSERT(lab==estate->label());
  ROSE_ASSERT(cs==estate->callString);
  ROSE_ASSERT(estate);
  pair<int,CallString> p(lab.getId(),cs);
  _summaryCSStateMap[p]=estate;
}


const EState* CodeThorn::Analyzer::getBottomSummaryState(Label lab, CallString cs) {
  InputOutput io;
  io.recordBot();
  ROSE_ASSERT(_initialPStateStored);
  ROSE_ASSERT(_emptycsetstored);
  EState estate(lab,cs,_initialPStateStored,_emptycsetstored,io);
  const EState* bottomElement=processNewOrExisting(estate);
  return bottomElement;
}

void CodeThorn::Analyzer::initializeSummaryStates(const CodeThorn::PState* initialPStateStored, 
                                                  const CodeThorn::ConstraintSet* emptycsetstored) {
  _initialPStateStored=initialPStateStored;
  _emptycsetstored=emptycsetstored;
#if 0
  for(auto label:*getLabeler()) {
    // create bottom elements for each label
    InputOutput io;
    io.recordBot();
    CallString cs; // empty callstring
    EState estate(label,cs,initialPStateStored,emptycsetstored,io); // implicitly empty cs
    const EState* bottomElement=processNewOrExisting(getBottomSummaryState());
    setSummaryState(label,estate.callString,bottomElement);
  }
#endif
}

bool CodeThorn::Analyzer::getPrintDetectedViolations() {
  return exprAnalyzer.getPrintDetectedViolations();
}

void CodeThorn::Analyzer::setPrintDetectedViolations(bool flag) {
  exprAnalyzer.setPrintDetectedViolations(flag);
}

void CodeThorn::Analyzer::setIgnoreFunctionPointers(bool skip) {
  exprAnalyzer.setIgnoreFunctionPointers(skip);
}

bool CodeThorn::Analyzer::getIgnoreFunctionPointers() {
  return exprAnalyzer.getIgnoreFunctionPointers();
}

void CodeThorn::Analyzer::setInterpretationMode(CodeThorn::InterpretationMode mode) {
  exprAnalyzer.setInterpretationMode(mode);
}

CodeThorn::InterpretationMode CodeThorn::Analyzer::getInterpretationMode() {
  return exprAnalyzer.getInterpretationMode();
}

void CodeThorn::Analyzer::setInterpretationModeOutputFileName(string fileName) {
  exprAnalyzer.setInterpretationModeFileName(fileName);
}

string CodeThorn::Analyzer::getInterpretationModeOutputFileName() {
  return exprAnalyzer.getInterpretationModeFileName();
}

void CodeThorn::Analyzer::setOptionContextSensitiveAnalysis(bool flag) {
  _contextSensitiveAnalysis=flag;
}

bool CodeThorn::Analyzer::getOptionContextSensitiveAnalysis() {
  return _contextSensitiveAnalysis;
}

void CodeThorn::Analyzer::printStatusMessage(string s, bool newLineFlag) {
#pragma omp critical (STATUS_MESSAGES)
  {
    if(CodeThorn::args.getBool("status")) {
      cout<<s;
      if(newLineFlag) {
        cout<<endl; // status output
      }
    }
  }
}

void CodeThorn::Analyzer::printStatusMessage(string s) {
  printStatusMessage(s,false);
}

void CodeThorn::Analyzer::printStatusMessageLine(string s) {
  printStatusMessage(s,true);
}

void CodeThorn::Analyzer::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::Analyzer", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

void CodeThorn::Analyzer::enableSVCompFunctionSemantics() {
  _svCompFunctionSemantics=true;
  exprAnalyzer.setSVCompFunctionSemantics(true);
  _externalErrorFunctionName="__VERIFIER_error";
  _externalNonDetIntFunctionName="__VERIFIER_nondet_int";
  _externalNonDetLongFunctionName="__VERIFIER_nondet_long";
  _externalExitFunctionName="exit";
}

void CodeThorn::Analyzer::disableSVCompFunctionSemantics() {
  _svCompFunctionSemantics=false;
  exprAnalyzer.setSVCompFunctionSemantics(false);
  _externalErrorFunctionName="";
  _externalNonDetIntFunctionName="";
  _externalNonDetLongFunctionName="";
  _externalExitFunctionName="";
  ROSE_ASSERT(getLabeler());
  getLabeler()->setExternalNonDetIntFunctionName(_externalNonDetIntFunctionName);
  getLabeler()->setExternalNonDetLongFunctionName(_externalNonDetLongFunctionName);
}

bool CodeThorn::Analyzer::svCompFunctionSemantics() { return _svCompFunctionSemantics; }
bool CodeThorn::Analyzer::getStdFunctionSemantics() { return exprAnalyzer.getStdFunctionSemantics(); }
void CodeThorn::Analyzer::setStdFunctionSemantics(bool flag) { exprAnalyzer.setStdFunctionSemantics(flag); }

// TODO: move to flow analyzer (reports label,init,final sets)
string CodeThorn::Analyzer::astNodeInfoAttributeAndNodeToString(SgNode* node) {
  string textual;
  if(node->attributeExists("info"))
    textual=node->getAttribute("info")->toString()+":";
  return textual+SgNodeHelper::nodeToString(node);
}

bool CodeThorn::Analyzer::isFunctionCallWithAssignment(Label lab,VariableId* varIdPtr){
  //return _labeler->getLabeler()->isFunctionCallWithAssignment(lab,varIdPtr);
  SgNode* node=getLabeler()->getNode(lab);
  if(getLabeler()->isFunctionCallLabel(lab)) {
    std::pair<SgVarRefExp*,SgFunctionCallExp*> p=SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp2(node);
    if(p.first) {
      if(varIdPtr) {
        *varIdPtr=variableIdMapping->variableId(p.first);
      }
      return true;
    }
  }
  return false;
}

void CodeThorn::Analyzer::writeWitnessToFile(string filename) {
  _counterexampleGenerator.setType(CounterexampleGenerator::TRACE_TYPE_SVCOMP_WITNESS);
  ROSE_ASSERT(_firstAssertionOccurences.size() == 1); //SV-COMP: Expecting exactly one reachability property
  list<pair<int, const EState*> >::iterator iter = _firstAssertionOccurences.begin(); 
  ExecutionTrace* trace = 
    _counterexampleGenerator.traceLeadingTo((*iter).second);
  if(SvcompWitness* witness = dynamic_cast<SvcompWitness*>(trace)) {
    witness->writeErrorAutomatonToFile(filename);
    delete witness;
    witness = nullptr;
  } else {
    throw CodeThorn::Exception("Downcast to SvcompWitness unsuccessful.");
  }
}


size_t CodeThorn::Analyzer::getNumberOfErrorLabels() {
  return _assertNodes.size();
}

string CodeThorn::Analyzer::labelNameOfAssertLabel(Label lab) {
  string labelName;
  for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=_assertNodes.begin();i!=_assertNodes.end();++i)
    if(lab==getLabeler()->getLabel((*i).second))
      labelName=SgNodeHelper::getLabelName((*i).first);
  //assert(labelName.size()>0);
  return labelName;
}

bool CodeThorn::Analyzer::isCppLabeledAssertLabel(Label lab) {
  return labelNameOfAssertLabel(lab).size()>0;
}
    

void CodeThorn::Analyzer::setGlobalTopifyMode(GlobalTopifyMode mode) {
  _globalTopifyMode=mode;
}

void CodeThorn::Analyzer::setExternalErrorFunctionName(std::string externalErrorFunctionName) {
  _externalErrorFunctionName=externalErrorFunctionName;
}

bool CodeThorn::Analyzer::isPrecise() {
  if (isActiveGlobalTopify()) {
    return false;
  }
  if (CodeThorn::args.getBool("explicit-arrays")==false && !CodeThorn::args.getBool("rers-binary")) {
    return false;
  }
  return true;
}

// only relevant for maximum values (independent of topify mode)
bool CodeThorn::Analyzer::isIncompleteSTGReady() {
  if(_maxTransitions==-1 && _maxIterations==-1 && _maxBytes==-1 && _maxSeconds==-1)
    return false;
  if ((_maxTransitions!=-1) && ((long int) transitionGraph.size()>=_maxTransitions))
    return true;
  if ((_maxIterations!=-1) && ((long int) getIterations() > _maxIterations))
    return true;
  if ((_maxBytes!=-1) && ((long int) getPhysicalMemorySize() > _maxBytes))
    return true;
  if ((_maxSeconds!=-1) && ((long int) analysisRunTimeInSeconds() > _maxSeconds))
    return true;
  // at least one maximum mode is active, but the corresponding limit has not yet been reached
  return false;
}

CodeThorn::ExprAnalyzer* CodeThorn::Analyzer::getExprAnalyzer() {
  return &exprAnalyzer;
}

void CodeThorn::Analyzer::setSolver(Solver* solver) {
  _solver=solver;
  _solver->setAnalyzer(this);
}

Solver* CodeThorn::Analyzer::getSolver() {
  return _solver;
}

void CodeThorn::Analyzer::runSolver() {
  _solver->run();
}

set<string> CodeThorn::Analyzer::variableIdsToVariableNames(CodeThorn::VariableIdSet s) {
  set<string> res;
  for(CodeThorn::VariableIdSet::iterator i=s.begin();i!=s.end();++i) {
    res.insert((AbstractValue(*i)).toString(getVariableIdMapping()));
  }
  return res;
}

CodeThorn::Analyzer::VariableDeclarationList CodeThorn::Analyzer::computeUnusedGlobalVariableDeclarationList(SgProject* root) {
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(root);
  CodeThorn::Analyzer::VariableDeclarationList usedGlobalVars=computeUsedGlobalVariableDeclarationList(root);
  for(CodeThorn::Analyzer::VariableDeclarationList::iterator i=usedGlobalVars.begin();i!=usedGlobalVars.end();++i) {
    globalVars.remove(*i);
  }
  return globalVars;
}

#define DO_NOT_FILTER_VARS
CodeThorn::Analyzer::VariableDeclarationList CodeThorn::Analyzer::computeUsedGlobalVariableDeclarationList(SgProject* root) {
  if(SgProject* project=isSgProject(root)) {
    CodeThorn::Analyzer::VariableDeclarationList usedGlobalVariableDeclarationList;
    list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
#ifdef DO_NOT_FILTER_VARS
    VariableIdSet setOfUsedVars=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,getVariableIdMapping());
#endif
    int filteredVars=0;
    for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
      VariableId globalVarId=variableIdMapping->variableId(*i);
      // do not filter
      usedGlobalVariableDeclarationList.push_back(*i);
#ifdef DO_NOT_FILTER_VARS
      if(true || setOfUsedVars.find(globalVarId)!=setOfUsedVars.end()) {
        usedGlobalVariableDeclarationList.push_back(*i);
      } else {
        filteredVars++;
      }
#else
      if(setOfUsedVars.find(globalVarId)!=setOfUsedVars.end()) {
        usedGlobalVariableDeclarationList.push_back(*i);
      } else {
        filteredVars++;
      }
#endif
    }
    return usedGlobalVariableDeclarationList;
  } else {
    SAWYER_MESG(logger[ERROR]) << "no global scope.";
    exit(1);
  }
}

void CodeThorn::Analyzer::setStgTraceFileName(string filename) {
  _stg_trace_filename=filename;
  ofstream fout;
  fout.open(_stg_trace_filename.c_str());    // create new file/overwrite existing file
  fout<<"START"<<endl;
  fout.close();    // close. Will be used with append.
}

void CodeThorn::Analyzer::recordTransition(const EState* sourceState, Edge e, const EState* targetState) {
  transitionGraph.add(Transition(sourceState,e,targetState));
}

void CodeThorn::Analyzer::printStatusMessage(bool forceDisplay) {
  // forceDisplay currently only turns on or off

  // report we are alive
  stringstream ss;
  if(forceDisplay) {
    long pstateSetSize;
    long estateSetSize;
    long transitionGraphSize;
    long constraintSetMaintainerSize;
    long estateWorkListCurrentSize;
#pragma omp critical(HASHSET)
    {
      pstateSetSize = pstateSet.size();
      estateSetSize = estateSet.size();
      transitionGraphSize = getTransitionGraph()->size();
      constraintSetMaintainerSize = constraintSetMaintainer.size();
    }
#pragma omp critical(ESTATEWL)
    {
      estateWorkListCurrentSize = estateWorkListCurrent->size();
    }
    ss <<color("white")<<"Number of pstates/estates/trans/csets/wl/iter: ";
    ss <<color("magenta")<<pstateSetSize
       <<color("white")<<"/"
       <<color("cyan")<<estateSetSize
       <<color("white")<<"/"
       <<color("blue")<<transitionGraphSize
       <<color("white")<<"/"
       <<color("yellow")<<constraintSetMaintainerSize
       <<color("white")<<"/"
       <<estateWorkListCurrentSize
       <<"/"<<getIterations()<<"-"<<getApproximatedIterations()
      ;
    ss<<" "<<color("normal")<<analyzerStateToString();
    ss<<endl;
    printStatusMessage(ss.str());
  }
}

string CodeThorn::Analyzer::analyzerStateToString() {
  stringstream ss;
  ss<<"isPrec:"<<isPrecise();
  ss<<" ";
  ss<<"TopMode:"<<_globalTopifyMode;
  ss<<" ";
  ss<<"RBin:"<<CodeThorn::args.getBool("rers-binary");
  ss<<" ";
  ss<<"incSTGReady:"<<isIncompleteSTGReady();
  return ss.str();
}

bool CodeThorn::Analyzer::isInWorkList(const EState* estate) {
  for(EStateWorkList::iterator i=estateWorkListCurrent->begin();i!=estateWorkListCurrent->end();++i) {
    if(*i==estate) return true;
  }
  return false;
}

void CodeThorn::Analyzer::incIterations() {
  if(isPrecise()) {
#pragma omp atomic
    _iterations+=1;
  } else {
#pragma omp atomic
    _approximated_iterations+=1;
  }
}

bool CodeThorn::Analyzer::isLoopCondLabel(Label lab) {
  SgNode* node=getLabeler()->getNode(lab);
  return SgNodeHelper::isLoopCond(node);
}

void CodeThorn::Analyzer::addToWorkList(const EState* estate) {
  ROSE_ASSERT(estate);
  ROSE_ASSERT(estateWorkListCurrent);
#pragma omp critical(ESTATEWL)
  {
    if(!estate) {
      SAWYER_MESG(logger[ERROR])<<"INTERNAL: null pointer added to work list."<<endl;
      exit(1);
    }
    switch(_explorationMode) {
    case EXPL_DEPTH_FIRST: estateWorkListCurrent->push_front(estate);break;
    case EXPL_BREADTH_FIRST: estateWorkListCurrent->push_back(estate);break;
    case EXPL_RANDOM_MODE1: {
      int perc=4; // 25%-chance depth-first
      int num=rand();
      int sel=num%perc;
      if(sel==0) {
        estateWorkListCurrent->push_front(estate);
      } else {
        estateWorkListCurrent->push_back(estate);
      }
      break;
    }
    case EXPL_LOOP_AWARE: {
      if(isLoopCondLabel(estate->label())) {
        estateWorkListCurrent->push_back(estate);
        // logger[DEBUG] << "push to WorkList: "<<_curr_iteration_cnt<<","<<_next_iteration_cnt<<":"<<_iterations<<endl;
        _next_iteration_cnt++;
      } else {
        estateWorkListCurrent->push_front(estate);
      }
      break;
    }
    case EXPL_LOOP_AWARE_SYNC: {
      if(isLoopCondLabel(estate->label())) {
        estateWorkListNext->push_back(estate);
      } else {
        estateWorkListCurrent->push_back(estate);
      }
      break;
    }
    default:
      SAWYER_MESG(logger[ERROR])<<"unknown exploration mode."<<endl;
      exit(1);
    }
  }
}

bool CodeThorn::Analyzer::isActiveGlobalTopify() {
  if(_maxTransitionsForcedTop==-1 && _maxIterationsForcedTop==-1 && _maxBytesForcedTop==-1 && _maxSecondsForcedTop==-1)
    return false;
  if(_topifyModeActive)
    return true;
  // TODO: add a critical section that guards "transitionGraph.size()"
  if( (_maxTransitionsForcedTop!=-1 && (long int)transitionGraph.size()>=_maxTransitionsForcedTop)
      || (_maxIterationsForcedTop!=-1 && getIterations() > _maxIterationsForcedTop)
      || (_maxBytesForcedTop!=-1 && getPhysicalMemorySize() > _maxBytesForcedTop)
      || (_maxSecondsForcedTop!=-1 && analysisRunTimeInSeconds() > _maxSecondsForcedTop) ) {
#pragma omp critical(ACTIVATE_TOPIFY_MODE)
    {
      if (!_topifyModeActive) {
        _topifyModeActive=true;
        eventGlobalTopifyTurnedOn();
        CodeThorn::args.setOption("rers-binary",false);
      }
    }
    return true;
  }
  return false;
}

void CodeThorn::Analyzer::eventGlobalTopifyTurnedOn() {
  cout << "STATUS: mode global-topify activated:"<<endl
       << "Transitions  : "<<(long int)transitionGraph.size()<<","<<_maxTransitionsForcedTop<<endl
       << "Iterations   : "<<getIterations()<<":"<< _maxIterationsForcedTop<<endl
       << "Memory(bytes): "<<getPhysicalMemorySize()<<":"<< _maxBytesForcedTop<<endl
       << "Runtime(s)   : "<<analysisRunTimeInSeconds() <<":"<< _maxSecondsForcedTop<<endl;

AbstractValueSet vset=variableValueMonitor.getVariables();
  int n=0;
  int nt=0;
  for(AbstractValueSet::iterator i=vset.begin();i!=vset.end();++i) {
    string name=(*i).toLhsString(getVariableIdMapping());
    bool isCompoundIncVar=(_compoundIncVarsSet.find(*i)!=_compoundIncVarsSet.end());
    bool isSmallActivityVar=(_smallActivityVarsSet.find(*i)!=_smallActivityVarsSet.end());
    bool isAssertCondVar=(_assertCondVarsSet.find(*i)!=_assertCondVarsSet.end());
    bool topifyVar=false;
    switch(_globalTopifyMode) {
    case GTM_IO:
      if(name!="input" && name!="output") {
        topifyVar=true;
      }
      break;
    case GTM_IOCF:
      if(name!="input" && name!="output" && name!="cf") {
        topifyVar=true;
      }
      break;
    case GTM_IOCFPTR:
      if(name!="input" && name!="output" && name!="cf" && !(*i).isPtr()) {
        topifyVar=true;
      }
      break;
    case GTM_COMPOUNDASSIGN:
      if(isCompoundIncVar) {
        topifyVar=true;
      }
      break;
    case GTM_FLAGS: {
      if(name=="input" || name=="output" || name=="cf") {
        topifyVar=false;
        break;
      }
      if(isCompoundIncVar) {
        topifyVar=true;
        break;
      }
      if(!isAssertCondVar) {
        topifyVar=true;
        break;
      }
      if(!isSmallActivityVar) {
        topifyVar=true;
        break;
      }
      topifyVar=false;
      break;
    }
    default:
      logger[ERROR]<<"unsupported topify mode selected. Bailing out."<<endl;
      exit(1);
    }

    if(topifyVar) {
      variableValueMonitor.setVariableMode(VariableValueMonitor::VARMODE_FORCED_TOP,*i);
      n++;
    }
    nt++;
  }

  if(CodeThorn::args.getBool("status")) {
    cout << "switched to static analysis (approximating "<<n<<" of "<<nt<<" variables with top-conversion)."<<endl;
  }
  //switch to the counter for approximated loop iterations if currently in a mode that counts iterations
  if (getExplorationMode()==EXPL_LOOP_AWARE || getExplorationMode()==EXPL_LOOP_AWARE_SYNC) {
#pragma omp atomic
    _iterations--;
#pragma omp atomic
    _approximated_iterations++;
  }
}

void CodeThorn::Analyzer::topifyVariable(PState& pstate, ConstraintSet& cset, AbstractValue varId) {
  pstate.writeTopToMemoryLocation(varId);
}

// does not use a context
EState CodeThorn::Analyzer::createEStateInternal(Label label, PState pstate, ConstraintSet cset) {
  // here is the best location to adapt the analysis results to certain global restrictions
  if(isActiveGlobalTopify()) {
#if 1
    AbstractValueSet varSet=pstate.getVariableIds();
    for(AbstractValueSet::iterator i=varSet.begin();i!=varSet.end();++i) {
      if(variableValueMonitor.isHotVariable(this,*i)) {
        //ROSE_ASSERT(false); // this branch is live
        topifyVariable(pstate, cset, *i);
      }
    }
#else
    //pstate.topifyState();
#endif
    // set cset in general to empty cset, otherwise cset can grow again arbitrarily
    ConstraintSet cset0; 
    cset=cset0; 
  }
  const PState* newPStatePtr=processNewOrExisting(pstate);
  const ConstraintSet* newConstraintSetPtr=processNewOrExisting(cset);
  EState estate=EState(label,newPStatePtr,newConstraintSetPtr);
  estate.io.recordNone();
  return estate;
}

EState CodeThorn::Analyzer::createEState(Label label, CallString cs, PState pstate, ConstraintSet cset) {
  EState estate=createEStateInternal(label,pstate,cset);
  estate.callString=cs;
  estate.io.recordNone(); // create NONE not bot by default
  return estate;
}

EState CodeThorn::Analyzer::createEState(Label label, CallString cs, PState pstate, ConstraintSet cset, InputOutput io) {
  EState estate=createEStateInternal(label,pstate,cset);
  estate.callString=cs;
  estate.io=io;
  return estate;
}

/*
EState CodeThorn::Analyzer::createEState(Label label, PState pstate, ConstraintSet cset, InputOutput io) {
  EState estate=createEState(label,pstate,cset);
  estate.io=io;
  return estate;
}
*/

bool CodeThorn::Analyzer::isStartLabel(Label label) {
  return getTransitionGraph()->getStartLabel()==label;
}

// Avoid calling critical sections from critical sections:
// worklist functions do not use each other.
bool CodeThorn::Analyzer::isEmptyWorkList() {
  bool res;
#pragma omp critical(ESTATEWL)
  {
    res=estateWorkListCurrent->empty();
  }
  return res;
}
const EState* CodeThorn::Analyzer::topWorkList() {
  const EState* estate=0;
#pragma omp critical(ESTATEWL)
  {
    if(!estateWorkListCurrent->empty())
      estate=*estateWorkListCurrent->begin();
  }
  return estate;
}
const EState* CodeThorn::Analyzer::popWorkList() {
  const EState* estate=0;
#pragma omp critical(ESTATEWL)
  {
    if(!estateWorkListCurrent->empty())
      estate=*estateWorkListCurrent->begin();
    if(estate) {
      estateWorkListCurrent->pop_front();
      if(getExplorationMode()==EXPL_LOOP_AWARE && isLoopCondLabel(estate->label())) {
        if(_curr_iteration_cnt==0) {
          _curr_iteration_cnt= (_next_iteration_cnt - 1);
          _next_iteration_cnt=0;
          incIterations();
        } else {
          _curr_iteration_cnt--;
        }
      }
    }
  }
  return estate;
}

// not used anywhere
const EState* CodeThorn::Analyzer::takeFromWorkList() {
  const EState* co=0;
#pragma omp critical(ESTATEWL)
  {
    if(estateWorkListCurrent->size()>0) {
      co=*estateWorkListCurrent->begin();
      estateWorkListCurrent->pop_front();
    }
  }
  return co;
}

// this function has to be protected by a critical section
// currently called once inside a critical section
void CodeThorn::Analyzer::swapWorkLists() {
  EStateWorkList* tmp = estateWorkListCurrent;
  estateWorkListCurrent = estateWorkListNext;
  estateWorkListNext = tmp;
  incIterations();
}


const EState* CodeThorn::Analyzer::addToWorkListIfNew(EState estate) {
  EStateSet::ProcessingResult res=process(estate);
  if(res.first==true) {
    const EState* newEStatePtr=res.second;
    ROSE_ASSERT(newEStatePtr);
    addToWorkList(newEStatePtr);
    return newEStatePtr;
  } else {
    // logger[DEBUG] << "EState already exists. Not added:"<<estate.toString()<<endl;
    const EState* existingEStatePtr=res.second;
    ROSE_ASSERT(existingEStatePtr);
    return existingEStatePtr;
  }
}

// set the size of an element determined by this type
void CodeThorn::Analyzer::setElementSize(VariableId variableId, SgType* elementType) {
  unsigned int typeSize=getVariableIdMapping()->getTypeSize(variableId);
  //cout<<"DEBUG: setElementSize: variableId name: "<<variableIdMapping.variableName(variableId)<<" typeSize: "<<typeSize<<" of "<<elementType->unparseToString()<<endl;
  variableIdMapping->setElementSize(variableId,typeSize);
}

// for arrays: number of elements (nested arrays not implemented yet)
// for variable: 1
// for structs: not implemented yet
int CodeThorn::Analyzer::computeNumberOfElements(SgVariableDeclaration* decl) {
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1);
  if(SgInitializedName* initName=isSgInitializedName(initName0)) {
    SgInitializer* initializer=initName->get_initializer();
    if(SgAggregateInitializer* aggregateInitializer=isSgAggregateInitializer(initializer)) {
      SgArrayType* arrayType=isSgArrayType(aggregateInitializer->get_type());
      ROSE_ASSERT(arrayType);
      //SgType* arrayElementType=arrayType->get_base_type();
      SgExprListExp* initListObjPtr=aggregateInitializer->get_initializers();
      SgExpressionPtrList& initList=initListObjPtr->get_expressions();
      // TODO: nested initializers, currently only outermost elements: {{1,2,3},{1,2,3}} evaluates to 2.
      SAWYER_MESG(logger[TRACE])<<"computeNumberOfElements returns "<<initList.size()<<": case : SgAggregateInitializer"<<aggregateInitializer->unparseToString()<<endl;
      return initList.size(); 
    } else if(isSgAssignInitializer(initializer)) {
      SAWYER_MESG(logger[TRACE])<<"computeNumberOfElements returns 0: case SgAssignInitializer: "<<initializer->unparseToString()<<endl;
      return 0; // MS 3/5/2019: changed from 1 to 0 
    }
  }
  return 0;
}

// sets all elements in PState according to aggregate
// initializer. Also models default values of Integers (floats not
// supported yet). EState is only used for lookup (modified is only
// the PState object).
PState CodeThorn::Analyzer::analyzeSgAggregateInitializer(VariableId initDeclVarId, SgAggregateInitializer* aggregateInitializer,PState pstate, /* for evaluation only  */ EState currentEState) {
  //cout<<"DEBUG: AST:"<<AstTerm::astTermWithNullValuesToString(aggregateInitializer)<<endl;
  // logger[DEBUG] <<"array-initializer found:"<<aggregateInitializer->unparseToString()<<endl;
  Label label=currentEState.label();
  PState newPState=pstate;
  int elemIndex=0;
  SgExprListExp* initListObjPtr=aggregateInitializer->get_initializers();
  SgExpressionPtrList& initList=initListObjPtr->get_expressions();

  for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
    AbstractValue arrayElemId=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex));
    SgExpression* exp=*i;
    SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
    if(assignInit==nullptr) {
      SAWYER_MESG(logger[WARN])<<"expected assign initializer but found "<<exp->unparseToString();
      SAWYER_MESG(logger[WARN])<<"AST: "<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
      AbstractValue newVal=AbstractValue::createTop();
      getExprAnalyzer()->writeToMemoryLocation(label,&newPState,arrayElemId,newVal);
    } else {
      // initialize element of array initializer in state
      SgExpression* assignInitExpr=assignInit->get_operand();
      // currentEState from above, newPState must be the same as in currentEState.
      AbstractValue newVal=singleValevaluateExpression(assignInitExpr,currentEState);
      getExprAnalyzer()->writeToMemoryLocation(label,&newPState,arrayElemId,newVal);
    }
    elemIndex++;
  }
  // initialize remaining elements (if there are any) with default value
  int aggregateSize=(int)getVariableIdMapping()->getNumberOfElements(initDeclVarId);
  // if array size is not 0 then it was determined from the type and remaining elements are initialized
  // otherwise the size is determined from the aggregate initializer itself (done above)
  if(aggregateSize!=0) {
    for(int i=elemIndex;i<aggregateSize;i++) {
      AbstractValue arrayElemId=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(i));
      AbstractValue defaultVal=AbstractValue(0); // TODO: float default values
      SAWYER_MESG(logger[TRACE])<<"Init aggregate default value: "<<arrayElemId.toString()<<" : "<<defaultVal.toString()<<endl;
      getExprAnalyzer()->writeToMemoryLocation(label,&newPState,arrayElemId,defaultVal);
    }
  } else {
    getVariableIdMapping()->setNumberOfElements(initDeclVarId,initList.size());
  }
  return newPState;
}

EState CodeThorn::Analyzer::analyzeVariableDeclaration(SgVariableDeclaration* decl,EState currentEState, Label targetLabel) {

  /*
    1) declaration of variable or array
    - AggregateInitializer (e.g. T a[]={1,2,3};)
    - AggregateInitializer (e.g. T a[5]={1,2,3};)
    - AssignInitializer (e.g. T x=1+2;)
    2) if array, determine size of array (from VariableIdMapping)
    3) if no size is provided, determine it from the initializer list (and add this information to the variableIdMapping - or update the variableIdMapping).
   */

  //cout<< "DEBUG: DECLARATION:"<<AstTerm::astTermWithNullValuesToString(decl)<<endl;
  CallString cs=currentEState.callString;
  Label label=currentEState.label();

  const SgInitializedNamePtrList& initNameList=decl->get_variables();
  SgNode* initName0;
  if(initNameList.size()>1) {
    cerr<<"Error: variable declaration contains more than one variable. Normalization required."<<endl;
    cerr<<"Error: "<<decl->unparseToString()<<endl;
    exit(1);
  } else {
    //SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
    ROSE_ASSERT(initNameList.size()==1);
    initName0=*initNameList.begin();
  }
  if(initName0!=nullptr) {
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      VariableId initDeclVarId=getVariableIdMapping()->variableId(initName);
      // not possible to support yet. getIntValue must succeed on declarations.
      if(false && variableValueMonitor.isHotVariable(this,initDeclVarId)) {
        // DEAD CODE
        PState newPState=*currentEState.pstate();
        newPState.writeTopToMemoryLocation(initDeclVarId);
        ConstraintSet cset=*currentEState.constraints();
        InputOutput io;
        io.recordNone();
        return createEState(targetLabel,cs,newPState,cset);
      }
      
      if(variableIdMapping->hasArrayType(initDeclVarId) && CodeThorn::args.getBool("explicit-arrays")==false) {
        // in case of a constant array the array (and its members) are not added to the state.
        // they are considered to be determined from the initializer without representing them
        // in the state
        // logger[DEBUG] <<"not adding array to PState."<<endl;
        PState newPState=*currentEState.pstate();
        ConstraintSet cset=*currentEState.constraints();
        return createEState(targetLabel,cs,newPState,cset);
      }
      //SgName initDeclVarName=initDeclVar->get_name();
      //string initDeclVarNameString=initDeclVarName.getString();
      //cout << "INIT-DECLARATION: var:"<<initDeclVarNameString<<endl;
      //cout << "DEBUG: DECLARATION: var:"<<SgNodeHelper::nodeToString(decl)<<endl;
      ConstraintSet cset=*currentEState.constraints();
      SgInitializer* initializer=initName->get_initializer();
      if(initializer) {
        // special case that initializer is an assignment (reusing transferAssignOp)
        if(SgAssignInitializer* assignInit=isSgAssignInitializer(initializer)) {
          SgExpression* assignInitOperand=assignInit->get_operand_i();
          ROSE_ASSERT(assignInitOperand);
          if(SgAssignOp* assignOp=isSgAssignOp(assignInitOperand)) {
            SAWYER_MESG(logger[TRACE])<<"assignment in initializer: "<<decl->unparseToString()<<endl;
            Edge dummyEdge(targetLabel,EDGE_FORWARD,targetLabel); // only target label is used in transferAssignOp
            std::list<EState> estateList=transferAssignOp(assignOp, dummyEdge, &currentEState);
            ROSE_ASSERT(estateList.size()==1);
            return *estateList.begin();
          }
        }
        //cout<<"DEBUG: initialize with "<<initializer->unparseToString()<<endl;
        //cout<<"DEBUG: lhs type: "<<getVariableIdMapping()->getType(initDeclVarId)->unparseToString()<<endl;
        if(getVariableIdMapping()->hasClassType(initDeclVarId)) {
          // TODO: initialization of structs not supported yet
          SAWYER_MESG(logger[WARN])<<"initialization of structs not supported yet (not added to state) "<<decl->unparseToString()<<endl;
          //AbstractValue arrayAddress=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex))
          //getExprAnalyzer()->writeToMemoryLocation(label,&newPState,arrayAddress,CodeThorn::Top());
          PState newPState=*currentEState.pstate();
          return createEState(targetLabel,cs,newPState,cset);
        }
        if(getVariableIdMapping()->hasReferenceType(initDeclVarId)) {
          // TODO: initialization of references not supported yet
          SAWYER_MESG(logger[WARN])<<"initialization of references not supported yet (not added to state) "<<decl->unparseToString()<<endl;
          PState newPState=*currentEState.pstate();
          return createEState(targetLabel,cs,newPState,cset);
        }
        // has aggregate initializer
        if(SgAggregateInitializer* aggregateInitializer=isSgAggregateInitializer(initializer)) {
          if(SgArrayType* arrayType=isSgArrayType(aggregateInitializer->get_type())) {
            SgType* arrayElementType=arrayType->get_base_type();
            setElementSize(initDeclVarId,arrayElementType);
            // only set size from aggregate initializer if not known from type
            if(variableIdMapping->getNumberOfElements(initDeclVarId)==0) {
              // TODO: requires a sizeof computation of an aggregate initializer (e.g. {{1,2},{1,2}} == 4)
               variableIdMapping->setNumberOfElements(initDeclVarId, computeNumberOfElements(decl));
            }
            PState newPState=*currentEState.pstate();
            newPState=analyzeSgAggregateInitializer(initDeclVarId, aggregateInitializer,newPState, currentEState);
            return createEState(targetLabel,cs,newPState,cset);
          } else {
            // type not supported yet
            SAWYER_MESG(logger[WARN])<<"aggregate initializer: unsupported type: "<<aggregateInitializer->get_type()->unparseToString()<<endl;
            // do not modify state. Value remains top.
            PState newPState=*currentEState.pstate();
            ConstraintSet cset=*currentEState.constraints();
            return createEState(targetLabel,cs,newPState,cset);
          }
        } else if(SgAssignInitializer* assignInitializer=isSgAssignInitializer(initializer)) {
          SgExpression* rhs=assignInitializer->get_operand_i();
          ROSE_ASSERT(rhs);
          SAWYER_MESG(logger[TRACE])<<"declaration with assign initializer:"<<" lhs:"<<initDeclVarId.toString(getVariableIdMapping())<<" rhs:"<<assignInitializer->unparseToString()<<" decl-term:"<<AstTerm::astTermWithNullValuesToString(initName)<<endl;
          
          // only create string in state with variable as pointer-address if it is an array (not for the case it is a char* pointer)
          // in the case of char* it is handled as a pointer initializer (and the string-pointer is already available in state)
          if(SgStringVal* stringValNode=isSgStringVal(assignInitializer->get_operand())) {
            if(isSgArrayType(initName->get_type())) {
              // handle special cases of: char a[]="abc"; char a[4]="abc";
              // TODO: a[5]="ab";
              SAWYER_MESG(logger[TRACE])<<"Initalizing (array) with string: "<<stringValNode->unparseToString()<<endl;
              PState newPState=*currentEState.pstate();
              initializeStringLiteralInState(newPState,stringValNode,initDeclVarId);
              //size_t stringLen=stringValNode->get_value().size();
              if(variableIdMapping->getNumberOfElements(initDeclVarId)==0) {
                VariableId stringLiteralId=getVariableIdMapping()->getStringLiteralVariableId(stringValNode);
                size_t stringLiteralMemoryRegionSize=getVariableIdMapping()->getNumberOfElements(stringLiteralId);
                variableIdMapping->setNumberOfElements(initDeclVarId,stringLiteralMemoryRegionSize);
                SAWYER_MESG(logger[TRACE])<<"Determined size of array from literal string memory region size: "<<stringLiteralMemoryRegionSize<<endl;
              } else {
                SAWYER_MESG(logger[TRACE])<<"Determined size of array from array variable (containing string memory region) size: "<<variableIdMapping->getNumberOfElements(initDeclVarId)<<endl;
              }
              SgType* variableType=initializer->get_type(); // for char and wchar
              setElementSize(initDeclVarId,variableType);
              ConstraintSet cset=*currentEState.constraints();
              return createEState(targetLabel,cs,newPState,cset);
            }
          }
          // set type info for initDeclVarId
          variableIdMapping->setNumberOfElements(initDeclVarId,1); // single variable
          SgType* variableType=initializer->get_type();
          setElementSize(initDeclVarId,variableType);
          
          // build lhs-value dependent on type of declared variable
          AbstractValue lhsAbstractAddress=AbstractValue(initDeclVarId); // creates a pointer to initDeclVar
          list<SingleEvalResultConstInt> res=exprAnalyzer.evaluateExpression(rhs,currentEState);
          if(res.size()!=1) {
            if(res.size()>1) {
              cerr<<"Error: multiple results in rhs evaluation."<<endl;
              cerr<<"expr: "<<SgNodeHelper::sourceLineColumnToString(decl)<<": "<<decl->unparseToString()<<endl;
              exit(1);
            } else {
              ROSE_ASSERT(res.size()==0);
              SAWYER_MESG(logger[TRACE])<<"no results in rhs evaluation (returning top): "<<rhs->unparseToString()<<endl;
              EState estate=currentEState;
              PState newPState=*estate.pstate();
              getExprAnalyzer()->writeToMemoryLocation(label,&newPState,lhsAbstractAddress,CodeThorn::Top());
              ConstraintSet cset=*estate.constraints();
              return createEState(targetLabel,cs,newPState,cset);
            }
          }
          ROSE_ASSERT(res.size()==1);
          SingleEvalResultConstInt evalResult=*res.begin();
          SAWYER_MESG(logger[TRACE])<<"rhs eval result: "<<evalResult.result.toString()<<endl;

          EState estate=evalResult.estate;
          PState newPState=*estate.pstate();
          getExprAnalyzer()->writeToMemoryLocation(label,&newPState,lhsAbstractAddress,evalResult.value());
          ConstraintSet cset=*estate.constraints();
          return createEState(targetLabel,cs,newPState,cset);
        } else {
          SAWYER_MESG(logger[WARN]) << "unsupported initializer in declaration: "<<decl->unparseToString()<<" not adding to state (assuming arbitrary value)"<<endl;
          PState newPState=*currentEState.pstate();
          return createEState(targetLabel,cs,newPState,cset);
        }
      } else {
        // no initializer (model default cases)
        ROSE_ASSERT(initName!=nullptr);
        SgArrayType* arrayType=isSgArrayType(initName->get_type());
        if(arrayType) {
          SgType* arrayElementType=arrayType->get_base_type();
          setElementSize(initDeclVarId,arrayElementType);
          int numElements=variableIdMapping->getArrayElementCount(arrayType);
          if(numElements==0) {
            SAWYER_MESG(logger[TRACE])<<"Number of elements in array is 0 (from variableIdMapping) - evaluating expression"<<endl;
            std::vector<SgExpression*> arrayDimExps=SageInterface::get_C_array_dimensions(*arrayType);
            if(arrayDimExps.size()>1) {
              SAWYER_MESG(logger[WARN])<<"multi-dimensional arrays not supported yet. Only linear arrays are supported. Not added to state (assuming arbitrary value)."<<endl;
              // not adding it to state. Will be used as unknown.
              PState newPState=*currentEState.pstate();
              return createEState(targetLabel,cs,newPState,cset);
            }
            ROSE_ASSERT(arrayDimExps.size()==1);
            SgExpression* arrayDimExp=*arrayDimExps.begin();
            SAWYER_MESG(logger[TRACE])<<"Array dimension expression: "<<arrayDimExp->unparseToString()<<endl;
            list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evaluateExpression(arrayDimExp,currentEState);
            ROSE_ASSERT(evalResultList.size()==1);
            SingleEvalResultConstInt evalRes=*evalResultList.begin();
            AbstractValue arrayDimAVal=evalRes.result;
            SAWYER_MESG(logger[TRACE])<<"Computed array dimension: "<<arrayDimAVal.toString()<<endl;
            if(arrayDimAVal.isConstInt()) {
              numElements=arrayDimAVal.getIntValue();
              variableIdMapping->setNumberOfElements(initDeclVarId,numElements);
            } else {
              // TODO: size of array remains 1?
            }
          } else {
            variableIdMapping->setNumberOfElements(initDeclVarId,numElements);
          }
        } else {
          // set type info for initDeclVarId
          variableIdMapping->setNumberOfElements(initDeclVarId,1); // single variable
          SgType* variableType=initName->get_type();
          setElementSize(initDeclVarId,variableType);
        }
        
        PState newPState=*currentEState.pstate();
        if(variableIdMapping->hasArrayType(initDeclVarId)) {
          // add default array elements to PState
          size_t length=variableIdMapping->getNumberOfElements(initDeclVarId);
          for(size_t elemIndex=0;elemIndex<length;elemIndex++) {
            AbstractValue newArrayElementAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex));
            // set default init value
            getExprAnalyzer()->writeUndefToMemoryLocation(&newPState,newArrayElementAddr);
          }

        } else if(variableIdMapping->hasClassType(initDeclVarId)) {
          // create only address start address of struct (on the
          // stack) alternatively addresses for all member variables
          // can be created; however, a member var can only be
          // assigned by denoting an element relative to the start of
          // the struct, similar only a pointer can be created. the
          // value is actually uninitialized and therefore is
          // implicitly bot.
          AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
          SAWYER_MESG(logger[TRACE])<<"declaration of struct: "<<variableIdMapping->getVariableDeclaration(initDeclVarId)->unparseToString()<<" : "<<pointerVal.toString(getVariableIdMapping())<<endl;
          getExprAnalyzer()->writeUndefToMemoryLocation(&newPState,pointerVal);
        } else if(variableIdMapping->hasPointerType(initDeclVarId)) {
          // create pointer value and set it to top (=any value possible (uninitialized pointer variable declaration))
          AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
          getExprAnalyzer()->writeUndefToMemoryLocation(&newPState,pointerVal);
        } else {
          // set it to top (=any value possible (uninitialized)) for
          // all remaining cases. It will become an error-path once
          // all cases are addressed explicitly above.
          SAWYER_MESG(logger[TRACE])<<"declaration of variable (other): "<<variableIdMapping->getVariableDeclaration(initDeclVarId)->unparseToString()<<endl;
          getExprAnalyzer()->writeUndefToMemoryLocation(&newPState,initDeclVarId);
        }
        return createEState(targetLabel,cs,newPState,cset);
      }
    } else {
      logger[ERROR] << "in declaration (@initializedName) no variable found ... bailing out."<<endl;
      exit(1);
    }
  } else {
    logger[ERROR] << "in declaration: no variable found ... bailing out."<<endl;
    exit(1);
  }
  ROSE_ASSERT(false); // non-reachable
}

bool CodeThorn::Analyzer::isFailedAssertEState(const EState* estate) {
  if(estate->io.isFailedAssertIO())
    return true;
  if(_treatStdErrLikeFailedAssert) {
    return estate->io.isStdErrIO();
  }
  return false;
}

bool CodeThorn::Analyzer::isVerificationErrorEState(const EState* estate) {
  if(estate->io.isVerificationError())
    return true;
  return false;
}

EState CodeThorn::Analyzer::createFailedAssertEState(EState estate, Label target) {
  EState newEState=estate;
  newEState.io.recordFailedAssert();
  newEState.setLabel(target);
  return newEState;
}

// creates a state that represents that the verification error function was called
// the edge is the external call edge
EState CodeThorn::Analyzer::createVerificationErrorEState(EState estate, Label target) {
  EState newEState=estate;
  newEState.io.recordVerificationError();
  newEState.setLabel(target);
  return newEState;
}

void CodeThorn::Analyzer::initLabeledAssertNodes(SgProject* root) {
  _assertNodes=listOfLabeledAssertNodes(root);
}

list<pair<SgLabelStatement*,SgNode*> > CodeThorn::Analyzer::listOfLabeledAssertNodes(SgProject* root) {
  list<pair<SgLabelStatement*,SgNode*> > assertNodes;
  list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=funDefs.begin();
      i!=funDefs.end();
      ++i) {
    RoseAst ast(*i);
    RoseAst::iterator prev=ast.begin();
    for(RoseAst::iterator j=ast.begin();j!=ast.end();++j) {
      if(SgNodeHelper::Pattern::matchAssertExpr(*j)) {
        if(prev!=j && isSgLabelStatement(*prev)) {
          SgLabelStatement* labStmt=isSgLabelStatement(*prev);
          //string name=labStmt->get_label().getString();
          // TODO check prefix error_
          //logger[INFO] <<"Found label "<<assertNodes.size()<<": "<<name<<endl;
          assertNodes.push_back(make_pair(labStmt,*j));
        }
      }
      prev=j;
    }
  }
  return assertNodes;
}

const EState* CodeThorn::Analyzer::processCompleteNewOrExisting(const EState* es) {
  const PState* ps=es->pstate();
  const ConstraintSet* cset=es->constraints();
  const PState* ps2=pstateSet.processNewOrExisting(ps);
  // TODO: ps2 check as below
  const ConstraintSet* cset2=constraintSetMaintainer.processNewOrExisting(cset);
  // TODO: cset2 check as below
  const EState* es2=new EState(es->label(),ps2,cset2, es->io);
  const EState* es3=estateSet.processNewOrExisting(es2);
  // equivalent object (but with different address) already exists
  // discard superfluous new object and use existing object pointer.
  if(es2!=es3) {
    delete es2;
  }
  return es3;
}

const PState* CodeThorn::Analyzer::processNew(PState& s) {
  return pstateSet.processNew(s);
}
const PState* CodeThorn::Analyzer::processNewOrExisting(PState& s) {
  return pstateSet.processNewOrExisting(s);
}

const EState* CodeThorn::Analyzer::processNew(EState& s) {
  return estateSet.processNew(s);
}

const EState* CodeThorn::Analyzer::processNewOrExisting(EState& estate) {
  return estateSet.processNewOrExisting(estate);
}

const ConstraintSet* CodeThorn::Analyzer::processNewOrExisting(ConstraintSet& cset) {
  return constraintSetMaintainer.processNewOrExisting(cset);
}

EStateSet::ProcessingResult CodeThorn::Analyzer::process(EState& estate) {
  return estateSet.process(estate);
}

std::list<EState> CodeThorn::Analyzer::elistify() {
  std::list<EState> resList;
  return resList;
}

std::list<EState> CodeThorn::Analyzer::elistify(EState res) {
  //assert(res.state);
  //assert(res.constraints());
  std::list<EState> resList;
  resList.push_back(res);
  return resList;
}

// wrapper function for reusing exprAnalyzer's function. This function
// is only relevant for external functions (when the implementation
// does not exist in input program and the semantics are available in
// the analyzer (e.g. malloc, strlen, etc.))
list<EState> CodeThorn::Analyzer::evaluateFunctionCallArguments(Edge edge, SgFunctionCallExp* funCall, EState currentEState, bool useConstraints) {
  CallString cs=currentEState.callString;
  logger[TRACE] <<"evaluating arguments of function call:"<<funCall->unparseToString()<<endl;
  list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evalFunctionCallArguments(funCall, currentEState);
  ROSE_ASSERT(evalResultList.size()>0);
  list<SingleEvalResultConstInt>::iterator resultListIter=evalResultList.begin();
  SingleEvalResultConstInt evalResult=*resultListIter;
  if(evalResultList.size()>1) {
    logger[ERROR] <<"multi-state generating operators in function call parameters not supported."<<endl;
    exit(1);
  }
  PState newPState=*evalResult.estate.pstate();
  ConstraintSet cset=*evalResult.estate.constraints();
  return elistify(createEState(edge.target(),cs,newPState,cset));
}

list<EState> CodeThorn::Analyzer::transferEdgeEState(Edge edge, const EState* estate) {
  ROSE_ASSERT(edge.source()==estate->label());
  //cout<<"ESTATE: "<<estate->toString(getVariableIdMapping())<<endl;
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();
  // 1. we handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(edge.source());
  ROSE_ASSERT(nextNodeToAnalyze1);
  if(edge.isType(EDGE_LOCAL)) {
    return transferFunctionCallLocalEdge(edge,estate);
  } else if(SgNodeHelper::Pattern::matchAssertExpr(nextNodeToAnalyze1)) {
    // handle assert(0)
    return elistify(createFailedAssertEState(currentEState,edge.target()));
  } else if(edge.isType(EDGE_CALL)) {
    return transferFunctionCall(edge,estate);
  } else if(edge.isType(EDGE_EXTERNAL)) {
    return transferFunctionCallExternal(edge,estate);
  } else if(isSgReturnStmt(nextNodeToAnalyze1) && !SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
    // "return x;": add $return=eval() [but not for "return f();"]
    return transferReturnStmt(edge,estate);
  } else if(getLabeler()->isFunctionExitLabel(edge.source())) {
    return transferFunctionExit(edge,estate);
  } else if(getLabeler()->isFunctionCallReturnLabel(edge.source())) {
    return transferFunctionCallReturn(edge,estate);
  } else if(SgCaseOptionStmt* caseStmt=isSgCaseOptionStmt(nextNodeToAnalyze1)) {
    return transferCaseOptionStmt(caseStmt,edge,estate);
  } else if(SgDefaultOptionStmt* caseStmt=isSgDefaultOptionStmt(nextNodeToAnalyze1)) {
    return transferDefaultOptionStmt(caseStmt,edge,estate);
  } else if(SgVariableDeclaration* decl=isSgVariableDeclaration(nextNodeToAnalyze1)) {
    return transferVariableDeclaration(decl,edge,estate);
  } else if(isSgExprStatement(nextNodeToAnalyze1) || SgNodeHelper::isForIncExpr(nextNodeToAnalyze1)) {
    return transferExprStmt(nextNodeToAnalyze1, edge, estate);
  } else if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
    // TODO: this case should be handled as part of transferExprStmt (or ExpressionRoot)
    //cout<<"DEBUG: function call"<<(isCondition?" (inside condition) ":"")<<nextNodeToAnalyze1->unparseToString()<<endl;
    // this case cannot happen for normalized code
    logger[ERROR]<<"Function call detected (not represented in ICFG). Normalization required:"<<SgNodeHelper::sourceLineColumnToString(funCall)<<":"<<funCall->unparseToString()<<endl;
    //bool useConstraints=false;
    exit(1);
    //return evaluateFunctionCallArguments(edge,funCall,*estate,useConstraints);
  } else {
      ROSE_ASSERT(!edge.isType(EDGE_EXTERNAL));
      ROSE_ASSERT(!edge.isType(EDGE_CALLRETURN));
    // nothing to analyze, just create new estate (from same State) with target label of edge
    // can be same state if edge is a backedge to same cfg node
    EState newEState=currentEState;
    newEState.setLabel(edge.target());
    return elistify(newEState);
  }
}

list<EState> CodeThorn::Analyzer::transferIdentity(Edge edge, const EState* estate) {
  // nothing to analyze, just create new estate (from same State) with target label of edge
  // can be same state if edge is a backedge to same cfg node
  EState newEState=*estate;
  newEState.setLabel(edge.target());
  return elistify(newEState);
}

void CodeThorn::Analyzer::initializeStringLiteralInState(PState& initialPState,SgStringVal* stringValNode, VariableId stringVarId) {
  //cout<<"DEBUG: TODO: initializeStringLiteralInState"<<endl;
  string theString=stringValNode->get_value();
  int pos;
  for(pos=0;pos<(int)theString.size();pos++) {
    AbstractValue character(theString[pos]);
    initialPState.writeToMemoryLocation(AbstractValue::createAddressOfArrayElement(stringVarId,pos),character);
  }
  // add terminating 0 to string in state
  initialPState.writeToMemoryLocation(AbstractValue::createAddressOfArrayElement(stringVarId,pos),AbstractValue(0));
}

void CodeThorn::Analyzer::initializeStringLiteralsInState(PState& initialPState) {
  ROSE_ASSERT(getVariableIdMapping());
  //cout<<"DEBUG: TODO: initializeStringLiteralsInState"<<endl;
  std::map<SgStringVal*,VariableId>* map=getVariableIdMapping()->getStringLiteralsToVariableIdMapping();
  for(auto iter=map->begin();iter!=map->end();++iter) {
    auto dataPair=*iter;
    SgStringVal* stringValNode=dataPair.first;
    VariableId stringVarId=dataPair.second;
    initializeStringLiteralInState(initialPState,stringValNode,stringVarId);
    /*
    string theString=stringValNode->get_value();
    for(int pos=0;pos<(int)theString.size();pos++) {
      AbstractValue character(theString[pos]);
      initialPState.writeToMemoryLocation(AbstractValue::createAddressOfArrayElement(stringVarId,pos),character);
    }
    */
  }
}

void CodeThorn::Analyzer::initializeVariableIdMapping(SgProject* project) {
  variableIdMapping->computeVariableSymbolMapping(project);
  variableIdMapping->computeTypeSizes(); // only available in extended VIM
  exprAnalyzer.setVariableIdMapping(getVariableIdMapping());
  AbstractValue::setVariableIdMapping(getVariableIdMapping());
  SAWYER_MESG(logger[TRACE])<<"initializeStructureAccessLookup started."<<endl;
  exprAnalyzer.initializeStructureAccessLookup(project);
  SAWYER_MESG(logger[TRACE])<<"initializeStructureAccessLookup finished."<<endl;
  functionIdMapping.computeFunctionSymbolMapping(project);
  functionCallMapping.computeFunctionCallMapping(project);
}

void CodeThorn::Analyzer::initializeCommandLineArgumentsInState(PState& initialPState) {
  // TODO1: add formal paramters of solo-function
  // SgFunctionDefinition* startFunRoot: node of function
  // estate=analyzeVariableDeclaration(SgVariableDeclaration*,estate,estate.label());
  string functionName=SgNodeHelper::getFunctionName(startFunRoot);
  SgInitializedNamePtrList& initNamePtrList=SgNodeHelper::getFunctionDefinitionFormalParameterList(startFunRoot);
  VariableId argcVarId;
  VariableId argvVarId;
  size_t mainFunArgNr=0;
  for(SgInitializedNamePtrList::iterator i=initNamePtrList.begin();i!=initNamePtrList.end();++i) {
    VariableId varId=variableIdMapping->variableId(*i);
    if(functionName=="main" && initNamePtrList.size()==2) {
      //string varName=getVariableIdMapping()->variableName(varId)) {
      switch(mainFunArgNr) {
      case 0:
	argcVarId=varId;
	SAWYER_MESG(logger[TRACE])<<"INIT CLARGS: found argc in main function."<<endl;
	break;
      case 1:
	argvVarId=varId;
	SAWYER_MESG(logger[TRACE])<<"INIT CLARGS: found argv in main function."<<endl;
	break;
      default:
        throw CodeThorn::Exception("Error: main function has more than 2 parameters.");
      }
      mainFunArgNr++;
    }
    ROSE_ASSERT(varId.isValid());
    // initialize all formal parameters of function (of extremal label) with top
    //initialPState[varId]=AbstractValue(CodeThorn::Top());
    initialPState.writeTopToMemoryLocation(varId);
  }
  // if function main exists and has 2 arguments then argcvarid and argvvarid have valid ids now.
  // otherwise the command line options are ignored (because this is the correct behaviour)
  if(argcVarId.isValid() && argvVarId.isValid()) {
    if(_commandLineOptions.size()>0) {
      // create command line option array argv and argc in initial pstate if argv and argc exist in the program
      int argc=0;
      VariableId argvArrayMemoryId=variableIdMapping->createAndRegisterNewMemoryRegion("$argvmem",(int)_commandLineOptions.size());
      AbstractValue argvAddress=AbstractValue::createAddressOfArray(argvArrayMemoryId);
      initialPState.writeToMemoryLocation(argvVarId,argvAddress);
      for (auto argvElem:_commandLineOptions) {
        SAWYER_MESG(logger[TRACE])<<"INIT: Initial state: "
                                  <<variableIdMapping->variableName(argvVarId)<<"["<<argc+1<<"]: "
                                  <<argvElem<<endl;
        int regionSize=(int)string(argvElem).size();
        SAWYER_MESG(logger[TRACE])<<"argv["<<argc+1<<"] size: "<<regionSize<<endl;
	
        stringstream memRegionName;
        memRegionName<<"$argv"<<argc<<"mem";
        VariableId argvElemArrayMemoryId=variableIdMapping->createAndRegisterNewMemoryRegion(memRegionName.str(),regionSize);
        AbstractValue argvElemAddress=AbstractValue::createAddressOfArray(argvElemArrayMemoryId);
        initialPState.writeToMemoryLocation(AbstractValue::createAddressOfArrayElement(argvVarId,argc),argvElemAddress);
	
        // copy concrete command line argument strings char by char to State
        for(int j=0;_commandLineOptions[argc][j]!=0;j++) {
          SAWYER_MESG(logger[TRACE])<<"INIT: Copying: @argc="<<argc<<" char: "<<_commandLineOptions[argc][j]<<endl;
          AbstractValue argvElemAddressWithIndexOffset;
          AbstractValue AbstractIndex=AbstractValue(j);
          argvElemAddressWithIndexOffset=argvElemAddress+AbstractIndex;
          initialPState.writeToMemoryLocation(argvElemAddressWithIndexOffset,AbstractValue(_commandLineOptions[argc][j]));
        }
        argc++;
      }
      // this also covers the case that no command line options were provided. In this case argc==0. argv is non initialized.
      SAWYER_MESG(logger[TRACE])<<"INIT: Initial state argc:"<<argc<<endl;
      AbstractValue abstractValueArgc(argc);
      initialPState.writeToMemoryLocation(argcVarId,abstractValueArgc);
    } else {
      // argc and argv present in program but no command line arguments provided
      SAWYER_MESG(logger[TRACE])<<"INIT: no command line arguments provided. Initializing argc=0."<<endl;
      AbstractValue abstractValueArgc(0);
      initialPState.writeToMemoryLocation(argcVarId,abstractValueArgc);
    }
  } else {
    // argv and argc not present in program. argv and argc are not added to initialPState.
    // in this case it is irrelevant whether command line arguments were provided (correct behaviour)
    // nothing to do.
  }
}

void CodeThorn::Analyzer::initializeSolver(std::string functionToStartAt,SgNode* root, bool oneFunctionOnly) {
  ROSE_ASSERT(root);
  resetInputSequenceIterator();
  std::string funtofind=functionToStartAt;
  RoseAst completeast(root);
  startFunRoot=completeast.findFunctionByName(funtofind);
  if(startFunRoot==0) {
    std::cout << "Function '"<<funtofind<<"' not found.\n";
    exit(1);
  } else {
    SAWYER_MESG(logger[TRACE])<< "INFO: starting at function '"<<funtofind<<"'."<<endl;
  }
  SAWYER_MESG(logger[TRACE])<< "INIT: Initializing AST node info."<<endl;
  initAstNodeInfo(root);

  SAWYER_MESG(logger[TRACE])<< "INIT: Creating Labeler."<<endl;
  Labeler* labeler= new CTIOLabeler(root,getVariableIdMapping());
  //SAWYER_MESG(logger[TRACE])<< "INIT: Initializing VariableIdMapping."<<endl;
  //exprAnalyzer.setVariableIdMapping(getVariableIdMapping());
  SAWYER_MESG(logger[TRACE])<< "INIT: Creating CFAnalysis."<<endl;
  cfanalyzer=new CFAnalysis(labeler,true);

  //FunctionIdMapping* funIdMapping=new FunctionIdMapping();
  //ROSE_ASSERT(isSgProject(root));
  //funIdMapping->computeFunctionSymbolMapping(isSgProject(root));
  //cfanalyzer->setFunctionIdMapping(funIdMapping);
  cfanalyzer->setFunctionIdMapping(getFunctionIdMapping());
  cfanalyzer->setFunctionCallMapping(getFunctionCallMapping());

  getLabeler()->setExternalNonDetIntFunctionName(_externalNonDetIntFunctionName);
  getLabeler()->setExternalNonDetLongFunctionName(_externalNonDetLongFunctionName);


  // logger[DEBUG]<< "mappingLabelToLabelProperty: "<<endl<<getLabeler()->toString()<<endl;
  SAWYER_MESG(logger[TRACE])<< "INIT: Building CFGs."<<endl;

  if(oneFunctionOnly)
    flow=cfanalyzer->flow(startFunRoot);
  else
    flow=cfanalyzer->flow(root);

  // Runs consistency checks on the fork / join and workshare / barrier nodes in the parallel CFG
  // If the --omp-ast flag is not selected by the user, the parallel nodes are not inserted into the CFG
  if (CodeThorn::args.getBool("omp-ast")) {
    cfanalyzer->forkJoinConsistencyChecks(flow);
  }

  SAWYER_MESG(logger[TRACE])<< "STATUS: Building CFGs finished."<<endl;
  if(CodeThorn::args.getBool("reduce-cfg")) {
    int cnt=cfanalyzer->optimizeFlow(flow);
    SAWYER_MESG(logger[TRACE])<< "INIT: CFG reduction OK. (eliminated "<<cnt<<" nodes)"<<endl;
  }
  SAWYER_MESG(logger[TRACE])<< "INIT: Intra-Flow OK. (size: " << flow.size() << " edges)"<<endl;
  if(oneFunctionOnly) {
    SAWYER_MESG(logger[TRACE])<<"INFO: analyzing one function only."<<endl;
  }
  InterFlow interFlow=cfanalyzer->interFlow(flow);
  SAWYER_MESG(logger[TRACE])<< "INIT: Inter-Flow OK. (size: " << interFlow.size()*2 << " edges)"<<endl;
  cfanalyzer->intraInterFlow(flow,interFlow);
  SAWYER_MESG(logger[TRACE])<< "INIT: ICFG OK. (size: " << flow.size() << " edges)"<<endl;

#if 0
  if(CodeThorn::args.getBool("reduce-cfg")) {
    int cnt=cfanalyzer->inlineTrivialFunctions(flow);
    cout << "INIT: CFG reduction OK. (inlined "<<cnt<<" functions; eliminated "<<cnt*4<<" nodes)"<<endl;
  }
#endif

  // create empty state
  PState initialPState;
  initializeCommandLineArgumentsInState(initialPState);
  if(optionStringLiteralsInState) {
    initializeStringLiteralsInState(initialPState);
    if(CodeThorn::args.getBool("status")) {
      cout<<"STATUS: created "<<getVariableIdMapping()->numberOfRegisteredStringLiterals()<<" string literals in initial state."<<endl;
    }
  }
  const PState* initialPStateStored=processNew(initialPState);
  ROSE_ASSERT(initialPStateStored);
  //SAWYER_MESG(logger[TRACE])<< "INIT: initial pstate(stored): "<<initialPStateStored->toString(getVariableIdMapping())<<endl;
  SAWYER_MESG(logger[TRACE])<< "INIT: initial pstate(stored): "<<initialPStateStored->toString(getVariableIdMapping())<<endl;
  ROSE_ASSERT(cfanalyzer);
  ConstraintSet cset;
  const ConstraintSet* emptycsetstored=constraintSetMaintainer.processNewOrExisting(cset);
  Label startLabel=cfanalyzer->getLabel(startFunRoot);
  transitionGraph.setStartLabel(startLabel);
  transitionGraph.setAnalyzer(this);
  
  EState estate(startLabel,initialPStateStored,emptycsetstored);

  if(SgProject* project=isSgProject(root)) {
    SAWYER_MESG(logger[TRACE])<< "STATUS: Number of global variables: ";
    list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
    SAWYER_MESG(logger[TRACE])<< globalVars.size()<<endl;

#if 1
    // do not use usedVariablesInsideFunctions(project,variableIdMapping->; (on full C)
    // this will not filter any variables
    VariableIdSet setOfUsedVars;
#else
    VariableIdSet setOfUsedVars=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,variableIdMapping);
#endif
    SAWYER_MESG(logger[TRACE])<< "STATUS: Number of used variables: "<<setOfUsedVars.size()<<endl;

    int filteredVars=0;
    for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
      VariableId globalVarId=variableIdMapping->variableId(*i);
      // TODO: investigate why array variables get filtered (but should not)
      if(true || (setOfUsedVars.find(globalVarId)!=setOfUsedVars.end() && _variablesToIgnore.find(globalVarId)==_variablesToIgnore.end())) {
        globalVarName2VarIdMapping[variableIdMapping->variableName(variableIdMapping->variableId(*i))]=variableIdMapping->variableId(*i);
        estate=analyzeVariableDeclaration(*i,estate,estate.label());
      } else {
        filteredVars++;
      }
    }
    SAWYER_MESG(logger[TRACE])<< "STATUS: Number of filtered variables for initial pstate: "<<filteredVars<<endl;
    if(_variablesToIgnore.size()>0)
      SAWYER_MESG(logger[TRACE])<< "STATUS: Number of ignored variables for initial pstate: "<<_variablesToIgnore.size()<<endl;
  } else {
    SAWYER_MESG(logger[TRACE])<< "INIT: no global scope.";
  }

  estate.io.recordNone(); // ensure that extremal value is different to bot
  const EState* initialEState=processNew(estate);
  ROSE_ASSERT(initialEState);
  variableValueMonitor.init(initialEState);
  addToWorkList(initialEState);
  SAWYER_MESG(logger[TRACE]) << "INIT: start state (extremal value): "<<initialEState->toString(variableIdMapping)<<endl;

  // initialize summary states map for abstract model checking mode
  initializeSummaryStates(initialPStateStored,emptycsetstored);

  if(CodeThorn::args.getBool("rers-binary")) {
    //initialize the global variable arrays in the linked binary version of the RERS problem
    SAWYER_MESG(logger[DEBUG])<< "init of globals with arrays for "<< _numberOfThreadsToUse << " threads. " << endl;
    RERS_Problem::rersGlobalVarsArrayInitFP(_numberOfThreadsToUse);
    RERS_Problem::createGlobalVarAddressMapsFP(this);
  }

  SAWYER_MESG(logger[TRACE])<< "INIT: finished."<<endl;
}

// TODO: this function should be implemented with a call of ExprCodeThorn::Analyzer::evaluateExpression
PState CodeThorn::Analyzer::analyzeAssignRhs(Label lab, PState currentPState, VariableId lhsVar, SgNode* rhs, ConstraintSet& cset) {
  ROSE_ASSERT(isSgExpression(rhs));
  AbstractValue rhsIntVal=CodeThorn::Top();
  bool isRhsIntVal=false;
  bool isRhsVar=false;

  if(SgCastExp* castExp=isSgCastExp(rhs)) {
    // just skip the cast for now (casting is addressed in the new expression evaluation)
    rhs=castExp->get_operand();
  }
  
  // TODO: -1 is OK, but not -(-1); yet.
  if(SgMinusOp* minusOp=isSgMinusOp(rhs)) {
    if(SgIntVal* intValNode=isSgIntVal(SgNodeHelper::getFirstChild(minusOp))) {
      // found integer on rhs
      rhsIntVal=-((int)intValNode->get_value());
      isRhsIntVal=true;
    }
  }
  // extracted info: isRhsIntVal:rhsIntVal
  if(SgIntVal* intValNode=isSgIntVal(rhs)) {
    // found integer on rhs
    rhsIntVal=(int)intValNode->get_value();
    isRhsIntVal=true;
  }
  // allow single var on rhs
  if(SgVarRefExp* varRefExp=isSgVarRefExp(rhs)) {
    VariableId rhsVarId;
    isRhsVar=exprAnalyzer.variable(varRefExp,rhsVarId);
    ROSE_ASSERT(isRhsVar);
    ROSE_ASSERT(rhsVarId.isValid());
    // x=y: constraint propagation for var1=var2 assignments
    // we do not perform this operation on assignments yet, as the constraint set could become inconsistent.
    //cset.addEqVarVar(lhsVar, rhsVarId);

    if(currentPState.varExists(rhsVarId)) {
      rhsIntVal=getExprAnalyzer()->readFromMemoryLocation(lab,&currentPState,rhsVarId);
    } else {
      if(variableIdMapping->hasArrayType(rhsVarId) && CodeThorn::args.getBool("explicit-arrays")==false) {
        // in case of an array the id itself is the pointer value
        ROSE_ASSERT(rhsVarId.isValid());
        rhsIntVal=rhsVarId.getIdCode();
      } else {
        SAWYER_MESG(logger[WARN])<< "access to variable "<<variableIdMapping->uniqueVariableName(rhsVarId)<< " id:"<<rhsVarId.toString()<<" on rhs of assignment, but variable does not exist in state. Initializing with top."<<endl;
        rhsIntVal=CodeThorn::Top();
        isRhsIntVal=true; 
      }
    }
  }
  PState newPState=currentPState;

  // handle pointer assignment
  if(variableIdMapping->hasPointerType(lhsVar)) {
    //cout<<"DEBUG: "<<lhsVar.toString()<<" = "<<rhs->unparseToString()<<" : "<<astTermWithNullValuesToString(rhs)<<" : ";
    //cout<<"LHS: pointer variable :: "<<lhsVar.toString()<<endl;
    if(SgVarRefExp* rhsVarExp=isSgVarRefExp(rhs)) {
      VariableId rhsVarId=variableIdMapping->variableId(rhsVarExp);
      if(variableIdMapping->hasArrayType(rhsVarId)) {
        //cout<<" of array type.";
        // we use the id-code as int-value (points-to info)
        int idCode=rhsVarId.getIdCode();
        getExprAnalyzer()->writeToMemoryLocation(lab,&newPState,lhsVar,CodeThorn::AbstractValue(AbstractValue(idCode)));
        //cout<<" id-code: "<<idCode;
        return newPState;
      } else {
        logger[ERROR] <<"RHS: unknown : type: ";
        logger[ERROR]<<AstTerm::astTermWithNullValuesToString(isSgExpression(rhs)->get_type());
        exit(1);
      }
      //cout<<endl;
    }
  }

  if(isRhsIntVal && isPrecise()) {
    ROSE_ASSERT(!rhsIntVal.isTop());
  }

  if(newPState.varExists(lhsVar)) {
    if(!isRhsIntVal && !isRhsVar) {
      rhsIntVal=CodeThorn::Top();
      ROSE_ASSERT(!isPrecise());
    }
    // we are using AbstractValue here (and  operator== is overloaded for AbstractValue==AbstractValue)
    // for this comparison isTrue() is also false if any of the two operands is CodeThorn::Top()
    if( (getExprAnalyzer()->readFromMemoryLocation(lab,&newPState,lhsVar).operatorEq(rhsIntVal)).isTrue() ) {
      // update of existing variable with same value
      // => no state change
      return newPState;
    } else {
      // update of existing variable with new value
      //newPState[lhsVar]=rhsIntVal;
      getExprAnalyzer()->writeToMemoryLocation(lab,&newPState,lhsVar,rhsIntVal);
#if 0
      if((!rhsIntVal.isTop() && !isRhsVar))
        cset.removeAllConstraintsOfVar(lhsVar);
#endif
      return newPState;
    }
  } else {
    if(_variablesToIgnore.size()>0 && (_variablesToIgnore.find(lhsVar)!=_variablesToIgnore.end())) {
      // nothing to do because variable is ignored
    } else {
      // new variable with new value.
      getExprAnalyzer()->writeToMemoryLocation(lab,&newPState,lhsVar,rhsIntVal);
    }
    // no update of constraints because no constraints can exist for a new variable
    return newPState;
  }
  // make sure, we only create/propagate contraints if a non-const value is assigned or if a variable is on the rhs.
#if 0
  if(!rhsIntVal.isTop() && !isRhsVar)
    cset.removeAllConstraintsOfVar(lhsVar);
#endif
  return newPState;
}

void CodeThorn::Analyzer::initAstNodeInfo(SgNode* node) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    AstNodeInfo* attr=new AstNodeInfo();
    (*i)->addNewAttribute("info",attr);
  }
}

void CodeThorn::Analyzer::generateAstNodeInfo(SgNode* node) {
  ROSE_ASSERT(node);
  if(!cfanalyzer) {
    logger[ERROR]<< "CodeThorn::Analyzer:: no cfanalyzer available."<<endl;
    exit(1);
  }
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin().withoutNullValues();i!=ast.end();++i) {
    ROSE_ASSERT(*i);
    AstNodeInfo* attr=dynamic_cast<AstNodeInfo*>((*i)->getAttribute("info"));
    if(attr) {
      if(cfanalyzer->getLabel(*i)!=Label()) {
        if(isSgFunctionCallExp(*i)) {
          SAWYER_MESG(logger[TRACE])<<"skipping SgFunctionCallExp (direct pointer, not pattern): "<<(*i)->unparseToString()<<endl;
          continue;
        }
        attr->setLabel(cfanalyzer->getLabel(*i));
        attr->setInitialLabel(cfanalyzer->initialLabel(*i));
        attr->setFinalLabels(cfanalyzer->finalLabels(*i));
      } else {
        (*i)->removeAttribute("info");
      }
    }
#if 0
    if(attr) cout<<":"<<attr->toString();
    else cout<<": no attribute!"<<endl;
#endif
  }
}

// experimental functions
bool CodeThorn::Analyzer::checkTransitionGraph() {
  set<const EState*> ess=transitionGraph.estateSet();
  bool ok=isConsistentEStatePtrSet(ess);
  ok=ok && getTransitionGraph()->checkConsistency();
  return ok;
}

bool CodeThorn::Analyzer::checkEStateSet() {
  for(EStateSet::iterator i=estateSet.begin();i!=estateSet.end();++i) {
    if(estateSet.estateId(*i)==NO_ESTATE || (*i)->label()==Label()) {
      logger[ERROR]<< "estateSet inconsistent. "<<endl;
      logger[ERROR]<< "  label   :"<<(*i)->label()<<endl;
      logger[ERROR]<< "   estateId: "<<estateSet.estateId(*i)<<endl;
      return false;
    }
  }
  SAWYER_MESG(logger[TRACE])<< "INFO: estateSet of size "<<estateSet.size()<<" consistent."<<endl;
  return true;
}

bool CodeThorn::Analyzer::isConsistentEStatePtrSet(set<const EState*> estatePtrSet)  {
  for(set<const EState*>::iterator i=estatePtrSet.begin();i!=estatePtrSet.end();++i) {
    if(estateSet.estateId(*i)==NO_ESTATE || (*i)->label()==Label()) {
      logger[ERROR]<< "estatePtrSet inconsistent. "<<endl;
      logger[ERROR]<< "  label   :"<<(*i)->label()<<endl;
      logger[ERROR]<< "   estateId: "<<estateSet.estateId(*i)<<endl;
      return false;
    }
  }
  SAWYER_MESG(logger[TRACE])<< "INFO: estatePtrSet of size "<<estatePtrSet.size()<<" consistent."<<endl;
  return true;
}

CTIOLabeler* CodeThorn::Analyzer::getLabeler() const {
  CTIOLabeler* ioLabeler=dynamic_cast<CTIOLabeler*>(cfanalyzer->getLabeler());
  ROSE_ASSERT(ioLabeler);
  return ioLabeler;
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
void CodeThorn::Analyzer::resetAnalysis() {
  // reset miscellaneous state variables
  _topifyModeActive = false;
  _iterations = 0;
  _approximated_iterations = 0;
  _curr_iteration_cnt = 0;
  _next_iteration_cnt = 0;
  // reset worklists
  estateWorkListCurrent->clear();
  estateWorkListNext->clear();
  // reset state sets, but re-add STG start state (TODO: also reset constraint set)
  EState startEState=*(transitionGraph.getStartEState());
  PState startPState=*(startEState.pstate());
  EStateSet newEStateSet;
  estateSet = newEStateSet;
  PStateSet newPStateSet;
  pstateSet = newPStateSet;
  estateSet.max_load_factor(0.7);
  pstateSet.max_load_factor(0.7);
  const PState* processedPState=processNew(startPState);
  ROSE_ASSERT(processedPState);
  startEState.setPState(processedPState);
  const EState* processedEState=processNew(startEState);
  ROSE_ASSERT(processedEState);
  // reset STG //TODO: implement "void TransitionGraph::clear()"
  TransitionGraph emptyStg;
  emptyStg.setModeLTLDriven(transitionGraph.getModeLTLDriven());
  if(transitionGraph.getModeLTLDriven()) {
    emptyStg.setStartEState(processedEState);
    emptyStg.setAnalyzer(this);
  }
  emptyStg.setStartLabel(processedEState->label());
  emptyStg.setIsPrecise(transitionGraph.isPrecise());
  emptyStg.setIsComplete(transitionGraph.isComplete());
  transitionGraph = emptyStg;
  // reset variableValueMonitor
  VariableValueMonitor newVariableValueMonitor;
  variableValueMonitor = newVariableValueMonitor;
  variableValueMonitor.init(processedEState);
  // re-init worklist with STG start state
  addToWorkList(processedEState);
  // check if the reset yields the expected sizes of corresponding data structures
  ROSE_ASSERT(estateSet.size() == 1);
  ROSE_ASSERT(pstateSet.size() == 1);
  ROSE_ASSERT(transitionGraph.size() == 0);
  ROSE_ASSERT(estateWorkListCurrent->size() == 1);
  ROSE_ASSERT(estateWorkListNext->size() == 0);
}

/*! 
  * \author Marc Jasper
  * \date 2014, 2015.
 */
void CodeThorn::Analyzer::storeStgBackup() {
  backupTransitionGraph = transitionGraph;
}

/*! 
  * \author Marc Jasper
  * \date 2014, 2015.
 */
void CodeThorn::Analyzer::swapStgWithBackup() {
  TransitionGraph tTemp = transitionGraph;
  transitionGraph = backupTransitionGraph;
  backupTransitionGraph = tTemp;
}


/*! 
  * \author Markus Schordan
  * \date 2019.
 */

#define FAST_GRAPH_REDUCE
void CodeThorn::Analyzer::reduceStg(function<bool(const EState*)> predicate) {
#ifdef FAST_GRAPH_REDUCE
  // MS 3/17/2019: new faster implementation
  transitionGraph.reduceEStates3(predicate);
#else
  _stgReducer.reduceStgToStatesSatisfying(predicate);
#endif
}


/*! 
 * \author Marc Jasper
 * \date 2017.
 */                    
void CodeThorn::Analyzer::reduceStgToInOutStates() {
  function<bool(const EState*)> predicate = [](const EState* s) { 
    return s->io.isStdInIO() || s->io.isStdOutIO();
  };
  reduceStg(predicate);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
void CodeThorn::Analyzer::reduceStgToInOutAssertStates() {
  function<bool(const EState*)> predicate = [](const EState* s) { 
    return s->io.isStdInIO() || s->io.isStdOutIO() || s->io.isFailedAssertIO();
  };
  reduceStg(predicate);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
void CodeThorn::Analyzer::reduceStgToInOutAssertErrStates() {
  function<bool(const EState*)> predicate = [](const EState* s) { 
    return s->io.isStdInIO() || s->io.isStdOutIO()  || s->io.isFailedAssertIO() || s->io.isStdErrIO();
  };
  reduceStg(predicate);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
void CodeThorn::Analyzer::reduceStgToInOutAssertWorklistStates() {
  // copy elements from worklist into hashset (faster access within the predicate)
  unordered_set<const EState*> worklistSet(estateWorkListCurrent->begin(), estateWorkListCurrent->end());
  function<bool(const EState*)> predicate = [&worklistSet](const EState* s) { 
    return s->io.isStdInIO() || s->io.isStdOutIO() 
    || s->io.isFailedAssertIO() || (worklistSet.find(s) != worklistSet.end());
  };
  reduceStg(predicate);
}

int CodeThorn::Analyzer::reachabilityAssertCode(const EState* currentEStatePtr) {
  if(CodeThorn::args.getBool("rers-binary")) {
    int outputVal = getExprAnalyzer()->readFromMemoryLocation(currentEStatePtr->label(),currentEStatePtr->pstate(),globalVarIdByName("output")).getIntValue();
    if (outputVal > -100) {  //either not a failing assertion or a stderr output treated as a failing assertion)
      return -1;
    }
    int assertCode = ((outputVal+100)*(-1));
    ROSE_ASSERT(assertCode>=0 && assertCode <=99);
    return assertCode;
  }
  string name=labelNameOfAssertLabel(currentEStatePtr->label());
  if(name.size()==0)
    return -1;
  if(name=="globalError")
    name="error_60";
  name=name.substr(6,name.size()-6);
  std::istringstream ss(name);
  int num;
  ss>>num;
  return num;
}

void CodeThorn::Analyzer::setSkipSelectedFunctionCalls(bool flag) {
  _skipSelectedFunctionCalls=flag; 
  exprAnalyzer.setSkipSelectedFunctionCalls(flag);
}

void CodeThorn::Analyzer::setSkipArrayAccesses(bool skip) {
  exprAnalyzer.setSkipArrayAccesses(skip);
}

bool CodeThorn::Analyzer::getSkipArrayAccesses() {
  return exprAnalyzer.getSkipArrayAccesses();
}

void CodeThorn::Analyzer::setIgnoreUndefinedDereference(bool skip) {
  exprAnalyzer.setIgnoreUndefinedDereference(skip);
}

bool CodeThorn::Analyzer::getIgnoreUndefinedDereference() {
  return exprAnalyzer.getIgnoreUndefinedDereference();
}

void CodeThorn::Analyzer::set_finished(std::vector<bool>& v, bool val) {
  ROSE_ASSERT(v.size()>0);
  for(vector<bool>::iterator i=v.begin();i!=v.end();++i) {
    *i=val;
  }
}

bool CodeThorn::Analyzer::all_false(std::vector<bool>& v) {
  ROSE_ASSERT(v.size()>0);
  bool res=false;
#pragma omp critical
  {
  for(vector<bool>::iterator i=v.begin();i!=v.end();++i) {
    res=res||(*i);
  }
  }
  return !res;
}

void CodeThorn::Analyzer::mapGlobalVarInsert(std::string name, int* addr) {
  mapGlobalVarAddress[name]=addr;
  mapAddressGlobalVar[addr]=name;
}

void CodeThorn::Analyzer::setCompoundIncVarsSet(set<AbstractValue> ciVars) {
  _compoundIncVarsSet=ciVars;
}

void CodeThorn::Analyzer::setSmallActivityVarsSet(set<AbstractValue> saVars) {
  _smallActivityVarsSet=saVars;
}

void CodeThorn::Analyzer::setAssertCondVarsSet(set<AbstractValue> acVars) {
  _assertCondVarsSet=acVars;
}


long CodeThorn::Analyzer::analysisRunTimeInSeconds() {
  long result;
#pragma omp critical(TIMER)
  {
    result = (long) (_analysisTimer.getTimeDuration().seconds());
  }
  return result;
}

std::list<EState> CodeThorn::Analyzer::transferFunctionCall(Edge edge, const EState* estate) {
   // 1) obtain actual parameters from source
   // 2) obtain formal parameters from target
   // 3) eval each actual parameter and assign result to formal parameter in state
   // 4) create new estate and update callstring (context sensitive analysis)
  SAWYER_MESG(logger[TRACE])<<"transferFunctionCall: "<<getLabeler()->getNode(edge.source())->unparseToString()<<endl;
  Label currentLabel=estate->label();
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  // ad 1)
  SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(getLabeler()->getNode(edge.source()));
  ROSE_ASSERT(funCall);

  if(CodeThorn::args.getBool("rers-binary")) {
    // if rers-binary function call is selected then we skip the static analysis for this function (specific to rers)
    string funName=SgNodeHelper::getFunctionName(funCall);
    if(funName=="calculate_outputFP") {
      // logger[DEBUG]<< "rers-binary mode: skipped static-analysis call."<<endl;
      return elistify();
    }
  }

  SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
  // ad 2)
  SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.target()));
  SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
  ROSE_ASSERT(funDef);
  // ad 3)
  PState newPState=currentPState;
  SgInitializedNamePtrList::iterator i=formalParameters.begin();
  SgExpressionPtrList::iterator j=actualParameters.begin();
  while(i!=formalParameters.end() || j!=actualParameters.end()) {
    SgInitializedName* formalParameterName=*i;
    ROSE_ASSERT(formalParameterName);
    // test formal parameter (instead of argument type) to allow for expressions in arguments
    VariableId formalParameterVarId=variableIdMapping->variableId(formalParameterName);
    AbstractValue evalResultValue;
    if(variableIdMapping->hasClassType(formalParameterVarId)) {
      if(getOptionOutputWarnings()) {
        cout<<"Warning: imprecision: "<<SgNodeHelper::sourceLineColumnToString(funCall)<< ": passing of class/Struct/Union types per value as function parameters (assuming top)."<<endl;
      }
      evalResultValue=AbstractValue::createTop();
    } else {
      // VariableName varNameString=name->get_name();
      SgExpression* actualParameterExpr=*j;
      ROSE_ASSERT(actualParameterExpr);
      // check whether the actualy parameter is a single variable: In this case we can propagate the constraints of that variable to the formal parameter.
      // pattern: call: f(x), callee: f(int y) => constraints of x are propagated to y
      VariableId actualParameterVarId;
      ROSE_ASSERT(actualParameterExpr);
#if 0
      if(exprAnalyzer.variable(actualParameterExpr,actualParameterVarId)) {
        // propagate constraint from actualParamterVarId to formalParameterVarId
        cset.addAssignEqVarVar(formalParameterVarId,actualParameterVarId);
      }
#endif
      // general case: the actual argument is an arbitrary expression (including a single variable)
      list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evaluateExpression(actualParameterExpr,currentEState);
      if(evalResultList.size()==0) {
        cerr<<"Internal error: no state computed for argument evaluation at: "<<SgNodeHelper::sourceLineColumnToString(getLabeler()->getNode(edge.source()))<<endl;
        cerr<<"Argument expression: "<<actualParameterExpr->unparseToString()<<endl;
        cerr<<"EState: "<<currentEState.toString(getVariableIdMapping())<<endl;
        exit(1);
      }
      list<SingleEvalResultConstInt>::iterator resultListIter=evalResultList.begin();
      SingleEvalResultConstInt evalResult=*resultListIter;
      if(evalResultList.size()>1) {
        logger[ERROR] <<"multi-state generating operators in function call parameters not supported."<<endl;
        exit(1);
      }
      evalResultValue=evalResult.value();
    }
    // above evaluateExpression does not use constraints (par3==false). Therefore top vars remain top vars
    getExprAnalyzer()->writeToMemoryLocation(currentLabel,&newPState,formalParameterVarId,evalResultValue);
    ++i;++j;
  }
  // assert must hold if #formal-params==#actual-params (TODO: default values)
  ROSE_ASSERT(i==formalParameters.end() && j==actualParameters.end());

  // ad 4
  CallString cs=currentEState.callString;
  if(_contextSensitiveAnalysis) {
     cs=transferFunctionCallContext(cs, currentEState.label());
  }
  EState newEState=createEState(edge.target(),cs,newPState,cset);
  return elistify(newEState);
}

// value semantics for upates 
CallString CodeThorn::Analyzer::transferFunctionCallContext(CallString cs, Label lab) {
  SAWYER_MESG(logger[TRACE])<<"FunctionCallTransfer: adding "<<lab.toString()<<" to cs: "<<cs.toString()<<endl;
  cs.addLabel(lab);
  return cs;
}

std::list<EState> CodeThorn::Analyzer::transferFunctionCallLocalEdge(Edge edge, const EState* estate) {
  Label lab=estate->label();
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();
  if(CodeThorn::args.getBool("rers-binary")) {
    // logger[DEBUG]<<"ESTATE: "<<estate->toString(&variableIdMapping)<<endl;
    SgNode* nodeToAnalyze=getLabeler()->getNode(edge.source());
    if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nodeToAnalyze)) {
      ROSE_ASSERT(funCall);
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(funName=="calculate_outputFP") {
        // RERS global vars binary handling
        PState _pstate=*estate->pstate();
        RERS_Problem::rersGlobalVarsCallInitFP(this,_pstate, omp_get_thread_num());
#if 0
        //input variable passed as a parameter (obsolete since usage of script "transform_globalinputvar")
        int rers_result=RERS_Problem::calculate_outputFP(argument);
        (void) RERS_Problem::calculate_outputFP(argument);
#else
        (void) RERS_Problem::calculate_outputFP( omp_get_thread_num() );
        int rers_result=RERS_Problem::output[omp_get_thread_num()];
#endif
        //logger[DEBUG]<< "Called calculate_outputFP("<<argument<<")"<<" :: result="<<rers_result<<endl;
        if(rers_result<=-100) {
          // we found a failing assert
          // = rers_result*(-1)-100 : rers error-number
          // = -160 : rers globalError (2012 only)
          int index=((rers_result+100)*(-1));
          ROSE_ASSERT(index>=0 && index <=99);
          binaryBindingAssert[index]=true;
          //reachabilityResults.reachable(index); //previous location in code
          //logger[DEBUG]<<"found assert Error "<<index<<endl;
          ConstraintSet _cset=*estate->constraints();
          InputOutput _io;
          _io.recordFailedAssert();
          // error label encoded in the output value, storing it in the new failing assertion EState
          PState newPstate  = _pstate;
          //newPstate[globalVarIdByName("output")]=CodeThorn::AbstractValue(rers_result);
          getExprAnalyzer()->writeToMemoryLocation(lab,&newPstate,
                                                   globalVarIdByName("output"),
                                                   CodeThorn::AbstractValue(rers_result));
          EState _eState=createEState(edge.target(),estate->callString,newPstate,_cset,_io);
          return elistify(_eState);
        }
        RERS_Problem::rersGlobalVarsCallReturnInitFP(this,_pstate, omp_get_thread_num());
        InputOutput newio;
        if (rers_result == -2) {
          newio.recordVariable(InputOutput::STDERR_VAR,globalVarIdByName("input"));	  
        }
        // TODO: _pstate[VariableId(output)]=rers_result;
        // matches special case of function call with return value, otherwise handles call without return value (function call is matched above)
        if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nodeToAnalyze)) {
          SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nodeToAnalyze));
          VariableId lhsVarId;
          bool isLhsVar=exprAnalyzer.variable(lhs,lhsVarId);
          ROSE_ASSERT(isLhsVar); // must hold
          // logger[DEBUG]<< "lhsvar:rers-result:"<<lhsVarId.toString()<<"="<<rers_result<<endl;
          //_pstate[lhsVarId]=AbstractValue(rers_result);
          getExprAnalyzer()->writeToMemoryLocation(lab,&_pstate,lhsVarId,AbstractValue(rers_result));
          ConstraintSet _cset=*estate->constraints();
          //_cset.removeAllConstraintsOfVar(lhsVarId);
          EState _eState=createEState(edge.target(),estate->callString,_pstate,_cset,newio);
          return elistify(_eState);
        } else {
          ConstraintSet _cset=*estate->constraints();
          EState _eState=createEState(edge.target(),estate->callString,_pstate,_cset,newio);
          return elistify(_eState);
        }
        cout <<"PState:"<< _pstate<<endl;
        logger[ERROR] <<"RERS-MODE: call of unknown function."<<endl;
        exit(1);
        // _pstate now contains the current state obtained from the binary
      }
      // logger[DEBUG]<< "@LOCAL_EDGE: function call:"<<SgNodeHelper::nodeToString(funCall)<<endl;
    }
  }
  return elistify();
}

std::list<EState> CodeThorn::Analyzer::transferReturnStmt(Edge edge, const EState* estate) {
  EState currentEState=*estate;
  CallString cs=currentEState.callString;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(edge.source());
  ROSE_ASSERT(nextNodeToAnalyze1);
  SgNode* expr=SgNodeHelper::getFirstChild(nextNodeToAnalyze1);

  if(isSgNullExpression(expr)) {
      // null expr is a no-op
    return elistify(createEState(edge.target(),cs,currentPState,cset));
  } else {
    VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
    {
      returnVarId=variableIdMapping->createUniqueTemporaryVariableId(string("$return"));
    }
    PState newPState=analyzeAssignRhs(currentEState.label(),currentPState,
                                      returnVarId,
                                      expr,
                                      cset);
    return elistify(createEState(edge.target(),cs,newPState,cset));
  }
}

bool CodeThorn::Analyzer::isFeasiblePathContext(CallString& cs,Label lab) {
  return cs.getLength()==0||cs.isLastLabel(lab);
}

std::list<EState> CodeThorn::Analyzer::transferFunctionCallReturn(Edge edge, const EState* estate) {
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  // determine functionCallLabel corresponding to functioncallReturnLabel.
  Label functionCallReturnLabel=edge.source();
  SgNode* node=getLabeler()->getNode(functionCallReturnLabel);
  Label functionCallLabel=getLabeler()->functionCallLabel(node);
  SAWYER_MESG(logger[TRACE])<<"FunctionCallReturnTransfer: "<<functionCallLabel.toString()<<":"<<functionCallReturnLabel.toString()<<" cs: "<<estate->callString.toString()<<endl;

  CallString cs=currentEState.callString;
  if(_contextSensitiveAnalysis) {
    if(getLabeler()->isExternalFunctionCallLabel(functionCallLabel)) {
      // nothing to do for external function call (label is not added
      // to callstring by external function call)
    } else {
      if(isFeasiblePathContext(cs,functionCallLabel)) {
        cs.removeLastLabel(); 
      } else {
        if(cs.isEmpty()) {
          SAWYER_MESG(logger[WARN])<<"Empty context on non-feasable path at label "<<functionCallLabel.toString()<<endl;
        }
        // definitely not feasible path, do not return a state
        SAWYER_MESG(logger[TRACE])<<"definitly on non-feasable path at label (no next state)"<<functionCallLabel.toString()<<endl;
        std::list<EState> emptyList;
        return emptyList;
      }
    }
  }
  
  // 1. handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(edge.source());
  ROSE_ASSERT(nextNodeToAnalyze1);

  if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
    // case 1: return f(); pass estate through
    EState newEState=currentEState;
    newEState.setLabel(edge.target());
    return elistify(newEState);
  } else if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nextNodeToAnalyze1)) {
    // case 2a: x=f(); bind variable x to value of $return
    if(CodeThorn::args.getBool("rers-binary")) {
      if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
        string funName=SgNodeHelper::getFunctionName(funCall);
        if(funName=="calculate_outputFP") {
          EState newEState=currentEState;
          newEState.setLabel(edge.target());
          newEState.callString=cs;
          return elistify(newEState);
        }
      }
    }
    SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1));
    VariableId lhsVarId;
    bool isLhsVar=exprAnalyzer.variable(lhs,lhsVarId);
    ROSE_ASSERT(isLhsVar); // must hold
    PState newPState=*currentEState.pstate();
    // we only create this variable here to be able to find an existing $return variable!
    VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
    {
      returnVarId=variableIdMapping->createUniqueTemporaryVariableId(string("$return"));
    }

    if(newPState.varExists(returnVarId)) {
      AbstractValue evalResult=getExprAnalyzer()->readFromMemoryLocation(currentEState.label(),&newPState,returnVarId);
      getExprAnalyzer()->writeToMemoryLocation(currentEState.label(),&newPState,lhsVarId,evalResult);
      newPState.deleteVar(returnVarId); // remove $return from state
      return elistify(createEState(edge.target(),cs,newPState,cset));
    } else {
      // no $return variable found in state. This can be the case for an extern function.
      // alternatively a $return variable could be added in the external function call to
      // make this handling here uniform
      // for external functions no constraints are generated in the call-return node
      return elistify(createEState(edge.target(),cs,newPState,cset));
    }
  } else if(SgNodeHelper::Pattern::matchFunctionCallExpInVariableDeclaration(nextNodeToAnalyze1)) {
    // case 2b: Type x=f(); bind variable x to value of $return for function call in declaration
    if(CodeThorn::args.getBool("rers-binary")) {
      if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
        string funName=SgNodeHelper::getFunctionName(funCall);
        if(funName=="calculate_outputFP") {
          EState newEState=currentEState;
          newEState.setLabel(edge.target());
          newEState.callString=cs;
          return elistify(newEState);
        }
      }
    }

    // these two lines are different to x=f();
    SgVariableDeclaration* varDecl=SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall(nextNodeToAnalyze1);
    VariableId lhsVarId=getVariableIdMapping()->variableId(varDecl);
    ROSE_ASSERT(lhsVarId.isValid());

    PState newPState=*currentEState.pstate();
    // this variable is created here only to be able to find an existing $return variable in the state
    VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
    {
      returnVarId=variableIdMapping->createUniqueTemporaryVariableId(string("$return"));
    }

    if(newPState.varExists(returnVarId)) {
      AbstractValue evalResult=getExprAnalyzer()->readFromMemoryLocation(currentEState.label(),&newPState,returnVarId);
      getExprAnalyzer()->writeToMemoryLocation(currentEState.label(),&newPState,lhsVarId,evalResult);
      newPState.deleteVar(returnVarId); // remove $return from state
      SAWYER_MESG(logger[TRACE])<<"transferFunctionCallReturn(initialization): LHSVAR:"<<getVariableIdMapping()->variableName(lhsVarId)<<" value: "<<evalResult.toString()<<endl;
      return elistify(createEState(edge.target(),cs,newPState,cset));
    } else {
      // no $return variable found in state. This can be the case for an extern function.
      // alternatively a $return variable could be added in the external function call to
      // make this handling here uniform
      // for external functions no constraints are generated in the call-return node
      SAWYER_MESG(logger[TRACE])<<"-------------------------------------------------"<<endl;
      SAWYER_MESG(logger[TRACE])<<"transferFunctionCallReturn: Variable declaration with function call: no function-return variable $return found! @ "<<SgNodeHelper::sourceLineColumnToString(nextNodeToAnalyze1)<<":"<<nextNodeToAnalyze1->unparseToString()<<endl;
      SAWYER_MESG(logger[TRACE])<<estate->toString(getVariableIdMapping())<<endl;
      SAWYER_MESG(logger[TRACE])<<"-------------------------------------------------"<<endl;
      return elistify(createEState(edge.target(),cs,newPState,cset));
    }
  } else  if(SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(nextNodeToAnalyze1)) {
    // case 3: f(); remove $return from state (discard value)
    PState newPState=*currentEState.pstate();
    VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
    {
      returnVarId=variableIdMapping->createUniqueTemporaryVariableId(string("$return"));
    }
    // no effect if $return does not exist
    newPState.deleteVar(returnVarId);
    return elistify(createEState(edge.target(),cs,newPState,cset));
  } else {
    logger[FATAL] << "function call-return from unsupported call type:"<<nextNodeToAnalyze1->unparseToString()<<endl;
    exit(1);
  }
}

std::list<EState> CodeThorn::Analyzer::transferFunctionExit(Edge edge, const EState* estate) {
  EState currentEState=*estate;
  if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.source()))) {
    // 1) determine all local variables (including formal parameters) of function
    // 2) delete all local variables from state
    // 2a) remove variable from state
    // 2b) remove all constraints concerning this variable
    // 3) create new EState and return

    // ad 1)
    set<SgVariableDeclaration*> varDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
    // ad 2)
    ConstraintSet cset=*currentEState.constraints();
    PState newPState=*(currentEState.pstate());
    VariableIdMapping::VariableIdSet localVars=variableIdMapping->determineVariableIdsOfVariableDeclarations(varDecls);
    SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
    VariableIdMapping::VariableIdSet formalParams=variableIdMapping->determineVariableIdsOfSgInitializedNames(formalParamInitNames);
    VariableIdMapping::VariableIdSet vars=localVars+formalParams;
    set<string> names=variableIdsToVariableNames(vars);

    for(VariableIdMapping::VariableIdSet::iterator i=vars.begin();i!=vars.end();++i) {
      VariableId varId=*i;
      newPState.deleteVar(varId);
      //cset.removeAllConstraintsOfVar(varId);
    }
    // ad 3)
    return elistify(createEState(edge.target(),estate->callString,newPState,cset));
  } else {
    logger[FATAL] << "no function definition associated with function exit label."<<endl;
    exit(1);
  }
}

SgNode* findExprNodeInAstUpwards(VariantT variant,SgNode* node) {
  while(node!=nullptr&&isSgExpression(node)&&(node->variantT()!=variant)) {
    node=node->get_parent();
  }
  if(node)
    // if the search did not find the node and continued to the stmt level 
    // this check ensures that a nullptr is returned
    return isSgExpression(node); 
  else
    return nullptr;
}

std::list<EState> CodeThorn::Analyzer::transferFunctionCallExternal(Edge edge, const EState* estate) {
  EState currentEState=*estate;
  CallString cs=currentEState.callString;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  // 1. we handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(edge.source());
  ROSE_ASSERT(nextNodeToAnalyze1);

  SAWYER_MESG(logger[TRACE]) << "transferFunctionCallExternal: "<<nextNodeToAnalyze1->unparseToString()<<endl;

  //cout<<"DEBUG: transferFunctionCallExternal: "<<nextNodeToAnalyze1->unparseToString()<<endl;

  InputOutput newio;
  Label lab=getLabeler()->getLabel(nextNodeToAnalyze1);

  VariableId varId;
  bool isFunctionCallWithAssignmentFlag=isFunctionCallWithAssignment(lab,&varId);

  SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1);
  evaluateFunctionCallArguments(edge,funCall,*estate,false);

  // TODO: check whether the following test is superfluous meanwhile, since isStdInLabel does take NonDetX functions into account
  bool isExternalNonDetXFunction=false;
  if(svCompFunctionSemantics()) {
    if(funCall) {
      string externalFunctionName=SgNodeHelper::getFunctionName(funCall);
      if(externalFunctionName==_externalNonDetIntFunctionName||externalFunctionName==_externalNonDetLongFunctionName) {
        isExternalNonDetXFunction=true;
      }
    }
  }
  if(isExternalNonDetXFunction || getLabeler()->isStdInLabel(lab,&varId)) {
    if(_inputSequence.size()>0) {
      PState newPState=*currentEState.pstate();
      ConstraintSet newCSet=*currentEState.constraints();
      //newCSet.removeAllConstraintsOfVar(varId);
      list<EState> resList;
      int newValue;
      if(_inputSequenceIterator!=_inputSequence.end()) {
        newValue=*_inputSequenceIterator;
        ++_inputSequenceIterator;
        //cout<<"INFO: input sequence value: "<<newValue<<endl;
      } else {
        return resList; // return no state (this ends the analysis)
      }
      if(CodeThorn::args.getBool("input-values-as-constraints")) {
        cerr<<"Option input-values-as-constraints no longer supported."<<endl;
        exit(1);
        //newCSet.removeAllConstraintsOfVar(varId);
        //newPState[varId]=CodeThorn::Top();
        //newPState.writeTopToMemoryLocation(varId);
        //newCSet.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,AbstractValue(newValue)));
        //ROSE_ASSERT(newCSet.size()>0);
      } else {
        //newCSet.removeAllConstraintsOfVar(varId);
        //newPState[varId]=AbstractValue(newValue);
        getExprAnalyzer()->writeToMemoryLocation(currentEState.label(),&newPState,varId,AbstractValue(newValue));
      }
      newio.recordVariable(InputOutput::STDIN_VAR,varId);
      EState newEState=createEState(edge.target(),cs,newPState,newCSet,newio);
      resList.push_back(newEState);
      // logger[DEBUG]<< "created "<<_inputVarValues.size()<<" input states."<<endl;
      return resList;
    } else {
      if(_inputVarValues.size()>0) {
        // update state (remove all existing constraint on that variable and set it to top)
        PState newPState=*currentEState.pstate();
        ConstraintSet newCSet=*currentEState.constraints();
        //newCSet.removeAllConstraintsOfVar(varId);
        list<EState> resList;
        for(set<int>::iterator i=_inputVarValues.begin();i!=_inputVarValues.end();++i) {
          PState newPState=*currentEState.pstate();
          if(CodeThorn::args.getBool("input-values-as-constraints")) {
            cerr<<"Option input-values-as-constraints no longer supported."<<endl;
            exit(1);
            //newCSet.removeAllConstraintsOfVar(varId);
            //newPState[varId]=CodeThorn::Top();
            //newPState.writeTopToMemoryLocation(varId);
            //newCSet.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,AbstractValue(*i)));
            //assert(newCSet.size()>0);
          } else {
            //newCSet.removeAllConstraintsOfVar(varId);
            // new input value must be const (otherwise constraints must be used)
            //newPState[varId]=AbstractValue(*i);
            getExprAnalyzer()->writeToMemoryLocation(currentEState.label(),&newPState,varId,AbstractValue(*i));
          }
          newio.recordVariable(InputOutput::STDIN_VAR,varId);
          EState newEState=createEState(edge.target(),estate->callString,newPState,newCSet,newio);
          resList.push_back(newEState);
        }
        // logger[DEBUG]<< "created "<<_inputVarValues.size()<<" input states."<<endl;
        return resList;
      } else {
        // without specified input values (default mode: analysis performed for all possible input values)
        // update state (remove all existing constraint on that variable and set it to top)
        PState newPState=*currentEState.pstate();
        ConstraintSet newCSet=*currentEState.constraints();
        // update input var
        //newCSet.removeAllConstraintsOfVar(varId);
        newPState.writeTopToMemoryLocation(varId);
        newio.recordVariable(InputOutput::STDIN_VAR,varId);

        // external call context
        // call string is reused from input-estate. An external function call does not change the call string
        // callReturn node must check for being an external call
        return elistify(createEState(edge.target(),cs,newPState,newCSet,newio));
      }
    }
  }

  if(getInterpretationMode()!=IM_CONCRETE) {
    int constvalue=0;
    if(getLabeler()->isStdOutVarLabel(lab,&varId)) {
      newio.recordVariable(InputOutput::STDOUT_VAR,varId);
      ROSE_ASSERT(newio.var==varId);
      return elistify(createEState(edge.target(),cs,*currentEState.pstate(),*currentEState.constraints(),newio));
    } else if(getLabeler()->isStdOutConstLabel(lab,&constvalue)) {
      newio.recordConst(InputOutput::STDOUT_CONST,constvalue);
      return elistify(createEState(edge.target(),cs,*currentEState.pstate(),*currentEState.constraints(),newio));
    } else if(getLabeler()->isStdErrLabel(lab,&varId)) {
      newio.recordVariable(InputOutput::STDERR_VAR,varId);
      ROSE_ASSERT(newio.var==varId);
      return elistify(createEState(edge.target(),cs,*currentEState.pstate(),*currentEState.constraints(),newio));
    }
  }

  /* handling of specific semantics for external function */
  if(funCall) {
    string funName=SgNodeHelper::getFunctionName(funCall);
    if(svCompFunctionSemantics()) {
      if(funName==_externalErrorFunctionName) {
        //cout<<"DETECTED error function: "<<_externalErrorFunctionName<<endl;
        return elistify(createVerificationErrorEState(currentEState,edge.target()));
      } else if(funName==_externalExitFunctionName) {
        /* the exit function is modeled to terminate the program
           (therefore no successor state is generated)
        */
        return elistify();
      } else {
        // dispatch all other external function calls to the other
        // transferFunctions where the external function call is handled as an expression
        if(isFunctionCallWithAssignmentFlag) {
          // here only the specific format x=f(...) can exist
          SgAssignOp* assignOp=isSgAssignOp(findExprNodeInAstUpwards(V_SgAssignOp,funCall));
          ROSE_ASSERT(assignOp);
          return transferAssignOp(assignOp,edge,estate);
        } else {
          // special case: void function call f(...);
          list<SingleEvalResultConstInt> res=exprAnalyzer.evalFunctionCall(funCall,currentEState);
          // build new estate(s) from single eval result list
          list<EState> estateList;
          for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
            EState estate=(*i).estate;
            PState newPState=*estate.pstate();
            ConstraintSet cset=*estate.constraints();
            estateList.push_back(createEState(edge.target(),cs,newPState,cset));
          }
          return estateList;
        }
      }
    }
    if(isFunctionCallWithAssignmentFlag) {
      // here only the specific format x=f(...) can exist
      SgAssignOp* assignOp=isSgAssignOp(findExprNodeInAstUpwards(V_SgAssignOp,funCall));
      ROSE_ASSERT(assignOp);
      return transferAssignOp(assignOp,edge,estate);
    } else {
      // all other cases, evaluate function call as expression
      //cout<<"DEBUG: external function call: "<<funCall->unparseToString()<<"; evaluating as expression."<<endl;
      list<SingleEvalResultConstInt> res2=exprAnalyzer.evaluateExpression(funCall,currentEState);
      ROSE_ASSERT(res2.size()==1);
      SingleEvalResultConstInt evalResult2=*res2.begin();
      SAWYER_MESG(logger[TRACE])<<"EXTERNAL FUNCTION: "<<SgNodeHelper::getFunctionName(funCall)<<" result(added to state):"<<evalResult2.result.toString()<<endl;

      //EState estate2=evalResult2.estate;
      // create new estate with added return variable (for inter-procedural analysis)
      CallString cs=evalResult2.estate.callString;
      PState newPState=*evalResult2.estate.pstate();
      ConstraintSet cset=*evalResult2.estate.constraints();
      VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
      {
        returnVarId=variableIdMapping->createUniqueTemporaryVariableId(string("$return"));
      }
      getExprAnalyzer()->writeToMemoryLocation(currentEState.label(),&newPState,returnVarId,evalResult2.result);
      //estate2.setLabel(edge.target());
      return elistify(createEState(edge.target(),cs,newPState,cset,evalResult2.estate.io));
    }
  }
  //cout<<"DEBUG: identity: "<<funCall->unparseToString()<<endl; // fflush is an example in the test cases
  // for all other external functions we use identity as transfer function
  EState newEState=currentEState;
  newEState.io=newio;
  newEState.setLabel(edge.target());
  return elistify(newEState);
}

AbstractValue CodeThorn::Analyzer::singleValevaluateExpression(SgExpression* expr,EState currentEState) {
  list<SingleEvalResultConstInt> resultList=exprAnalyzer.evaluateExpression(expr,currentEState);
  ROSE_ASSERT(resultList.size()==1);
  SingleEvalResultConstInt valueResult=*resultList.begin();
  AbstractValue val=valueResult.result;
  return val;
}

std::list<EState> CodeThorn::Analyzer::transferDefaultOptionStmt(SgDefaultOptionStmt* defaultStmt,Edge edge, const EState* estate) {
  SAWYER_MESG(logger[TRACE])<<"DEBUG: DEFAULTSTMT: "<<defaultStmt->unparseToString()<<endl;

  Label targetLabel=edge.target();
  CallString cs=estate->callString;
  PState newPState=*estate->pstate();
  ConstraintSet cset=*estate->constraints();
  SgStatement* blockStmt=isSgBasicBlock(defaultStmt->get_parent());
  ROSE_ASSERT(blockStmt);
  SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
  ROSE_ASSERT(switchStmt);
  SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
  ROSE_ASSERT(condStmt);
  SgExpression* condExpr=isSgExpression(SgNodeHelper::getExprStmtChild(condStmt));

  EState currentEState=*estate;

  // value of switch expression
  AbstractValue switchCondVal=singleValevaluateExpression(condExpr,currentEState);

  // create filter for all case labels (TODO: precompute this set)
  set<SgCaseOptionStmt*> caseStmtSet=SgNodeHelper::switchRelevantCaseStmtNodes(switchStmt);
  for(set<SgCaseOptionStmt*>::iterator i=caseStmtSet.begin();i!=caseStmtSet.end();++i) {
    SgCaseOptionStmt* caseStmt=*i;
    SgExpression* caseExpr=caseStmt->get_key();
    SgExpression* caseExprOptionalRangeEnd=caseStmt->get_key_range_end();
    if(caseExprOptionalRangeEnd) {
      cerr<<"Error: GNU extension range in case statement not supported."<<endl;
      exit(1);
    }
    // value of constant case value
    AbstractValue caseVal=singleValevaluateExpression(caseExpr,currentEState);
    // compare case constant with switch expression value
    SAWYER_MESG(logger[TRACE])<<"switch-default filter cmp: "<<switchCondVal.toString(getVariableIdMapping())<<"=?="<<caseVal.toString(getVariableIdMapping())<<endl;
    // check that not any case label may be equal to the switch-expr value (exact for concrete values)
    AbstractValue comparisonVal=caseVal.operatorEq(switchCondVal);
    if(comparisonVal.isTop()||comparisonVal.isTrue()) {
      // determined that at least one case may be reachable
      SAWYER_MESG(logger[TRACE])<<"switch-default: continuing."<<endl;
      return elistify(createEState(targetLabel,cs,newPState,cset));
    }
  }
  // detected infeasable path (default is not reachable)
  SAWYER_MESG(logger[TRACE])<<"switch-default: infeasable path."<<endl;
  list<EState> emptyList;
  return emptyList;
}

std::list<EState> CodeThorn::Analyzer::transferCaseOptionStmt(SgCaseOptionStmt* caseStmt,Edge edge, const EState* estate) {
  SAWYER_MESG(logger[TRACE])<<"DEBUG: CASESTMT: "<<caseStmt->unparseToString()<<endl;
  Label targetLabel=edge.target();
  CallString cs=estate->callString;
  PState newPState=*estate->pstate();
  ConstraintSet cset=*estate->constraints();
  SgStatement* blockStmt=isSgBasicBlock(caseStmt->get_parent());
  ROSE_ASSERT(blockStmt);
  SgSwitchStatement* switchStmt=isSgSwitchStatement(blockStmt->get_parent());
  ROSE_ASSERT(switchStmt);
  SgStatement* condStmt=isSgStatement(SgNodeHelper::getCond(switchStmt));
  ROSE_ASSERT(condStmt);
  SgExpression* condExpr=isSgExpression(SgNodeHelper::getExprStmtChild(condStmt));

  EState currentEState=*estate;

  // value of switch expression
  AbstractValue switchCondVal=singleValevaluateExpression(condExpr,currentEState);
  
  SgExpression* caseExpr=caseStmt->get_key();
  SgExpression* caseExprOptionalRangeEnd=caseStmt->get_key_range_end();
  if(caseExprOptionalRangeEnd) {
    cerr<<"Error: GNU extension range in case statement not supported."<<endl;
    exit(1);
  }
  // value of constant case value
  AbstractValue caseVal=singleValevaluateExpression(caseExpr,currentEState);
  // compare case constant with switch expression value
  SAWYER_MESG(logger[TRACE])<<"switch cmp: "<<switchCondVal.toString(getVariableIdMapping())<<"=?="<<caseVal.toString(getVariableIdMapping())<<endl;
  AbstractValue comparisonVal=caseVal.operatorEq(switchCondVal);
  if(comparisonVal.isTop()||comparisonVal.isTrue()) {
    SAWYER_MESG(logger[TRACE])<<"switch-case: continuing."<<endl;
    return elistify(createEState(targetLabel,cs,newPState,cset));
  } else {
    // detected infeasable path
    SAWYER_MESG(logger[TRACE])<<"switch-case: infeasable path."<<endl;
    list<EState> emptyList;
    return emptyList;
  }
}

std::list<EState> CodeThorn::Analyzer::transferVariableDeclaration(SgVariableDeclaration* decl, Edge edge, const EState* estate) {
  return elistify(analyzeVariableDeclaration(decl,*estate, edge.target()));
}

std::list<EState> CodeThorn::Analyzer::transferExprStmt(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate) {
  SgNode* nextNodeToAnalyze2=0;
  if(isSgExprStatement(nextNodeToAnalyze1))
    nextNodeToAnalyze2=SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1);
  if(SgNodeHelper::isForIncExpr(nextNodeToAnalyze1)) {
    nextNodeToAnalyze2=nextNodeToAnalyze1;
  }
  ROSE_ASSERT(nextNodeToAnalyze2);

  //Label newLabel;
  //PState newPState;
  //ConstraintSet newCSet;
  if(edge.isType(EDGE_TRUE) || edge.isType(EDGE_FALSE)) {
    return transferTrueFalseEdge(nextNodeToAnalyze2, edge, estate);
  } else if(SgNodeHelper::isPrefixIncDecOp(nextNodeToAnalyze2) || SgNodeHelper::isPostfixIncDecOp(nextNodeToAnalyze2)) {
    return transferIncDecOp(nextNodeToAnalyze2,edge,estate);
  } else if(SgAssignOp* assignOp=isSgAssignOp(nextNodeToAnalyze2)) {
    return transferAssignOp(assignOp, edge, estate);
  } else if(isSgCompoundAssignOp(nextNodeToAnalyze2)) {
    logger[ERROR] <<"compound assignment operators not supported. Use normalization to eliminate these operators."<<endl;
    logger[ERROR] <<"expr: "<<nextNodeToAnalyze2->unparseToString()<<endl;
    exit(1);
  } else if(isSgConditionalExp(nextNodeToAnalyze2)) {
    // this is meanwhile modeled in the ExprAnalyzer - TODO: utilize as expr-stmt.
    logger[ERROR] <<"found conditional expression outside expression (should have been dispatched to ExprAnalyzer)."<<endl;
    exit(1);
  } else {
    return transferIdentity(edge,estate);
  }
}

list<EState> CodeThorn::Analyzer::transferIncDecOp(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate) {
  EState currentEState=*estate;
  CallString cs=estate->callString;

  SgNode* nextNodeToAnalyze3=SgNodeHelper::getUnaryOpChild(nextNodeToAnalyze2);
  VariableId var;
  if(exprAnalyzer.variable(nextNodeToAnalyze3,var)) {
    list<SingleEvalResultConstInt> res=exprAnalyzer.evaluateExpression(nextNodeToAnalyze3,currentEState);
    ROSE_ASSERT(res.size()==1); // must hold for currently supported limited form of ++,--
    list<SingleEvalResultConstInt>::iterator i=res.begin();
    EState estate=(*i).estate;
    PState newPState=*estate.pstate();
    ConstraintSet cset=*estate.constraints();

    AbstractValue oldVarVal=getExprAnalyzer()->readFromMemoryLocation(estate.label(),&newPState,var);
    AbstractValue newVarVal;
    AbstractValue const1=1;
    switch(nextNodeToAnalyze2->variantT()) {
    case V_SgPlusPlusOp:
      newVarVal=oldVarVal+const1; // overloaded binary + operator
      break;
    case V_SgMinusMinusOp:
      newVarVal=oldVarVal-const1; // overloaded binary - operator
      break;
    default:
      logger[ERROR] << "Operator-AST:"<<AstTerm::astTermToMultiLineString(nextNodeToAnalyze2,2)<<endl;
      logger[ERROR] << "Operator:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze2)<<endl;
      logger[ERROR] << "Operand:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze3)<<endl;
      logger[ERROR] <<"programmatic error in handling of inc/dec operators."<<endl;
      exit(1);
    }
    //newPState[var]=varVal;
    getExprAnalyzer()->writeToMemoryLocation(estate.label(),&newPState,var,newVarVal);

#if 0
    if(!(*i).result.isTop())
      cset.removeAllConstraintsOfVar(var);
#endif
    list<EState> estateList;
    estateList.push_back(createEState(edge.target(),cs,newPState,cset));
    return estateList;
  } else {
    throw CodeThorn::Exception("Error: currently inc/dec operators are only supported for variables.");
  }
}

std::list<EState> CodeThorn::Analyzer::transferAssignOp(SgAssignOp* nextNodeToAnalyze2, Edge edge, const EState* estate) {
  CallString cs=estate->callString;
  //cout<<"DEBUG: AssignOp: "<<nextNodeToAnalyze2->unparseToString()<<endl;
  EState currentEState=*estate;
  SgNode* lhs=SgNodeHelper::getLhs(nextNodeToAnalyze2);
  SgNode* rhs=SgNodeHelper::getRhs(nextNodeToAnalyze2);
  list<SingleEvalResultConstInt> res=exprAnalyzer.evaluateExpression(rhs,currentEState, CodeThorn::ExprAnalyzer::MODE_VALUE);
  list<EState> estateList;
  for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
    VariableId lhsVar;
    bool isLhsVar=exprAnalyzer.variable(lhs,lhsVar);
    if(isLhsVar) {
      EState estate=(*i).estate;
      Label currentLabel=estate.label();
      PState newPState=*estate.pstate();
      ConstraintSet cset=*estate.constraints();
      if(variableIdMapping->hasClassType(lhsVar)) {
        // assignments to struct variables are not supported yet (this test does not detect s1.s2 (where s2 is a struct, see below)).
        logger[WARN]<<"assignment of structs (copy constructor) is not supported yet. Target update ignored! (unsound)"<<endl;
      } else if(variableIdMapping->hasCharType(lhsVar)) {
        getExprAnalyzer()->writeToMemoryLocation(currentLabel,&newPState,lhsVar,(*i).result);
      } else if(variableIdMapping->hasIntegerType(lhsVar)) {
        getExprAnalyzer()->writeToMemoryLocation(currentLabel,&newPState,lhsVar,(*i).result);
      } else if(variableIdMapping->hasFloatingPointType(lhsVar)) {
        getExprAnalyzer()->writeToMemoryLocation(currentLabel,&newPState,lhsVar,(*i).result);
      } else if(variableIdMapping->hasBoolType(lhsVar)) {
        getExprAnalyzer()->writeToMemoryLocation(currentLabel,&newPState,lhsVar,(*i).result);
      } else if(variableIdMapping->hasPointerType(lhsVar)) {
        // we assume here that only arrays (pointers to arrays) are assigned
        //newPState[lhsVar]=(*i).result;
        getExprAnalyzer()->writeToMemoryLocation(currentLabel,&newPState,lhsVar,(*i).result);
      } else {
        cerr<<"Error at "<<SgNodeHelper::sourceFilenameLineColumnToString(nextNodeToAnalyze2)<<endl;
        cerr<<"Unsupported type on LHS side of assignment: "
            <<"type: '"<<variableIdMapping->getType(lhsVar)->unparseToString()<<"'"
            <<", "
            <<"AST type node: "<<variableIdMapping->getType(lhsVar)->class_name()
            <<endl;
        exit(1);
      }
#if 0
      if(!(*i).result.isTop()) {
        cset.removeAllConstraintsOfVar(lhsVar);
      }
#endif
      estateList.push_back(createEState(edge.target(),cs,newPState,cset));
    } else if(isSgDotExp(lhs)) {
      SAWYER_MESG(logger[TRACE])<<"detected dot operator on lhs "<<lhs->unparseToString()<<"."<<endl;
      list<SingleEvalResultConstInt> lhsRes=exprAnalyzer.evaluateExpression(lhs,currentEState, CodeThorn::ExprAnalyzer::MODE_ADDRESS);
      for (auto lhsAddressResult : lhsRes) {
        EState estate=(*i).estate;
        Label label=estate.label();
        PState newPState=*estate.pstate();
        ConstraintSet cset=*estate.constraints();
        AbstractValue lhsAddress=lhsAddressResult.result;
        SAWYER_MESG(logger[TRACE])<<"detected dot operator on lhs: writing to "<<lhsAddress.toString(getVariableIdMapping())<<"."<<endl;
        getExprAnalyzer()->writeToMemoryLocation(label,&newPState,lhsAddress,(*i).result);
        estateList.push_back(createEState(edge.target(),cs,newPState,cset));
      }
    } else if(isSgArrowExp(lhs)) {
      SAWYER_MESG(logger[TRACE])<<"detected arrow operator on lhs "<<lhs->unparseToString()<<"."<<endl;
      list<SingleEvalResultConstInt> lhsRes=exprAnalyzer.evaluateExpression(lhs,currentEState, CodeThorn::ExprAnalyzer::MODE_ADDRESS);
      for (auto lhsAddressResult : lhsRes) {
        EState estate=(*i).estate;
        Label label=estate.label();
        PState newPState=*estate.pstate();
        ConstraintSet cset=*estate.constraints();
        AbstractValue lhsAddress=lhsAddressResult.result;
        SAWYER_MESG(logger[TRACE])<<"detected arrow operator on lhs: writing to "<<lhsAddress.toString(getVariableIdMapping())<<"."<<endl;
        getExprAnalyzer()->writeToMemoryLocation(label,&newPState,lhsAddress,(*i).result);
        estateList.push_back(createEState(edge.target(),cs,newPState,cset));
      }
    } else if(isSgPntrArrRefExp(lhs)) {
      // for now we ignore array refs on lhs
      // TODO: assignments in index computations of ignored array ref
      // see ExprAnalyzer.C: case V_SgPntrArrRefExp:
      // since nothing can change (because of being ignored) state remains the same
      VariableIdMapping* _variableIdMapping=variableIdMapping;
      EState estate=(*i).estate;
      Label label=estate.label();
      PState oldPState=*estate.pstate();
      ConstraintSet oldcset=*estate.constraints();
      if(getSkipArrayAccesses()) {
        // TODO: remove constraints on array-element(s) [currently no constraints are computed for arrays]
        estateList.push_back(createEState(edge.target(),cs,oldPState,oldcset));
      } else {
        SgExpression* arrExp=isSgExpression(SgNodeHelper::getLhs(lhs));
        SgExpression* indexExp=isSgExpression(SgNodeHelper::getRhs(lhs));
        if(SgVarRefExp* varRefExp=isSgVarRefExp(arrExp)) {
          PState pstate2=oldPState;
          VariableId arrayVarId=_variableIdMapping->variableId(varRefExp);
          AbstractValue arrayPtrValue;
          // two cases
          if(_variableIdMapping->hasArrayType(arrayVarId)) {
            // create array element 0 (in preparation to have index added, or, if not index is used, it is already the correct index (=0).
            arrayPtrValue=AbstractValue::createAddressOfArray(arrayVarId);
          } else if(_variableIdMapping->hasPointerType(arrayVarId)) {
            // in case it is a pointer retrieve pointer value
            AbstractValue ptr=AbstractValue::createAddressOfArray(arrayVarId);
            if(pstate2.varExists(ptr)) {
              //cout<<"DEBUG: pointer exists (OK): "<<ptr.toString(_variableIdMapping)<<endl;
              arrayPtrValue=getExprAnalyzer()->readFromMemoryLocation(estate.label(),&pstate2,ptr);
              //cout<<"DEBUG: arrayPtrValue: "<<arrayPtrValue.toString(_variableIdMapping)<<endl;
              // convert integer to VariableId
              if(arrayPtrValue.isTop()||arrayPtrValue.isBot()) {
                if(getOptionOutputWarnings()) 
                  cout<<"Warning: "<<nextNodeToAnalyze2->unparseToString()<<arrayPtrValue.toString(_variableIdMapping)<<" array index is top or bot. Not supported yet."<<endl;
              }
              // logger[DEBUG]<<"defering pointer-to-array: ptr:"<<_variableIdMapping->variableName(arrayVarId);
            } else {
              if(getOptionOutputWarnings())
                cout<<"Warning: lhs array access: pointer variable does not exist in PState:"<<ptr.toString()<<endl;
              arrayPtrValue=AbstractValue::createTop();
            }
          } else {
            SAWYER_MESG(logger[ERROR]) <<"lhs array access: unknown type of array or pointer."<<endl;
            exit(1);
          }
          AbstractValue arrayElementId;
          //AbstractValue aValue=(*i).value();
          list<SingleEvalResultConstInt> res=exprAnalyzer.evaluateExpression(indexExp,currentEState);
          ROSE_ASSERT(res.size()==1); // TODO: temporary restriction
          AbstractValue indexValue=(*(res.begin())).value();
          AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrayPtrValue,indexValue);
          //cout<<"DEBUG: arrayPtrPlusIndexValue: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;

          {
            //VariableId arrayVarId2=arrayPtrPlusIndexValue.getVariableId();
            MemoryAccessBounds boundsCheckResult=exprAnalyzer.checkMemoryAccessBounds(arrayPtrPlusIndexValue);
            switch(boundsCheckResult) {
            case ACCESS_DEFINITELY_NP:
              exprAnalyzer.recordDefinitiveNullPointerDereferenceLocation(estate.label());
              SAWYER_MESG(logger[TRACE])<<"Program error detected at "<<SgNodeHelper::sourceLineColumnToString(nextNodeToAnalyze2)<<" : definitive null pointer dereference detected."<<endl;// ["<<lhs->unparseToString()<<"]"<<endl;
              SAWYER_MESG(logger[TRACE])<<"Violating pointer (np): "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
              return estateList;
              break;
            case ACCESS_DEFINITELY_INSIDE_BOUNDS:
              // continue for all values
              break;
            case ACCESS_POTENTIALLY_OUTSIDE_BOUNDS:
              if(arrayPtrPlusIndexValue.isTop()) {
                exprAnalyzer.recordPotentialNullPointerDereferenceLocation(estate.label());
              }
              exprAnalyzer.recordPotentialOutOfBoundsAccessLocation(estate.label());
              // continue for other values
              break;
            case ACCESS_DEFINITELY_OUTSIDE_BOUNDS: {
              exprAnalyzer.recordDefinitiveOutOfBoundsAccessLocation(estate.label());
              int index2=arrayPtrPlusIndexValue.getIndexIntValue();
              SAWYER_MESG(logger[TRACE])<<"Program error detected at "<<SgNodeHelper::sourceLineColumnToString(nextNodeToAnalyze2)<<" : write access out of bounds."<<endl;// ["<<lhs->unparseToString()<<"]"<<endl;
              SAWYER_MESG(logger[TRACE])<<"Violating pointer (bounds): "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;
              //cerr<<"arrayVarId2: "<<arrayVarId2.toUniqueString(_variableIdMapping)<<endl;
              //cerr<<"array size: "<<_variableIdMapping->getNumberOfElements(arrayVarId)<<endl;
              // no state can follow, return estateList (may be empty)
              return estateList;
              break;
            }
            case ACCESS_NON_EXISTING:
              // memory is known not to exist - infeasable path? return empty state list.
              return estateList;
              break;
            default:
              cerr<<"Error: unknown bounds check result: "<<boundsCheckResult<<endl;
              exit(1);
            }
          }
          arrayElementId=arrayPtrPlusIndexValue;
          //cout<<"DEBUG: arrayElementId: "<<arrayElementId.toString(_variableIdMapping)<<endl;
          //SAWYER_MESG(logger[TRACE])<<"arrayElementVarId:"<<arrayElementId.toString()<<":"<<_variableIdMapping->variableName(arrayVarId)<<" Index:"<<index<<endl;
          ROSE_ASSERT(!arrayElementId.isBot());
          // read value of variable var id (same as for VarRefExp - TODO: reuse)
          // TODO: check whether arrayElementId (or array) is a constant array (arrayVarId)
          if(pstate2.varExists(arrayElementId)) {
            // TODO: handle constraints
            getExprAnalyzer()->writeToMemoryLocation(label,&pstate2,arrayElementId,(*i).value()); // *i is assignment-rhs evaluation result
            estateList.push_back(createEState(edge.target(),cs,pstate2,oldcset));
          } else {
            // check that array is constant array (it is therefore ok that it is not in the state)
            //SAWYER_MESG(logger[TRACE]) <<"lhs array-access index does not exist in state (creating it now). Array element id:"<<arrayElementId.toString(_variableIdMapping)<<" PState size:"<<pstate2.size()<<endl;
            //SAWYER_MESG(logger[TRACE])<<"PState:"<<pstate2.toString(getVariableIdMapping())<<endl;
            getExprAnalyzer()->writeToMemoryLocation(label,&pstate2,arrayElementId,(*i).value()); // *i is assignment-rhs evaluation result
            estateList.push_back(createEState(edge.target(),cs,pstate2,oldcset));
          }
        } else {
          logger[ERROR] <<"array-access uses expr for denoting the array. Normalization missing."<<endl;
          logger[ERROR] <<"expr: "<<lhs->unparseToString()<<endl;
          logger[ERROR] <<"arraySkip: "<<getSkipArrayAccesses()<<endl;
          exit(1);
        }
      }
    } else if(SgPointerDerefExp* lhsDerefExp=isSgPointerDerefExp(lhs)) {
      SgExpression* lhsOperand=lhsDerefExp->get_operand();
      SAWYER_MESG(logger[TRACE])<<"lhsOperand: "<<lhsOperand->unparseToString()<<endl;
      list<SingleEvalResultConstInt> resLhs=exprAnalyzer.evaluateExpression(lhsOperand,currentEState);
      if(resLhs.size()>1) {
        throw CodeThorn::Exception("more than 1 execution path (probably due to abstraction) in operand's expression of pointer dereference operator on lhs of "+nextNodeToAnalyze2->unparseToString());
      }
      ROSE_ASSERT(resLhs.size()==1);
      AbstractValue lhsPointerValue=(*resLhs.begin()).result;
      SAWYER_MESG(logger[TRACE])<<"lhsPointerValue: "<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
      if(lhsPointerValue.isNullPtr()) {
        getExprAnalyzer()->recordDefinitiveNullPointerDereferenceLocation(estate->label());
        // no state can follow, return estateList (may be empty)
        // TODO: create error state here
        return estateList;
      } else if(lhsPointerValue.isTop()) {
        getExprAnalyzer()->recordPotentialNullPointerDereferenceLocation(estate->label());
        // specific case a pointer expr evaluates to top. Dereference operation
        // potentially modifies any memory location in the state.
        PState pstate2=*(estate->pstate());
        // iterate over all elements of the state and merge with rhs value
        pstate2.combineValueAtAllMemoryLocations((*i).result);
        estateList.push_back(createEState(edge.target(),cs,pstate2,*(estate->constraints())));
      } else if(!(lhsPointerValue.isPtr())) {
        // changed isUndefined to isTop (2/17/20)
        if(lhsPointerValue.isTop() && getIgnoreUndefinedDereference()) {
          //cout<<"DEBUG: lhsPointerValue:"<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
          PState pstate2=*(estate->pstate());
          // skip write access, just create new state (no effect)
          estateList.push_back(createEState(edge.target(),cs,pstate2,*(estate->constraints())));
        } else {
          SAWYER_MESG(logger[WARN])<<"not a pointer value (or top) in dereference operator: lhs-value:"<<lhsPointerValue.toLhsString(getVariableIdMapping())<<" lhs: "<<lhs->unparseToString()<<" : assuming change of any memory location."<<endl;
          getExprAnalyzer()->recordPotentialNullPointerDereferenceLocation(estate->label());
          // specific case a pointer expr evaluates to top. Dereference operation
          // potentially modifies any memory location in the state.
          PState pstate2=*(estate->pstate());
          // iterate over all elements of the state and merge with rhs value
          pstate2.combineValueAtAllMemoryLocations((*i).result);
          estateList.push_back(createEState(edge.target(),cs,pstate2,*(estate->constraints())));
        }
      } else {
        //cout<<"DEBUG: lhsPointerValue:"<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
        PState pstate2=*(estate->pstate());
        getExprAnalyzer()->writeToMemoryLocation(estate->label(),&pstate2,lhsPointerValue,(*i).result);
        estateList.push_back(createEState(edge.target(),cs,pstate2,*(estate->constraints())));
      }
    } else {
      //cout<<"DEBUG: else (no var, no ptr) ... "<<endl;
      if(getSkipArrayAccesses()&&isSgPointerDerefExp(lhs)) {
        logger[WARN]<<"skipping pointer dereference: "<<lhs->unparseToString()<<endl;
      } else {
        logger[ERROR] << "transferfunction:SgAssignOp: unrecognized expression on lhs."<<endl;
        logger[ERROR] << "expr: "<< lhs->unparseToString()<<endl;
        logger[ERROR] << "type: "<<lhs->class_name()<<endl;
        //cerr << "performing no update of state!"<<endl;
        exit(1);
      }
    }
  }
  return estateList;
}

//#define CONSTR_ELIM_DEBUG

list<EState> CodeThorn::Analyzer::transferTrueFalseEdge(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate) {
  EState currentEState=*estate;
  CallString cs=estate->callString;
  Label newLabel;
  PState newPState;
  ConstraintSet newCSet;
  // MS: the use of contraints is necessary here (for LTL verification). The evaluation of conditions is the only necessary case.
#ifndef CONSTR_ELIM_DEBUG
  list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evaluateExpression(nextNodeToAnalyze2,currentEState);
#else
  list<SingleEvalResultConstInt> evalResultListF=exprAnalyzer.evaluateExpression(nextNodeToAnalyze2,currentEState,false);
  list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evaluateExpression(nextNodeToAnalyze2,currentEState,true);
  //  if(evalResultListF.size()!=evalResultList.size()) {
    cout<<"DEBUG: different evalresultList sizes (false vs true):"<<evalResultList.size()<<":"<<evalResultListF.size()<<endl;
    for(list<SingleEvalResultConstInt>::iterator i=evalResultList.begin();
        i!=evalResultList.end();
        ++i) {
      SingleEvalResultConstInt evalResult=*i;
      if(evalResult.isBot()) cout <<" bot";
      if(evalResult.isTop()) cout <<" top";
      if(evalResult.isTrue()) cout <<" true";
      if(evalResult.isFalse()) cout <<" false";
    }
    cout <<" vs ";
    for(list<SingleEvalResultConstInt>::iterator i=evalResultListF.begin();
        i!=evalResultListF.end();
        ++i) {
      SingleEvalResultConstInt evalResult=*i;
      if(evalResult.isBot()) cout <<" bot";
      if(evalResult.isTop()) cout <<" top";
      if(evalResult.isTrue()) cout <<" true";
      if(evalResult.isFalse()) cout <<" false";
    }
    cout<<" @ "<<nextNodeToAnalyze2->unparseToString();
    cout<<endl;
    //}
#endif
  list<EState> newEStateList;
  for(list<SingleEvalResultConstInt>::iterator i=evalResultList.begin();
      i!=evalResultList.end();
      ++i) {
    SingleEvalResultConstInt evalResult=*i;
    if(evalResult.isBot()) {
      cerr<<"PSTATE: "<<estate->pstate()->toString(getVariableIdMapping())<<endl;
      cerr<<"Error: CONDITION EVALUATES TO BOT : "<<nextNodeToAnalyze2->unparseToString()<<endl;
      exit(1);
    }
    if((evalResult.isTrue() && edge.isType(EDGE_TRUE)) || (evalResult.isFalse() && edge.isType(EDGE_FALSE)) || evalResult.isTop()) {
      // pass on EState
      newLabel=edge.target();
      newPState=*evalResult.estate.pstate();
#if 0
      // merge with collected constraints of expr (exprConstraints)
      if(edge.isType(EDGE_TRUE)) {
        newCSet=*evalResult.estate.constraints()+evalResult.exprConstraints;
      } else if(edge.isType(EDGE_FALSE)) {
        ConstraintSet s1=*evalResult.estate.constraints();
        ConstraintSet s2=evalResult.exprConstraints;
        newCSet=s1+s2;
      } else {
        logger[ERROR]<<"Expected true/false edge. Found edge:"<<edge.toString()<<endl;
        exit(1);
      }
#endif
#if 0
      // make check-ltl-rers-topify (topify => constraints are collected)
      if(newCSet.size()>0) {
        cout<<"DEBUG: cset: "<<newCSet.toString()<<endl;
        cout<<"DEBUG: pstate: "<<estate->pstate()->toString(getVariableIdMapping())<<endl;
      }
#endif
      // use new empty cset instead of computed cset
      ROSE_ASSERT(newCSet.size()==0);
      EState newEstate=createEState(newLabel,cs,newPState,newCSet);
      newEStateList.push_back(newEstate);
    } else {
      // we determined not to be on an execution path, therefore do nothing (do not add any result to resultlist)
      //cout<<"DEBUG: not on feasable execution path. skipping."<<endl;
    }
  }
  return newEStateList;
}

void CodeThorn::Analyzer::setCFAnalyzer(CFAnalysis* cf) { cfanalyzer=cf; }
CodeThorn::CFAnalysis* CodeThorn::Analyzer::getCFAnalyzer() const { return cfanalyzer; }

VariableIdMappingExtended* CodeThorn::Analyzer::getVariableIdMapping() { return variableIdMapping; }
CodeThorn::FunctionIdMapping* CodeThorn::Analyzer::getFunctionIdMapping() { return &functionIdMapping; }
CodeThorn::FunctionCallMapping* CodeThorn::Analyzer::getFunctionCallMapping() { return &functionCallMapping; }
CodeThorn::Flow* CodeThorn::Analyzer::getFlow() { return &flow; }
CodeThorn::EStateSet* CodeThorn::Analyzer::getEStateSet() { return &estateSet; }
CodeThorn::PStateSet* CodeThorn::Analyzer::getPStateSet() { return &pstateSet; }
TransitionGraph* CodeThorn::Analyzer::getTransitionGraph() { return &transitionGraph; }
ConstraintSetMaintainer* CodeThorn::Analyzer::getConstraintSetMaintainer() { return &constraintSetMaintainer; }
std::list<FailedAssertion> CodeThorn::Analyzer::getFirstAssertionOccurences(){return _firstAssertionOccurences;}

void CodeThorn::Analyzer::setCommandLineOptions(vector<string> clOptions) {
  _commandLineOptions=clOptions;
}

bool CodeThorn::Analyzer::isLTLRelevantEState(const EState* estate) {
  ROSE_ASSERT(estate);
  return ((estate)->io.isStdInIO()
          || (estate)->io.isStdOutIO()
          || (estate)->io.isStdErrIO()
          || (estate)->io.isFailedAssertIO());
}

std::string CodeThorn::Analyzer::typeSizeMappingToString() {
  return variableIdMapping->typeSizeMappingToString();
}
