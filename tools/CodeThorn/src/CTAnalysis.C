#include "sage3basic.h"
#include "Labeler.h"
#include "ClassHierarchyGraph.h"
#include "CTAnalysis.h"
#include "CodeThornCommandLineOptions.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "AstUtility.h"
#include "SvcompWitness.h"
#include "CodeThornException.h"

#include <unordered_set>
//~ #include <boost/bind.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "TimeMeasurement.h"
#include "CollectionOperators.h"
#include "RersSpecialization.h"
#include "RERS_empty_specialization.h"
#include "CodeThornLib.h"
#include "TopologicalSort.h"
#include "Miscellaneous2.h"
#include "CodeThornPasses.h"
#include "Solver18.h"

using namespace std;
using namespace Sawyer::Message;
using namespace CodeThorn;

Sawyer::Message::Facility CodeThorn::CTAnalysis::logger;

CodeThorn::CTAnalysis::CTAnalysis():
  _startFunRoot(0),
  //cfanalyzer(0),
  _globalTopifyMode(GTM_IO),
  _stgReducer(&estateSet, &transitionGraph),
  _counterexampleGenerator(&transitionGraph),
  _resourceLimitDiff(10000),
  //  _solver(nullptr),
  _analyzerMode(AM_ALL_STATES),
  _maxTransitions(-1),
  _maxIterations(-1),
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
  for(int i=0;i<100;i++) {
    binaryBindingAssert.push_back(false);
  }
  estateSet.max_load_factor(0.7);
  pstateSet.max_load_factor(0.7);
  //constraintSetMaintainer.max_load_factor(0.7);
  resetInputSequenceIterator();
  ROSE_ASSERT(_estateTransferFunctions==nullptr);
  _estateTransferFunctions=new EStateTransferFunctions();
  _estateTransferFunctions->setAnalyzer(this);
}

CodeThorn::CTAnalysis::~CTAnalysis() {
  if(_estateTransferFunctions)
    delete _estateTransferFunctions;
  deleteWorkLists();
  deleteAllStates();
}

std::string CodeThorn::CTAnalysis::hashSetConsistencyReport() {
  stringstream ss;

  {
    ss<<"HashSet consistency: estateSet: ";
    auto estateWarnings=estateSet.getWarnings();
    if(estateWarnings>0)
      ss<<"FAIL("<<estateWarnings<<")";
    else
      ss<<"PASS";
  }
  {
    ss<<", pstateSet: ";
    auto pstateWarnings=pstateSet.getWarnings();
    if(pstateWarnings>0)
      ss<<"FAIL("<<pstateWarnings<<")";
    else
      ss<<"PASS";
  }

  {
    ss<<", transitionGraphSet: ";
    auto transitionGraphWarnings=transitionGraph.getWarnings();
    if(transitionGraphWarnings>0)
      ss<<"FAIL("<<transitionGraphWarnings<<")";
    else
      ss<<"PASS";
    ss<<endl;
  }

  return ss.str();
}

void CodeThorn::CTAnalysis::deleteAllStates() {

  if(getSolver()->getId()==18) {
    dynamic_cast<Solver18*>(getSolver())->deleteAllStates();
    return;
  }

  if(_ctOpt.status) {
    cout<<"STATUS: "<<hashSetConsistencyReport();
  }

  EState::checkPointAllocationHistory();
  size_t numSetEStates=estateSet.size();
  if(_ctOpt.status) cout<<"STATUS: deleting "<<estateSet.size()<<" estates, ";
  for(auto es : estateSet) {
    delete es;
  }
  estateSet.clear();
  size_t numSetPStates=pstateSet.size();
  if(_ctOpt.status) cout<<pstateSet.size()<<" pstates, ";
  for(auto ps : pstateSet) {
    delete ps;
  }
  pstateSet.clear();

  EState::checkPointAllocationHistory();

  if(_ctOpt.status) cout<<transitionGraph.size()<<" transitions in TS."<<endl;
  transitionGraph.clear();

  // check is necessary for mixed modes. If states already deleted above, they cannot be deleted again
  if(numSetEStates==0 && numSetPStates==0 && _abstractCSStateMapMap.size()>0) {
    size_t cnt=0;
    for(auto entry : _abstractCSStateMapMap) {
      auto map=entry.second;
      for(auto entry2 : map) {
        delete entry2.second;
      cnt++;
      }
    }
    if(_ctOpt.status) cout<<"STATUS: deleted "<<cnt<<" CS-estates."<<endl;
  }
}

void CodeThorn::CTAnalysis::run() {
  runSolver();
}

TopologicalSort* CodeThorn::CTAnalysis::getTopologicalSort() {
  return _topologicalSort;
}

void CodeThorn::CTAnalysis::ensureToplogicSortFlowConsistency() {
  auto l2pMap=getTopologicalSort()->labelToPriorityMap();
  LabelSet allLabels=getFlow()->nodeLabels();
  uint64_t numFail=0;
  for(auto lab : allLabels) {
    if(!lab.isValid()) {
      cerr<<"Error: checkToplogicSortFlowConsistency failed: invalid label."<<endl;
      numFail++;
    }
    if(l2pMap[lab]==0) {
      SgNode* node=getLabeler()->getNode(lab);
      cerr<<"Error: checkToplogicSortFlowConsistency failed: flow-node not in topological sort: L"<<lab<<": "<<SgNodeHelper::locationAndSourceCodeToString(node,25,40)<<endl;
      numFail++;
    }
  }
  if(numFail>0) {
    cerr<<"Error: checkToplogicSortFlowConsistency: "<<numFail<<" inconsistencies detected. Exiting."<<endl;
    exit(1);
  }
  if(_ctOpt.status) {
    cout<<"INFO: ensureToplogicSortFlowConsistency: PASS"<<endl;
  }
}

bool CodeThorn::CTAnalysis::isPassThroughLabel(Label lab) {
  ROSE_ASSERT(getLabeler());
  return getFlow()->isPassThroughLabel(lab,getLabeler());
}

void CodeThorn::CTAnalysis::insertInputVarValue(int i) {
  _inputVarValues.insert(i);
}

std::set<int> CodeThorn::CTAnalysis::getInputVarValues() {
  return _inputVarValues;
}

// also sets inputvarvalues
void CodeThorn::CTAnalysis::setLtlRersMapping(CodeThorn::LtlRersMapping m) {
  _ltlRersMapping=m;
}
CodeThorn::LtlRersMapping CodeThorn::CTAnalysis::getLtlRersMapping() {
  return _ltlRersMapping;
}

void CodeThorn::CTAnalysis::deleteWorkLists() {
  if(estateWorkListCurrent) {
    delete estateWorkListCurrent;
  }
  if(estateWorkListNext) {
    delete estateWorkListNext;
  }
}

void CodeThorn::CTAnalysis::setWorkLists(ExplorationMode explorationMode) {
  deleteWorkLists();
  switch(_explorationMode) {
  case EXPL_UNDEFINED:
    cerr<<"Error: undefined exploration mode in constructing of CTAnalysis."<<endl;
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
    auto sLabelSetSize=getFlow()->getStartLabelSet().size();
    if(sLabelSetSize>0) {
      if(_ctOpt.status) cout<<"STATUS: creating topologic sort of "<<getFlow()->size()<<" labels ... "<<flush;
      auto cg=getCFAnalyzer()->getCallGraph();
      _topologicalSort=new TopologicalSort(*getLabeler(),*getFlow(),cg);
      _topologicalSort->setReverseFunctionOrdering(true);
      ROSE_ASSERT(_topologicalSort);
      _topologicalSort->computeLabelToPriorityMap();
#if 0
      std::list<Label> labelList=_topologicalSort.topologicallySortedLabelList();
      cout<<"Topologic Sort:";
      for(auto label : labelList) {
        cout<<label.toString()<<" ";
      }
      cout<<endl;
#endif
      ROSE_ASSERT(_topologicalSort);
      TopologicalSort::LabelToPriorityMap map=_topologicalSort->labelToPriorityMap();
      if(_ctOpt.status) cout<<"done."<<endl;

      ROSE_ASSERT(map.size()>0);
      estateWorkListCurrent = new EStatePriorityWorkList(map);
      estateWorkListNext = new EStatePriorityWorkList(map); // only used in loop aware mode
      SAWYER_MESG(logger[INFO])<<"STATUS: using topologic worklist."<<endl;
      break;
    } else {
      // for empty flow initialize with empty workslists
      TopologicalSort::LabelToPriorityMap map;
      estateWorkListCurrent = new EStatePriorityWorkList(map);
      estateWorkListNext = new EStatePriorityWorkList(map); // only used in loop aware mode
      if(_ctOpt.status) cout<<"STATUS: using empty topologic worklist (because of empty control flow graph)."<<endl;
    }
  }
  } // end switch
}

void CodeThorn::CTAnalysis::setExplorationMode(ExplorationMode em) {
  _explorationMode=em;
}

ExplorationMode CodeThorn::CTAnalysis::getExplorationMode() {
  return _explorationMode;
}

