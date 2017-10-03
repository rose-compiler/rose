/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"
#include "Analyzer.h"
#include "CommandLineOptions.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "AnalysisAbstractionLayer.h"
#include "SvcompWitness.h"
#include "CodeThornException.h"

#include <unordered_set>
#include <boost/bind.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "Timer.h"
#include "CollectionOperators.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility Analyzer::logger;

void Analyzer::printStatusMessage(string s, bool newLineFlag) {
#pragma omp critical (STATUS_MESSAGES)
  {
    if(args.getBool("status")) {
      cout<<s;
      if(newLineFlag) {
        cout<<endl;
      }
    }
  }
}

void Analyzer::printStatusMessage(string s) {
  printStatusMessage(s,false);
}

void Analyzer::printStatusMessageLine(string s) {
  printStatusMessage(s,true);
}

string Analyzer::lineColSource(SgNode* node) {
  return SgNodeHelper::sourceLineColumnToString(node)+": "+SgNodeHelper::nodeToString(node);
}

void Analyzer::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::Analyzer", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

string Analyzer::nodeToString(SgNode* node) {
  string textual;
  if(node->attributeExists("info"))
    textual=node->getAttribute("info")->toString()+":";
  return textual+SgNodeHelper::nodeToString(node);
}

bool Analyzer::isFunctionCallWithAssignment(Label lab,VariableId* varIdPtr){
  //return _labeler->getLabeler()->isFunctionCallWithAssignment(lab,varIdPtr);
  SgNode* node=getLabeler()->getNode(lab);
  if(getLabeler()->isFunctionCallLabel(lab)) {
    std::pair<SgVarRefExp*,SgFunctionCallExp*> p=SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp2(node);
    if(p.first) {
      if(varIdPtr) {
        *varIdPtr=variableIdMapping.variableId(p.first);
      }
      return true;
    }
  }
  return false;
}

void Analyzer::enableSVCompFunctionSemantics() {
  _svCompFunctionSemantics=true;
  exprAnalyzer.setSVCompFunctionSemantics(true);
  _externalErrorFunctionName="__VERIFIER_error";
  _externalNonDetIntFunctionName="__VERIFIER_nondet_int";
  _externalNonDetLongFunctionName="__VERIFIER_nondet_long";
  _externalExitFunctionName="exit";
}

void Analyzer::disableSVCompFunctionSemantics() {
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

void Analyzer::writeWitnessToFile(string filename) {
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

Analyzer::Analyzer():
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
  _svCompFunctionSemantics(false)
{
  initDiagnostics();
  _analysisTimer.start();
  _analysisTimer.stop();
  variableIdMapping.setModeVariableIdForEachArrayElement(true);
  for(int i=0;i<100;i++) {
    binaryBindingAssert.push_back(false);
  }
  estateWorkListCurrent = &estateWorkListOne;
  estateWorkListNext = &estateWorkListTwo;
  estateSet.max_load_factor(0.7);
  pstateSet.max_load_factor(0.7);
  constraintSetMaintainer.max_load_factor(0.7);
  resetInputSequenceIterator();
}

Analyzer::~Analyzer() {
}

size_t Analyzer::getNumberOfErrorLabels() {
  return _assertNodes.size();
}

string Analyzer::labelNameOfAssertLabel(Label lab) {
  string labelName;
  for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=_assertNodes.begin();i!=_assertNodes.end();++i)
    if(lab==getLabeler()->getLabel((*i).second))
      labelName=SgNodeHelper::getLabelName((*i).first);
  //assert(labelName.size()>0);
  return labelName;
}

bool Analyzer::isCppLabeledAssertLabel(Label lab) {
  return labelNameOfAssertLabel(lab).size()>0;
}
    

void Analyzer::setGlobalTopifyMode(GlobalTopifyMode mode) {
  _globalTopifyMode=mode;
}

void Analyzer::setExternalErrorFunctionName(std::string externalErrorFunctionName) {
  _externalErrorFunctionName=externalErrorFunctionName;
}

bool Analyzer::isPrecise() {
  if (isActiveGlobalTopify()) {
    return false;
  }
  if (args.getBool("explicit-arrays")==false && !args.getBool("rers-binary")) {
    return false;
  }
  return true;
}

bool Analyzer::isIncompleteSTGReady() {
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

ExprAnalyzer* Analyzer::getExprAnalyzer() {
  return &exprAnalyzer;
}

void Analyzer::setSolver(Solver* solver) {
  _solver=solver;
  _solver->setAnalyzer(this);
}

Solver* Analyzer::getSolver() {
  return _solver;
}

void Analyzer::runSolver() {
  _solver->run();
}

set<string> Analyzer::variableIdsToVariableNames(SPRAY::VariableIdSet s) {
  set<string> res;
  for(SPRAY::VariableIdSet::iterator i=s.begin();i!=s.end();++i) {
    res.insert((AbstractValue(*i)).toString(getVariableIdMapping()));
  }
  return res;
}

Analyzer::VariableDeclarationList Analyzer::computeUnusedGlobalVariableDeclarationList(SgProject* root) {
  list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(root);
  Analyzer::VariableDeclarationList usedGlobalVars=computeUsedGlobalVariableDeclarationList(root);
  for(Analyzer::VariableDeclarationList::iterator i=usedGlobalVars.begin();i!=usedGlobalVars.end();++i) {
    globalVars.remove(*i);
  }
  return globalVars;
}

Analyzer::VariableDeclarationList Analyzer::computeUsedGlobalVariableDeclarationList(SgProject* root) {
  if(SgProject* project=isSgProject(root)) {
    Analyzer::VariableDeclarationList usedGlobalVariableDeclarationList;
    list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
    VariableIdSet setOfUsedVars=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,getVariableIdMapping());
    int filteredVars=0;
    for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
      VariableId globalVarId=variableIdMapping.variableId(*i);
      if(setOfUsedVars.find(globalVarId)!=setOfUsedVars.end()) {
        usedGlobalVariableDeclarationList.push_back(*i);
      } else {
        filteredVars++;
      }
    }
    return usedGlobalVariableDeclarationList;
  } else {
    logger[ERROR] << "no global scope.";
    exit(1);
  }
}

void Analyzer::setStgTraceFileName(string filename) {
  _stg_trace_filename=filename;
  ofstream fout;
  fout.open(_stg_trace_filename.c_str());    // create new file/overwrite existing file
  fout<<"START"<<endl;
  fout.close();    // close. Will be used with append.
}

void Analyzer::recordTransition(const EState* sourceState, Edge e, const EState* targetState) {
  transitionGraph.add(Transition(sourceState,e,targetState));
}

void Analyzer::printStatusMessage(bool forceDisplay) {
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
    ss<<" "<<analyzerStateToString();
    ss<<endl;
    printStatusMessage(ss.str());
  }
}

string Analyzer::analyzerStateToString() {
  stringstream ss;
  ss<<"isPrec:"<<isPrecise();
  ss<<" ";
  ss<<"TopMode:"<<_globalTopifyMode;
  ss<<" ";
  ss<<"RBin:"<<args.getBool("rers-binary");
  ss<<" ";
  ss<<"incSTGReady:"<<isIncompleteSTGReady();
  return ss.str();
}

bool Analyzer::isInWorkList(const EState* estate) {
  for(EStateWorkList::iterator i=estateWorkListCurrent->begin();i!=estateWorkListCurrent->end();++i) {
    if(*i==estate) return true;
  }
  return false;
}

void Analyzer::incIterations() {
  if(isPrecise()) {
#pragma omp atomic
    _iterations+=1;
  } else {
#pragma omp atomic
    _approximated_iterations+=1;
  }
}

bool Analyzer::isLoopCondLabel(Label lab) {
  SgNode* node=getLabeler()->getNode(lab);
  return SgNodeHelper::isLoopCond(node);
}

void Analyzer::addToWorkList(const EState* estate) {
  ROSE_ASSERT(estate);
  ROSE_ASSERT(estateWorkListCurrent);
#pragma omp critical(ESTATEWL)
  {
    if(!estate) {
      logger[ERROR]<<"INTERNAL: null pointer added to work list."<<endl;
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
      logger[ERROR]<<"unknown exploration mode."<<endl;
      exit(1);
    }
  }
}

