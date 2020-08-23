/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"
#include "Labeler.h"
#include "ClassHierarchyGraph.h"
#include "Analyzer.h"
#include "CodeThornCommandLineOptions.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "AstUtility.h"
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
#include "TopologicalSort.h"

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
  variableIdMapping=new VariableIdMappingExtended();
  //variableIdMapping->setModeVariableIdForEachArrayElement(true);
  for(int i=0;i<100;i++) {
    binaryBindingAssert.push_back(false);
  }
  estateSet.max_load_factor(0.7);
  pstateSet.max_load_factor(0.7);
  constraintSetMaintainer.max_load_factor(0.7);
  resetInputSequenceIterator();
  exprAnalyzer.setAnalyzer(this);
  ROSE_ASSERT(_estateTransferFunctions==nullptr);
  _estateTransferFunctions=new EStateTransferFunctions();
  _estateTransferFunctions->setAnalyzer(this);
}

void CodeThorn::Analyzer::deleteWorkLists() {
  if(estateWorkListCurrent) {
    delete estateWorkListCurrent;
  }
  if(estateWorkListNext) {
    delete estateWorkListNext;
  }
}

void CodeThorn::Analyzer::setWorkLists(ExplorationMode explorationMode) {
  deleteWorkLists();
  switch(_explorationMode) {
  case EXPL_UNDEFINED:
    cerr<<"Error: undefined exploration mode in constructing of Analyzer."<<endl;
    exit(1);
  case EXPL_DEPTH_FIRST:
  case EXPL_BREADTH_FIRST:
  case EXPL_LOOP_AWARE:
  case EXPL_LOOP_AWARE_SYNC:
  case EXPL_RANDOM_MODE1:
    estateWorkListCurrent=new EStateWorkList();
    estateWorkListNext=new EStateWorkList();
    break;
  case EXPL_TOPOLOGIC_SORT: {
    ROSE_ASSERT(getLabeler());
    ROSE_ASSERT(getFlow()->getStartLabel().isValid());
    TopologicalSort topSort(*getLabeler(),*getFlow());
    std::list<Label> labelList=topSort.topologicallySortedLabelList();
    cout<<"Topologic Sort:";
    for(auto label : labelList) {
      cout<<label.toString()<<" ";
    }
    cout<<endl;
    estateWorkListCurrent = new EStatePriorityWorkList();
    estateWorkListNext = new EStatePriorityWorkList(); // currently not used in loop aware mode
    cout<<"STATUS: using topologic worklist."<<endl;
    break;
  }
  }
}

void CodeThorn::Analyzer::setExplorationMode(ExplorationMode em) {
  _explorationMode=em;
}

ExplorationMode CodeThorn::Analyzer::getExplorationMode() {
  return _explorationMode;
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

  delete getFunctionCallMapping2()->getClassHierarchy();
  delete getFunctionCallMapping()->getClassHierarchy();
  delete _estateTransferFunctions;
  deleteWorkLists();
}