CodeThorn::CTAnalysis::SubSolverResultType CodeThorn::CTAnalysis::subSolver(EStatePtr currentEStatePtr) {
  // start the timer if not yet done
  startAnalysisTimer();

  // first, check size of global EStateSet and print status or switch to topify/terminate analysis accordingly.
  unsigned long estateSetSize;
  bool earlyTermination = false;
  int threadNum = 0; //subSolver currently does not support multiple threads.
  // print status message if required
  if (_ctOpt.status && _ctOpt.displayDiff) {
#pragma omp critical(HASHSET)
    {
      estateSetSize = estateSet.size();
    }
    if(threadNum==0 && (estateSetSize>(_prevStateSetSizeDisplay+_ctOpt.displayDiff))) {
      printStatusMessage(true);
      _prevStateSetSizeDisplay=estateSetSize;
    }
  }
  // switch to topify mode or terminate analysis if resource limits are exceeded
  if (_ctOpt.maxMemory != -1 || _maxBytesForcedTop != -1 || _ctOpt.maxTime != -1 || _maxSecondsForcedTop != -1
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
  std::set<EStatePtr> existingEStateSet;
  if (earlyTermination) {
    if(_ctOpt.status) {
      SAWYER_MESG(logger[INFO]) << "STATUS: Early termination within subSolver (resource limit reached)." << endl;
    }
    transitionGraph.setForceQuitExploration(true);
  } else {
    // run the actual sub-solver
    EStateWorkList localWorkList;
    localWorkList.push_back(currentEStatePtr);
    while(!localWorkList.empty()) {
      // logger[DEBUG]<<"local work list size: "<<localWorkList.size()<<endl;
      EStatePtr currentEStatePtr=localWorkList.front();
      localWorkList.pop_front();
      if(isFailedAssertEState(currentEStatePtr)) {
        // ensure we do not compute any successors of a failed assert state
        continue;
      }
      Flow edgeSet=getFlow()->outEdges(currentEStatePtr->label());
      for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
        Edge e=*i;
        list<EStatePtr> newEStateList;
        newEStateList=transferEdgeEState(e,currentEStatePtr);
        for(list<EStatePtr>::iterator nesListIter=newEStateList.begin();
            nesListIter!=newEStateList.end();
            ++nesListIter) {
          // newEstate is passed by value (not created yet)
          EStatePtr newEStatePtr0=*nesListIter;
          ROSE_ASSERT(newEStatePtr0->label()!=Labeler::NO_LABEL);

          if((!isFailedAssertEState(newEStatePtr0)&&!isVerificationErrorEState(newEStatePtr0))) {
            HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=process(newEStatePtr0); // sub solver
            EStatePtr newEStatePtr=const_cast<EStatePtr>(pres.second);
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
                existingEStateSet.insert(const_cast<EStatePtr>(newEStatePtr));
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
          if(((isFailedAssertEState(newEStatePtr0))||isVerificationErrorEState(newEStatePtr0))) {
            // failed-assert end-state: do not add to work list but do add it to the transition graph
            EStatePtr newEStatePtr=processNewOrExisting(newEStatePtr0); // sub solver
            // TODO: create reduced transition set at end of this function
            if(!getModeLTLDriven()) {
              recordTransition(currentEStatePtr,e,newEStatePtr);
            }
            deferedWorkList.push_back(newEStatePtr);
            if(isVerificationErrorEState(newEStatePtr)) {
              SAWYER_MESG(logger[TRACE])<<"STATUS: detected verification error state ... terminating early"<<endl;
              // set flag for terminating early
              reachabilityResults.reachable(0);
              _firstAssertionOccurences.push_back(pair<int, EStatePtr>(0, newEStatePtr));
              EStateWorkList emptyWorkList;
              EStatePtrSet emptyExistingStateSet;
              return make_pair(emptyWorkList,emptyExistingStateSet);
            } else if(isFailedAssertEState(newEStatePtr)) {
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
                      _firstAssertionOccurences.push_back(pair<int, EStatePtr>(assertCode, newEStatePtr));
                    }
                  }
                  reachabilityResults.reachable(assertCode);
                }     // record failed assert
              }
            } // end of failed assert handling
          } // end of if
        } // end of loop on transfer function return-estates
      } // edge set iterator
    }
  }
  return make_pair(deferedWorkList,existingEStateSet);
}

std::string CodeThorn::CTAnalysis::programPositionInfo(CodeThorn::Label lab) {
  SgNode* node=getLabeler()->getNode(lab);
  return SgNodeHelper::lineColumnNodeToString(node);
}

bool CodeThorn::CTAnalysis::getOptionOutputWarnings() {
  return _estateTransferFunctions->getOptionOutputWarnings();
}

void CodeThorn::CTAnalysis::setOptionOutputWarnings(bool flag) {
  _estateTransferFunctions->setOptionOutputWarnings(flag);
}

bool CodeThorn::CTAnalysis::isUnreachableLabel(Label lab) {
  // (1) if code is unreachable no state is computed for it. In this case no entry is found for this label
  bool isUnreachable;
  if(getSolver()->getId()==18) {
    // solver 18 uses its own states
    Solver18* solver18=dynamic_cast<CodeThorn::Solver18*>(getSolver());
    ROSE_ASSERT(solver18);
    isUnreachable=solver18->isUnreachableLabel(lab);
  } else {
    isUnreachable=_abstractCSStateMapMap.find(lab.getId())==_abstractCSStateMapMap.end()&&(lab!=getFlow()->getStartLabel());
  }

  // (2) unreachable in the ICFG (excluded from analysis)
  bool isCfgUnreachable=false;
  if(auto topSort=getTopologicalSort()) {
    LabelSet u=topSort->unreachableLabels();
    isCfgUnreachable=(u.find(lab)!=u.end());
  }
  // (3) take into account that Flow is optimized by excluding BlockBeginEnd nodes
  bool isBlockBeginEndLabel=getLabeler()->isBlockBeginLabel(lab)||getLabeler()->isBlockEndLabel(lab);

  return (isUnreachable || isCfgUnreachable) && !isBlockBeginEndLabel;
}

bool CodeThorn::CTAnalysis::isReachableLabel(Label lab) {
  return !isUnreachableLabel(lab);
}

bool CodeThorn::CTAnalysis::isRegisterAddress(AbstractValue memLoc) {
  if(memLoc.isConstInt()) {
    uint64_t address=(uint64_t)memLoc.getLongIntValue();
    return address>=getOptionsRef().getRegisterRangeStart()&&address<=getOptionsRef().getRegisterRangeEnd();
  }
  return false;
}

EStatePtr CodeThorn::CTAnalysis::getAbstractState(CodeThorn::Label lab, CodeThorn::CallString cs) {
  EStatePtr res;
#pragma omp critical(SUMMARY_STATES)
  {
    auto iter1=_abstractCSStateMapMap.find(lab.getId());
    if(iter1==_abstractCSStateMapMap.end()) {
      res=getBottomAbstractState(lab,cs);
    } else {
      AbstractCSStateMap& abstractCSStateMap=(*iter1).second;
      auto iter2=abstractCSStateMap.find(cs);
      if(iter2==abstractCSStateMap.end()) {
        res=getBottomAbstractState(lab,cs);
      } else {
        res=(*iter2).second;
      }
    }
  }
  return res;
}

void CodeThorn::CTAnalysis::setAbstractState(CodeThorn::Label lab, CodeThorn::CallString cs, EStatePtr estate) {
  ROSE_ASSERT(lab==estate->label());
  ROSE_ASSERT(cs==estate->getCallString());
  ROSE_ASSERT(estate);

  //pair<int,CallString> p(lab.getId(),cs);
  //_summaryCSStateMap[p]=estate;
#pragma omp critical(SUMMARY_STATES)
  {
    auto iter1=_abstractCSStateMapMap.find(lab.getId());
    if(iter1==_abstractCSStateMapMap.end()) {
      // create new
      AbstractCSStateMap newAbstractCSStateMap;
      newAbstractCSStateMap[cs]=estate;
      _abstractCSStateMapMap[lab.getId()]=newAbstractCSStateMap;
    } else {
      AbstractCSStateMap& abstractCSStateMap=(*iter1).second;
      abstractCSStateMap[cs]=estate;
    }
  }
}


EStatePtr CodeThorn::CTAnalysis::getBottomAbstractState(Label lab, CallString cs) {
  InputOutput io;
  io.recordBot();
  if(EState::sharedPStates) {
    PStatePtr bottomPState=processNewOrExisting(*new PState());
    EState estate(lab,cs,bottomPState,io);
    EStatePtr bottomElement=processNewOrExisting(estate);
    return bottomElement;
  } else {
    PStatePtr initialEmpty=new PState();
    EState estate(lab,cs,initialEmpty,io);
    if(getSolver()->createsTransitionSystem()) {
      EStatePtr bottomElement=processNewOrExisting(estate);
      return bottomElement;
    } else {
      return new EState(estate); // solver 17 does not use the estate hash set
    }
  }
}

void CodeThorn::CTAnalysis::initializeAbstractStates(PStatePtr initialPStateStored) {
  _initialPStateStored=initialPStateStored;
#if 0
  for(auto label:*getLabeler()) {
    // create bottom elements for each label
    InputOutput io;
    io.recordBot();
    CallString cs; // empty callstring
    EState estate(label,cs,initialPStateStored,io); // implicitly empty cs
    EStatePtr bottomElement=processNewOrExisting(getBottomAbstractState());
    setAbstractState(label,estate.getCallString(),bottomElement);
  }
#endif
}