bool Analyzer::isActiveGlobalTopify() {
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
        args.setOption("rers-binary",false);
      }
    }
    return true;
  }
  return false;
}

void Analyzer::eventGlobalTopifyTurnedOn() {
  logger[TRACE] << "mode global-topify activated."<<endl;
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

  if(args.getBool("status")) {
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

void Analyzer::topifyVariable(PState& pstate, ConstraintSet& cset, AbstractValue varId) {
  pstate.writeTopToMemoryLocation(varId);
  //cset.removeAllConstraintsOfVar(varId);
}

EState Analyzer::createEState(Label label, PState pstate, ConstraintSet cset) {
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
  return estate;
}

EState Analyzer::createEState(Label label, PState pstate, ConstraintSet cset, InputOutput io) {
  EState estate=createEState(label,pstate,cset);
  estate.io=io;
  return estate;
}

bool Analyzer::isStartLabel(Label label) {
  return getTransitionGraph()->getStartLabel()==label;
}

// Avoid calling critical sections from critical sections:
// worklist functions do not use each other.
bool Analyzer::isEmptyWorkList() {
  bool res;
#pragma omp critical(ESTATEWL)
  {
    res=estateWorkListCurrent->empty();
  }
  return res;
}
const EState* Analyzer::topWorkList() {
  const EState* estate=0;
#pragma omp critical(ESTATEWL)
  {
    if(!estateWorkListCurrent->empty())
      estate=*estateWorkListCurrent->begin();
  }
  return estate;
}
const EState* Analyzer::popWorkList() {
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
const EState* Analyzer::takeFromWorkList() {
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
void Analyzer::swapWorkLists() {
  EStateWorkList* tmp = estateWorkListCurrent;
  estateWorkListCurrent = estateWorkListNext;
  estateWorkListNext = tmp;
  incIterations();
}

size_t Analyzer::memorySizeContentEStateWorkLists() {
  size_t mem = 0;
#pragma omp critical(ESTATEWL)
  {
    for (EStateWorkList::iterator i=estateWorkListOne.begin(); i!=estateWorkListOne.end(); ++i) {
      mem+=sizeof(*i);
    }
    for (EStateWorkList::iterator i=estateWorkListTwo.begin(); i!=estateWorkListTwo.end(); ++i) {
      mem+=sizeof(*i);
    }
  }
  return mem;
}

const EState* Analyzer::addToWorkListIfNew(EState estate) {
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
void Analyzer::setElementSize(VariableId variableId, SgType* elementType) {
  variableIdMapping.setElementSize(variableId,getTypeSizeMapping()->determineTypeSize(elementType));
}

EState Analyzer::analyzeVariableDeclaration(SgVariableDeclaration* decl,EState currentEState, Label targetLabel) {

  /*
    1) declaration of variable or array
    - AggregateInitializer (e.g. T a[]={1,2,3};)
    - AggregateInitializer (e.g. T a[5]={1,2,3};)
    - AssignInitializer (e.g. T x=1+2;)
    2) if array, determine size of array (from VariableIdMapping)
    3) if no size is provided, determine it from the initializer list (and add this information to the variableIdMapping - or update the variableIdMapping).
   */

  //cout<< "DEBUG: DECLARATION:"<<AstTerm::astTermWithNullValuesToString(decl)<<endl;
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  if(initName0!=nullptr) {
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      VariableId initDeclVarId=getVariableIdMapping()->variableId(initName);
      // not possible to support yet. getIntValue must succeed on declarations.
      if(false && variableValueMonitor.isHotVariable(this,initDeclVarId)) {
        PState newPState=*currentEState.pstate();
        newPState.writeTopToMemoryLocation(initDeclVarId);
        ConstraintSet cset=*currentEState.constraints();
        return createEState(targetLabel,newPState,cset);
      }

      if(variableIdMapping.hasArrayType(initDeclVarId) && args.getBool("explicit-arrays")==false) {
        // in case of a constant array the array (and its members) are not added to the state.
        // they are considered to be determined from the initializer without representing them
        // in the state
        // logger[DEBUG] <<"not adding array to PState."<<endl;
        PState newPState=*currentEState.pstate();
        ConstraintSet cset=*currentEState.constraints();
        return createEState(targetLabel,newPState,cset);
      }
      //SgName initDeclVarName=initDeclVar->get_name();
      //string initDeclVarNameString=initDeclVarName.getString();
      //cout << "INIT-DECLARATION: var:"<<initDeclVarNameString<<endl;
      //cout << "DECLARATION: var:"<<SgNodeHelper::nodeToString(decl)<<endl;
      ConstraintSet cset=*currentEState.constraints();
      SgInitializer* initializer=initName->get_initializer();
      if(initializer) {
        // has aggregate initializer
        if(isSgAggregateInitializer(initializer)) {
          // logger[DEBUG] <<"array-initializer found:"<<initializer->unparseToString()<<endl;
          PState newPState=*currentEState.pstate();
          int elemIndex=0;
          SgExpressionPtrList& initList=SgNodeHelper::getInitializerListOfAggregateDeclaration(decl);
          variableIdMapping.setNumberOfElements(initDeclVarId,initList.size());
          SgArrayType* arrayType=isSgArrayType(initializer->get_type());
          ROSE_ASSERT(arrayType);
          SgType* arrayElementType=arrayType->get_base_type();
          setElementSize(initDeclVarId,arrayElementType);
          for(SgExpressionPtrList::iterator i=initList.begin();i!=initList.end();++i) {
            AbstractValue arrayElemId=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex));
            SgExpression* exp=*i;
            SgAssignInitializer* assignInit=isSgAssignInitializer(exp);
            ROSE_ASSERT(assignInit);
            // TODO: model arbitrary RHS values (use:analyzeAssignRhs (see below))
            if(SgIntVal* intValNode=isSgIntVal(assignInit->get_operand_i())) {
              int intVal=intValNode->get_value();
              // logger[DEBUG] <<"initializing array element:"<<arrayElemId.toString()<<"="<<intVal<<endl;
              newPState.writeToMemoryLocation(arrayElemId,CodeThorn::AbstractValue(intVal));
            } else {
              logger[ERROR] <<"unsupported array initializer value:"<<exp->unparseToString()<<" AST:"<<AstTerm::astTermWithNullValuesToString(exp)<<endl;
              exit(1);
            }
            elemIndex++;
          }
          return createEState(targetLabel,newPState,cset);
        } else if(SgAssignInitializer* assignInitializer=isSgAssignInitializer(initializer)) {
          SgExpression* rhs=assignInitializer->get_operand_i();
          ROSE_ASSERT(rhs);
          //cout<<"DEBUG: assign initializer:"<<" lhs:"<<initDeclVarId.toString(getVariableIdMapping())<<" rhs:"<<assignInitializer->unparseToString()<<" decl-term:"<<AstTerm::astTermWithNullValuesToString(initName)<<endl;

          // set type info for initDeclVarId
          variableIdMapping.setNumberOfElements(initDeclVarId,1); // single variable
          SgType* variableType=initializer->get_type();
          setElementSize(initDeclVarId,variableType);
          
          // build lhs-value dependent on type of declared variable
          AbstractValue lhsAbstractAddress=AbstractValue(initDeclVarId); // creates a pointer to initDeclVar
          list<SingleEvalResultConstInt> res=exprAnalyzer.evalConstInt(rhs,currentEState,true);

          ROSE_ASSERT(res.size()==1);
          SingleEvalResultConstInt evalResult=*res.begin();

          EState estate=evalResult.estate;
          PState newPState=*estate.pstate();
          newPState.writeToMemoryLocation(lhsAbstractAddress,evalResult.value());
          ConstraintSet cset=*estate.constraints();
          return createEState(targetLabel,newPState,cset);
        } else {
          logger[ERROR] << "unsupported initializer in declaration: "<<decl->unparseToString()<<endl;
          exit(1);
        }
      } else {
        // no initializer (model default cases)
        ROSE_ASSERT(initName!=nullptr);
        SgArrayType* arrayType=isSgArrayType(initName->get_type());
        if(arrayType) {
          SgType* arrayElementType=arrayType->get_base_type();
          setElementSize(initDeclVarId,arrayElementType);
          variableIdMapping.setNumberOfElements(initDeclVarId,variableIdMapping.getArrayElementCount(arrayType));
        } else {
          // set type info for initDeclVarId
          variableIdMapping.setNumberOfElements(initDeclVarId,1); // single variable
          SgType* variableType=initName->get_type();
          setElementSize(initDeclVarId,variableType);
        }
        
        PState newPState=*currentEState.pstate();
        if(variableIdMapping.hasArrayType(initDeclVarId)) {
          // add default array elements to PState
          size_t length=variableIdMapping.getNumberOfElements(initDeclVarId);
          for(size_t elemIndex=0;elemIndex<length;elemIndex++) {
            AbstractValue newArrayElementId=AbstractValue::createAddressOfArrayElement(initDeclVarId,AbstractValue(elemIndex));
            // set default init value
            newPState.writeTopToMemoryLocation(newArrayElementId);
          }

        } else if(variableIdMapping.hasClassType(initDeclVarId)) {
          // not supported yet. Declarations may exist in header files,
          // therefore silently ignore it here, but if will cause an
          // error later as read/write of class variables is not
          // allowed yet in expressions.
        } else if(variableIdMapping.hasPointerType(initDeclVarId)) {
          // create pointer value and set it to top (=any value possible (uninitialized pointer variable declaration))
          AbstractValue pointerVal=AbstractValue::createAddressOfVariable(initDeclVarId);
          newPState.writeTopToMemoryLocation(pointerVal);
        } else {
          // set it to top (=any value possible (uninitialized)) for
          // all remaining cases. It will become an error-path once
          // all cases are addressed explicitly above.
          newPState.writeTopToMemoryLocation(initDeclVarId);
        }
        return createEState(targetLabel,newPState,cset);
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

bool Analyzer::isFailedAssertEState(const EState* estate) {
  if(estate->io.isFailedAssertIO())
    return true;
  if(_treatStdErrLikeFailedAssert) {
    return estate->io.isStdErrIO();
  }
  return false;
}

bool Analyzer::isVerificationErrorEState(const EState* estate) {
  if(estate->io.isVerificationError())
    return true;
  return false;
}

EState Analyzer::createFailedAssertEState(EState estate, Label target) {
  EState newEState=estate;
  newEState.io.recordFailedAssert();
  newEState.setLabel(target);
  return newEState;
}

// creates a state that represents that the verification error function was called
// the edge is the external call edge
EState Analyzer::createVerificationErrorEState(EState estate, Label target) {
  EState newEState=estate;
  newEState.io.recordVerificationError();
  newEState.setLabel(target);
  return newEState;
}

list<SgNode*> Analyzer::listOfAssertNodes(SgProject* root) {
  list<SgNode*> assertNodes;
  list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=funDefs.begin();
      i!=funDefs.end();
      ++i) {
    RoseAst ast(*i);
    for(RoseAst::iterator j=ast.begin();j!=ast.end();++j) {
      if(SgNodeHelper::Pattern::matchAssertExpr(*j)) {
        assertNodes.push_back(*j);
      }
    }
  }
  return assertNodes;
}

void Analyzer::initLabeledAssertNodes(SgProject* root) {
  _assertNodes=listOfLabeledAssertNodes(root);
}

list<pair<SgLabelStatement*,SgNode*> > Analyzer::listOfLabeledAssertNodes(SgProject* root) {
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

const EState* Analyzer::processCompleteNewOrExisting(const EState* es) {
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

const PState* Analyzer::processNew(PState& s) {
  return pstateSet.processNew(s);
}
const PState* Analyzer::processNewOrExisting(PState& s) {
  return pstateSet.processNewOrExisting(s);
}

const EState* Analyzer::processNew(EState& s) {
  return estateSet.processNew(s);
}

const EState* Analyzer::processNewOrExisting(EState& estate) {
  return estateSet.processNewOrExisting(estate);
}

const ConstraintSet* Analyzer::processNewOrExisting(ConstraintSet& cset) {
  return constraintSetMaintainer.processNewOrExisting(cset);
}

EStateSet::ProcessingResult Analyzer::process(EState& estate) {
  return estateSet.process(estate);
}

std::list<EState> Analyzer::elistify() {
  std::list<EState> resList;
  return resList;
}

std::list<EState> Analyzer::elistify(EState res) {
  //assert(res.state);
  //assert(res.constraints());
  std::list<EState> resList;
  resList.push_back(res);
  return resList;
}

list<EState> Analyzer::transferEdgeEState(Edge edge, const EState* estate) {
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
  } else if(SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)
     ||edge.isType(EDGE_EXTERNAL)
     ||edge.isType(EDGE_CALLRETURN)) {
    // this is supposed to be dead code meanwhile
    ROSE_ASSERT(false);
    // special case external call
    //EState newEState=currentEState;
    //newEState.setLabel(edge.target());
    //return elistify(newEState);
  } else if(SgVariableDeclaration* decl=isSgVariableDeclaration(nextNodeToAnalyze1)) {
    return transferVariableDeclaration(decl,edge,estate);
  } else if(isSgExprStatement(nextNodeToAnalyze1) || SgNodeHelper::isForIncExpr(nextNodeToAnalyze1)) {
    return transferExprStmt(nextNodeToAnalyze1, edge, estate);
  } else {
    // nothing to analyze, just create new estate (from same State) with target label of edge
    // can be same state if edge is a backedge to same cfg node
    EState newEState=currentEState;
    newEState.setLabel(edge.target());
    return elistify(newEState);
  }
}

list<EState> Analyzer::transferIdentity(Edge edge, const EState* estate) {
  // nothing to analyze, just create new estate (from same State) with target label of edge
  // can be same state if edge is a backedge to same cfg node
  EState newEState=*estate;
  newEState.setLabel(edge.target());
  return elistify(newEState);
}

void Analyzer::initializeCommandLineArgumentsInState(PState& initialPState) {
  // TODO1: add formal paramters of solo-function
  // SgFunctionDefinition* startFunRoot: node of function
  // estate=analyzeVariableDeclaration(SgVariableDeclaration*,estate,estate.label());
  string functionName=SgNodeHelper::getFunctionName(startFunRoot);
  SgInitializedNamePtrList& initNamePtrList=SgNodeHelper::getFunctionDefinitionFormalParameterList(startFunRoot);
  VariableId argcVarId;
  VariableId argvVarId;
  size_t mainFunArgNr=0;
  for(SgInitializedNamePtrList::iterator i=initNamePtrList.begin();i!=initNamePtrList.end();++i) {
    VariableId varId=variableIdMapping.variableId(*i);
    if(functionName=="main") {
      //string varName=getVariableIdMapping()->variableName(varId)) {
      switch(mainFunArgNr) {
      case 0: argcVarId=varId;break;
      case 1: argvVarId=varId;break;
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
  if(_commandLineOptions.size()>0) {

    // create command line option array argv and argc in initial pstate
    int argc=0;
    VariableId argvArrayMemoryId=variableIdMapping.createAndRegisterNewMemoryRegion("$argvmem",(int)_commandLineOptions.size());
    AbstractValue argvAddress=AbstractValue::createAddressOfArray(argvArrayMemoryId);
    initialPState.writeToMemoryLocation(argvVarId,argvAddress);
    for (auto argvElem:_commandLineOptions) {
      cout<<"Initial state: "
          <<variableIdMapping.variableName(argvVarId)<<"["<<argc+1<<"]: "
          <<argvElem;
      int regionSize=(int)string(argvElem).size();
      cout<<" size: "<<regionSize<<endl;

      stringstream memRegionName;
      memRegionName<<"$argv"<<argc<<"mem";
      VariableId argvElemArrayMemoryId=variableIdMapping.createAndRegisterNewMemoryRegion(memRegionName.str(),regionSize);
      AbstractValue argvElemAddress=AbstractValue::createAddressOfArray(argvElemArrayMemoryId);
      initialPState.writeToMemoryLocation(AbstractValue::createAddressOfArrayElement(argvVarId,argc),argvElemAddress);

      // copy concrete command line argument strings char by char to State
      for(int j=0;_commandLineOptions[argc][j]!=0;j++) {
        cout<<"Copying: @argc="<<argc<<" char: "<<_commandLineOptions[argc][j]<<endl;
        AbstractValue argvElemAddressWithIndexOffset;
        AbstractValue AbstractIndex=AbstractValue(j);
        argvElemAddressWithIndexOffset=argvElemAddress+AbstractIndex;
        initialPState.writeToMemoryLocation(argvElemAddressWithIndexOffset,AbstractValue(_commandLineOptions[argc][j]));
      }
      argc++;
    }
    cout<<"Initial state argc:"<<argc<<endl;
    AbstractValue abstractValueArgc(argc);
    initialPState.writeToMemoryLocation(argcVarId,abstractValueArgc);
  }
}

void Analyzer::initializeSolver(std::string functionToStartAt,SgNode* root, bool oneFunctionOnly) {
  ROSE_ASSERT(root);
  resetInputSequenceIterator();
  std::string funtofind=functionToStartAt;
  RoseAst completeast(root);
  startFunRoot=completeast.findFunctionByName(funtofind);
  if(startFunRoot==0) {
    std::cout << "Function '"<<funtofind<<"' not found.\n";
    exit(1);
  } else {
    logger[TRACE]<< "INFO: starting at function '"<<funtofind<<"'."<<endl;
  }
  logger[TRACE]<< "INIT: Initializing AST node info."<<endl;
  initAstNodeInfo(root);

  logger[TRACE]<< "INIT: Creating Labeler."<<endl;
  Labeler* labeler= new CTIOLabeler(root,getVariableIdMapping());
  logger[TRACE]<< "INIT: Initializing VariableIdMapping."<<endl;
  exprAnalyzer.setVariableIdMapping(getVariableIdMapping());
  logger[TRACE]<< "INIT: Creating CFAnalysis."<<endl;
  cfanalyzer=new CFAnalysis(labeler,true);
  getLabeler()->setExternalNonDetIntFunctionName(_externalNonDetIntFunctionName);
  getLabeler()->setExternalNonDetLongFunctionName(_externalNonDetLongFunctionName);

  // logger[DEBUG]<< "mappingLabelToLabelProperty: "<<endl<<getLabeler()->toString()<<endl;
  logger[TRACE]<< "INIT: Building CFGs."<<endl;

  if(oneFunctionOnly)
    flow=cfanalyzer->flow(startFunRoot);
  else
    flow=cfanalyzer->flow(root);

  logger[TRACE]<< "STATUS: Building CFGs finished."<<endl;
  if(args.getBool("reduce-cfg")) {
    int cnt=cfanalyzer->optimizeFlow(flow);
    logger[TRACE]<< "INIT: CFG reduction OK. (eliminated "<<cnt<<" nodes)"<<endl;
  }
  logger[TRACE]<< "INIT: Intra-Flow OK. (size: " << flow.size() << " edges)"<<endl;
  if(oneFunctionOnly) {
    logger[TRACE]<<"INFO: analyzing one function only."<<endl;
  }
  InterFlow interFlow=cfanalyzer->interFlow(flow);
  logger[TRACE]<< "INIT: Inter-Flow OK. (size: " << interFlow.size()*2 << " edges)"<<endl;
  cfanalyzer->intraInterFlow(flow,interFlow);
  logger[TRACE]<< "INIT: ICFG OK. (size: " << flow.size() << " edges)"<<endl;

#if 0
  if(args.getBool("reduce-cfg")) {
    int cnt=cfanalyzer->inlineTrivialFunctions(flow);
    cout << "INIT: CFG reduction OK. (inlined "<<cnt<<" functions; eliminated "<<cnt*4<<" nodes)"<<endl;
  }
#endif

  // create empty state
  PState initialPState;
  initializeCommandLineArgumentsInState(initialPState);
  const PState* initialPStateStored=processNew(initialPState);
  ROSE_ASSERT(initialPStateStored);
  logger[TRACE]<< "INIT: initial state(stored): "<<initialPStateStored->toString()<<endl;
  ROSE_ASSERT(cfanalyzer);
  ConstraintSet cset;
  const ConstraintSet* emptycsetstored=constraintSetMaintainer.processNewOrExisting(cset);
  Label startLabel=cfanalyzer->getLabel(startFunRoot);
  transitionGraph.setStartLabel(startLabel);
  transitionGraph.setAnalyzer(this);

  EState estate(startLabel,initialPStateStored,emptycsetstored);

  if(SgProject* project=isSgProject(root)) {
    logger[TRACE]<< "STATUS: Number of global variables: ";
    list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
    logger[TRACE]<< globalVars.size()<<endl;

    VariableIdSet setOfUsedVars=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,&variableIdMapping);

    logger[TRACE]<< "STATUS: Number of used variables: "<<setOfUsedVars.size()<<endl;

    int filteredVars=0;
    for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
      VariableId globalVarId=variableIdMapping.variableId(*i);
      // TODO: investigate why array variables get filtered (but should not)
      if(true || (setOfUsedVars.find(globalVarId)!=setOfUsedVars.end() && _variablesToIgnore.find(globalVarId)==_variablesToIgnore.end())) {
        globalVarName2VarIdMapping[variableIdMapping.variableName(variableIdMapping.variableId(*i))]=variableIdMapping.variableId(*i);
        estate=analyzeVariableDeclaration(*i,estate,estate.label());
      } else {
        filteredVars++;
      }
    }
    logger[TRACE]<< "STATUS: Number of filtered variables for initial pstate: "<<filteredVars<<endl;
    if(_variablesToIgnore.size()>0)
      logger[TRACE]<< "STATUS: Number of ignored variables for initial pstate: "<<_variablesToIgnore.size()<<endl;
  } else {
    logger[TRACE]<< "INIT: no global scope.";
  }

  const EState* currentEState=processNew(estate);
  ROSE_ASSERT(currentEState);
  variableValueMonitor.init(currentEState);
  addToWorkList(currentEState);
  // cout << "INIT: start state: "<<currentEState->toString(&variableIdMapping)<<endl;

  if(args.getBool("rers-binary")) {
    //initialize the global variable arrays in the linked binary version of the RERS problem
    logger[DEBUG]<< "init of globals with arrays for "<< _numberOfThreadsToUse << " threads. " << endl;
    RERS_Problem::rersGlobalVarsArrayInit(_numberOfThreadsToUse);
    RERS_Problem::createGlobalVarAddressMaps(this);
  }

  logger[TRACE]<< "INIT: finished."<<endl;
}

set<const EState*> Analyzer::transitionSourceEStateSetOfLabel(Label lab) {
  set<const EState*> estateSet;
  for(TransitionGraph::iterator j=transitionGraph.begin();j!=transitionGraph.end();++j) {
    if((*j)->source->label()==lab)
      estateSet.insert((*j)->source);
  }
  return estateSet;
}

// PState is maintainted to allow for assignments on the rhs
// TODO: change lhsVar to lhsAbstractValue
PState Analyzer::analyzeAssignRhsExpr(PState currentPState,VariableId lhsVar, SgNode* rhs, ConstraintSet& cset) {
  // TODO DECLARATION:SgVariableDeclaration(null,SgInitializedName(SgAssignInitializer(SgCastExp(SgIntVal))))
  //                  rhs=SgCastExp(SgIntVal) // not handled yet in below function
  //AbstractValue lhsValue=AbstractValue(lhsVar);
  ROSE_ASSERT(false);
}

// TODO: this function should be implemented with a call of ExprAnalyzer::evalConstInt
PState Analyzer::analyzeAssignRhs(PState currentPState,VariableId lhsVar, SgNode* rhs, ConstraintSet& cset) {
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
      rhsIntVal=currentPState.readFromMemoryLocation(rhsVarId);
    } else {
      if(variableIdMapping.hasArrayType(rhsVarId) && args.getBool("explicit-arrays")==false) {
        // in case of an array the id itself is the pointer value
        ROSE_ASSERT(rhsVarId.isValid());
        rhsIntVal=rhsVarId.getIdCode();
      } else {
        logger[WARN]<< "access to variable "<<variableIdMapping.uniqueLongVariableName(rhsVarId)<< " id:"<<rhsVarId.toString()<<" on rhs of assignment, but variable does not exist in state. Initializing with top."<<endl;
        rhsIntVal=CodeThorn::Top();
        isRhsIntVal=true; 
      }
    }
  }
  PState newPState=currentPState;

  // handle pointer assignment
  if(variableIdMapping.hasPointerType(lhsVar)) {
    //cout<<"DEBUG: "<<lhsVar.toString()<<" = "<<rhs->unparseToString()<<" : "<<astTermWithNullValuesToString(rhs)<<" : ";
    //cout<<"LHS: pointer variable :: "<<lhsVar.toString()<<endl;
    if(SgVarRefExp* rhsVarExp=isSgVarRefExp(rhs)) {
      VariableId rhsVarId=variableIdMapping.variableId(rhsVarExp);
      if(variableIdMapping.hasArrayType(rhsVarId)) {
        //cout<<" of array type.";
        // we use the id-code as int-value (points-to info)
        int idCode=rhsVarId.getIdCode();
        newPState.writeToMemoryLocation(lhsVar,CodeThorn::AbstractValue(AbstractValue(idCode)));
        //cout<<" id-code: "<<idCode;
        return newPState;
      } else {
        logger[ERROR] <<"RHS: unknown : type: ";
        logger[ERROR]<<AstTerm::astTermWithNullValuesToString(isSgExpression(rhs)->get_type());
        exit(1);
      }
      cout<<endl;
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
    if( (newPState.readFromMemoryLocation(lhsVar).operatorEq(rhsIntVal)).isTrue() ) {
      // update of existing variable with same value
      // => no state change
      return newPState;
    } else {
      // update of existing variable with new value
      //newPState[lhsVar]=rhsIntVal;
      newPState.writeToMemoryLocation(lhsVar,rhsIntVal);
      if((!rhsIntVal.isTop() && !isRhsVar))
        cset.removeAllConstraintsOfVar(lhsVar);
      return newPState;
    }
  } else {
    if(_variablesToIgnore.size()>0 && (_variablesToIgnore.find(lhsVar)!=_variablesToIgnore.end())) {
      // nothing to do because variable is ignored
    } else {
      // new variable with new value.
      newPState.writeToMemoryLocation(lhsVar,rhsIntVal);
    }
    // no update of constraints because no constraints can exist for a new variable
    return newPState;
  }
  // make sure, we only create/propagate contraints if a non-const value is assigned or if a variable is on the rhs.
  if(!rhsIntVal.isTop() && !isRhsVar)
    cset.removeAllConstraintsOfVar(lhsVar);
  return newPState;
}

void Analyzer::initAstNodeInfo(SgNode* node) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    AstNodeInfo* attr=new AstNodeInfo();
    (*i)->addNewAttribute("info",attr);
  }
}

void Analyzer::generateAstNodeInfo(SgNode* node) {
  ROSE_ASSERT(node);
  if(!cfanalyzer) {
    logger[ERROR]<< "DFAnalyzer: no cfanalyzer found."<<endl;
    exit(1);
  }
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin().withoutNullValues();i!=ast.end();++i) {
    ROSE_ASSERT(*i);
    AstNodeInfo* attr=dynamic_cast<AstNodeInfo*>((*i)->getAttribute("info"));
    if(attr) {
      if(cfanalyzer->getLabel(*i)!=Label()) {
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
bool Analyzer::checkTransitionGraph() {
  set<const EState*> ess=transitionGraph.estateSet();
  bool ok=isConsistentEStatePtrSet(ess);
  ok=ok && getTransitionGraph()->checkConsistency();
  return ok;
}

bool Analyzer::checkEStateSet() {
  for(EStateSet::iterator i=estateSet.begin();i!=estateSet.end();++i) {
    if(estateSet.estateId(*i)==NO_ESTATE || (*i)->label()==Label()) {
      logger[ERROR]<< "estateSet inconsistent. "<<endl;
      logger[ERROR]<< "  label   :"<<(*i)->label()<<endl;
      logger[ERROR]<< "   estateId: "<<estateSet.estateId(*i)<<endl;
      return false;
    }
  }
  logger[TRACE]<< "INFO: estateSet of size "<<estateSet.size()<<" consistent."<<endl;
  return true;
}

bool Analyzer::isConsistentEStatePtrSet(set<const EState*> estatePtrSet)  {
  for(set<const EState*>::iterator i=estatePtrSet.begin();i!=estatePtrSet.end();++i) {
    if(estateSet.estateId(*i)==NO_ESTATE || (*i)->label()==Label()) {
      logger[ERROR]<< "estatePtrSet inconsistent. "<<endl;
      logger[ERROR]<< "  label   :"<<(*i)->label()<<endl;
      logger[ERROR]<< "   estateId: "<<estateSet.estateId(*i)<<endl;
      return false;
    }
  }
  logger[TRACE]<< "INFO: estatePtrSet of size "<<estatePtrSet.size()<<" consistent."<<endl;
  return true;
}

CTIOLabeler* Analyzer::getLabeler() const {
  CTIOLabeler* ioLabeler=dynamic_cast<CTIOLabeler*>(cfanalyzer->getLabeler());
  ROSE_ASSERT(ioLabeler);
  return ioLabeler;
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
void Analyzer::resetAnalysis() {
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
void Analyzer::storeStgBackup() {
  backupTransitionGraph = transitionGraph;
}

/*! 
  * \author Marc Jasper
  * \date 2014, 2015.
 */
void Analyzer::swapStgWithBackup() {
  TransitionGraph tTemp = transitionGraph;
  transitionGraph = backupTransitionGraph;
  backupTransitionGraph = tTemp;
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
void Analyzer::reduceStgToInOutStates() {
  function<bool(const EState*)> predicate = [](const EState* s) { 
    return s->io.isStdInIO() || s->io.isStdOutIO();
  };
  _stgReducer.reduceStgToStatesSatisfying(predicate);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
void Analyzer::reduceStgToInOutAssertStates() {
  function<bool(const EState*)> predicate = [](const EState* s) { 
    return s->io.isStdInIO() || s->io.isStdOutIO() 
    || s->io.isFailedAssertIO();
  };
  _stgReducer.reduceStgToStatesSatisfying(predicate);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
void Analyzer::reduceStgToInOutAssertErrStates() {
  function<bool(const EState*)> predicate = [](const EState* s) { 
    return s->io.isStdInIO() || s->io.isStdOutIO() 
    || s->io.isFailedAssertIO() || s->io.isStdErrIO();
  };
  _stgReducer.reduceStgToStatesSatisfying(predicate);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
void Analyzer::reduceStgToInOutAssertWorklistStates() {
  // copy elements from worklist into hashset (faster access within the predicate)
  unordered_set<const EState*> worklistSet(estateWorkListCurrent->begin(), estateWorkListCurrent->end());
  function<bool(const EState*)> predicate = [&worklistSet](const EState* s) { 
    return s->io.isStdInIO() || s->io.isStdOutIO() 
    || s->io.isFailedAssertIO() || (worklistSet.find(s) != worklistSet.end());
  };
  _stgReducer.reduceStgToStatesSatisfying(predicate);
}

int Analyzer::reachabilityAssertCode(const EState* currentEStatePtr) {
  if(args.getBool("rers-binary")) {
    PState* pstate = const_cast<PState*>( (currentEStatePtr)->pstate() );
    int outputVal = pstate->readFromMemoryLocation(globalVarIdByName("output")).getIntValue();
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

void Analyzer::setSkipSelectedFunctionCalls(bool defer) {
  _skipSelectedFunctionCalls=true; 
  exprAnalyzer.setSkipSelectedFunctionCalls(true);
}

void Analyzer::set_finished(std::vector<bool>& v, bool val) {
  ROSE_ASSERT(v.size()>0);
  for(vector<bool>::iterator i=v.begin();i!=v.end();++i) {
    *i=val;
  }
}

bool Analyzer::all_false(std::vector<bool>& v) {
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

void Analyzer::mapGlobalVarInsert(std::string name, int* addr) {
  mapGlobalVarAddress[name]=addr;
  mapAddressGlobalVar[addr]=name;
}

void Analyzer::setCompoundIncVarsSet(set<AbstractValue> ciVars) {
  _compoundIncVarsSet=ciVars;
}

void Analyzer::setSmallActivityVarsSet(set<AbstractValue> saVars) {
  _smallActivityVarsSet=saVars;
}

void Analyzer::setAssertCondVarsSet(set<AbstractValue> acVars) {
  _assertCondVarsSet=acVars;
}


long Analyzer::analysisRunTimeInSeconds() {
  long result;
#pragma omp critical(TIMER)
  {
    result = (long) (_analysisTimer.getElapsedTimeInMilliSec() / 1000);
  }
  return result;
}

std::list<EState> Analyzer::transferFunctionCall(Edge edge, const EState* estate) {
   // 1) obtain actual parameters from source
   // 2) obtain formal parameters from target
   // 3) eval each actual parameter and assign result to formal parameter in state
   // 4) create new estate
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  // ad 1)
  SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(getLabeler()->getNode(edge.source()));
  ROSE_ASSERT(funCall);
  string funName=SgNodeHelper::getFunctionName(funCall);
  // handling of error function (TODO: generate dedicated state (not failedAssert))

  if(args.getBool("rers-binary")) {
    // if rers-binary function call is selected then we skip the static analysis for this function (specific to rers)
    string funName=SgNodeHelper::getFunctionName(funCall);
    if(funName=="calculate_output") {
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
    VariableId formalParameterVarId=variableIdMapping.variableId(formalParameterName);
    // VariableName varNameString=name->get_name();
    SgExpression* actualParameterExpr=*j;
    ROSE_ASSERT(actualParameterExpr);
    // check whether the actualy parameter is a single variable: In this case we can propagate the constraints of that variable to the formal parameter.
    // pattern: call: f(x), callee: f(int y) => constraints of x are propagated to y
    VariableId actualParameterVarId;
    ROSE_ASSERT(actualParameterExpr);
    if(exprAnalyzer.variable(actualParameterExpr,actualParameterVarId)) {
      // propagate constraint from actualParamterVarId to formalParameterVarId
      cset.addAssignEqVarVar(formalParameterVarId,actualParameterVarId);
    }
    // general case: the actual argument is an arbitrary expression (including a single variable)
    // we use for the third parameter "false": do not use constraints when extracting values.
    // Consequently, formalparam=actualparam remains top, even if constraints are available, which
    // would allow to extract a constant value (or a range (when relational constraints are added)).
    list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evalConstInt(actualParameterExpr,currentEState,false);
    ROSE_ASSERT(evalResultList.size()>0);
    list<SingleEvalResultConstInt>::iterator resultListIter=evalResultList.begin();
    SingleEvalResultConstInt evalResult=*resultListIter;
    if(evalResultList.size()>1) {
      logger[ERROR] <<"multi-state generating operators in function call parameters not supported."<<endl;
      exit(1);
    }
    // above evalConstInt does not use constraints (par3==false). Therefore top vars remain top vars (which is what we want here)
    //newPState[formalParameterVarId]=evalResult.value();
    newPState.writeToMemoryLocation(formalParameterVarId,evalResult.value());
    ++i;++j;
  }
  // assert must hold if #formal-params==#actual-params (TODO: default values)
  ROSE_ASSERT(i==formalParameters.end() && j==actualParameters.end());
  // ad 4
  return elistify(createEState(edge.target(),newPState,cset));
}

std::list<EState> Analyzer::transferFunctionCallLocalEdge(Edge edge, const EState* estate) {
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();
  if(args.getBool("rers-binary")) {
    // logger[DEBUG]<<"ESTATE: "<<estate->toString(&variableIdMapping)<<endl;
    SgNode* nodeToAnalyze=getLabeler()->getNode(edge.source());
    if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nodeToAnalyze)) {
      ROSE_ASSERT(funCall);
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(funName=="calculate_output") {
        // RERS global vars binary handling
        PState _pstate=*estate->pstate();
        RERS_Problem::rersGlobalVarsCallInit(this,_pstate, omp_get_thread_num());
#if 0
        //input variable passed as a parameter (obsolete since usage of script "transform_globalinputvar")
        int rers_result=RERS_Problem::calculate_output(argument);
        (void) RERS_Problem::calculate_output(argument);
#else
        (void) RERS_Problem::calculate_output( omp_get_thread_num() );
        int rers_result=RERS_Problem::output[omp_get_thread_num()];
#endif
        //logger[DEBUG]<< "Called calculate_output("<<argument<<")"<<" :: result="<<rers_result<<endl;
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
	  newPstate.writeToMemoryLocation(globalVarIdByName("output"),
                                       CodeThorn::AbstractValue(rers_result));
	  EState _eState=createEState(edge.target(),newPstate,_cset,_io);
	  return elistify(_eState);
        }
        RERS_Problem::rersGlobalVarsCallReturnInit(this,_pstate, omp_get_thread_num());
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
	  _pstate.writeToMemoryLocation(lhsVarId,AbstractValue(rers_result));
	  ConstraintSet _cset=*estate->constraints();
	  _cset.removeAllConstraintsOfVar(lhsVarId);
	  EState _eState=createEState(edge.target(),_pstate,_cset,newio);
	  return elistify(_eState);
	} else {
	  ConstraintSet _cset=*estate->constraints();
	  EState _eState=createEState(edge.target(),_pstate,_cset,newio);
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

std::list<EState> Analyzer::transferReturnStmt(Edge edge, const EState* estate) {
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(edge.source());
  ROSE_ASSERT(nextNodeToAnalyze1);
  SgNode* expr=SgNodeHelper::getFirstChild(nextNodeToAnalyze1);

  if(isSgNullExpression(expr)) {
      // return without expr
    return elistify(createEState(edge.target(),currentPState,cset));
  } else {
    VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
    {
      returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));
    }
    PState newPState=analyzeAssignRhs(currentPState,
                                      returnVarId,
                                      expr,
                                      cset);
    return elistify(createEState(edge.target(),newPState,cset));
  }
}

std::list<EState> Analyzer::transferFunctionCallReturn(Edge edge, const EState* estate) {
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  // 1. we handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(edge.source());
  ROSE_ASSERT(nextNodeToAnalyze1);

  if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
    // case 1: return f(); pass estate trough
    EState newEState=currentEState;
    newEState.setLabel(edge.target());
    return elistify(newEState);
  } else if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nextNodeToAnalyze1)) {
    // case 2: x=f(); bind variable x to value of $return
    if(args.getBool("rers-binary")) {
      if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
        string funName=SgNodeHelper::getFunctionName(funCall);
        if(funName=="calculate_output") {
          EState newEState=currentEState;
          newEState.setLabel(edge.target());
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
      returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));
    }

    if(newPState.varExists(returnVarId)) {
      AbstractValue evalResult=newPState.readFromMemoryLocation(returnVarId);
      //newPState[lhsVarId]=evalResult;
      newPState.writeToMemoryLocation(lhsVarId,evalResult);

      cset.addAssignEqVarVar(lhsVarId,returnVarId);
      newPState.deleteVar(returnVarId); // remove $return from state
      cset.removeAllConstraintsOfVar(returnVarId); // remove constraints of $return

      return elistify(createEState(edge.target(),newPState,cset));
    } else {
      // no $return variable found in state. This can be the case for an extern function.
      // alternatively a $return variable could be added in the external function call to
      // make this handling here uniform
      // for external functions no constraints are generated in the call-return node
      return elistify(createEState(edge.target(),newPState,cset));
    }
  } else if(SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(nextNodeToAnalyze1)) {
    // case 3: f(); remove $return from state (discard value)
    PState newPState=*currentEState.pstate();
    VariableId returnVarId;
#pragma omp critical(VAR_ID_MAPPING)
    {
      returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));
    }
    // no effect if $return does not exist
    newPState.deleteVar(returnVarId);
    cset.removeAllConstraintsOfVar(returnVarId); // remove constraints of $return
    //ConstraintSet cset=*currentEState.constraints; ???
    return elistify(createEState(edge.target(),newPState,cset));
  } else {
    logger[FATAL] << "function call-return from unsupported call type:"<<nextNodeToAnalyze1->unparseToString()<<endl;
    exit(1);
  }
}

std::list<EState> Analyzer::transferFunctionExit(Edge edge, const EState* estate) {
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
    VariableIdMapping::VariableIdSet localVars=variableIdMapping.determineVariableIdsOfVariableDeclarations(varDecls);
    SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
    VariableIdMapping::VariableIdSet formalParams=variableIdMapping.determineVariableIdsOfSgInitializedNames(formalParamInitNames);
    VariableIdMapping::VariableIdSet vars=localVars+formalParams;
    set<string> names=variableIdsToVariableNames(vars);

    for(VariableIdMapping::VariableIdSet::iterator i=vars.begin();i!=vars.end();++i) {
      VariableId varId=*i;
      newPState.deleteVar(varId);
      cset.removeAllConstraintsOfVar(varId);
    }
    // ad 3)
    return elistify(createEState(edge.target(),newPState,cset));
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

std::list<EState> Analyzer::transferFunctionCallExternal(Edge edge, const EState* estate) {
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();

  // 1. we handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(edge.source());
  ROSE_ASSERT(nextNodeToAnalyze1);

  //cout<<"DEBUG: external function call (statement): "<<nextNodeToAnalyze1->unparseToString()<<endl;

  InputOutput newio;
  Label lab=getLabeler()->getLabel(nextNodeToAnalyze1);

  VariableId varId;
  bool isFunctionCallWithAssignmentFlag=isFunctionCallWithAssignment(lab,&varId);

  SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1);

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
      newCSet.removeAllConstraintsOfVar(varId);
      list<EState> resList;
      int newValue;
      if(_inputSequenceIterator!=_inputSequence.end()) {
        newValue=*_inputSequenceIterator;
        ++_inputSequenceIterator;
        //cout<<"INFO: input sequence value: "<<newValue<<endl;
      } else {
        return resList; // return no state (this ends the analysis)
      }
      if(args.getBool("input-values-as-constraints")) {
        newCSet.removeAllConstraintsOfVar(varId);
        //newPState[varId]=CodeThorn::Top();
        newPState.writeTopToMemoryLocation(varId);
        newCSet.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,AbstractValue(newValue)));
        ROSE_ASSERT(newCSet.size()>0);
      } else {
        newCSet.removeAllConstraintsOfVar(varId);
        //newPState[varId]=AbstractValue(newValue);
        newPState.writeToMemoryLocation(varId,AbstractValue(newValue));
      }
      newio.recordVariable(InputOutput::STDIN_VAR,varId);
      EState estate=createEState(edge.target(),newPState,newCSet,newio);
      resList.push_back(estate);
      // logger[DEBUG]<< "created "<<_inputVarValues.size()<<" input states."<<endl;
      return resList;
    } else {
      if(_inputVarValues.size()>0) {
        // update state (remove all existing constraint on that variable and set it to top)
        PState newPState=*currentEState.pstate();
        ConstraintSet newCSet=*currentEState.constraints();
        newCSet.removeAllConstraintsOfVar(varId);
        list<EState> resList;
        for(set<int>::iterator i=_inputVarValues.begin();i!=_inputVarValues.end();++i) {
          PState newPState=*currentEState.pstate();
          if(args.getBool("input-values-as-constraints")) {
            newCSet.removeAllConstraintsOfVar(varId);
            //newPState[varId]=CodeThorn::Top();
            newPState.writeTopToMemoryLocation(varId);
            newCSet.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,AbstractValue(*i)));
            assert(newCSet.size()>0);
          } else {
            newCSet.removeAllConstraintsOfVar(varId);
            // new input value must be const (otherwise constraints must be used)
            //newPState[varId]=AbstractValue(*i);
            newPState.writeToMemoryLocation(varId,AbstractValue(*i));
          }
          newio.recordVariable(InputOutput::STDIN_VAR,varId);
          EState estate=createEState(edge.target(),newPState,newCSet,newio);
          resList.push_back(estate);
        }
        // logger[DEBUG]<< "created "<<_inputVarValues.size()<<" input states."<<endl;
        return resList;
      } else {
        // without specified input values (default mode: analysis performed for all possible input values)
        // update state (remove all existing constraint on that variable and set it to top)
        PState newPState=*currentEState.pstate();
        ConstraintSet newCSet=*currentEState.constraints();
        // update input var
        newCSet.removeAllConstraintsOfVar(varId);
        newPState.writeTopToMemoryLocation(varId);
        newio.recordVariable(InputOutput::STDIN_VAR,varId);
        return elistify(createEState(edge.target(),newPState,newCSet,newio));
      }
    }
  }
  int constvalue=0;
  if(getLabeler()->isStdOutVarLabel(lab,&varId)) {
    newio.recordVariable(InputOutput::STDOUT_VAR,varId);
    ROSE_ASSERT(newio.var==varId);
  } else if(getLabeler()->isStdOutConstLabel(lab,&constvalue)) {
    newio.recordConst(InputOutput::STDOUT_CONST,constvalue);
  } else if(getLabeler()->isStdErrLabel(lab,&varId)) {
    newio.recordVariable(InputOutput::STDERR_VAR,varId);
    ROSE_ASSERT(newio.var==varId);
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
          list<SingleEvalResultConstInt> res=exprAnalyzer.evalFunctionCall(funCall,currentEState,true);
          // build new estate(s) from single eval result list
          list<EState> estateList;
          for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
            EState estate=(*i).estate;
            PState newPState=*estate.pstate();
            ConstraintSet cset=*estate.constraints();
            estateList.push_back(createEState(edge.target(),newPState,cset));
          }
          return estateList;
        }
      }
    }
  }

  // for all other external functions we use identity as transfer function
  EState newEState=currentEState;
  newEState.io=newio;
  newEState.setLabel(edge.target());
  return elistify(newEState);
}