CodeThorn::Analyzer::SubSolverResultType CodeThorn::Analyzer::subSolver(const CodeThorn::EState* currentEStatePtr) {
  // start the timer if not yet done
  startAnalysisTimer();

  // first, check size of global EStateSet and print status or switch to topify/terminate analysis accordingly.
  unsigned long estateSetSize;
  bool earlyTermination = false;
  int threadNum = 0; //subSolver currently does not support multiple threads.
  // print status message if required
  if (_ctOpt.status && _displayDiff) {
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
    if(_ctOpt.status) {
      cout << "STATUS: Early termination within subSolver (resource limit reached)." << endl;
    }
    transitionGraph.setForceQuitExploration(true);
  } else {
    // run the actual sub-solver
    EStateWorkList localWorkList;
    localWorkList.push_back(currentEStatePtr);
    while(!localWorkList.empty()) {
      // logger[DEBUG]<<"local work list size: "<<localWorkList.size()<<endl;
      const EState* currentEStatePtr=localWorkList.front();
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
              if(_ctOpt.rers.rersBinary) {
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
                  if(_ltlOpt.withCounterExamples || _ltlOpt.withAssertCounterExamples) {
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

void CodeThorn::Analyzer::setInterpreterMode(CodeThorn::InterpreterMode mode) {
  exprAnalyzer.setInterpreterMode(mode);
}

CodeThorn::InterpreterMode CodeThorn::Analyzer::getInterpreterMode() {
  return exprAnalyzer.getInterpreterMode();
}

void CodeThorn::Analyzer::setInterpreterModeOutputFileName(string fileName) {
  exprAnalyzer.setInterpreterModeFileName(fileName);
}

string CodeThorn::Analyzer::getInterpreterModeOutputFileName() {
  return exprAnalyzer.getInterpreterModeFileName();
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
    if(_ctOpt.status) {
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
  // MS 05/20/20: removed (eliminated explicitArrays mode)
  //if (_ctOpt.arraysNotInState==true && !_ctOpt.rers.rersBinary) {
  //  return false;
  //}
  // analysis is precise if it is not in any abstraction mode
  return !(isActiveGlobalTopify()||_ctOpt.abstractionMode>0);
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
  if ((_maxSeconds!=-1) && ((long int) analysisRunTimeInSeconds() > _maxSeconds)) {
    return true;
  }
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
  startAnalysisTimer();
  _solver->run();
  stopAnalysisTimer();
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
    VariableIdSet setOfUsedVars=AstUtility::usedVariablesInsideFunctions(project,getVariableIdMapping());
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
    ss <<color("white")<<"Number of pstates/estates/trans/csets/wl/iter/time: ";
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
       <<" "<<analysisRunTimeInSeconds()<<" secs"
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
  ss<<"RBin:"<<_ctOpt.rers.rersBinary;
  ss<<" ";
  ss<<"incSTGReady:"<<isIncompleteSTGReady();
  return ss.str();
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
    case EXPL_UNDEFINED:
      cerr<<"Error: undefined state exploration mode. Bailing out."<<endl;
      exit(1);
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
    case EXPL_TOPOLOGIC_SORT:
      // must be estatePriorityWorkList
      ROSE_ASSERT(dynamic_cast<EStatePriorityWorkList*>(estateWorkListCurrent));
      estateWorkListCurrent->push_front(estate);
      break;
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
        _ctOpt.rers.rersBinary=false;
      }
    }
    return true;
  }
  return false;
}

void CodeThorn::Analyzer::eventGlobalTopifyTurnedOn() {
  stringstream ss;
  ss << "STATUS: mode global-topify activated:"<<endl
     << "Transitions  : "<<(long int)transitionGraph.size()<<","<<_maxTransitionsForcedTop<<endl
     << "Iterations   : "<<getIterations()<<":"<< _maxIterationsForcedTop<<endl
     << "Memory(bytes): "<<getPhysicalMemorySize()<<":"<< _maxBytesForcedTop<<endl
     << "Runtime(s)   : "<<analysisRunTimeInSeconds() <<":"<< _maxSecondsForcedTop<<endl;
  printStatusMessage(ss.str());

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

  if(_ctOpt.status) {
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
      estate=estateWorkListCurrent->front();
  }
  return estate;
}
const EState* CodeThorn::Analyzer::popWorkList() {
  const EState* estate=0;
#pragma omp critical(ESTATEWL)
  {
    if(!estateWorkListCurrent->empty())
      estate=estateWorkListCurrent->front();
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

// this function has to be protected by a critical section
// currently called once inside a critical section
void CodeThorn::Analyzer::swapWorkLists() {
  EStateWorkList* tmp = estateWorkListCurrent;
  estateWorkListCurrent = estateWorkListNext;
  estateWorkListNext = tmp;
  incIterations();
}


// set the size of an element determined by this type
void CodeThorn::Analyzer::setElementSize(VariableId variableId, SgType* elementType) {
  unsigned int typeSize=getVariableIdMapping()->getTypeSize(elementType);
  if(getVariableIdMapping()->getElementSize(variableId)!=0
     && getVariableIdMapping()->getElementSize(variableId)!=typeSize) {
    logger[WARN]<<"Element type size mismatch: "
                <<"Analyzer::setElementSize: variableId name: "<<variableIdMapping->variableName(variableId)
                <<"typesize(from VID):"<<getVariableIdMapping()->getElementSize(variableId)
                <<" typeSize(fromtype): "<<typeSize<<" of "<<elementType->unparseToString()
                <<endl;
    // keep existing size from variableIdMapping
    return;
  }
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
  ROSE_ASSERT(aggregateInitializer);
  Label label=currentEState.label();
  PState newPState=pstate;
  int elemIndex=0;
  SgExprListExp* initListObjPtr=aggregateInitializer->get_initializers();
  SgExpressionPtrList& initList=initListObjPtr->get_expressions();

  for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
    AbstractValue arrayElemAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex));
    SgExpression* exp=*i;
    SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
    if(assignInit==nullptr) {
      SAWYER_MESG(logger[WARN])<<"expected assign initializer but found "<<exp->unparseToString();
      SAWYER_MESG(logger[WARN])<<"AST: "<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
      AbstractValue newVal=AbstractValue::createTop();
      getExprAnalyzer()->initializeMemoryLocation(label,&newPState,arrayElemAddr,newVal);
    } else {
      // initialize element of array initializer in state
      SgExpression* assignInitExpr=assignInit->get_operand();
      // currentEState from above, newPState must be the same as in currentEState.
      AbstractValue newVal=singleValevaluateExpression(assignInitExpr,currentEState);
      getExprAnalyzer()->initializeMemoryLocation(label,&newPState,arrayElemAddr,newVal);
    }
    elemIndex++;
  }
  // initialize remaining elements (if there are any) with default value
  int aggregateSize=(int)getVariableIdMapping()->getNumberOfElements(initDeclVarId);
  // if array size is not 0 then it was determined from the type and remaining elements are initialized
  if(aggregateSize==0) {
    // update aggregate size if it was not set yet
    aggregateSize=initList.size();
    getVariableIdMapping()->setNumberOfElements(initDeclVarId,aggregateSize);
  }

  // otherwise the size is determined from the aggregate initializer itself (done above)
  if(aggregateSize!=0) {
    for(int i=elemIndex;i<aggregateSize;i++) {
      AbstractValue arrayElemAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(i));
      SAWYER_MESG(logger[TRACE])<<"Init aggregate default value: "<<arrayElemAddr.toString()<<endl;
      // there must be a default value because an aggregate initializer exists (it is never undefined in this case)
      // note: int a[3]={}; also forces the array to be initialized with {0,0,0}, the list can be empty.
      // whereas with int a[3]; all 3 are undefined
      AbstractValue defaultValue=AbstractValue(0); // TODO: cases where default value is not 0.
      getExprAnalyzer()->initializeMemoryLocation(label,&newPState,arrayElemAddr,defaultValue);
    }
  } else {
    // if aggregate size is 0 there is nothing to do
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

  //cout<<"DEBUG: declaration:"<<decl->unparseToString()<<" : "<<AstTerm::astTermWithNullValuesToString(decl)<<endl;
  CallString cs=currentEState.callString;
  Label label=currentEState.label();

  const SgInitializedNamePtrList& initNameList=decl->get_variables();
  SgNode* initName0;
  if(initNameList.size()>1) {
    SAWYER_MESG(logger[ERROR])<<"Error: variable declaration contains more than one variable. Normalization required."<<endl;
    SAWYER_MESG(logger[ERROR])<<"Error: "<<decl->unparseToString()<<endl;

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
      /*
      if(false && variableValueMonitor.isHotVariable(this,initDeclVarId)) {
        // DEAD CODE
        PState newPState=*currentEState.pstate();
        newPState.reserveMemoryLocation(initDeclVarId);
        ConstraintSet cset=*currentEState.constraints();
        InputOutput io;
        io.recordNone();
        return createEState(targetLabel,cs,newPState,cset);
      }
      */
      // deactivated 05/20/2020
      //      if(variableIdMapping->hasArrayType(initDeclVarId) && _ctOpt.explicitArrays==false) {
        // in case of a constant array the array (and its members) are not added to the state.
        // they are considered to be determined from the initializer without representing them
        // in the state
        // logger[DEBUG] <<"not adding array to PState."<<endl;
      //  PState newPState=*currentEState.pstate();
      //  ConstraintSet cset=*currentEState.constraints();
      //  return createEState(targetLabel,cs,newPState,cset);
      //}

      ConstraintSet cset=*currentEState.constraints();
      SgInitializer* initializer=initName->get_initializer();
      if(initializer) {
        //cout<<"DEBUG: decl-init: "<<decl->unparseToString()<<":AST:"<<AstTerm::astTermWithNullValuesToString(initializer)<<endl;
        // special case that initializer is an assignment (reusing transferAssignOp)
        if(SgAssignInitializer* assignInit=isSgAssignInitializer(initializer)) {
          SgExpression* assignInitOperand=assignInit->get_operand_i();
          ROSE_ASSERT(assignInitOperand);
          if(SgAssignOp* assignOp=isSgAssignOp(assignInitOperand)) {
            SAWYER_MESG(logger[TRACE])<<"assignment in initializer: "<<decl->unparseToString()<<endl;
            //cout<<"DEBUG: assignment in initializer: "<<decl->unparseToString()<<endl;
            Edge dummyEdge(targetLabel,EDGE_FORWARD,targetLabel); // only target label is used in transferAssignOp
            //std::list<EState> estateList=transferAssignOp(assignOp, dummyEdge, &currentEState);
            CodeThorn::Analyzer::MemoryUpdateList memUpdList=evalAssignOp(assignOp,dummyEdge,&currentEState);
            std::list<EState> estateList;
            ROSE_ASSERT(memUpdList.size()==1);
            auto memUpd=*memUpdList.begin();
            // code is normalized, lhs must be a variable : tmpVar= var=val;
            // store result of assignment in declaration variable
            EState estate=memUpd.first;
            PState newPState=*estate.pstate();
            AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
            AbstractValue lhsAddr=memUpd.second.first;
            AbstractValue rhsValue=memUpd.second.second;
            getExprAnalyzer()->writeToMemoryLocation(label,&newPState,lhsAddr,rhsValue); // assignment in initializer
            getExprAnalyzer()->initializeMemoryLocation(label,&newPState,initDeclVarAddr,rhsValue); // initialization of declared var
            return createEState(targetLabel,cs,newPState,cset);
          } else if(SgFunctionRefExp* funRefExp=isSgFunctionRefExp(assignInitOperand)) {
            //cout<<"DEBUG: DETECTED isSgFunctionRefExp: evaluating expression ..."<<endl;
            list<SingleEvalResultConstInt> res=exprAnalyzer.evaluateExpression(funRefExp,currentEState);
            if(res.size()!=1) {
              if(res.size()>1) {
                SAWYER_MESG(logger[ERROR])<<"Error: multiple results in rhs evaluation."<<endl;
                SAWYER_MESG(logger[ERROR])<<"expr: "<<SgNodeHelper::sourceLineColumnToString(decl)<<": "<<decl->unparseToString()<<endl;
                exit(1);
              } else {
                ROSE_ASSERT(res.size()==0);
                SAWYER_MESG(logger[TRACE])<<"no results in rhs evaluation (returning top): "<<decl->unparseToString()<<endl;
              }
            } else {
              ROSE_ASSERT(res.size()==1);
              SingleEvalResultConstInt evalResult=*res.begin();
              SAWYER_MESG(logger[TRACE])<<"rhs eval result: "<<evalResult.result.toString()<<endl;
              
              EState estate=evalResult.estate;
              PState newPState=*estate.pstate();
              AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
              getExprAnalyzer()->initializeMemoryLocation(label,&newPState,initDeclVarAddr,evalResult.value());
              ConstraintSet cset=*estate.constraints();
              return createEState(targetLabel,cs,newPState,cset);
            }
          }
        }
        if(getVariableIdMapping()->hasClassType(initDeclVarId)) {
          SAWYER_MESG(logger[WARN])<<"initialization of structs not supported yet (not added to state) "<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<endl;
          // TODO: for(offset(membervar) : membervars {initialize(address(initDeclVarId)+offset,eval(initializer+));}
          //AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
          // TODO: STRUCT VARIABLE DECLARATION
          //getExprAnalyzer()->reserveMemoryLocation(label,&newPState,pointerVal);
          PState newPState=*currentEState.pstate();
          return createEState(targetLabel,cs,newPState,cset);
        }
        if(getVariableIdMapping()->hasReferenceType(initDeclVarId)) {
          // TODO: initialization of references not supported yet
          SAWYER_MESG(logger[INFO])<<"initialization of reference:"<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<endl;
          SgAssignInitializer* assignInit=isSgAssignInitializer(initializer);
          ROSE_ASSERT(assignInit);
          SgExpression* assignInitOperand=assignInit->get_operand_i();
          ROSE_ASSERT(assignInitOperand);
          //list<SingleEvalResultConstInt> res=exprAnalyzer.evaluateExpression(assignInitOperand,currentEState, CodeThorn::ExprAnalyzer::MODE_ADDRESS);
          list<SingleEvalResultConstInt> res=exprAnalyzer.evaluateLExpression(assignInitOperand,currentEState);

          SAWYER_MESG(logger[INFO])<<"initialization of reference:"<<AstTerm::astTermWithNullValuesToString(assignInitOperand)<<endl;
          if(res.size()!=1) {
            if(res.size()>1) {
              SAWYER_MESG(logger[ERROR])<<"Error: multiple results in rhs evaluation."<<endl;
              SAWYER_MESG(logger[ERROR])<<"expr: "<<SgNodeHelper::sourceLineColumnToString(decl)<<": "<<decl->unparseToString()<<endl;
              exit(1);
            } else {
              ROSE_ASSERT(res.size()==0);
              SAWYER_MESG(logger[TRACE])<<"no results in rhs evaluation (returning top): "<<decl->unparseToString()<<endl;
            }
          } else {
            ROSE_ASSERT(res.size()==1);
            SingleEvalResultConstInt evalResult=*res.begin();
            SAWYER_MESG(logger[INFO])<<"rhs (reference init) result: "<<evalResult.result.toString()<<endl;
            
            EState estate=evalResult.estate;
            PState newPState=*estate.pstate();
            AbstractValue initDeclVarAddr=AbstractValue::createAddressOfVariable(initDeclVarId);
            //initDeclVarAddr.setRefType(); // known to be ref from hasReferenceType above
            // creates a memory cell in state that contains the address of the referred memory cell
            getExprAnalyzer()->initializeMemoryLocation(label,&newPState,initDeclVarAddr,evalResult.value());
            ConstraintSet cset=*estate.constraints();
            return createEState(targetLabel,cs,newPState,cset);
          }
          ROSE_ASSERT(false); // not reachable
        }
        // has aggregate initializer
        if(SgAggregateInitializer* aggregateInitializer=isSgAggregateInitializer(initializer)) {
          if(SgArrayType* arrayType=isSgArrayType(aggregateInitializer->get_type())) {
            SgType* arrayElementType=arrayType->get_base_type();
            setElementSize(initDeclVarId,arrayElementType);
            // only set size from aggregate initializer if not known from type
            if(variableIdMapping->getNumberOfElements(initDeclVarId)==0) {
               variableIdMapping->setNumberOfElements(initDeclVarId, computeNumberOfElements(decl));
            }
            PState newPState=*currentEState.pstate();
            newPState=analyzeSgAggregateInitializer(initDeclVarId, aggregateInitializer,newPState, currentEState);
            return createEState(targetLabel,cs,newPState,cset);
          } else {
            // type not supported yet
            SAWYER_MESG(logger[WARN])<<"aggregate initializer: unsupported type at: "<<SgNodeHelper::sourceFilenameLineColumnToString(decl)<<" : "<<aggregateInitializer->get_type()->unparseToString()<<endl;
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
              setElementSize(initDeclVarId,variableType); // this must be a pointer, if it's not an array
              PState newPState=*currentEState.pstate();
              initializeStringLiteralInState(newPState,stringValNode,initDeclVarId);
              ConstraintSet cset=*currentEState.constraints();
              return createEState(targetLabel,cs,newPState,cset);
            }
          }
          // set type info for initDeclVarId
          variableIdMapping->setNumberOfElements(initDeclVarId,1); // single variable
          SgType* variableType=initializer->get_type();
          setElementSize(initDeclVarId,variableType);

          // build lhs-value dependent on type of declared variable
          //AbstractValue lhsAbstractAddress=AbstractValue(initDeclVarId); // creates a pointer to initDeclVar
          AbstractValue lhsAbstractAddress=AbstractValue::createAddressOfVariable(initDeclVarId); // creates a pointer to initDeclVar
          list<SingleEvalResultConstInt> res=exprAnalyzer.evaluateExpression(rhs,currentEState);
          if(res.size()!=1) {
            if(res.size()>1) {
              SAWYER_MESG(logger[ERROR])<<"Error: multiple results in rhs evaluation."<<endl;
              SAWYER_MESG(logger[ERROR])<<"expr: "<<SgNodeHelper::sourceLineColumnToString(decl)<<": "<<decl->unparseToString()<<endl;
              exit(1);
            } else {
              ROSE_ASSERT(res.size()==0);
              SAWYER_MESG(logger[TRACE])<<"no results in rhs evaluation (returning top): "<<rhs->unparseToString()<<endl;
              EState estate=currentEState;
              PState newPState=*estate.pstate();
              // getExprAnalyzer()->writeToMemoryLocation(label,&newPState,lhsAbstractAddress,CodeThorn::Top());
              getExprAnalyzer()->reserveMemoryLocation(label,&newPState,lhsAbstractAddress);
              ConstraintSet cset=*estate.constraints();
              return createEState(targetLabel,cs,newPState,cset);
            }
          }
          ROSE_ASSERT(res.size()==1);
          SingleEvalResultConstInt evalResult=*res.begin();
          SAWYER_MESG(logger[TRACE])<<"rhs eval result: "<<evalResult.result.toString()<<endl;

          EState estate=evalResult.estate;
          PState newPState=*estate.pstate();
          getExprAnalyzer()->initializeMemoryLocation(label,&newPState,lhsAbstractAddress,evalResult.value());
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
              if(getAbstractionMode()==3) {
                CodeThorn::Exception("multi-dimensional arrays not supported yet.");
              } else {
                SAWYER_MESG(logger[WARN])<<"multi-dimensional arrays not supported yet. Only linear arrays are supported. Not added to state (assuming arbitrary value)."<<endl;
              }
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
              if(getAbstractionMode()==3) {
                CodeThorn::Exception("Could not determine size of array (non-const size).");
              }
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
          //cout<<"DEBUG: DECLARING ARRAY: size: "<<decl->unparseToString()<<length<<endl;
          for(size_t elemIndex=0;elemIndex<length;elemIndex++) {
            AbstractValue newArrayElementAddr=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex));
            // set default init value
            getExprAnalyzer()->reserveMemoryLocation(label,&newPState,newArrayElementAddr);
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
          // TODO: STRUCT VARIABLE DECLARATION
          getExprAnalyzer()->reserveMemoryLocation(label,&newPState,pointerVal);
        } else if(variableIdMapping->hasPointerType(initDeclVarId)) {
          // create pointer value and set it to top (=any value possible (uninitialized pointer variable declaration))
          AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
          getExprAnalyzer()->writeUndefToMemoryLocation(&newPState,pointerVal);
        } else {
          // set it to top (=any value possible (uninitialized)) for
          // all remaining cases. It will become an error-path once
          // all cases are addressed explicitly above.
          SAWYER_MESG(logger[TRACE])<<"declaration of variable (other): "<<variableIdMapping->getVariableDeclaration(initDeclVarId)->unparseToString()<<endl;
          getExprAnalyzer()->reserveMemoryLocation(label,&newPState,AbstractValue::createAddressOfVariable(initDeclVarId));
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

void CodeThorn::Analyzer::recordAnalyzedFunction(SgFunctionDefinition* funDef) {
#pragma omp critical(functionrecording)
  {
    analyzedFunctions.insert(funDef);
  }
}

std::string CodeThorn::Analyzer::analyzedFunctionsToString() {
  ostringstream ss;
  for (auto funDef : analyzedFunctions)  {
    ss<<SgNodeHelper::getFunctionName(funDef)
      <<","
      <<SgNodeHelper::sourceLineColumnToString(funDef)
      <<","
      <<SgNodeHelper::sourceFilenameToString(funDef)
      <<endl;
  }
  return ss.str();
}

std::string CodeThorn::Analyzer::analyzedFilesToString() {
  unordered_set<string> fileNameSet;
  for (auto funDef : analyzedFunctions)  {
    fileNameSet.insert(SgNodeHelper::sourceFilenameToString(funDef));
  }
  ostringstream ss;
  for (auto fn : fileNameSet)  {
    ss<<fn<<endl;
  }
  return ss.str();
}

std::string CodeThorn::Analyzer::externalFunctionsToString() {
  ostringstream ss;
  for (auto funCall : externalFunctions)  {
    ss<<SgNodeHelper::getFunctionName(funCall)<<endl;
  }
  return ss.str();
}

void CodeThorn::Analyzer::recordExternalFunctionCall(SgFunctionCallExp* funCall) {
#pragma omp critical(funcallrecording)
  {
    externalFunctions.insert(funCall);
  }
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
  } else if(edge.isType(EDGE_CALL) && SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
    return transferFunctionCall(edge,estate);
  } else if(edge.isType(EDGE_EXTERNAL) && SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
    // \todo the && condition excludes constructor calls
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
    exit(1);
    //bool useConstraints=false
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
  exprAnalyzer.setVariableIdMapping(getVariableIdMapping());
  AbstractValue::setVariableIdMapping(getVariableIdMapping());
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
    AbstractValue address=AbstractValue::createAddressOfVariable(varId);
    initialPState.writeTopToMemoryLocation(address);
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

void CodeThorn::Analyzer::startAnalysisTimer() {
  if (!_timerRunning) {
    _analysisTimer.start();
    _timerRunning=true;
    SAWYER_MESG(logger[INFO])<<"INFO: solver timer started."<<endl;
  }
}

void CodeThorn::Analyzer::stopAnalysisTimer() {
  _timerRunning=false;
  _analysisTimer.stop();
  SAWYER_MESG(logger[INFO])<<"INFO: solver timer stopped."<<endl;
}

void CodeThorn::Analyzer::initializeSolver(std::string functionToStartAt,SgNode* root, bool oneFunctionOnly) {
  startAnalysisTimer();
  ROSE_ASSERT(root);
  resetInputSequenceIterator();
  std::string funtofind=functionToStartAt;
  RoseAst completeast(root);
  startFunRoot=completeast.findFunctionByName(funtofind);
  if(startFunRoot==0) {
    SAWYER_MESG(logger[ERROR]) << "Function '"<<funtofind<<"' not found.\n";
    exit(1);
  } else {
    SAWYER_MESG(logger[INFO])<< "starting at function '"<<funtofind<<"'."<<endl;
  }
  SAWYER_MESG(logger[TRACE])<< "INIT: Initializing AST node info."<<endl;
  initAstNodeInfo(root);

  SAWYER_MESG(logger[TRACE])<< "INIT: Creating Labeler."<<endl;
  Labeler* labeler= new CTIOLabeler(root,getVariableIdMapping());
  //SAWYER_MESG(logger[TRACE])<< "INIT: Initializing VariableIdMapping."<<endl;
  //exprAnalyzer.setVariableIdMapping(getVariableIdMapping());
  SAWYER_MESG(logger[TRACE])<< "INIT: Creating CFAnalysis."<<endl;
  cfanalyzer=new CFAnalysis(labeler,true);
  
  getFunctionCallMapping2()->setLabeler(labeler);

  if (SgProject* prj = isSgProject(root))
  {
    ClassHierarchyWrapper* chw = new ClassHierarchyWrapper(prj);
    
    if (SgNodeHelper::WITH_EXTENDED_NORMALIZED_CALL)
    { 
      getFunctionCallMapping2()->setClassHierarchy(chw);
      getFunctionCallMapping2()->computeFunctionCallMapping(prj);
    }
    
    getFunctionCallMapping()->setClassHierarchy(new ClassHierarchyWrapper(*chw));
  }
  else
    SAWYER_MESG(logger[WARN])<< "WARN: Need a SgProject object for building the class hierarchy\n"
                             << "      virtual function call analysis not available!"
                             << std::endl;

  //FunctionIdMapping* funIdMapping=new FunctionIdMapping();
  //ROSE_ASSERT(isSgProject(root));
  //funIdMapping->computeFunctionSymbolMapping(isSgProject(root));
  //cfanalyzer->setFunctionIdMapping(funIdMapping);
  cfanalyzer->setFunctionIdMapping(getFunctionIdMapping());
  cfanalyzer->setFunctionCallMapping(getFunctionCallMapping());
  cfanalyzer->setFunctionCallMapping2(getFunctionCallMapping2());

  getLabeler()->setExternalNonDetIntFunctionName(_externalNonDetIntFunctionName);
  getLabeler()->setExternalNonDetLongFunctionName(_externalNonDetLongFunctionName);


  // logger[DEBUG]<< "mappingLabelToLabelProperty: "<<endl<<getLabeler()->toString()<<endl;
  SAWYER_MESG(logger[INFO])<< "INIT: Building CFGs."<<endl;

  flow=cfanalyzer->flow(root);
  Label slab=getLabeler()->getLabel(startFunRoot);
  ROSE_ASSERT(slab.isValid());
  flow.setStartLabel(slab);
  // Runs consistency checks on the fork / join and workshare / barrier nodes in the parallel CFG
  // If the --omp-ast flag is not selected by the user, the parallel nodes are not inserted into the CFG
  if (_ctOpt.ompAst) {
    cfanalyzer->forkJoinConsistencyChecks(flow);
  }

  SAWYER_MESG(logger[INFO])<< "STATUS: Building CFGs finished."<<endl;
  if(_ctOpt.reduceCfg) {
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
  SAWYER_MESG(logger[INFO])<< "INIT: ICFG OK. (size: " << flow.size() << " edges)"<<endl;

#if 0
  if(_ctOpt.reduceCfg) {
    int cnt=cfanalyzer->inlineTrivialFunctions(flow);
    cout << "INIT: CFG reduction OK. (inlined "<<cnt<<" functions; eliminated "<<cnt*4<<" nodes)"<<endl;
  }
#endif

  // create empty state
  PState initialPState;
  initializeCommandLineArgumentsInState(initialPState);
  if(optionStringLiteralsInState) {
    initializeStringLiteralsInState(initialPState);
    if(_ctOpt.status) {
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
    VariableIdSet setOfUsedVars=AstUtility::usedVariablesInsideFunctions(project,variableIdMapping);
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

  setWorkLists(_explorationMode);
  estate.io.recordNone(); // ensure that extremal value is different to bot
  const EState* initialEState=processNew(estate);
  ROSE_ASSERT(initialEState);
  variableValueMonitor.init(initialEState);
  addToWorkList(initialEState);
  SAWYER_MESG(logger[TRACE]) << "INIT: start state (extremal value): "<<initialEState->toString(variableIdMapping)<<endl;

  // initialize summary states map for abstract model checking mode
  initializeSummaryStates(initialPStateStored,emptycsetstored);

  if(_ctOpt.rers.rersBinary) {
    //initialize the global variable arrays in the linked binary version of the RERS problem
    SAWYER_MESG(logger[DEBUG])<< "init of globals with arrays for "<< _numberOfThreadsToUse << " threads. " << endl;
    RERS_Problem::rersGlobalVarsArrayInitFP(_numberOfThreadsToUse);
    RERS_Problem::createGlobalVarAddressMapsFP(this);
  }

  SAWYER_MESG(logger[TRACE])<< "INIT: finished."<<endl;
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

Label CodeThorn::Analyzer::getFunctionEntryLabel(SgFunctionRefExp* funRefExp) {
  Label lab;
  return lab;
}


/*!
 * \author Marc Jasper
 * \date 2017.
 */
// MS 05/31/2020: this function is not used anywhere
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
  if(_ctOpt.rers.rersBinary) {
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

void CodeThorn::Analyzer::setSkipUnknownFunctionCalls(bool flag) {
  _skipSelectedFunctionCalls=flag; 
  exprAnalyzer.setSkipUnknownFunctionCalls(flag);
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
    result = (long) (_analysisTimer.getTimeDurationAndKeepRunning().seconds());
  }
  return result;
}

void CodeThorn::Analyzer::setCFAnalyzer(CFAnalysis* cf) { cfanalyzer=cf; }
CodeThorn::CFAnalysis* CodeThorn::Analyzer::getCFAnalyzer() const { return cfanalyzer; }

VariableIdMappingExtended* CodeThorn::Analyzer::getVariableIdMapping() { return variableIdMapping; }
CodeThorn::FunctionIdMapping* CodeThorn::Analyzer::getFunctionIdMapping() { return &functionIdMapping; }
CodeThorn::FunctionCallMapping* CodeThorn::Analyzer::getFunctionCallMapping() { return &functionCallMapping; }
CodeThorn::FunctionCallMapping2* CodeThorn::Analyzer::getFunctionCallMapping2() { return &functionCallMapping2; }
CodeThorn::Flow* CodeThorn::Analyzer::getFlow() { return &flow; }
CodeThorn::EStateSet* CodeThorn::Analyzer::getEStateSet() { return &estateSet; }
CodeThorn::PStateSet* CodeThorn::Analyzer::getPStateSet() { return &pstateSet; }
TransitionGraph* CodeThorn::Analyzer::getTransitionGraph() { return &transitionGraph; }
ConstraintSetMaintainer* CodeThorn::Analyzer::getConstraintSetMaintainer() { return &constraintSetMaintainer; }
std::list<CodeThorn::FailedAssertion> CodeThorn::Analyzer::getFirstAssertionOccurences(){return _firstAssertionOccurences;}

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

void CodeThorn::Analyzer::setOptions(CodeThornOptions options) {
  _ctOpt=options;
}

CodeThornOptions& CodeThorn::Analyzer::getOptionsRef() {
  return _ctOpt;
}

void CodeThorn::Analyzer::setLtlOptions(LTLOptions ltlOptions) {
  _ltlOpt=ltlOptions;
}

LTLOptions& CodeThorn::Analyzer::getLtlOptionsRef() {
  return _ltlOpt;
}

// value semantics for upates
CallString CodeThorn::Analyzer::transferFunctionCallContext(CallString cs, Label lab) {
  SAWYER_MESG(logger[TRACE])<<"FunctionCallTransfer: adding "<<lab.toString()<<" to cs: "<<cs.toString()<<endl;
  cs.addLabel(lab);
  return cs;
}
bool CodeThorn::Analyzer::isFeasiblePathContext(CallString& cs,Label lab) {
  return cs.getLength()==0||cs.isLastLabel(lab);
}

//
// utility functions
//

AbstractValue CodeThorn::Analyzer::singleValevaluateExpression(SgExpression* expr,EState currentEState) {
  list<SingleEvalResultConstInt> resultList=exprAnalyzer.evaluateExpression(expr,currentEState);
  ROSE_ASSERT(resultList.size()==1);
  SingleEvalResultConstInt valueResult=*resultList.begin();
  AbstractValue val=valueResult.result;
  return val;
}

CodeThorn::Analyzer::MemoryUpdateList
CodeThorn::Analyzer::evalAssignOp(SgAssignOp* nextNodeToAnalyze2, Edge edge, const EState* estatePtr) {
  MemoryUpdateList memoryUpdateList;
  CallString cs=estatePtr->callString;
  //cout<<"DEBUG: AssignOp: "<<nextNodeToAnalyze2->unparseToString()<<endl;
  EState currentEState=*estatePtr;
  SgNode* lhs=SgNodeHelper::getLhs(nextNodeToAnalyze2);
  SgNode* rhs=SgNodeHelper::getRhs(nextNodeToAnalyze2);
  list<SingleEvalResultConstInt> res=exprAnalyzer.evaluateExpression(rhs,currentEState, CodeThorn::ExprAnalyzer::MODE_VALUE);
  //list<EState> estateList;
  for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
    VariableId lhsVar;
    bool isLhsVar=exprAnalyzer.checkIfVariableAndDetermineVarId(lhs,lhsVar);
    if(isLhsVar) {
      EState estate=(*i).estate;
      if(variableIdMapping->hasClassType(lhsVar)) {
        // assignments to struct variables are not supported yet (this test does not detect s1.s2 (where s2 is a struct, see below)).
        logger[WARN]<<"assignment of structs (copy constructor) is not supported yet. Target update ignored! (unsound)"<<endl;
      } else if(variableIdMapping->hasCharType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(variableIdMapping->hasIntegerType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(variableIdMapping->hasEnumType(lhsVar)) /* PP */ {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(variableIdMapping->hasFloatingPointType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(variableIdMapping->hasBoolType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(variableIdMapping->hasPointerType(lhsVar)) {
        // assume here that only arrays (pointers to arrays) are assigned
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else if(variableIdMapping->hasReferenceType(lhsVar)) {
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsVar,(*i).result)));
      } else {
        SAWYER_MESG(logger[ERROR])<<"Error at "<<SgNodeHelper::sourceFilenameLineColumnToString(nextNodeToAnalyze2)<<endl;
        SAWYER_MESG(logger[ERROR])<<"Unsupported type on LHS side of assignment: "
                                  <<"type lhs: '"<<variableIdMapping->getType(lhsVar)->unparseToString()<<"'"
                                  <<", "
                                  <<"AST type node: "<<variableIdMapping->getType(lhsVar)->class_name()
                                  <<", Expr:"<<nextNodeToAnalyze2->unparseToString()
                                  <<endl;
        exit(1);
      }
    } else if(isSgDotExp(lhs)) {
      SAWYER_MESG(logger[TRACE])<<"detected dot operator on lhs "<<lhs->unparseToString()<<"."<<endl;
      list<SingleEvalResultConstInt> lhsRes=exprAnalyzer.evaluateExpression(lhs,currentEState, CodeThorn::ExprAnalyzer::MODE_ADDRESS);
      for (auto lhsAddressResult : lhsRes) {
        EState estate=(*i).estate;
        AbstractValue lhsAddress=lhsAddressResult.result;
        SAWYER_MESG(logger[TRACE])<<"detected dot operator on lhs: writing to "<<lhsAddress.toString(getVariableIdMapping())<<"."<<endl;
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsAddress,(*i).result)));
      }
    } else if(isSgArrowExp(lhs)) {
      SAWYER_MESG(logger[TRACE])<<"detected arrow operator on lhs "<<lhs->unparseToString()<<"."<<endl;
      list<SingleEvalResultConstInt> lhsRes=exprAnalyzer.evaluateExpression(lhs,currentEState, CodeThorn::ExprAnalyzer::MODE_ADDRESS);
      for (auto lhsAddressResult : lhsRes) {
        EState estate=(*i).estate;
        AbstractValue lhsAddress=lhsAddressResult.result;
        SAWYER_MESG(logger[TRACE])<<"detected arrow operator on lhs: writing to "<<lhsAddress.toString(getVariableIdMapping())<<"."<<endl;
        memoryUpdateList.push_back(make_pair(estate,make_pair(lhsAddress,(*i).result)));
      }
    } else if(isSgPntrArrRefExp(lhs)) {
      VariableIdMapping* _variableIdMapping=variableIdMapping;
      EState estate=(*i).estate;
      PState oldPState=*estate.pstate();
      if(getSkipArrayAccesses()) {
        // estateList.push_back(createEState(edge.target(),cs,oldPState,oldcset));
        // nothing to do (no memory access to add to list)
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
          list<SingleEvalResultConstInt> res=exprAnalyzer.evaluateExpression(indexExp,currentEState);
          ROSE_ASSERT(res.size()==1); // this should always hold for normalized ASTs
          AbstractValue indexValue=(*(res.begin())).value();
          AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrayPtrValue,indexValue);
          AbstractValue arrayElementAddr=arrayPtrPlusIndexValue;
          //cout<<"DEBUG: arrayElementId: "<<arrayElementId.toString(_variableIdMapping)<<endl;
          //SAWYER_MESG(logger[TRACE])<<"arrayElementVarId:"<<arrayElementId.toString()<<":"<<_variableIdMapping->variableName(arrayVarId)<<" Index:"<<index<<endl;
          if(arrayElementAddr.isBot()) {
            // inaccessible memory location, return empty estate list
            return memoryUpdateList;
          }
          // read value of variable var id (same as for VarRefExp - TODO: reuse)
          // TODO: check whether arrayElementId (or array) is a constant array (arrayVarId)
          //TODO: getExprAnalyzer()->writeToMemoryLocation(label,&pstate2,arrayElementId,(*i).value()); // *i is assignment-rhs evaluation result
          memoryUpdateList.push_back(make_pair(estate,make_pair(arrayElementAddr,(*i).result)));
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
        getExprAnalyzer()->recordDefinitiveNullPointerDereferenceLocation(estatePtr->label());
        // no state can follow, return estateList (may be empty)
        // TODO: create error state here?
        //return estateList;
        return memoryUpdateList;
      } else if(lhsPointerValue.isTop()) {
        getExprAnalyzer()->recordPotentialNullPointerDereferenceLocation(estatePtr->label());
        // specific case a pointer expr evaluates to top. Dereference operation
        // potentially modifies any memory location in the state.
        AbstractValue lhsAddress=lhsPointerValue;
        memoryUpdateList.push_back(make_pair(*estatePtr,make_pair(lhsAddress,(*i).result))); // lhsAddress is TOP!!!
      } else if(!(lhsPointerValue.isPtr())) {
        // changed isUndefined to isTop (2/17/20)
        if(lhsPointerValue.isTop() && getIgnoreUndefinedDereference()) {
          //cout<<"DEBUG: lhsPointerValue:"<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
          // skip write access, just create new state (no effect)
          // nothing to do, as it request to not update the state in this case
        } else {
          SAWYER_MESG(logger[WARN])<<"not a pointer value (or top) in dereference operator: lhs-value:"<<lhsPointerValue.toLhsString(getVariableIdMapping())<<" lhs: "<<lhs->unparseToString()<<" : assuming change of any memory location."<<endl;
          getExprAnalyzer()->recordPotentialNullPointerDereferenceLocation(estatePtr->label());
          // specific case a pointer expr evaluates to top. Dereference operation
          // potentially modifies any memory location in the state.
          // iterate over all elements of the state and merge with rhs value
          memoryUpdateList.push_back(make_pair(*estatePtr,make_pair(lhsPointerValue,(*i).result))); // lhsPointerValue is TOP!!!
        }
      } else {
        //cout<<"DEBUG: lhsPointerValue:"<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
        memoryUpdateList.push_back(make_pair(*estatePtr,make_pair(lhsPointerValue,(*i).result)));
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
  return memoryUpdateList;
}

//
// wrapper functions to follow
//

std::list<EState> CodeThorn::Analyzer::elistify() {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->elistify();
}

std::list<EState> CodeThorn::Analyzer::elistify(EState res) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->elistify(res);
}

std::list<EState> CodeThorn::Analyzer::transferFunctionCallLocalEdge(Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferFunctionCallLocalEdge(edge,estate);
}

std::list<EState> CodeThorn::Analyzer::transferFunctionCall(Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferFunctionCall(edge,estate);
}

std::list<EState> CodeThorn::Analyzer::transferReturnStmt(Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferReturnStmt(edge,estate);
}

std::list<EState> CodeThorn::Analyzer::transferFunctionCallReturn(Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferFunctionCallReturn(edge,estate);
}
std::list<EState> CodeThorn::Analyzer::transferFunctionExit(Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferFunctionExit(edge,estate);
}

std::list<EState> CodeThorn::Analyzer::transferFunctionCallExternal(Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferFunctionCallExternal(edge,estate);
}

std::list<EState> CodeThorn::Analyzer::transferDefaultOptionStmt(SgDefaultOptionStmt* defaultStmt,Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferDefaultOptionStmt(defaultStmt,edge,estate);
}

std::list<EState> CodeThorn::Analyzer::transferCaseOptionStmt(SgCaseOptionStmt* caseStmt,Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferCaseOptionStmt(caseStmt,edge,estate);
}

std::list<EState> CodeThorn::Analyzer::transferVariableDeclaration(SgVariableDeclaration* decl, Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferVariableDeclaration(decl,edge,estate);
}

std::list<EState> CodeThorn::Analyzer::transferExprStmt(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferExprStmt(nextNodeToAnalyze1,edge,estate);
}

list<EState> CodeThorn::Analyzer::transferIncDecOp(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferIncDecOp(nextNodeToAnalyze2,edge,estate);
}

list<EState> CodeThorn::Analyzer::transferIdentity(Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferIdentity(edge,estate);
}

std::list<EState> CodeThorn::Analyzer::transferAssignOp(SgAssignOp* nextNodeToAnalyze2, Edge edge, const EState* estate) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->transferAssignOp(nextNodeToAnalyze2,edge,estate);
}

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
      SAWYER_MESG(logger[ERROR])<<"PSTATE: "<<estate->pstate()->toString(getVariableIdMapping())<<endl;
      SAWYER_MESG(logger[ERROR])<<"Error: CONDITION EVALUATES TO BOT : "<<nextNodeToAnalyze2->unparseToString()<<endl;
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

void CodeThorn::Analyzer::configureOptionSets(CodeThornOptions& ctOpt) {
  string optionName="options-set";  // only used for error reporting
  int optionValue=ctOpt.optionsSet; // only used for error reporting
  switch(optionValue) {
  case 0:
    // fall-through for default
    break;
  case 1:
    ctOpt.arraysNotInState=false;
    ctOpt.inStateStringLiterals=true;
    ctOpt.ignoreUnknownFunctions=true;
    ctOpt.ignoreFunctionPointers=true;
    ctOpt.stdFunctions=true;
    ctOpt.contextSensitive=true;
    ctOpt.normalizeAll=true;
    ctOpt.abstractionMode=1;
    AbstractValue::strictChecking=false;
    break;
  case 2:
    ctOpt.arraysNotInState=false;
    ctOpt.inStateStringLiterals=true;
    ctOpt.ignoreUnknownFunctions=true;
    ctOpt.ignoreFunctionPointers=false;
    ctOpt.stdFunctions=true;
    ctOpt.contextSensitive=true;
    ctOpt.normalizeAll=true;
    ctOpt.abstractionMode=1;
    AbstractValue::strictChecking=false;
    break;
  case 3:
    ctOpt.arraysNotInState=false;
    ctOpt.inStateStringLiterals=false;
    ctOpt.ignoreUnknownFunctions=true;
    ctOpt.ignoreFunctionPointers=false;
    ctOpt.stdFunctions=false;
    ctOpt.contextSensitive=true;
    ctOpt.normalizeAll=true;
    ctOpt.abstractionMode=1;
    AbstractValue::strictChecking=false;
    break;
  case 4:
    ctOpt.arraysNotInState=false;
    ctOpt.inStateStringLiterals=false;
    ctOpt.ignoreUnknownFunctions=true;
    ctOpt.ignoreFunctionPointers=false;
    ctOpt.stdFunctions=false;
    ctOpt.contextSensitive=true;
    ctOpt.normalizeAll=true;
    ctOpt.abstractionMode=1;
    AbstractValue::strictChecking=true;
    break;
  case 11:
    ctOpt.arraysNotInState=false;
    ctOpt.inStateStringLiterals=true;
    ctOpt.ignoreUnknownFunctions=true;
    ctOpt.ignoreFunctionPointers=false;
    ctOpt.stdFunctions=false;
    ctOpt.contextSensitive=true;
    ctOpt.normalizeAll=true;
    ctOpt.abstractionMode=0;
    break;
  default:
    cerr<<"Error: unsupported "<<optionName<<" value: "<<optionValue<<endl;
    exit(1);
  }
}

void CodeThorn::Analyzer::setFunctionResolutionModeInCFAnalysis(CodeThornOptions& ctOpt) {
  switch(int argVal=ctOpt.functionResolutionMode) {
  case 1: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_TRANSLATION_UNIT;break;
  case 2: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_WHOLE_AST_LOOKUP;break;
  case 3: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_FUNCTION_ID_MAPPING;break;
  case 4: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_FUNCTION_CALL_MAPPING;break;
  default: 
    cerr<<"Error: unsupported argument value of "<<argVal<<" for function-resolution-mode.";
    exit(1);
  }
}