bool CodeThorn::CTAnalysis::getPrintDetectedViolations() {
  return _estateTransferFunctions->getPrintDetectedViolations();
}

void CodeThorn::CTAnalysis::setPrintDetectedViolations(bool flag) {
  _estateTransferFunctions->setPrintDetectedViolations(flag);
}

bool CodeThorn::CTAnalysis::getIgnoreFunctionPointers() {
  return _ctOpt.ignoreFunctionPointers;
}

void CodeThorn::CTAnalysis::setInterpreterMode(CodeThorn::InterpreterMode mode) {
  _estateTransferFunctions->setInterpreterMode(mode);
}

CodeThorn::InterpreterMode CodeThorn::CTAnalysis::getInterpreterMode() {
  return _estateTransferFunctions->getInterpreterMode();
}

void CodeThorn::CTAnalysis::setInterpreterModeOutputFileName(string fileName) {
  _estateTransferFunctions->setInterpreterModeFileName(fileName);
}

string CodeThorn::CTAnalysis::getInterpreterModeOutputFileName() {
  return _estateTransferFunctions->getInterpreterModeFileName();
}

void CodeThorn::CTAnalysis::setOptionContextSensitiveAnalysis(bool flag) {
  _contextSensitiveAnalysis=flag;
}

bool CodeThorn::CTAnalysis::getOptionContextSensitiveAnalysis() {
  return _contextSensitiveAnalysis;
}

void CodeThorn::CTAnalysis::printStatusMessage(string s, bool newLineFlag) {
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

void CodeThorn::CTAnalysis::printStatusMessage(string s) {
  printStatusMessage(s,false);
}

void CodeThorn::CTAnalysis::printStatusMessageLine(string s) {
  printStatusMessage(s,true);
}

void CodeThorn::CTAnalysis::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::CTAnalysis", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

void CodeThorn::CTAnalysis::enableSVCompFunctionSemantics() {
  _svCompFunctionSemantics=true;
  _estateTransferFunctions->setSVCompFunctionSemantics(true);
  _externalErrorFunctionName="__VERIFIER_error";
  _externalNonDetIntFunctionName="__VERIFIER_nondet_int";
  _externalNonDetLongFunctionName="__VERIFIER_nondet_long";
  _externalExitFunctionName="exit";
}

void CodeThorn::CTAnalysis::disableSVCompFunctionSemantics() {
  _svCompFunctionSemantics=false;
  _estateTransferFunctions->setSVCompFunctionSemantics(false);
  _externalErrorFunctionName="";
  _externalNonDetIntFunctionName="";
  _externalNonDetLongFunctionName="";
  _externalExitFunctionName="";
  ROSE_ASSERT(getLabeler());
  if(CTIOLabeler* ctioLabeler=dynamic_cast<CTIOLabeler*>(getLabeler())) {
    ctioLabeler->setExternalNonDetIntFunctionName(_externalNonDetIntFunctionName);
    ctioLabeler->setExternalNonDetLongFunctionName(_externalNonDetLongFunctionName);
  }
}

bool CodeThorn::CTAnalysis::svCompFunctionSemantics() { return _svCompFunctionSemantics; }
bool CodeThorn::CTAnalysis::getStdFunctionSemantics() { return _estateTransferFunctions->getStdFunctionSemantics(); }
void CodeThorn::CTAnalysis::setStdFunctionSemantics(bool flag) { _estateTransferFunctions->setStdFunctionSemantics(flag); }

void CodeThorn::CTAnalysis::writeWitnessToFile(string filename) {
  _counterexampleGenerator.setType(CounterexampleGenerator::TRACE_TYPE_SVCOMP_WITNESS);
  ROSE_ASSERT(_firstAssertionOccurences.size() == 1); //SV-COMP: Expecting exactly one reachability property
  list<pair<int, EStatePtr> >::iterator iter = _firstAssertionOccurences.begin();
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


size_t CodeThorn::CTAnalysis::getNumberOfErrorLabels() {
  return _assertNodes.size();
}

string CodeThorn::CTAnalysis::labelNameOfAssertLabel(Label lab) {
  string labelName;
  for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=_assertNodes.begin();i!=_assertNodes.end();++i)
    if(lab==getLabeler()->getLabel((*i).second))
      labelName=SgNodeHelper::getLabelName((*i).first);
  //assert(labelName.size()>0);
  return labelName;
}

bool CodeThorn::CTAnalysis::isCppLabeledAssertLabel(Label lab) {
  return labelNameOfAssertLabel(lab).size()>0;
}


void CodeThorn::CTAnalysis::setGlobalTopifyMode(GlobalTopifyMode mode) {
  _globalTopifyMode=mode;
}

void CodeThorn::CTAnalysis::setExternalErrorFunctionName(std::string externalErrorFunctionName) {
  _externalErrorFunctionName=externalErrorFunctionName;
}

bool CodeThorn::CTAnalysis::isPrecise() {
  // MS 05/20/20: removed (eliminated explicitArrays mode)
  //if (_ctOpt.arraysNotInState==true && !_ctOpt.rers.rersBinary) {
  //  return false;
  //}
  // analysis is precise if it is not in any abstraction mode
  return !(isActiveGlobalTopify()||_ctOpt.abstractionMode>0);
}

// only relevant for maximum values (independent of topify mode)
bool CodeThorn::CTAnalysis::isIncompleteSTGReady() {
  if(_ctOpt.maxTransitions==-1 && _ctOpt.maxIterations==-1 && _ctOpt.maxMemory==-1 && _ctOpt.maxTime==-1)
    return false;
  if ((_ctOpt.maxTransitions!=-1) && ((long int) transitionGraph.size()>=_ctOpt.maxTransitions))
    return true;
  if ((_ctOpt.maxIterations!=-1) && ((long int) getIterations() > _ctOpt.maxIterations))
    return true;
  if ((_ctOpt.maxMemory!=-1) && ((long int) getPhysicalMemorySize() > _ctOpt.maxMemory))
    return true;
  if ((_ctOpt.maxTime!=-1) && ((long int) analysisRunTimeInSeconds() > _ctOpt.maxTime)) {
    return true;
  }
  // at least one maximum mode is active, but the corresponding limit has not yet been reached
  return false;
}

void CodeThorn::CTAnalysis::setSolver(Solver* solver) {
  _solver=solver;
  solver->setAnalyzer(this);
}

Solver* CodeThorn::CTAnalysis::getSolver() {
  return dynamic_cast<CodeThorn::Solver*>(_solver);
}

void CodeThorn::CTAnalysis::runSolver() {
  startAnalysisTimer();
  CodeThorn::Solver* ctSolver=dynamic_cast<CodeThorn::Solver*>(_solver);
  ROSE_ASSERT(ctSolver);
  if(_ctOpt.status) cout<<"STATUS: running solver "<<ctSolver->getId()<<" (sharedpstates:"<<EState::sharedPStates<<")"<<endl;
  ctSolver->run();
  stopAnalysisTimer();
}

void CodeThorn::CTAnalysis::runAnalysisPhase1(SgProject* root, TimingCollector& tc) {
  SAWYER_MESG(logger[INFO])<< "Ininitializing CT analysis solver "<<this->getSolver()->getId()<<" started"<<endl;
  this->runAnalysisPhase1Sub1(root,tc);
  SAWYER_MESG(logger[INFO])<< "Initializing CT analysis solver "<<this->getSolver()->getId()<<" finished"<<endl;
}

void CodeThorn::CTAnalysis::runAnalysisPhase2(TimingCollector& tc) {
  tc.startTimer();
  initializeSolverWithInitialEState(this->_root);
  runAnalysisPhase2Sub1(tc);
  tc.stopTimer(TimingCollector::transitionSystemAnalysis);
}

void CodeThorn::CTAnalysis::runAnalysisPhase2Sub1(TimingCollector& tc) {
  this->printStatusMessageLine("==============================================================");
  if(_ctOpt.status) cout<<"Initializing initial state(s) ... ";
  //ROSE_ASSERT(this->_root);
  //initializeSolverWithInitialEState(this->_root);
  if(_ctOpt.status) cout<<"done."<<endl;
  if(_ctOpt.getInterProceduralFlag()) {
    if(_ctOpt.status) {
      if(_ctOpt.contextSensitive) {
        cout<<"STATUS: context sensitive analysis with call string length "<<_ctOpt.callStringLength<<"."<<endl;
      }
      cout<<"STATS: max time   (threshold): "<<((_ctOpt.maxTime!=-1)?CodeThorn::readableRunTimeFromSeconds(_ctOpt.maxTime):string("no limit"))<<endl;
      cout<<"STATS: max memory (threshold): "<<((_ctOpt.maxMemory!=-1)?(std::to_string(_ctOpt.maxMemory)+" bytes"):string("no limit"))<<endl;
    }
    if(!this->getModeLTLDriven() && _ctOpt.z3BasedReachabilityAnalysis==false && _ctOpt.ssa==false) {
      switch(_ctOpt.abstractionMode) {
      case 0:
      case 1:
  this->runSolver();
  break;
      default:
  cout<<"Error: unknown abstraction mode "<<_ctOpt.abstractionMode<< "(analysis phase 2)"<<endl;
  exit(1);
      }
    }
  } else {
    // intra-procedural analysis, each function is analyzed separately
#if 0
    // improved version to also consider reachability in call graph + function address taken + fork function calls (TODO)
    auto callGraph=getCFAnalyzer()->getCallGraph();
    Label startLabel=callGraph->getStartLabel();
    if(!startLabel.isValid()) {
      cerr<<"Error: intra-procedural analysis: no valid start label provided. Exiting."<<endl;
      exit(1);
    }
    LabelSet reachableEntryLabels=callGraph->reachableNodes(startLabel);
    LabelSet startLabels=reachableEntryLabels;
#else
    LabelSet entryLabels=getCFAnalyzer()->functionEntryLabels(*getFlow());
    setTotalNumberOfFunctions(entryLabels.size());
    LabelSet startLabels=entryLabels;
#endif

    ROSE_ASSERT(estateWorkListCurrent);
    //if(_ctOpt.status) cout<<"STATUS: intra-procedural analysis: entryLabels: "<<entryLabels.size()
    //        <<" initial work list length: "<<estateWorkListCurrent->size()<<endl;
    // intra-procedural analysis initial states
    ROSE_ASSERT(!getModeLTLDriven());
    eraseWorkList();

    size_t numStartLabels=startLabels.size();
    printStatusMessage("STATUS: intra-procedural analysis with "+std::to_string(getTotalNumberOfFunctions())+" functions.",true);
    long int fCnt=1;
    for(auto slab : startLabels) {
      getFlow()->setStartLabel(slab);
      // initialize intra-procedural analysis with all function entry points
      if(_ctOpt.status) {
  SgNode* node=getLabeler()->getNode(slab);
  string functionName=SgNodeHelper::getFunctionName(node);
  string fileName=SgNodeHelper::sourceFilenameToString(node);
#pragma omp critical (STATUS_MESSAGES)
  {
    SAWYER_MESG(logger[INFO])<<"Intra-procedural analysis: initializing function "<<fCnt++<<" of "<<numStartLabels<<": "<<fileName<<":"<<functionName<<endl;
  }
      }
      EStatePtr initialEStateObj=createInitialEState(this->_root,slab);
      initialEStateObj->setLabel(slab);
      // do not "process" estate if solver 17 is used (does not use estate hash set)
      EStatePtr initialEState=(getSolver()->createsTransitionSystem())? processNewOrExisting(initialEStateObj) : initialEStateObj;
      ROSE_ASSERT(initialEState);
      variableValueMonitor.init(initialEState);
      addToWorkList(initialEState);
      this->runSolver();
      if(this->isIncompleteSTGReady()) {
  _statsIntraUnfinishedFunctions++;
      } else {
  _statsIntraFinishedFunctions++;
      }

    }
  }
}

VariableIdMappingExtended* CodeThorn::CTAnalysis::getVariableIdMapping() {
  return _variableIdMapping;
}

void CodeThorn::CTAnalysis::setVariableIdMapping(VariableIdMappingExtended* vid) {
  _variableIdMapping=vid;
}

CFAnalysis* CodeThorn::CTAnalysis::getCFAnalyzer() {
  return _cfAnalysis;
}

VariableValueMonitor* CodeThorn::CTAnalysis::getVariableValueMonitor() {
  return &variableValueMonitor;
}

size_t CodeThorn::CTAnalysis::getEStateSetSize() {
  return estateSet.size();
}

size_t CodeThorn::CTAnalysis::getTransitionGraphSize() {
  return transitionGraph.size();
}


set<string> CodeThorn::CTAnalysis::variableIdsToVariableNames(CodeThorn::VariableIdSet s) {
  set<string> res;
  for(CodeThorn::VariableIdSet::iterator i=s.begin();i!=s.end();++i) {
    res.insert((AbstractValue(*i)).toString(getVariableIdMapping()));
  }
  return res;
}

// deprecated
CodeThorn::CTAnalysis::VariableDeclarationList CodeThorn::CTAnalysis::computeUnusedGlobalVariableDeclarationList(SgProject* root) {
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(root);
  CodeThorn::CTAnalysis::VariableDeclarationList usedGlobalVars=computeUsedGlobalVariableDeclarationList(root);
  for(CodeThorn::CTAnalysis::VariableDeclarationList::iterator i=usedGlobalVars.begin();i!=usedGlobalVars.end();++i) {
    globalVars.remove(*i);
  }
  return globalVars;
}

// deprecated
CodeThorn::CTAnalysis::VariableDeclarationList CodeThorn::CTAnalysis::computeUsedGlobalVariableDeclarationList(SgProject* root) {
  if(SgProject* project=isSgProject(root)) {
    CodeThorn::CTAnalysis::VariableDeclarationList usedGlobalVariableDeclarationList;
    list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
    VariableIdSet setOfUsedVars=AstUtility::usedVariablesInsideFunctions(project,getVariableIdMapping());
    int filteredVars=0;
    for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
      VariableId globalVarId=getVariableIdMapping()->variableId(*i);
      if(setOfUsedVars.find(globalVarId)!=setOfUsedVars.end()) {
        usedGlobalVariableDeclarationList.push_back(*i);
      } else {
        filteredVars++;
      }
    }
    return usedGlobalVariableDeclarationList;
  } else {
    SAWYER_MESG(logger[ERROR]) << "no global scope.";
    exit(1);
  }
}