std::list<EState> Analyzer::transferVariableDeclaration(SgVariableDeclaration* decl, Edge edge, const EState* estate) {
  return elistify(analyzeVariableDeclaration(decl,*estate, edge.target()));
}

std::list<EState> Analyzer::transferExprStmt(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate) {
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

list<EState> Analyzer::transferIncDecOp(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate) {
  EState currentEState=*estate;
  SgNode* nextNodeToAnalyze3=SgNodeHelper::getUnaryOpChild(nextNodeToAnalyze2);
  VariableId var;
  if(exprAnalyzer.variable(nextNodeToAnalyze3,var)) {
    list<SingleEvalResultConstInt> res=exprAnalyzer.evalConstInt(nextNodeToAnalyze3,currentEState,true);
    ROSE_ASSERT(res.size()==1); // must hold for currently supported limited form of ++,--
    list<SingleEvalResultConstInt>::iterator i=res.begin();
    EState estate=(*i).estate;
    PState newPState=*estate.pstate();
    ConstraintSet cset=*estate.constraints();

    AbstractValue varVal=newPState.readFromMemoryLocation(var);
    AbstractValue const1=1;
    switch(nextNodeToAnalyze2->variantT()) {
    case V_SgPlusPlusOp:
      varVal=varVal+const1; // overloaded binary + operator
      break;
    case V_SgMinusMinusOp:
      varVal=varVal-const1; // overloaded binary - operator
      break;
    default:
      logger[ERROR] << "Operator-AST:"<<AstTerm::astTermToMultiLineString(nextNodeToAnalyze2,2)<<endl;
      logger[ERROR] << "Operator:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze2)<<endl;
      logger[ERROR] << "Operand:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze3)<<endl;
      logger[ERROR] <<"programmatic error in handling of inc/dec operators."<<endl;
      exit(1);
    }
    //newPState[var]=varVal;
    newPState.writeToMemoryLocation(var,varVal);

    if(!(*i).result.isTop())
      cset.removeAllConstraintsOfVar(var);
    list<EState> estateList;
    estateList.push_back(createEState(edge.target(),newPState,cset));
    return estateList;
  } else {
    throw CodeThorn::Exception("Error: currently inc/dec operators are only supported for variables.");
  }
}

std::list<EState> Analyzer::transferAssignOp(SgAssignOp* nextNodeToAnalyze2, Edge edge, const EState* estate) {
  //cout<<"DEBUG: @ "<<nextNodeToAnalyze2->unparseToString()<<endl;
  EState currentEState=*estate;
  SgNode* lhs=SgNodeHelper::getLhs(nextNodeToAnalyze2);
  SgNode* rhs=SgNodeHelper::getRhs(nextNodeToAnalyze2);
  list<SingleEvalResultConstInt> res=exprAnalyzer.evalConstInt(rhs,currentEState,true);
  list<EState> estateList;
  for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
    VariableId lhsVar;
    bool isLhsVar=exprAnalyzer.variable(lhs,lhsVar);
    if(isLhsVar) {
      EState estate=(*i).estate;
      PState newPState=*estate.pstate();
      ConstraintSet cset=*estate.constraints();
      // only update integer variables. Ensure values of floating-point variables are not computed
      if(variableIdMapping.hasIntegerType(lhsVar)) {
        newPState.writeToMemoryLocation(lhsVar,(*i).result);
      } else if(variableIdMapping.hasPointerType(lhsVar)) {
        // we assume here that only arrays (pointers to arrays) are assigned
        //newPState[lhsVar]=(*i).result;
        newPState.writeToMemoryLocation(lhsVar,(*i).result);
      }
      if(!(*i).result.isTop()) {
        cset.removeAllConstraintsOfVar(lhsVar);
      }
      estateList.push_back(createEState(edge.target(),newPState,cset));
    } else if(isSgPntrArrRefExp(lhs)) {
      // for now we ignore array refs on lhs
      // TODO: assignments in index computations of ignored array ref
      // see ExprAnalyzer.C: case V_SgPntrArrRefExp:
      // since nothing can change (because of being ignored) state remains the same
      VariableIdMapping* _variableIdMapping=&variableIdMapping;
      EState estate=(*i).estate;
      PState oldPState=*estate.pstate();
      ConstraintSet oldcset=*estate.constraints();
      if(getSkipArrayAccesses()) {
        // TODO: remove constraints on array-element(s) [currently no constraints are computed for arrays]
        estateList.push_back(createEState(edge.target(),oldPState,oldcset));
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
              arrayPtrValue=pstate2.readFromMemoryLocation(ptr);
              //cout<<"DEBUG: arrayPtrValue: "<<arrayPtrValue.toString(_variableIdMapping)<<endl;
              // convert integer to VariableId
              if(arrayPtrValue.isTop()||arrayPtrValue.isBot()) {
                logger[ERROR] <<"Error: unsupported feature: "<<nextNodeToAnalyze2->unparseToString()<<arrayPtrValue.toString(_variableIdMapping)<<" array index is top or bot. Not supported yet."<<endl;
                exit(1);
              }
              // logger[DEBUG]<<"defering pointer-to-array: ptr:"<<_variableIdMapping->variableName(arrayVarId);
            } else {
              logger[ERROR] <<"lhs array access: pointer variable does not exist in PState."<<endl;
              exit(1);
            }
          } else {
            logger[ERROR] <<"lhs array access: unkown type of array or pointer."<<endl;
            exit(1);
          }
          AbstractValue arrayElementId;
          //AbstractValue aValue=(*i).value();
          list<SingleEvalResultConstInt> res=exprAnalyzer.evalConstInt(indexExp,currentEState,true);
          ROSE_ASSERT(res.size()==1); // TODO: temporary restriction
          AbstractValue indexValue=(*(res.begin())).value();
          AbstractValue arrayPtrPlusIndexValue=AbstractValue::operatorAdd(arrayPtrValue,indexValue);
          //cout<<"DEBUG: arrayPtrPlusIndexValue: "<<arrayPtrPlusIndexValue.toString(_variableIdMapping)<<endl;

          // TODO: rewrite to use AbstractValue only
          {
            VariableId arrayVarId2=arrayPtrPlusIndexValue.getVariableId();
            int index2=arrayPtrPlusIndexValue.getIndexIntValue();
            if(!exprAnalyzer.checkArrayBounds(arrayVarId2,index2)) {
              cerr<<"Program error detected at "<<SgNodeHelper::sourceLineColumnToString(nextNodeToAnalyze2)<<" : write access out of bounds."<<endl;// ["<<lhs->unparseToString()<<"]"<<endl;
            }
          }
          arrayElementId=arrayPtrPlusIndexValue;
          //cout<<"DEBUG: arrayElementId: "<<arrayElementId.toString(_variableIdMapping)<<endl;
          //logger[TRACE]<<"arrayElementVarId:"<<arrayElementId.toString()<<":"<<_variableIdMapping->variableName(arrayVarId)<<" Index:"<<index<<endl;
          ROSE_ASSERT(!arrayElementId.isBot());
          // read value of variable var id (same as for VarRefExp - TODO: reuse)
          // TODO: check whether arrayElementId (or array) is a constant array (arrayVarId)
          if(pstate2.varExists(arrayElementId)) {
            // TODO: handle constraints
            pstate2.writeToMemoryLocation(arrayElementId,(*i).value()); // *i is assignment-rhs evaluation result
            estateList.push_back(createEState(edge.target(),pstate2,oldcset));
          } else {
            // check that array is constant array (it is therefore ok that it is not in the state)
            //logger[TRACE] <<"lhs array-access index does not exist in state (creating it now). Array element id:"<<arrayElementId.toString(_variableIdMapping)<<" PState size:"<<pstate2.size()<<endl;
            //logger[TRACE]<<"PState:"<<pstate2.toString(getVariableIdMapping())<<endl;
            pstate2.writeToMemoryLocation(arrayElementId,(*i).value()); // *i is assignment-rhs evaluation result
            estateList.push_back(createEState(edge.target(),pstate2,oldcset));
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
      list<SingleEvalResultConstInt> resLhs=exprAnalyzer.evalConstInt(lhsOperand,currentEState,true);
      if(resLhs.size()>1) {
        throw CodeThorn::Exception("more than 1 execution path (probably due to abstraction) in operand's expression of pointer dereference operator on lhs of "+nextNodeToAnalyze2->unparseToString());
      }
      ROSE_ASSERT(resLhs.size()==1);
      AbstractValue lhsPointerValue=(*resLhs.begin()).result;
      if(lhsPointerValue.isTop()) {
        // special case. Expr evaluates to top (should be dereferenced)
        PState pstate2=*(estate->pstate());
        estateList.push_back(createEState(edge.target(),pstate2,*(estate->constraints())));
      }
      if(!(lhsPointerValue.isPtr())) {
        cerr<<"Error: not a pointer value (or top) in dereference operator: lhs-value:"<<lhsPointerValue.toLhsString(getVariableIdMapping())<<" lhs: "<<lhs->unparseToString()<<endl;
        exit(1);
      }
      //cout<<"DEBUG: lhsPointerValue:"<<lhsPointerValue.toString(getVariableIdMapping())<<endl;
      PState pstate2=*(estate->pstate());
      pstate2.writeToMemoryLocation(lhsPointerValue,(*i).result);
      estateList.push_back(createEState(edge.target(),pstate2,*(estate->constraints())));
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

list<EState> Analyzer::transferTrueFalseEdge(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate) {
  EState currentEState=*estate;
  Label newLabel;
  PState newPState;
  ConstraintSet newCSet;
  list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evalConstInt(nextNodeToAnalyze2,currentEState,true);
  list<EState> newEStateList;
  for(list<SingleEvalResultConstInt>::iterator i=evalResultList.begin();
      i!=evalResultList.end();
      ++i) {
    SingleEvalResultConstInt evalResult=*i;
    if((evalResult.isTrue() && edge.isType(EDGE_TRUE)) || (evalResult.isFalse() && edge.isType(EDGE_FALSE)) || evalResult.isTop()) {
      // pass on EState
      newLabel=edge.target();
      newPState=*evalResult.estate.pstate();
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
      EState estate=createEState(newLabel,newPState,newCSet);
      newEStateList.push_back(estate);
    } else {
      // we determined not to be on an execution path, therefore do nothing (do not add any result to resultlist)
    }
  }
  return newEStateList;
}

void Analyzer::setTypeSizeMapping(SgTypeSizeMapping* typeSizeMapping) {
  AbstractValue::setTypeSizeMapping(typeSizeMapping);
}

SgTypeSizeMapping* Analyzer::getTypeSizeMapping() {
  return AbstractValue::getTypeSizeMapping();
}

void Analyzer::setCommandLineOptions(vector<string> clOptions) {
  _commandLineOptions=clOptions;
}