void CodeThorn::CTAnalysis::openStgTraceFile() {
  ofstream fout;
  fout.open(getOptionsRef().stgTraceFileName.c_str());    // create new file/overwrite existing file
  fout<<"START"<<endl;
  fout.close();    // close. Will be used with append.
}

void CodeThorn::CTAnalysis::recordTransition(EStatePtr sourceState, Edge e, EStatePtr targetState) {
  transitionGraph.add(Transition(sourceState,e,targetState));
}

void CodeThorn::CTAnalysis::printStatusMessage(bool forceDisplay) {
  // forceDisplay currently only turns on or off

  // report we are alive
  stringstream ss;
  if(forceDisplay) {
    long pstateSetSize;
    long estateSetSize;
    long transitionGraphSize;
    long estateWorkListCurrentSize;
#pragma omp critical(HASHSET)
    {
      pstateSetSize = pstateSet.size();
      estateSetSize = estateSet.size();
      transitionGraphSize = getTransitionGraph()->size();
    }
#pragma omp critical(ESTATEWL)
    {
      estateWorkListCurrentSize = estateWorkListCurrent->size();
    }
    ss <<color("white")<<"Number of pstates/estates/trans/wl/iter/time/mem: ";
    ss <<color("magenta")<<pstateSetSize
       <<color("white")<<"/"
       <<color("cyan")<<estateSetSize
       <<color("white")<<"/"
       <<color("blue")<<transitionGraphSize
       <<color("white")<<"/"
       <<estateWorkListCurrentSize
       <<"/"<<getIterations()<<"-"<<getApproximatedIterations()
       <<"/"<<analysisRunTimeInSeconds()<<"s<"<<(_ctOpt.maxTime!=-1?std::to_string(_ctOpt.maxTime)+"s"     :"inf")
       <<"/"<<getPhysicalMemorySize()/(1024*1024) <<" MiB <"<<(_ctOpt.maxMemory  !=-1?std::to_string(_ctOpt.maxMemory/(1024*1024))  +" MiB":"inf MiB")
       <<color("normal") //<<"/"<<analyzerStateToString()
       <<endl
    ;
    printStatusMessage(ss.str());
  }
}

string CodeThorn::CTAnalysis::analyzerStateToString() {
  stringstream ss;
  ss<<"[P:"<<isPrecise();
  ss<<" ";
  ss<<"T:"<<_globalTopifyMode;
  ss<<" ";
  ss<<"B:"<<_ctOpt.rers.rersBinary;
  ss<<" ";
  ss<<"R:"<<isIncompleteSTGReady();
  ss<<"]";
  return ss.str();
}

void CodeThorn::CTAnalysis::incIterations() {
  if(isPrecise()) {
#pragma omp atomic
    _iterations+=1;
  } else {
#pragma omp atomic
    _approximated_iterations+=1;
  }
}

bool CodeThorn::CTAnalysis::isLoopCondLabel(Label lab) {
  SgNode* node=getLabeler()->getNode(lab);
  return SgNodeHelper::isLoopCond(node);
}

void CodeThorn::CTAnalysis::addToWorkList(EStatePtr estate) {
  ROSE_ASSERT(estate);
  ROSE_ASSERT(estateWorkListCurrent);
#pragma omp critical(ESTATEWL)
  {
    if(!estate) {
      SAWYER_MESG(logger[ERROR])<<"(internal): null pointer added to work list."<<endl;
      exit(1);
    }
    switch(_explorationMode) {
    case EXPL_UNDEFINED:
      cerr<<"Error: undefined state exploration mode. Bailing out."<<endl;
      exit(1);
    case EXPL_DEPTH_FIRST: estateWorkListCurrent->push_front(estate);break;
    case EXPL_BREADTH_FIRST: estateWorkListCurrent->push_back(estate);break;
    case EXPL_RANDOM_MODE1: {
      int perc=4; // perc-chance depth-first
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

bool CodeThorn::CTAnalysis::isActiveGlobalTopify() {
  if(_maxTransitionsForcedTop==-1 && _maxIterationsForcedTop==-1 && _maxBytesForcedTop==-1 && _maxSecondsForcedTop==-1)
    return false;
  if(_topifyModeActive)
    return true;
  // TODO: add a critical section that guards "transitionGraph.size()"
  if( (_maxTransitionsForcedTop!=-1 && (long int)transitionGraph.size()>=_maxTransitionsForcedTop)
      || (_maxIterationsForcedTop!=-1 && (long int)getIterations() > _maxIterationsForcedTop)
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

void CodeThorn::CTAnalysis::eventGlobalTopifyTurnedOn() {
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

// obsolete, once EStateTransferFunctions::createInternalEState is removed
void CodeThorn::CTAnalysis::topifyVariable(PState& pstate, AbstractValue varId) {
  pstate.writeTopToMemoryLocation(varId);
}


bool CodeThorn::CTAnalysis::isStartLabel(Label label) {
  return getTransitionGraph()->getStartLabel()==label;
}

// Avoid calling critical sections from critical sections:
// worklist functions do not use each other.
bool CodeThorn::CTAnalysis::isEmptyWorkList() {
  bool res;
#pragma omp critical(ESTATEWL)
  {
    res=estateWorkListCurrent->empty();
  }
  return res;
}
EStatePtr CodeThorn::CTAnalysis::topWorkList() {
  EStatePtr estate=0;
#pragma omp critical(ESTATEWL)
  {
    if(!estateWorkListCurrent->empty())
      estate=estateWorkListCurrent->front();
  }
  return estate;
}

void CodeThorn::CTAnalysis::eraseWorkList() {
  while(!isEmptyWorkList())
    popWorkList();
}

EStatePtr CodeThorn::CTAnalysis::popWorkList() {
  EStatePtr estate=0;
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
void CodeThorn::CTAnalysis::swapWorkLists() {
  EStateWorkList* tmp = estateWorkListCurrent;
  estateWorkListCurrent = estateWorkListNext;
  estateWorkListNext = tmp;
  incIterations();
}

bool CodeThorn::CTAnalysis::isFailedAssertEState(EStatePtr estate) {
  if(estate->io.isFailedAssertIO())
    return true;
  if(_treatStdErrLikeFailedAssert) {
    return estate->io.isStdErrIO();
  }
  return false;
}

bool CodeThorn::CTAnalysis::isVerificationErrorEState(EStatePtr estate) {
  if(estate->io.isVerificationError())
    return true;
  return false;
}

EState CodeThorn::CTAnalysis::createFailedAssertEState(EState estate, Label target) {
  EState newEState=estate;
  newEState.io.recordFailedAssert();
  newEState.setLabel(target);
  return newEState;
}

// creates a state that represents that the verification error function was called
// the edge is the external call edge
EState CodeThorn::CTAnalysis::createVerificationErrorEState(EState estate, Label target) {
  EState newEState=estate;
  newEState.io.recordVerificationError();
  newEState.setLabel(target);
  return newEState;
}

void CodeThorn::CTAnalysis::initLabeledAssertNodes(SgProject* root) {
  _assertNodes=listOfLabeledAssertNodes(root);
  SAWYER_MESG(logger[INFO])<<"DEBUG: number of labeled assert nodes: "<<_assertNodes.size()<<endl;
}

list<pair<SgLabelStatement*,SgNode*> > CodeThorn::CTAnalysis::listOfLabeledAssertNodes(SgProject* root) {
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

PStatePtr CodeThorn::CTAnalysis::processNew(PState& s) {
  if(EState::sharedPStates) {
    return const_cast<PStatePtr>(pstateSet.processNew(s));
  } else {
    PState* newPState=new PState(s);
    return newPState;
  }
}

PStatePtr CodeThorn::CTAnalysis::processNew(PState* s) {
  return processNew(*s);
}

PStatePtr CodeThorn::CTAnalysis::processNewOrExisting(PState& s) {
  if(EState::sharedPStates) {
    return const_cast<PStatePtr>(pstateSet.processNewOrExisting(s));
  } else {
    return processNew(s);
  }
}
PStatePtr CodeThorn::CTAnalysis::processNewOrExisting(PState* s) {
  if(EState::sharedPStates) {
    return processNewOrExisting(*s);
  } else {
    return processNew(s);
  }
}

EStatePtr CodeThorn::CTAnalysis::processNew(EState& s) {
  return const_cast<EStatePtr>(estateSet.processNew(s));
}

EStatePtr CodeThorn::CTAnalysis::processNew(EState* s) {
  return processNew(*s);
}

EStatePtr CodeThorn::CTAnalysis::processNewOrExisting(EState& estate) {
  return const_cast<EStatePtr>(estateSet.processNewOrExisting(estate));
}

EStatePtr CodeThorn::CTAnalysis::processNewOrExisting(EState* estate) {
  return processNewOrExisting(*estate);
}

EStateSet::ProcessingResult CodeThorn::CTAnalysis::process(EState& estate) {
  return estateSet.process(estate);
}
EStateSet::ProcessingResult CodeThorn::CTAnalysis::process(EState* estate) {
  return estateSet.process(estate);
}

LabelSet CodeThorn::CTAnalysis::functionEntryLabels() {
  Flow& flow=*getFlow();
  LabelSet functionEntryLabels=getCFAnalyzer()->functionEntryLabels(flow);
  return functionEntryLabels;
}

LabelSet CodeThorn::CTAnalysis::reachableFunctionEntryLabels() {
  LabelSet funEntryLabs=functionEntryLabels();
  LabelSet reachable;
  for(auto lab : funEntryLabs) {
    if(isReachableLabel(lab))
      reachable.insert(lab);
  }
  return reachable;
}

SgFunctionDefinition* CodeThorn::CTAnalysis::getFunctionDefinitionOfEntryLabel(Label lab) {
  ROSE_ASSERT(getLabeler()->isFunctionEntryLabel(lab));
  SgNode* node=getLabeler()->getNode(lab);
  SgFunctionDefinition* funDef=isSgFunctionDefinition(node);
  ROSE_ASSERT(funDef);
  return funDef;
}

std::string CodeThorn::CTAnalysis::analyzedFunctionsToString() {
  ostringstream ss;
  LabelSet reachableFunLabels=reachableFunctionEntryLabels();
  for (auto funLab : reachableFunLabels)  {
    SgFunctionDefinition* funDef=getFunctionDefinitionOfEntryLabel(funLab);
    auto funDecl=funDef->get_declaration();
    ss
      <<SgNodeHelper::sourceFilenameToString(funDecl)
      <<","
      <<SgNodeHelper::sourceLineColumnToString(funDecl)
      <<","
      <<SgNodeHelper::getFunctionName(funDecl)
      <<endl;
  }
  return ss.str();
}

std::string CodeThorn::CTAnalysis::analyzedFilesToString() {
  std::unordered_set<string> fileNameSet;
  LabelSet reachableFunLabels=reachableFunctionEntryLabels();
  for (auto funLab : reachableFunLabels)  {
    SgFunctionDefinition* funDef=getFunctionDefinitionOfEntryLabel(funLab);
    fileNameSet.insert(SgNodeHelper::sourceFilenameToString(funDef));
  }
  ostringstream ss;
  for (auto fn : fileNameSet)  {
    ss<<fn<<endl;
  }
  return ss.str();
}

std::string CodeThorn::CTAnalysis::externalFunctionsToString() {
  set<string> s;
  for (auto funCall : externalFunctions)  {
    //ss<<SgNodeHelper::locationToString(node);
    //ss<<SgNodeHelper::getFunctionName(funCall)<<endl;
    ostringstream ss;
    auto funDecl=funCall->getAssociatedFunctionDeclaration();
    if(funDecl) {
      s.insert(ProgramLocationsReport::findOriginalProgramLocationOfNode(funDecl)+","+funDecl->unparseToString());
    } else {
      s.insert(ProgramLocationsReport::findOriginalProgramLocationOfNode(funCall)+","+"function call resolves to more than one target:"+funCall->unparseToString());
    }
  }
  stringstream ss;
  for(auto entry : s) {
    ss<<entry<<endl;
  }
  return ss.str();
}

void CodeThorn::CTAnalysis::recordExternalFunctionCall(SgFunctionCallExp* funCall) {
#pragma omp critical(funcallrecording)
  {
    externalFunctions.insert(funCall);
  }
}

list<EStatePtr> CodeThorn::CTAnalysis::transferEdgeEStateInPlace(Edge edge, EStatePtr estate) {
  ROSE_ASSERT(edge.source()==estate->label());
  return _estateTransferFunctions->transferEdgeEStateInPlace(edge,estate);
}

list<EStatePtr> CodeThorn::CTAnalysis::transferEdgeEState(Edge edge, EStatePtr estate) {
  ROSE_ASSERT(edge.source()==estate->label());
  return _estateTransferFunctions->transferEdgeEState(edge,estate);
}

void CodeThorn::CTAnalysis::startAnalysisTimer() {
  if (!_timerRunning) {
    _analysisTimer.start();
    _timerRunning=true;
    SAWYER_MESG(logger[DEBUG])<<"INFO: solver timer started."<<endl;
  }
}

void CodeThorn::CTAnalysis::stopAnalysisTimer() {
  _timerRunning=false;
  _analysisTimer.stop();
  SAWYER_MESG(logger[DEBUG])<<"INFO: solver timer stopped."<<endl;
}

SgNode* CodeThorn::CTAnalysis::getStartFunRoot() {
  return _startFunRoot;
}

EStatePtr CodeThorn::CTAnalysis::createInitialEState(SgProject* root, Label slab) {
  // create initial state
  PStatePtr initialPState=new PState();
  ROSE_ASSERT(slab.isValid());
  _estateTransferFunctions->initializeCommandLineArgumentsInState(slab,initialPState);
  if(_ctOpt.inStateStringLiterals) {
    ROSE_ASSERT(_estateTransferFunctions);
    _estateTransferFunctions->initializeStringLiteralsInState(slab,initialPState);
    if(_ctOpt.status) {
      cout<<"STATUS: created "<<getVariableIdMapping()->numberOfRegisteredStringLiterals()<<" string literals in initial state."<<endl;
    }
  }

  PStatePtr initialPStateStored=processNewOrExisting(*initialPState); // might reuse another pstate when initializing in level 1   // CHANGING PSTATE-ESTATE
  ROSE_ASSERT(initialPStateStored);
  SAWYER_MESG(logger[INFO])<< "INIT: initial pstate(stored): "<<initialPStateStored<<":"<<initialPStateStored->toString(getVariableIdMapping())<<endl;

  transitionGraph.setStartLabel(slab);
  transitionGraph.setAnalyzer(this);

  EStatePtr estate=new EState(slab,initialPStateStored);
  _estateTransferFunctions->initializeArbitraryMemory(estate);

  ROSE_ASSERT(_estateTransferFunctions);
  _estateTransferFunctions->initializeGlobalVariables(root, estate);
  SAWYER_MESG(logger[INFO]) <<"Initial state: number of entries:"<<estate->pstate()->stateSize()<<endl;


  // initialize summary states map for abstract model checking mode
  initializeAbstractStates(initialPStateStored);
  estate->io.recordNone(); // ensure that extremal value is different to bot

  return estate;
}
void CodeThorn::CTAnalysis::postInitializeSolver() {
  // empty in base class
}

void CodeThorn::CTAnalysis::initializeSolverWithInitialEState(SgProject* root) {
  // initialization of solver
  if(_ctOpt.runSolver) {
    if(_ctOpt.getInterProceduralFlag()) {
      // inter-procedural analysis initial state
      Label slab=getFlow()->getStartLabel();
      EStatePtr initialEState=createInitialEState(root, slab);
      // solver 17 does not use a estate hash set, do not process
      initialEState=(getSolver()->createsTransitionSystem())? processNewOrExisting(initialEState): initialEState;
      ROSE_ASSERT(initialEState);
      variableValueMonitor.init(initialEState);
      addToWorkList(initialEState);
      SAWYER_MESG(logger[INFO]) << "INIT: start state inter-procedural (extremal value size): "<<initialEState->pstate()->stateSize()<<" variables."<<endl;
      SAWYER_MESG(logger[TRACE]) << "INIT: start state inter-procedural (extremal value): "<<initialEState->toString(getVariableIdMapping())<<endl;
      postInitializeSolver(); // empty in this class, only overridden by IOAnalyzer for ltldriven analysis
    } else {
      ROSE_ASSERT(!getModeLTLDriven());

      // deactivated initialization here, initialization is done in phase 2 for each function separately
#if 0
      LabelSet startLabels=getCFAnalyzer()->functionEntryLabels(*getFlow());
      getFlow()->setStartLabelSet(startLabels);

      size_t numStartLabels=startLabels.size();
      printStatusMessage("STATUS: intra-procedural analysis with "+std::to_string(numStartLabels)+" start functions.",true);
      long int fCnt=1;
      for(auto slab : startLabels) {
  // initialize intra-procedural analysis with all function entry points
  if(_ctOpt.status) {
    SgNode* node=getLabeler()->getNode(slab);
    string functionName=SgNodeHelper::getFunctionName(node);
    string fileName=SgNodeHelper::sourceFilenameToString(node);
#pragma omp critical (STATUS_MESSAGES)
    {
      SAWYER_MESG(logger[INFO])<<"Intra-procedural analysis: initializing function "<<fCnt++<<" of "<<numStartLabels<<": "<<fileName<<":"<<functionName<<endl;
    }
  }
  EState initialEStateObj=createInitialEState(root,slab);
  initialEStateObj.setLabel(slab);
  EStatePtr initialEState=processNewOrExisting(initialEStateObj);
  ROSE_ASSERT(initialEState);
  variableValueMonitor.init(initialEState);
  addToWorkList(initialEState);
      }
#endif

    } // end of if

    if(_ctOpt.rers.rersBinary) {
      //initialize the global variable arrays in the linked binary version of the RERS problem
      SAWYER_MESG(logger[DEBUG])<< "init of globals with arrays for "<< _ctOpt.threads << " threads. " << endl;
      RERS_Problem::rersGlobalVarsArrayInitFP(_ctOpt.threads);
      RERS_Problem::createGlobalVarAddressMapsFP(this);
    }
    SAWYER_MESG(logger[INFO])<<"Initializing solver finished."<<endl;
  } else {
    if(_ctOpt.status) cout<<"STATUS: skipping solver run."<<endl;
  }
}

void CodeThorn::CTAnalysis::runAnalysisPhase1Sub1(SgProject* root, TimingCollector& tc) {
  SAWYER_MESG(logger[TRACE])<<"CTAnalysis::runAnalysisPhase1Sub1 started."<<endl;
  ROSE_ASSERT(root);
  this->_root=root;

  CodeThornOptions& ctOpt=getOptionsRef();
  Pass::normalization(ctOpt,root,tc);
   logger[INFO]<<"Normalization finished"<<endl;
  _variableIdMapping=Pass::createVariableIdMapping(ctOpt, root, tc); // normalization timer
   logger[INFO]<<"Vim finished"<<endl;
  _labeler=Pass::createLabeler(ctOpt, root, tc, _variableIdMapping); // labeler timer
    logger[INFO]<<"Labeler finished"<<endl;
  _classHierarchy=Pass::createClassAnalysis(ctOpt, root, tc); // class hierarchy timer
    logger[INFO]<<"Class hierarchy finished"<<endl; _virtualFunctions=Pass::createVirtualFunctionAnalysis(ctOpt, _classHierarchy, tc); // class hierarchy timer
    logger[INFO]<<"Virtual functions finished"<<endl;
  _cfAnalysis=Pass::createForwardIcfg(ctOpt,root,tc,_labeler,_classHierarchy,_virtualFunctions); // icfg constructino timer

  tc.startTimer(); // initialization timer (stopped at end of function)
  resetInputSequenceIterator();
  RoseAst completeast(root);

  _estateTransferFunctions->setVariableIdMapping(getVariableIdMapping());
  AbstractValue::setVariableIdMapping(getVariableIdMapping());

  /* set start function */
  {
    string startFunctionName;
    if(ctOpt.startFunctionName.size()>0) {
      startFunctionName = ctOpt.startFunctionName;
    } else {
      startFunctionName = "main";
    }
    if(_ctOpt.getInterProceduralFlag()) {
      _startFunRoot=completeast.findFunctionByName(startFunctionName);
      if(_startFunRoot==0) {
  SAWYER_MESG(logger[ERROR]) << "Function '"<<startFunctionName<<"' not found.\n";
  exit(1);
      } else {
  SAWYER_MESG(logger[INFO])<< "Starting at function '"<<startFunctionName<<"'."<<endl;
      }
    } else {
      // should not be required
      _startFunRoot=completeast.findFunctionByName(startFunctionName);
    }
    ROSE_ASSERT((_ctOpt.getInterProceduralFlag() && _startFunRoot) || (!_ctOpt.getInterProceduralFlag()));
  }

  SAWYER_MESG(logger[TRACE])<< "Initializing AST node info."<<endl;
  initAstNodeInfo(root);
  getCFAnalyzer()->setInterProcedural(_ctOpt.getInterProceduralFlag());

  if(CTIOLabeler* ctioLabeler=dynamic_cast<CTIOLabeler*>(getLabeler())) {
    ctioLabeler->setExternalNonDetIntFunctionName(_externalNonDetIntFunctionName);
    ctioLabeler->setExternalNonDetLongFunctionName(_externalNonDetLongFunctionName);
  }

  CallString::setMaxLength(_ctOpt.callStringLength);

  if(_estateTransferFunctions==nullptr) {
    EStateTransferFunctions* etf=new EStateTransferFunctions();
    etf->setAnalyzer(this);
    _estateTransferFunctions=etf;
  }
  _estateTransferFunctions->addParameterPassingVariables(); // DFTransferFunctions: adds pre-defined var-ids to VID for parameter passing

  if(_ctOpt.getInterProceduralFlag()) {
    // inter-procedural analysis: one start label
    Label slab2=getLabeler()->getLabel(_startFunRoot);
    ROSE_ASSERT(slab2.isValid());
    ROSE_ASSERT(getLabeler()->isFunctionEntryLabel(slab2));
    ROSE_ASSERT(getFlow());
    getFlow()->addStartLabel(slab2);
  } else {
    // intra-procedural analysis: multiple start labels
    LabelSet entryLabels=getCFAnalyzer()->functionEntryLabels(*getFlow());
    if(entryLabels.size()==0) {
      cout<<"Exit: No functions in program, nothing to analyze."<<endl;
      exit(0);
    }
    ROSE_ASSERT(getFlow());
    // set one label as start label (intra-proc loop will continue with 2nd label)
    getFlow()->setStartLabel(*entryLabels.begin());
  }
  SAWYER_MESG(logger[TRACE])<<"CTAnalysis::initializeSolver3i."<<endl;

  // Runs consistency checks on the fork / join and workshare / barrier nodes in the parallel CFG
  // If the --omp-ast flag is not selected by the user, the parallel nodes are not inserted into the CFG
  if (_ctOpt.ompAst) {
    getCFAnalyzer()->forkJoinConsistencyChecks(*getFlow());
  }

  if(_ctOpt.reduceCfg) {
    string oldFlowSize=getFlow()->numNodesEdgesToString();
    int cnt=getCFAnalyzer()->optimizeFlow(*getFlow());
    string newFlowSize=getFlow()->numNodesEdgesToString();
    if(_ctOpt.status) {
      cout<< "CFG optimization ON. Eliminated "<<cnt<<" nodes. "
          <<"Flow"<<oldFlowSize
          <<" => "
          <<"Flow"<<newFlowSize
          <<endl;
    }
  } else {
    if(_ctOpt.status) cout<< "CFG optimziation OFF."<<endl;
  }
  SAWYER_MESG(logger[TRACE])<< "Intra-Flow OK. (size: " << getFlow()->size() << " edges)"<<endl;
  ROSE_ASSERT(getCFAnalyzer());

  if(_ctOpt.status) cout<<"STATUS: creating empty worklist."<<endl;
  setWorkLists(_explorationMode);

  //initializeSolverWithInitialEState(this->_root);

  if(_ctOpt.status) cout<<"STATUS: analysis phase 1 finished."<<endl;
  tc.stopTimer(TimingCollector::init);

}

void CodeThorn::CTAnalysis::initAstNodeInfo(SgNode* node) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    AstNodeInfo* attr=new AstNodeInfo();
    (*i)->addNewAttribute("info",attr);
  }
}

void CodeThorn::CTAnalysis::generateAstNodeInfo(SgNode* node) {
  ROSE_ASSERT(node);
  if(!getCFAnalyzer()) {
    SAWYER_MESG(logger[ERROR])<< "CodeThorn::CTAnalysis:: no cfanalyzer available."<<endl;
    exit(1);
  }
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin().withoutNullValues();i!=ast.end();++i) {
    ROSE_ASSERT(*i);
    AstNodeInfo* attr=dynamic_cast<AstNodeInfo*>((*i)->getAttribute("info"));
    if(attr) {
      if(getCFAnalyzer()->getLabel(*i)!=Label()) {
        if(isSgFunctionCallExp(*i)) {
          SAWYER_MESG(logger[TRACE])<<"skipping SgFunctionCallExp (direct pointer, not pattern): "<<(*i)->unparseToString()<<endl;
          continue;
        }
        attr->setLabel(getCFAnalyzer()->getLabel(*i));
        attr->setInitialLabel(getCFAnalyzer()->initialLabel(*i));
        attr->setFinalLabels(getCFAnalyzer()->finalLabels(*i));
      } else if(SgInitializedName* iName=isSgInitializedName(*i)) {
  attr->setVarName(iName->unparseToString());
  attr->setVarType(iName->get_type()->unparseToString());
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

// TODO: move to flow analyzer (reports label,init,final sets)
string CodeThorn::CTAnalysis::astNodeInfoAttributeAndNodeToString(SgNode* node) {
  string textual;
  if(node->attributeExists("info"))
    textual=node->getAttribute("info")->toString()+":";
  return textual+SgNodeHelper::nodeToString(node);
}

// experimental functions
bool CodeThorn::CTAnalysis::checkTransitionGraph() {
  set<EStatePtr> ess=transitionGraph.estateSet();
  bool ok=isConsistentEStatePtrSet(ess);
  ok=ok && getTransitionGraph()->checkConsistency();
  return ok;
}

bool CodeThorn::CTAnalysis::checkEStateSet() {
  for(EStateSet::iterator i=estateSet.begin();i!=estateSet.end();++i) {
    if(estateSet.estateId(*i)==NO_ESTATE || (*i)->label()==Label()) {
      SAWYER_MESG(logger[ERROR])<< "estateSet inconsistent. "<<endl;
      SAWYER_MESG(logger[ERROR])<< "  label   :"<<(*i)->label()<<endl;
      SAWYER_MESG(logger[ERROR])<< "   estateId: "<<estateSet.estateId(*i)<<endl;
      return false;
    }
  }
  SAWYER_MESG(logger[TRACE])<< "INFO: estateSet of size "<<estateSet.size()<<" consistent."<<endl;
  return true;
}

bool CodeThorn::CTAnalysis::isConsistentEStatePtrSet(set<EStatePtr> estatePtrSet)  {
  for(set<EStatePtr>::iterator i=estatePtrSet.begin();i!=estatePtrSet.end();++i) {
    if(estateSet.estateId(*i)==NO_ESTATE || (*i)->label()==Label()) {
      SAWYER_MESG(logger[ERROR])<< "estatePtrSet inconsistent. "<<endl;
      SAWYER_MESG(logger[ERROR])<< "  label   :"<<(*i)->label()<<endl;
      SAWYER_MESG(logger[ERROR])<< "   estateId: "<<estateSet.estateId(*i)<<endl;
      return false;
    }
  }
  SAWYER_MESG(logger[TRACE])<< "INFO: estatePtrSet of size "<<estatePtrSet.size()<<" consistent."<<endl;
  return true;
}

Labeler* CodeThorn::CTAnalysis::getLabeler() const {
  return _labeler;
}

CTIOLabeler* CodeThorn::CTAnalysis::getIOLabeler() const {
  CTIOLabeler* ctioLabeler=dynamic_cast<CTIOLabeler*>(getLabeler());
  if(ctioLabeler==nullptr) {
    cerr<<"Warning: CodeThorn::CTAnalysis was initialized with a default Labeler, but CTIOLabeler is required in function getIOLabeler."<<endl;
  }
  return ctioLabeler;
}

/*
Label CodeThorn::CTAnalysis::getFunctionEntryLabel(SgFunctionRefExp* funRefExp) {
  Label lab;
  return lab;
}
*/

/*!
 * \author Marc Jasper
 * \date 2017.
 */
// MS 05/31/2020: this function is not used anywhere
void CodeThorn::CTAnalysis::resetAnalysis() {
  if(_ctOpt.status) cout<<"STATUS: resetting analysis."<<endl;
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
  PStatePtr processedPState=processNew(startPState); // CHANGING PSTATE-ESTATE
  ROSE_ASSERT(processedPState);
  startEState.setPState(processedPState);
  EStatePtr processedEState=processNew(startEState);
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
  if(EState::sharedPStates) {
    ROSE_ASSERT(pstateSet.size() == 1);
  }
  ROSE_ASSERT(transitionGraph.size() == 0);
  ROSE_ASSERT(estateWorkListCurrent->size() == 1);
  ROSE_ASSERT(estateWorkListNext->size() == 0);
}

/*!
  * \author Marc Jasper
  * \date 2014, 2015.
 */
void CodeThorn::CTAnalysis::storeStgBackup() {
  backupTransitionGraph = transitionGraph;
}

/*!
  * \author Marc Jasper
  * \date 2014, 2015.
 */
void CodeThorn::CTAnalysis::swapStgWithBackup() {
  TransitionGraph tTemp = transitionGraph;
  transitionGraph = backupTransitionGraph;
  backupTransitionGraph = tTemp;
}


#define FAST_GRAPH_REDUCE
void CodeThorn::CTAnalysis::reduceStg(function<bool(EStatePtr)> predicate) {
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
void CodeThorn::CTAnalysis::reduceStgToInOutStates() {
  function<bool(EStatePtr)> predicate = [](EStatePtr s) {
    return s->io.isStdInIO() || s->io.isStdOutIO();
  };
  reduceStg(predicate);
}

/*!
 * \author Marc Jasper
 * \date 2017.
 */
void CodeThorn::CTAnalysis::reduceStgToInOutAssertStates() {
  function<bool(EStatePtr)> predicate = [](EStatePtr s) {
    return s->io.isStdInIO() || s->io.isStdOutIO() || s->io.isFailedAssertIO();
  };
  reduceStg(predicate);
}

/*!
 * \author Marc Jasper
 * \date 2017.
 */
void CodeThorn::CTAnalysis::reduceStgToInOutAssertErrStates() {
  function<bool(EStatePtr)> predicate = [](EStatePtr s) {
    return s->io.isStdInIO() || s->io.isStdOutIO()  || s->io.isFailedAssertIO() || s->io.isStdErrIO();
  };
  reduceStg(predicate);
}

/*!
 * \author Marc Jasper
 * \date 2017.
 */
void CodeThorn::CTAnalysis::reduceStgToInOutAssertWorklistStates() {
  // copy elements from worklist into hashset (faster access within the predicate)
  std::unordered_set<EStatePtr> worklistSet(estateWorkListCurrent->begin(), estateWorkListCurrent->end());
  function<bool(EStatePtr)> predicate = [&worklistSet](EStatePtr s) {
    return s->io.isStdInIO() || s->io.isStdOutIO()
    || s->io.isFailedAssertIO() || (worklistSet.find(s) != worklistSet.end());
  };
  reduceStg(predicate);
}

int CodeThorn::CTAnalysis::reachabilityAssertCode(EStatePtr currentEStatePtr) {
  ROSE_ASSERT(_estateTransferFunctions);
  ROSE_ASSERT(getEStateTransferFunctions());
  if(_ctOpt.rers.rersBinary) {
    int outputVal = getEStateTransferFunctions()->readFromMemoryLocation(currentEStatePtr->label(),currentEStatePtr->pstate(),_estateTransferFunctions->globalVarIdByName("output")).getIntValue();
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

void CodeThorn::CTAnalysis::setSkipUnknownFunctionCalls(bool flag) {
  _skipSelectedFunctionCalls=flag;
  _estateTransferFunctions->setSkipUnknownFunctionCalls(flag);
}

void CodeThorn::CTAnalysis::setSkipArrayAccesses(bool skip) {
  _estateTransferFunctions->setSkipArrayAccesses(skip);
}

bool CodeThorn::CTAnalysis::getSkipArrayAccesses() {
  return _estateTransferFunctions->getSkipArrayAccesses();
}

bool CodeThorn::CTAnalysis::getIgnoreUndefinedDereference() {
  return _ctOpt.ignoreUndefinedDereference;
}

void CodeThorn::CTAnalysis::set_finished(std::vector<bool>& v, bool val) {
  ROSE_ASSERT(v.size()>0);
  for(vector<bool>::iterator i=v.begin();i!=v.end();++i) {
    *i=val;
  }
}

bool CodeThorn::CTAnalysis::all_false(std::vector<bool>& v) {
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

void CodeThorn::CTAnalysis::mapGlobalVarInsert(std::string name, int* addr) {
  mapGlobalVarAddress[name]=addr;
  mapAddressGlobalVar[addr]=name;
}

void CodeThorn::CTAnalysis::setCompoundIncVarsSet(set<AbstractValue> ciVars) {
  _compoundIncVarsSet=ciVars;
}

void CodeThorn::CTAnalysis::setSmallActivityVarsSet(set<AbstractValue> saVars) {
  _smallActivityVarsSet=saVars;
}

void CodeThorn::CTAnalysis::setAssertCondVarsSet(set<AbstractValue> acVars) {
  _assertCondVarsSet=acVars;
}


long CodeThorn::CTAnalysis::analysisRunTimeInSeconds() {
  long result;
#pragma omp critical(TIMER)
  {
    result = (long) (_analysisTimer.getTimeDuration().seconds());
  }
  return result;
}

CodeThorn::FunctionCallMapping* CodeThorn::CTAnalysis::getFunctionCallMapping() {
  ROSE_ASSERT(getCFAnalyzer());
  return getCFAnalyzer()->getFunctionCallMapping();
}

CodeThorn::FunctionCallMapping2* CodeThorn::CTAnalysis::getFunctionCallMapping2() {
  ROSE_ASSERT(getCFAnalyzer());
  return getCFAnalyzer()->getFunctionCallMapping2();
}

CodeThorn::Flow* CodeThorn::CTAnalysis::getFlow() {
  ROSE_ASSERT(_cfAnalysis);
  return _cfAnalysis->getIcfgFlow();
}

CodeThorn::InterFlow* CodeThorn::CTAnalysis::getInterFlow() {
  ROSE_ASSERT(_cfAnalysis);
  return _cfAnalysis->getInterFlow();
}

CodeThorn::EStateSet* CodeThorn::CTAnalysis::getEStateSet() { return &estateSet; }
CodeThorn::PStateSet* CodeThorn::CTAnalysis::getPStateSet() { return &pstateSet; }
TransitionGraph* CodeThorn::CTAnalysis::getTransitionGraph() { return &transitionGraph; }
std::list<CodeThorn::FailedAssertion> CodeThorn::CTAnalysis::getFirstAssertionOccurences(){return _firstAssertionOccurences;}

void CodeThorn::CTAnalysis::setCommandLineOptions(vector<string> clOptions) {
  _commandLineOptions=clOptions;
}
vector<string> CodeThorn::CTAnalysis::getCommandLineOptions() {
  return _commandLineOptions;
}

bool CodeThorn::CTAnalysis::isLTLRelevantEState(EStatePtr estate) {
  ROSE_ASSERT(estate);
  return ((estate)->io.isStdInIO()
          || (estate)->io.isStdOutIO()
          || (estate)->io.isStdErrIO()
          || (estate)->io.isFailedAssertIO());
}

std::string CodeThorn::CTAnalysis::typeSizeMappingToString() {
  return getVariableIdMapping()->typeSizeMappingToString();
}

void CodeThorn::CTAnalysis::setOptions(CodeThornOptions options) {
  _ctOpt=options;
  getEStateSet()->setExitOnHashError(_ctOpt.exitOnHashError);
  getPStateSet()->setExitOnHashError(_ctOpt.exitOnHashError);
  getTransitionGraph()->setExitOnHashError(_ctOpt.exitOnHashError);
}

CodeThornOptions& CodeThorn::CTAnalysis::getOptionsRef() {
  return _ctOpt;
}

CodeThornOptions CodeThorn::CTAnalysis::getOptions() {
  return _ctOpt;
}

void CodeThorn::CTAnalysis::setLtlOptions(LTLOptions ltlOptions) {
  _ltlOpt=ltlOptions;
}

LTLOptions& CodeThorn::CTAnalysis::getLtlOptionsRef() {
  return _ltlOpt;
}

//
// utility functions
//

//
// wrapper functions to follow
//
std::list<EStatePtr> CodeThorn::CTAnalysis::elistify() {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->elistify();
}

std::list<EStatePtr> CodeThorn::CTAnalysis::elistify(EState res) {
  ROSE_ASSERT(_estateTransferFunctions);
  return _estateTransferFunctions->elistify(res);
}

void CodeThorn::CTAnalysis::setFunctionResolutionModeInCFAnalysis(CodeThornOptions& ctOpt) {
  switch(int argVal=ctOpt.functionResolutionMode) {
  case 4: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_FUNCTION_CALL_MAPPING;break;
  default:
    cerr<<"Error: unsupported argument value of "<<argVal<<" for function-resolution-mode.";
    exit(1);
  }
  SAWYER_MESG(logger[TRACE])<<"TRACE: selected function resolution mode: "<<CFAnalysis::functionResolutionMode<<endl;

}

VariableId CodeThorn::CTAnalysis::globalVarIdByName(std::string varName) {
  ROSE_ASSERT(getEStateTransferFunctions());
  return getEStateTransferFunctions()->globalVarIdByName(varName);
}

CodeThorn::EStateTransferFunctions* CodeThorn::CTAnalysis::getEStateTransferFunctions() {
  return _estateTransferFunctions;
}

std::string CodeThorn::CTAnalysis::internalAnalysisReportToString() {
  stringstream ss;
  if(_ctOpt.getIntraProceduralFlag()) {
    uint32_t totalIntraFunctions=_statsIntraFinishedFunctions+_statsIntraUnfinishedFunctions;
    ss<<"Intra-procedural analysis"<<endl;
    ss<<"Number of finished functions  : "<<_statsIntraFinishedFunctions<<endl;
    ss<<"Number of canceled functions  : "<<_statsIntraUnfinishedFunctions<<" (max time: "<<_ctOpt.maxTime<<" seconds)"<<endl;
    ss<<"Total number of functions     : "<<totalIntraFunctions<<" ("<<getTotalNumberOfFunctions()<<")"<<endl;
  } else {
    ss<<"Inter-procedural analysis"<<endl;

    ss<<"Cyclic call graph    : ";
    if(auto topSort=getTopologicalSort())
      ss<<(topSort->isRecursive()?"yes":"no");
    else
      ss<<"not created";
    ss<<endl;

    ss<<"Call string length limit: "<<_ctOpt.callStringLength<<endl;
    ss<<"Max call string length  : "<<getEStateTransferFunctions()->getMaxCSLength()<<endl;
    ss<<"Max state size          : "<<getEStateTransferFunctions()->getMaxStateSize()<<endl;
  }
  ss<<hashSetConsistencyReport();
  return ss.str();
}

uint32_t CodeThorn::CTAnalysis::getTotalNumberOfFunctions() {
  return _totalNumberOfFunctions;
}

void CodeThorn::CTAnalysis::setTotalNumberOfFunctions(uint32_t num) {
  _totalNumberOfFunctions=num;
}

EStateWorkList* CodeThorn::CTAnalysis::getWorkList() {
  return estateWorkListCurrent;
}
EStateWorkList* CodeThorn::CTAnalysis::getWorkListNext() {
  return estateWorkListNext;
}
