/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"

#include "Analyzer.h"
#include "CommandLineOptions.h"
#include <fstream>
#include <unistd.h>
#include "Miscellaneous.h"

using namespace CodeThorn;

#include "CollectionOperators.h"

Analyzer::Analyzer():startFunRoot(0),cfanalyzer(0),_displayDiff(10000),_numberOfThreadsToUse(1),_ltlVerifier(2),_semanticFoldThreshold(5000) {
  for(int i=0;i<62;i++) {
    binaryBindingAssert.push_back(false);
  }
}

set<string> Analyzer::variableIdsToVariableNames(VariableIdMapping::VariableIdSet s) {
  set<string> res;
  for(VariableIdMapping::VariableIdSet::iterator i=s.begin();i!=s.end();++i) {
    res.insert(variableIdMapping.uniqueLongVariableName(*i));
  }
  return res;
}

string Analyzer::nodeToString(SgNode* node) {
  string textual;
  if(node->attributeExists("info"))
    textual=node->getAttribute("info")->toString()+":";
  return textual+SgNodeHelper::nodeToString(node);
}

Analyzer::~Analyzer() {
  // intentionally empty, nothing to free explicitly
}

void Analyzer::recordTransition(const EState* sourceState, Edge e, const EState* targetState) {
  transitionGraph.add(Transition(sourceState,e,targetState));
}

void Analyzer::printStatusMessage(bool forceDisplay) {
  // forceDisplay currently only turns on or off
  
  // report we are alife
  stringstream ss;
  if(forceDisplay) {
    ss <<color("white")<<"Number of pstates/estates/trans/csets: ";
    ss <<color("magenta")<<pstateSet.size()
       <<color("white")<<"/"
       <<color("cyan")<<estateSet.size()
       <<color("white")<<"/"
       <<color("blue")<<getTransitionGraph()->size()
       <<color("white")<<"/"
       <<color("yellow")<<constraintSetMaintainer.size()
       <<color("white")<<"";
    ss<<endl;
    cout<<ss.str();
  }
}

bool Analyzer::isInWorkList(const EState* estate) {
  for(EStateWorkList::iterator i=estateWorkList.begin();i!=estateWorkList.end();++i) {
    if(*i==estate) return true;
  }
  return false;
}

void Analyzer::addToWorkList(const EState* estate) { 
#pragma omp critical
  {
    if(!estate) {
      cerr<<"INTERNAL ERROR: null pointer added to work list."<<endl;
      exit(1);
    }
    estateWorkList.push_back(estate); 
  }
}

EState Analyzer::createEState(Label label, PState pstate, ConstraintSet cset) {
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

bool Analyzer::isLTLRelevantLabel(Label label) {
  bool t;
  t=(getLabeler()->isStdInLabel(label)
     || getLabeler()->isStdOutLabel(label)
     || getLabeler()->isStdErrLabel(label)
     || isTerminationRelevantLabel(label)
     )
    && (getTransitionGraph()->getStartLabel()!=label) // for simplicity, we keep the start state
    ;
  //cout << "INFO: L"<<label<<": "<<SgNodeHelper::nodeToString(getLabeler()->getNode(label))<< "LTL: "<<t<<endl;
  return t;
}

set<const EState*> Analyzer::nonLTLRelevantEStates() {
  set<const EState*> res;
  set<const EState*> allestates=transitionGraph.estateSet();
  for(set<const EState*>::iterator i=allestates.begin();i!=allestates.end();++i) {
    if(!isLTLRelevantLabel((*i)->label())) {
      res.insert(*i);

      // MS: deactivated this check because it impacts performance dramatically
#if 0
      if(estateSet.estateId(*i)==NO_ESTATE) {
        cerr<< "WARNING: no estate :estateId="<<estateSet.estateId(*i)<<endl;
      }
#endif
    }
  }
  return res;
}

bool Analyzer::isTerminationRelevantLabel(Label label) {
  Labeler* lab=getLabeler();
  if(SgNodeHelper::isLoopCond(lab->getNode(label))) {
    //cout << "DEBUG: is termination relevant node: "<<SgNodeHelper::nodeToString(lab->getNode(label))<<endl;
    return true;
  }
  return false;
}

// We want to avoid calling critical sections from critical sections:
// therefore all worklist functions do not use each other.
bool Analyzer::isEmptyWorkList() { 
  bool res;
  #pragma omp critical
  {
    res=(estateWorkList.size()==0);
  }
  return res;
}
const EState* Analyzer::topWorkList() {
  const EState* estate=0;
#pragma omp critical
  {
    if(estateWorkList.size()>0)
      estate=*estateWorkList.begin();
  }
  return estate;
}
const EState* Analyzer::popWorkList() {
  const EState* estate=0;
  #pragma omp critical
  {
    if(estateWorkList.size()>0)
      estate=*estateWorkList.begin();
    if(estate)
      estateWorkList.pop_front();
  }
  return estate;
}
const EState* Analyzer::takeFromWorkList() {
  const EState* co=0;
#pragma omp critical
  {
  if(estateWorkList.size()>0) {
    co=*estateWorkList.begin();
    estateWorkList.pop_front();
  }
  }
  return co;
}

#define PARALLELIZE_BRANCHES

void Analyzer::runSolver1() {
  size_t prevStateSetSize=0; // force immediate report at start
  omp_set_num_threads(_numberOfThreadsToUse);
  omp_set_dynamic(1);
  int threadNum;
  vector<const EState*> workVector(_numberOfThreadsToUse);
  printStatusMessage(true);
  while(1) {
    int workers;
    for(workers=0;workers<_numberOfThreadsToUse;++workers) {
      if(!(workVector[workers]=popWorkList()))
        break;
    }
    if(workers==0)
      break; // we are done
    
    if(_displayDiff && (estateSet.size()>(prevStateSetSize+_displayDiff))) {
      printStatusMessage(true);
      prevStateSetSize=estateSet.size();
    }
#pragma omp parallel for private(threadNum) shared(workVector)
    for(int j=0;j<workers;++j) {
      threadNum=omp_get_thread_num();
      const EState* currentEStatePtr=workVector[j];
      if(!currentEStatePtr) {
        cerr<<"Error: thread "<<threadNum<<" finished prematurely. Bailing out. "<<endl;
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
        exit(1);
      }
      assert(currentEStatePtr);
      
      Flow edgeSet=flow.outEdges(currentEStatePtr->label());
      //cerr << "DEBUG: edgeSet size:"<<edgeSet.size()<<endl;
#ifdef PARALLELIZE_BRANCHES
      // we can simplify this by adding the proper function to Flow.
      Flow::iterator i=edgeSet.begin();
      int edgeNum=edgeSet.size();
      vector<const Edge*> edgeVec(edgeNum);
      for(int edge_i=0;edge_i<edgeNum;++edge_i) {
        edgeVec[edge_i]=&(*i++);
      }
#pragma omp parallel for
      for(int edge_i=0; edge_i<edgeNum;edge_i++) {
        Edge e=*edgeVec[edge_i]; // *i
#else
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
#endif
          list<EState> newEStateList;
          newEStateList=transferFunction(e,currentEStatePtr);
          //cout << "DEBUG: transfer at edge:"<<e.toString()<<" succ="<<newEStateList.size()<< endl;
          for(list<EState>::iterator nesListIter=newEStateList.begin();
              nesListIter!=newEStateList.end();
              ++nesListIter) {
            EState newEState=*nesListIter;
            assert(newEState.label()!=Labeler::NO_LABEL);
            if((!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState))) {
              HSetMaintainer<EState,EStateHashFun>::ProcessingResult pres=process(newEState);
              const EState* newEStatePtr=pres.second;
              if(pres.first==true)
                addToWorkList(newEStatePtr);            
              recordTransition(currentEStatePtr,e,newEStatePtr);
            }
            if((!newEState.constraints()->disequalityExists()) && (isFailedAssertEState(&newEState))) {
              // failed-assert end-state: do not add to work list but do add it to the transition graph
              const EState* newEStatePtr;
              newEStatePtr=processNewOrExisting(newEState);
              recordTransition(currentEStatePtr,e,newEStatePtr);        
              
              if(boolOptions["report-failed-assert"]) {
#pragma omp critical
                {
                  cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
                }
              }
              if(_csv_assert_live_file.size()>0) {
                string name=labelNameOfAssertLabel(currentEStatePtr->label());
                if(name=="globalError")
                  name="error_60";
                name=name.substr(6,name.size()-6);
                std::ofstream fout;
                // csv_assert_live_file is the member-variable of analyzer
#pragma omp critical
                {
                  fout.open(_csv_assert_live_file.c_str(),ios::app);    // open file for appending
                  assert (!fout.fail( ));
                  fout << name << ",yes,9"<<endl;
                  //cout << "REACHABLE ASSERT FOUND: "<< name << ",yes,9"<<endl;
                  
                  fout.close(); 
                }
              } // if
            }
          } // end of loop on transfer function return-estates
#ifdef PARALLELIZE_BRANCHES
        } // edgeset-parallel for
#else
      } // just for proper auto-formatting in emacs
#endif
    } // worklist-parallel for
  } // while
  printStatusMessage(true);
  cout << "analysis finished (worklist is empty)."<<endl;
}
  
const EState* Analyzer::addToWorkListIfNew(EState estate) {
  EStateSet::ProcessingResult res=process(estate);
  if(res.first==true) {
    const EState* newEStatePtr=res.second;
    assert(newEStatePtr);
    addToWorkList(newEStatePtr);
    return newEStatePtr;
  } else {
    //cout << "DEBUG: EState already exists. Not added:"<<estate.toString()<<endl;
    const EState* existingEStatePtr=res.second;
    assert(existingEStatePtr);
    return existingEStatePtr;
  }
}

EState Analyzer::analyzeVariableDeclaration(SgVariableDeclaration* decl,EState currentEState, Label targetLabel) {
  //cout << "INFO1: we are at "<<astTermWithNullValuesToString(nextNodeToAnalyze1)<<endl;
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  if(initName0) {
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      SgSymbol* initDeclVar=initName->search_for_symbol_from_symbol_table();
      assert(initDeclVar);
      VariableId initDeclVarId=getVariableIdMapping()->variableId(initDeclVar);
      SgName initDeclVarName=initDeclVar->get_name();
      string initDeclVarNameString=initDeclVarName.getString();
      //cout << "INIT-DECLARATION: var:"<<initDeclVarNameString<<endl;
      //cout << "DECLARATION: var:"<<SgNodeHelper::nodeToString(decl)<<endl;
      SgInitializer* initializer=initName->get_initializer();
      //assert(initializer);
      ConstraintSet cset=*currentEState.constraints();
      SgAssignInitializer* assignInitializer=0;
      if(initializer && (assignInitializer=isSgAssignInitializer(initializer))) {
        //cout << "initializer found:"<<endl;
        SgExpression* rhs=assignInitializer->get_operand_i();
        assert(rhs);
        PState newPState=analyzeAssignRhs(*currentEState.pstate(),initDeclVarId,rhs,cset);
        return createEState(targetLabel,newPState,cset);
      } else {
        //cout << "no initializer (OK)."<<endl;
        PState newPState=*currentEState.pstate();
        newPState[initDeclVarId]=AType::Top();
        return createEState(targetLabel,newPState,cset);
      }
    } else {
      cerr << "Error: in declaration (@initializedName) no variable found ... bailing out."<<endl;
      exit(1);
    }
  } else {
    cerr << "Error: in declaration: no variable found ... bailing out."<<endl;
    exit(1);
  }
}

// this function has been moved to VariableIdMapping: TODO eliminate this function here
VariableIdMapping::VariableIdSet Analyzer::determineVariableIdsOfVariableDeclarations(set<SgVariableDeclaration*> varDecls) {
  VariableIdMapping::VariableIdSet resultSet;
  for(set<SgVariableDeclaration*>::iterator i=varDecls.begin();i!=varDecls.end();++i) {
    SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(*i);
    if(sym) {
      resultSet.insert(variableIdMapping.variableId(sym));
    }
  }
  return resultSet;
}

// this function has been moved to VariableIdMapping: TODO eliminate this function here
VariableIdMapping::VariableIdSet Analyzer::determineVariableIdsOfSgInitializedNames(SgInitializedNamePtrList& namePtrList) {
  VariableIdMapping::VariableIdSet resultSet;
  for(SgInitializedNamePtrList::iterator i=namePtrList.begin();i!=namePtrList.end();++i) {
    assert(*i);
    SgSymbol* sym=SgNodeHelper::getSymbolOfInitializedName(*i);
    if(sym) {
      resultSet.insert(variableIdMapping.variableId(sym));
    }
  }
  return resultSet;
}

#if 0
bool Analyzer::isAssertExpr(SgNode* node) {
  if(isSgExprStatement(node)) {
    node=SgNodeHelper::getExprStmtChild(node);
    // TODO: refine this to also check for name, paramters, etc.
    if(isSgConditionalExp(node))
      return true;
  }
  return false;
}
#endif

bool Analyzer::isFailedAssertEState(const EState* estate) {
  return estate->io.op==InputOutput::FAILED_ASSERT;
}

EState Analyzer::createFailedAssertEState(EState estate, Label target) {
  EState newEState=estate;
  newEState.io.recordFailedAssert();
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
          assertNodes.push_back(make_pair(labStmt,*j));
        }
      }
      prev=j;
    }
  }
  return assertNodes;
}

InputOutput::OpType Analyzer::ioOp(const EState* estate) const {
  Label lab=estate->label();
  if(getLabeler()->isStdInLabel(lab)) return InputOutput::STDIN_VAR;
  if(getLabeler()->isStdOutLabel(lab)) return InputOutput::STDOUT_VAR;
  if(getLabeler()->isStdErrLabel(lab)) return InputOutput::STDERR_VAR;
  return InputOutput::NONE;
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
  if(boolOptions["tg-ltl-reduced"]) {
    // experimental: passing of params (we can avoid the copying)
    EStateSet::ProcessingResult res=process(estate.label(),*estate.pstate(),*estate.constraints(),estate.io);
    assert(res.second);
    return res.second;
  } else {
    return estateSet.processNewOrExisting(estate);
  }
}

const ConstraintSet* Analyzer::processNewOrExisting(ConstraintSet& cset) {
  return constraintSetMaintainer.processNewOrExisting(cset);
}

EStateSet::ProcessingResult Analyzer::process(EState& estate) {
  if(boolOptions["tg-ltl-reduced"]) {
    // experimental passing of params (we can avoid the copying)
    return process(estate.label(),*estate.pstate(),*estate.constraints(),estate.io);
  } else {
    return estateSet.process(estate);
  }
}

EStateSet::ProcessingResult Analyzer::process(Label label, PState pstate, ConstraintSet cset, InputOutput io) {
  if(isLTLRelevantLabel(label) || io.op!=InputOutput::NONE || (!boolOptions["tg-ltl-reduced"])) {
    const PState* newPStatePtr=processNewOrExisting(pstate);
    const ConstraintSet* newCSetPtr=processNewOrExisting(cset);
    EState newEState=EState(label,newPStatePtr,newCSetPtr,io);
    return estateSet.process(newEState);
  } else {
    //cout << "INFO: allocating temporary estate."<<endl;
    // the following checks are not neccessary but ensure that we reuse pstates and constraint sets
#if 0
    const PState* newPStatePtr=pstateSet.determine(pstate);
    PState* newPStatePtr2=0;
    if(!newPStatePtr) {
      newPStatePtr2=new PState();
      *newPStatePtr2=pstate;
    } else {
      newPStatePtr2=const_cast<PState*>(newPStatePtr);
    }
    ConstraintSet* newCSetPtr=constraintSetMaintainer.determine(cset);
    ConstraintSet* newCSetPtr2;
    if(!newCSetPtr) {
      newCSetPtr2=new ConstraintSet();
      *newCSetPtr2=cset;
    } else {
      newCSetPtr2=const_cast<ConstraintSet*>(newCSetPtr);
    }
#else
    // TODO: temporary states must be marked to be able to free them later (or: any non-maintained state is considered a temporary state)
    PState* newPStatePtr2=new PState();
    *newPStatePtr2=pstate;
    ConstraintSet* newCSetPtr2=new ConstraintSet();
    *newCSetPtr2=cset;
#endif
    EState newEState=EState(label,newPStatePtr2,newCSetPtr2,io);
    const EState* newEStatePtr=estateSet.determine(newEState);
    if(!newEStatePtr) {
      // new estate (was not stored but was not inserted (this case does not exist for maintained state)
      // therefore we handle it as : has been inserted (hence, all tmp-states are considered to be not equal)
      newEStatePtr=new EState(label,newPStatePtr2,newCSetPtr2,io);
      return make_pair(true,newEStatePtr);
    } else {
      return make_pair(false,newEStatePtr);
    }

    return estateSet.process(newEState);
  }
  throw "Error: Analyzer::processNewOrExisting: programmatic error.";
}

list<EState> elistify() {
  list<EState> resList;
  return resList;
}
list<EState> elistify(EState res) {
  //assert(res.state);
  //assert(res.constraints());
  list<EState> resList;
  resList.push_back(res);
  return resList;
}

list<EState> Analyzer::transferFunction(Edge edge, const EState* estate) {
  assert(edge.source==estate->label());
  // we do not pass information on the local edge
  if(edge.isType(EDGE_LOCAL)) {
#ifdef RERS_SPECIALIZATION
    if(boolOptions["rers-binary"]) {
      SgNode* nodeToAnalyze=getLabeler()->getNode(edge.source);
      if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nodeToAnalyze)) {
        assert(funCall);
        string funName=SgNodeHelper::getFunctionName(funCall);
        if(funName=="calculate_output") {
          SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
          SgExpressionPtrList::iterator j=actualParameters.begin();
          SgExpression* actualParameterExpr=*j;
          assert(actualParameterExpr);
          VariableId actualParameterVarId;
          if(exprAnalyzer.variable(actualParameterExpr,actualParameterVarId)) {
            PState _pstate=*estate->pstate();
            AType::CppCapsuleConstIntLattice aval_capsule=_pstate[actualParameterVarId];
            AValue aval=aval_capsule.getValue();
            int argument=aval.getIntValue();
            //cout << "DEBUG: argument:"<<argument<<endl;
            // RERS global vars binary handling
            RERS_Problem::rersGlobalVarsCallInit(this,_pstate);
            //cout << "DEBUG: global vars initialized before call"<<endl;
            int rers_result=RERS_Problem::calculate_output(argument); 
            //cout << "DEBUG: Called calculate_output("<<argument<<")"<<" :: result="<<rers_result<<endl;
            if(rers_result<=-100) {
              // we found an assert
              // = -1000 : rers globalError
              // = rers_result*(-1)-100 : rers error-number
              if(rers_result==-1000) {
                binaryBindingAssert[61]=true;
              } else {
                int index=((rers_result+100)*(-1));
                assert(index>=0 && index <=60);
                binaryBindingAssert[index]=true;
              }
              return elistify();
            }
            RERS_Problem::rersGlobalVarsCallReturnInit(this,_pstate);
            // TODO: _pstate[VariableId(output)]=rers_result;
            if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nodeToAnalyze)) {
              SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nodeToAnalyze));
              VariableId lhsVarId;
              bool isLhsVar=exprAnalyzer.variable(lhs,lhsVarId);
              assert(isLhsVar); // must hold
              //cout << "DEBUG: lhsvar:rers-result:"<<lhsVarId.toString()<<"="<<rers_result<<endl;
              _pstate[lhsVarId]=AType::CppCapsuleConstIntLattice(rers_result);
              ConstraintSet _cset=*estate->constraints();
              _cset.removeAllConstraintsOfVar(lhsVarId);
              EState _eState=createEState(edge.target,_pstate,_cset);
              return elistify(_eState);
            }
            cout <<"PState:"<< _pstate<<endl;
            cerr<<"RERS-MODE: call of unknown function."<<endl;
            exit(1);
            // _pstate now contains the current state obtained from the binary
          }
        }
        //cout << "DEBUG: @LOCAL_EDGE: function call:"<<SgNodeHelper::nodeToString(funCall)<<endl;
      }
    }
#endif
    return elistify();
  }
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate();
  ConstraintSet cset=*currentEState.constraints();
  // 1. we handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(edge.source);
  assert(nextNodeToAnalyze1);
  // handle assert(0)
  if(SgNodeHelper::Pattern::matchAssertExpr(nextNodeToAnalyze1)) {
    return elistify(createFailedAssertEState(currentEState,edge.target));
  }

  if(edge.isType(EDGE_CALL)) {
    // 1) obtain actual parameters from source
    // 2) obtain formal parameters from target
    // 3) eval each actual parameter and assign result to formal parameter in state
    // 4) create new estate

    // ad 1)
    SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(getLabeler()->getNode(edge.source));
    assert(funCall);

#ifdef RERS_SPECIALIZATION
    if(boolOptions["rers-binary"]) {
      // if rers-binary function call is selected then we skip the static analysis for this function (specific to rers)
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(funName=="calculate_output") {
        //cout << "DEBUG: rers-binary mode: skipped static-analysis call."<<endl;
        return elistify();
      }
    }
#endif

    SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    // ad 2)
    SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.target));
    SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
    assert(funDef);
    // ad 3)
    PState newPState=currentPState;
    SgInitializedNamePtrList::iterator i=formalParameters.begin();
    SgExpressionPtrList::iterator j=actualParameters.begin();
    while(i!=formalParameters.end() || j!=actualParameters.end()) {
      SgInitializedName* formalParameterName=*i;
      assert(formalParameterName);
      VariableId formalParameterVarId=variableIdMapping.variableId(formalParameterName);
      // VariableName varNameString=name->get_name();
      SgExpression* actualParameterExpr=*j;
      assert(actualParameterExpr);
      // check whether the actualy parameter is a single variable: In this case we can propagate the constraints of that variable to the formal parameter.
      // pattern: call: f(x), callee: f(int y) => constraints of x are propagated to y
      VariableId actualParameterVarId;
      assert(actualParameterExpr);
      if(exprAnalyzer.variable(actualParameterExpr,actualParameterVarId)) {
        // propagate constraint from actualParamterVarId to formalParameterVarId
        cset.addAssignEqVarVar(formalParameterVarId,actualParameterVarId);
      }
      // general case: the actual argument is an arbitrary expression (including a single variable)
      // we use for the third parameter "false": do not use constraints when extracting values.
      // Consequently, formalparam=actualparam remains top, even if constraints are available, which
      // would allow to extract a constant value (or a range (when relational constraints are added)).
      list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evalConstInt(actualParameterExpr,currentEState,false, true);
      assert(evalResultList.size()>0);
      list<SingleEvalResultConstInt>::iterator resultListIter=evalResultList.begin();
      SingleEvalResultConstInt evalResult=*resultListIter;
      if(evalResultList.size()>1) {
        cerr<<"Error: We currently do not support multi-state generating operators in function call parameters (yet)."<<endl;
        exit(1);
      }
      // above evalConstInt does not use constraints (par3==false). Therefore top vars remain top vars (which is what we want here)
      newPState[formalParameterVarId]=evalResult.value();
      ++i;++j;
    }
    // assert must hold if #formal-params==#actual-params (TODO: default values)
    assert(i==formalParameters.end() && j==actualParameters.end()); 
    // ad 4
    return elistify(createEState(edge.target,newPState,cset));
  }
  // "return x;": add $return=eval() [but not for "return f();"]
  if(isSgReturnStmt(nextNodeToAnalyze1) && !SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {

    SgNode* expr=SgNodeHelper::getFirstChild(nextNodeToAnalyze1);
    ConstraintSet cset=*currentEState.constraints();
    PState newPState=analyzeAssignRhs(*(currentEState.pstate()),
                                    variableIdMapping.createUniqueTemporaryVariableId(string("$return")),
                                    expr,
                                    cset);
    return elistify(createEState(edge.target,newPState,cset));
  }

  // function exit node:
  if(getLabeler()->isFunctionExitLabel(edge.source)) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.source))) {
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
      VariableIdMapping::VariableIdSet localVars=determineVariableIdsOfVariableDeclarations(varDecls);
      SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
      VariableIdMapping::VariableIdSet formalParams=determineVariableIdsOfSgInitializedNames(formalParamInitNames);
      VariableIdMapping::VariableIdSet vars=localVars+formalParams;
      set<string> names=variableIdsToVariableNames(vars);

      for(VariableIdMapping::VariableIdSet::iterator i=vars.begin();i!=vars.end();++i) {
        VariableId varId=*i;
        newPState.deleteVar(varId);
        cset.removeAllConstraintsOfVar(varId);
      }
      // ad 3)
      return elistify(createEState(edge.target,newPState,cset));
    } else {
      cerr << "FATAL ERROR: no function definition associated with function exit label."<<endl;
      exit(1);
    }
  }
  if(getLabeler()->isFunctionCallReturnLabel(edge.source)) {
    // case 1: return f(); pass estate trough
    if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
      EState newEState=currentEState;
      newEState.setLabel(edge.target);
      return elistify(newEState);
    }
    // case 2: x=f(); bind variable x to value of $return
    if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nextNodeToAnalyze1)) {
#ifdef RERS_SPECIALIZATION
      if(boolOptions["rers-binary"]) {
        if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1)) {
          string funName=SgNodeHelper::getFunctionName(funCall);
          if(funName=="calculate_output") {
            EState newEState=currentEState;
            newEState.setLabel(edge.target);
            return elistify(newEState);
          }
        }
      }
#endif
      SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1));
      VariableId lhsVarId;
      bool isLhsVar=exprAnalyzer.variable(lhs,lhsVarId);
      assert(isLhsVar); // must hold
      PState newPState=*currentEState.pstate();
      // we only create this variable here to be able to find an existing $return variable!
      VariableId returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));
      AValue evalResult=newPState[returnVarId].getValue();
      newPState[lhsVarId]=evalResult;

      cset.addAssignEqVarVar(lhsVarId,returnVarId);

      newPState.deleteVar(returnVarId); // remove $return from state
      cset.removeAllConstraintsOfVar(returnVarId); // remove constraints of $return

      return elistify(createEState(edge.target,newPState,cset));
    }
    // case 3: f(); remove $return from state (discard value)
    if(SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(nextNodeToAnalyze1)) {
      PState newPState=*currentEState.pstate();
      VariableId returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));
      newPState.deleteVar(returnVarId);
      cset.removeAllConstraintsOfVar(returnVarId); // remove constraints of $return
      //ConstraintSet cset=*currentEState.constraints; ???
      return elistify(createEState(edge.target,newPState,cset));
    }
  }

  if(edge.isType(EDGE_EXTERNAL)) {
    InputOutput newio;
    Label lab=getLabeler()->getLabel(nextNodeToAnalyze1);
    VariableId varId;
    if(getLabeler()->isStdInLabel(lab,&varId)) {
      if(_inputVarValues.size()>0) {
        // update state (remove all existing constraint on that variable and set it to top)
        PState newPState=*currentEState.pstate();
        ConstraintSet newCSet=*currentEState.constraints();
        newCSet.removeAllConstraintsOfVar(varId);
        list<EState> resList;
        for(set<int>::iterator i=_inputVarValues.begin();i!=_inputVarValues.end();++i) {
          PState newPState=*currentEState.pstate();
          if(boolOptions["input-var-values-as-constraints"]) {
            newCSet.removeAllConstraintsOfVar(varId);
            newPState[varId]=AType::Top();
            newCSet.addConstraint(Constraint(Constraint::EQ_VAR_CONST,varId,AType::ConstIntLattice(*i)));
            assert(newCSet.size()>0);
          } else {
            newCSet.removeAllConstraintsOfVar(varId);
            newPState[varId]=AType::ConstIntLattice(*i);
          }
          newio.recordVariable(InputOutput::STDIN_VAR,varId);
          EState estate=createEState(edge.target,newPState,newCSet,newio);
          resList.push_back(estate);
        }
        //cout << "DEBUG: created "<<_inputVarValues.size()<<" input states."<<endl;
        return resList;
      } else {
        // without specified input values (default mode: analysis performed for all possible input values)
        // update state (remove all existing constraint on that variable and set it to top)
        PState newPState=*currentEState.pstate();
        ConstraintSet newCSet=*currentEState.constraints();
        if(boolOptions["abstract-interpreter"]) {
          cout<<"CodeThorn-abstract-interpreter(stdin)> ";
          AValue aval;
          CodeThorn::Parse::whitespaces(cin);
          cin >> aval;
          newCSet.removeAllConstraintsOfVar(varId);
          newPState[varId]=aval;
        } else {
          if(boolOptions["update-input-var"]) {
            newCSet.removeAllConstraintsOfVar(varId);
            newPState[varId]=AType::Top();
          }
          if(boolOptions["reset-state-on-input"]) {
            newPState.setAllVariablesToTop();
            ConstraintSet emptyCSet;
            newCSet=emptyCSet;
          }
        }
        newio.recordVariable(InputOutput::STDIN_VAR,varId);
        return elistify(createEState(edge.target,newPState,newCSet,newio));
      }
    }
    if(getLabeler()->isStdOutLabel(lab,&varId)) {
      newio.recordVariable(InputOutput::STDOUT_VAR,varId);
      assert(newio.var==varId);
      if(boolOptions["report-stdout"]) {
        cout << "REPORT: stdout:"<<varId.toString()<<":"<<estate->toString()<<endl;
      }
      if(boolOptions["abstract-interpreter"]) {
        PState* pstate=const_cast<PState*>(estate->pstate());
        AType::ConstIntLattice aint=(*pstate)[varId].getValue();
        // TODO: to make this more specific we must parse the printf string
        cout<<"CodeThorn-abstract-interpreter(stdout)> ";
        cout<<aint.toString()<<endl;
      }
    }
    if(getLabeler()->isStdErrLabel(lab,&varId)) {
      newio.recordVariable(InputOutput::STDERR_VAR,varId);
      assert(newio.var==varId);
      if(boolOptions["abstract-interpreter"]) {
        PState* pstate=const_cast<PState*>(estate->pstate());
        AType::ConstIntLattice aint=(*pstate)[varId].getValue();
        // TODO: to make this more specific we must parse the printf string
        cerr<<"CodeThorn-abstract-interpreter(stderr)> ";
        cerr<<aint.toString()<<endl;
      }
      if(boolOptions["report-stderr"]) {
        cout << "REPORT: stderr:"<<varId.toString()<<":"<<estate->toString()<<endl;
      }
    }

    // for all other external functions we use identity as transfer function
    EState newEState=currentEState;
    newEState.io=newio;
    newEState.setLabel(edge.target);
    return elistify(newEState);
  }

  // special case external call
  if(SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1) 
     ||edge.isType(EDGE_EXTERNAL)
     ||edge.isType(EDGE_CALLRETURN)) {
    EState newEState=currentEState;
    newEState.setLabel(edge.target);
    return elistify(newEState);
  }
  
  //cout << "INFO1: we are at "<<astTermWithNullValuesToString(nextNodeToAnalyze1)<<endl;
  if(SgVariableDeclaration* decl=isSgVariableDeclaration(nextNodeToAnalyze1)) {
    return elistify(analyzeVariableDeclaration(decl,currentEState, edge.target));
  }

  if(isSgExprStatement(nextNodeToAnalyze1) || SgNodeHelper::isForIncExpr(nextNodeToAnalyze1)) {
    SgNode* nextNodeToAnalyze2=0;
    if(isSgExprStatement(nextNodeToAnalyze1))
      nextNodeToAnalyze2=SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1);
    if(SgNodeHelper::isForIncExpr(nextNodeToAnalyze1)) {
      nextNodeToAnalyze2=nextNodeToAnalyze1;
    }
    assert(nextNodeToAnalyze2);
    Label newLabel;
    PState newPState;
    ConstraintSet newCSet;
    if(edge.isType(EDGE_TRUE) || edge.isType(EDGE_FALSE)) {
      list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evalConstInt(nextNodeToAnalyze2,currentEState,true,true);
      //assert(evalResultList.size()==1);
      list<EState> newEStateList;
      for(list<SingleEvalResultConstInt>::iterator i=evalResultList.begin();
          i!=evalResultList.end();
          ++i) {
        SingleEvalResultConstInt evalResult=*i;
        if((evalResult.isTrue() && edge.isType(EDGE_TRUE)) || (evalResult.isFalse() && edge.isType(EDGE_FALSE)) || evalResult.isTop()) {
          // pass on EState
          newLabel=edge.target;
          newPState=*evalResult.estate.pstate();
          // merge with collected constraints of expr (exprConstraints)
          if(edge.isType(EDGE_TRUE)) {
            newCSet=*evalResult.estate.constraints()+evalResult.exprConstraints;
          } else if(edge.isType(EDGE_FALSE)) {
            ConstraintSet s1=*evalResult.estate.constraints();
            ConstraintSet s2=evalResult.exprConstraints;
            newCSet=s1+s2;
          }
          newEStateList.push_back(createEState(newLabel,newPState,newCSet));
        } else {
          // we determined not to be on an execution path, therefore do nothing (do not add any result to resultlist)
        }
      }
      // return LIST
      return newEStateList;
    }
    if(isSgConditionalExp(nextNodeToAnalyze2)) {
      cerr<<"Error: found conditional expression outside assert. We do not support this form yet."<<endl;
      exit(1);
#if 0
      // TODO
      SgExpression* cond=nextNodeToAnalyze2->get_conditional_exp();
      SgExpression* trueexp=nextNodeToAnalyze2->get_true_exp();
      SgExpression* falseexp=nextNodeToAnalyze2->get_false_exp();
#endif
      // we currently only handle ConditionalExpressions as used in asserts (handled above)
      ConstraintSet cset=*currentEState.constraints();
      PState newPState=*currentEState.pstate();
      return elistify(createEState(edge.target,newPState,cset));
    }

    if(SgNodeHelper::isPrefixIncDecOp(nextNodeToAnalyze2)
       || SgNodeHelper::isPostfixIncDecOp(nextNodeToAnalyze2)) {
      SgNode* nextNodeToAnalyze3=SgNodeHelper::getUnaryOpChild(nextNodeToAnalyze2);
      VariableId var;
      if(exprAnalyzer.variable(nextNodeToAnalyze3,var)) {

        list<SingleEvalResultConstInt> res=exprAnalyzer.evalConstInt(nextNodeToAnalyze3,currentEState,true,true);
        assert(res.size()==1); // must hold for currently supported limited form of ++,--
        list<SingleEvalResultConstInt>::iterator i=res.begin();
        EState estate=(*i).estate;
        PState newPState=*estate.pstate();
        ConstraintSet cset=*estate.constraints();

        AType::ConstIntLattice varVal=newPState[var].getValue();
        AType::ConstIntLattice const1=1;
        switch(nextNodeToAnalyze2->variantT()) {          
        case V_SgPlusPlusOp:
          varVal=varVal+const1; // overloaded binary + operator
          break;
        case V_SgMinusMinusOp:
          varVal=varVal-const1; // overloaded binary - operator
          break;
        default:
          cerr << "Operator-AST:"<<astTermToMultiLineString(nextNodeToAnalyze2,2)<<endl;
          cerr << "Operator:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze2)<<endl;
          cerr << "Operand:"<<SgNodeHelper::nodeToString(nextNodeToAnalyze3)<<endl;
          cerr<<"Error: programmatic error in handling of inc/dec operators."<<endl;
          exit(1);
        }
        newPState[var]=varVal;

        if(!(*i).result.isTop())
          cset.removeAllConstraintsOfVar(var);
        list<EState> estateList;
        estateList.push_back(createEState(edge.target,newPState,cset));
        return estateList;
      } else {
        throw "Error: currently inc/dec operators are only supported for variables.";
      }
    }
    
    if(isSgAssignOp(nextNodeToAnalyze2)) {
      SgNode* lhs=SgNodeHelper::getLhs(nextNodeToAnalyze2);
      SgNode* rhs=SgNodeHelper::getRhs(nextNodeToAnalyze2);
      list<SingleEvalResultConstInt> res=exprAnalyzer.evalConstInt(rhs,currentEState,true,true);
      list<EState> estateList;
      for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
        VariableId lhsVar;
        bool isLhsVar=exprAnalyzer.variable(lhs,lhsVar);
        if(isLhsVar) {
          EState estate=(*i).estate;
          PState newPState=*estate.pstate();
          ConstraintSet cset=*estate.constraints();
          newPState[lhsVar]=(*i).result;
          if(!(*i).result.isTop())
            cset.removeAllConstraintsOfVar(lhsVar);
          estateList.push_back(createEState(edge.target,newPState,cset));
        } else {
          cerr << "Error: transferfunction:SgAssignOp: unrecognized expression on lhs."<<endl;
          exit(1);
        }
      }
      return estateList;
    }
  }
  // nothing to analyze, just create new estate (from same State) with target label of edge
  // can be same state if edge is a backedge to same cfg node
  EState newEState=currentEState;
  newEState.setLabel(edge.target);
  return elistify(newEState);
}

void Analyzer::initializeSolver1(std::string functionToStartAt,SgNode* root) {
  std::string funtofind=functionToStartAt;
  RoseAst completeast(root);
  startFunRoot=completeast.findFunctionByName(funtofind);
  if(startFunRoot==0) { 
    std::cerr << "Function '"<<funtofind<<"' not found.\n"; exit(1);
  }
  cout << "INIT: Initializing AST node info."<<endl;
  initAstNodeInfo(root);

  cout << "INIT: Creating Labeler."<<endl;
  Labeler* labeler= new Labeler(root,getVariableIdMapping());
  cout << "INIT: Initializing ExprAnalyzer."<<endl;
  exprAnalyzer.setVariableIdMapping(getVariableIdMapping());
  cout << "INIT: Creating CFAnalyzer."<<endl;
  cfanalyzer=new CFAnalyzer(labeler);
  //cout<< "DEBUG: mappingLabelToNode: "<<endl<<getLabeler()->toString()<<endl;
  cout << "INIT: Building CFGs."<<endl;
  flow=cfanalyzer->flow(root);
  cout << "STATUS: Building CFGs finished."<<endl;
  if(boolOptions["reduce-cfg"]) {
    int cnt=cfanalyzer->reduceBlockBeginNodes(flow);
    cout << "INIT: CFG reduction OK. (eliminated "<<cnt<<" nodes)"<<endl;
  }
  cout << "INIT: Intra-Flow OK. (size: " << flow.size() << " edges)"<<endl;
  InterFlow interFlow=cfanalyzer->interFlow(flow);
  cout << "INIT: Inter-Flow OK. (size: " << interFlow.size()*2 << " edges)"<<endl;
  cfanalyzer->intraInterFlow(flow,interFlow);
  cout << "INIT: IntraInter-CFG OK. (size: " << flow.size() << " edges)"<<endl;

  // create empty state
  PState emptyPState;
  const PState* emptyPStateStored=processNew(emptyPState);
  assert(emptyPStateStored);
  cout << "INIT: Empty state(stored): "<<emptyPStateStored->toString()<<endl;
  assert(cfanalyzer);
  ConstraintSet cset;
  const ConstraintSet* emptycsetstored=constraintSetMaintainer.processNewOrExisting(cset);
  Label startLabel=cfanalyzer->getLabel(startFunRoot);
  transitionGraph.setStartLabel(startLabel);
  EState estate(startLabel,emptyPStateStored,emptycsetstored);
  
  if(SgProject* project=isSgProject(root)) {
    cout << "STATUS: Number of global variables: ";
    list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
    cout << globalVars.size()<<endl;
    
    list<SgVarRefExp*> varRefExpList=SgNodeHelper::listOfUsedVarsInFunctions(project);
    // compute set of varIds (it is a set because we want multiple uses of the same var to be represented by one id)
    VariableIdMapping::VariableIdSet setOfUsedVars;
    for(list<SgVarRefExp*>::iterator i=varRefExpList.begin();i!=varRefExpList.end();++i) {
      setOfUsedVars.insert(variableIdMapping.variableId(*i));
    }
    cout << "STATUS: Number of used variables: "<<setOfUsedVars.size()<<endl;

    int filteredVars=0;
    for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
      VariableId globalVarId=variableIdMapping.variableId(*i);
      if(setOfUsedVars.find(globalVarId)!=setOfUsedVars.end() && _variablesToIgnore.find(globalVarId)==_variablesToIgnore.end()) {
        globalVarName2VarIdMapping[variableIdMapping.variableName(variableIdMapping.variableId(*i))]=variableIdMapping.variableId(*i);
        estate=analyzeVariableDeclaration(*i,estate,estate.label());
      } else {
        filteredVars++;
      }
    }
    cout << "STATUS: Number of filtered variables for initial pstate: "<<filteredVars<<endl;
    if(_variablesToIgnore.size()>0)
      cout << "STATUS: Number of ignored variables for initial pstate: "<<_variablesToIgnore.size()<<endl;
  } else {
    cout << "INIT: no global scope.";
  }    

  const EState* currentEState=processNew(estate);
  assert(currentEState);
  //cout << "INIT: "<<eStateSet.toString()<<endl;
  addToWorkList(currentEState);
  cout << "INIT: start state: "<<currentEState->toString(&variableIdMapping)<<endl;
  cout << "INIT: finished."<<endl;
}

set<const EState*> Analyzer::transitionSourceEStateSetOfLabel(Label lab) {
  set<const EState*> estateSet;
  for(TransitionGraph::iterator j=transitionGraph.begin();j!=transitionGraph.end();++j) {
    if((*j).source->label()==lab)
      estateSet.insert((*j).source);
  }
  return estateSet;
}

// TODO: this function should be implemented with a call of ExprAnalyzer::evalConstInt
// TODO: currently all rhs which are not a variable are evaluated to top by this function
// TODO: x=x eliminates constraints of x but it should not.
PState Analyzer::analyzeAssignRhs(PState currentPState,VariableId lhsVar, SgNode* rhs, ConstraintSet& cset) {
  assert(isSgExpression(rhs));
  AValue rhsIntVal=AType::Top();
  bool isRhsIntVal=false;
  bool isRhsVar=false;

  if(boolOptions["assign-top"]) {
    PState newPState=currentPState;
    cset.removeAllConstraintsOfVar(lhsVar);
    newPState[lhsVar]=AType::Top();
    return newPState;
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
    assert(isRhsVar);
    // x=y: constraint propagation for var1=var2 assignments
    // we do not perform this operation on assignments yet, as the constraint set could become inconsistent.
    //cset.addEqVarVar(lhsVar, rhsVarId);

    if(currentPState.varExists(rhsVarId)) {
      rhsIntVal=currentPState[rhsVarId].getValue();
    } else {
      if(_variablesToIgnore.size()==0)
        cerr << "WARNING: access to variable "<<variableIdMapping.uniqueLongVariableName(rhsVarId)<< "on rhs of assignment, but variable does not exist in state. Initializing with top."<<endl;
      rhsIntVal=AType::Top();
      isRhsIntVal=true;
    }
  }
  PState newPState=currentPState;
  if(newPState.varExists(lhsVar)) {
    if(!isRhsIntVal && !isRhsVar) {
      rhsIntVal=AType::Top();
    }
    // we are using AValue here (and  operator== is overloaded for AValue==AValue)
    // for this comparison isTrue() is also false if any of the two operands is AType::Top()
    if( (newPState[lhsVar].getValue()==rhsIntVal).isTrue() ) {
      // update of existing variable with same value
      // => no state change
      return newPState;
    } else {
      // update of existing variable with new value
      newPState[lhsVar]=rhsIntVal;
      if((!rhsIntVal.isTop() && !isRhsVar) || boolOptions["arith-top"])
        cset.removeAllConstraintsOfVar(lhsVar);
      return newPState;
    }
  } else {
    if(_variablesToIgnore.size()>0 && (_variablesToIgnore.find(lhsVar)!=_variablesToIgnore.end())) {
      // nothing to do because variable is ignored
    } else {
      // new variable with new value
      newPState[lhsVar]=rhsIntVal;
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
  assert(node);
  if(!cfanalyzer) {
    cerr << "Error: DFAnalyzer: no cfanalyzer found."<<endl;
    exit(1);
  }
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin().withoutNullValues();i!=ast.end();++i) {
    assert(*i);
    AstNodeInfo* attr=dynamic_cast<AstNodeInfo*>((*i)->getAttribute("info"));
    if(attr) {
      if(cfanalyzer->getLabel(*i)!=Labeler::NO_LABEL) {
        attr->setLabel(cfanalyzer->getLabel(*i));
        attr->setInitialLabel(cfanalyzer->initialLabel(*i));
        attr->setFinalLabels(cfanalyzer->finalLabels(*i));
      } else {
        (*i)->removeAttribute("info");
      }
    } 
#if 0
    cout << "DEBUG:"<<(*i)->sage_class_name();
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
    if(estateSet.estateId(*i)==NO_ESTATE || (*i).label()==Labeler::NO_LABEL) {
      cerr<< "ERROR: estateSet inconsistent. "<<endl;
      cerr << "  label   :"<<(*i).label()<<endl;
      cerr<< "   estateId: "<<estateSet.estateId(*i)<<endl;
      return false;
    }
  }
  cout << "INFO: estateSet of size "<<estateSet.size()<<" consistent."<<endl;
  return true;
}

bool Analyzer::isConsistentEStatePtrSet(set<const EState*> estatePtrSet)  {
  for(set<const EState*>::iterator i=estatePtrSet.begin();i!=estatePtrSet.end();++i) {
    if(estateSet.estateId(*i)==NO_ESTATE || (*i)->label()==Labeler::NO_LABEL) {
      cerr<< "ERROR: estatePtrSet inconsistent. "<<endl;
      cerr << "  label   :"<<(*i)->label()<<endl;
      cerr<< "   estateId: "<<estateSet.estateId(*i)<<endl;
      return false;
    }
  }
  cout << "INFO: estatePtrSet of size "<<estatePtrSet.size()<<" consistent."<<endl;
  return true;
}

void Analyzer::semanticFoldingOfTransitionGraph() {
  //#pragma omp critical // in conflict with TransitionGraph.add ...
  {
    //cout << "STATUS: (Experimental) semantic folding of transition graph ..."<<endl;
    //assert(checkEStateSet());
    if(boolOptions["post-semantic-fold"])
      cout << "STATUS: computing states to fold."<<endl;
    set<const EState*> xestates0=nonLTLRelevantEStates();

    // filter for worklist
    set<const EState*> xestates;
    for(set<const EState*>::iterator i=xestates0.begin();i!=xestates0.end();++i) {
      assert(*i);
      if(!isInWorkList(*i) && !(getTransitionGraph()->getStartLabel()==(*i)->label()))
        xestates.insert(*i);
    }
    if(xestates0.size()!=xestates.size()) {
      //cout << "INFO: Successfully avoided reducing node(s) which are in the work list."<<endl;
    }
    
    int tg_size_before_folding=getTransitionGraph()->size();
    getTransitionGraph()->reduceEStates2(xestates);
    int tg_size_after_folding=getTransitionGraph()->size();
    
#if 1
    for(set<const EState*>::iterator i=xestates.begin();i!=xestates.end();++i) {
      bool res=estateSet.erase(**i);
      if(res==false) {
        cerr<< "Error: Semantic folding of transition graph: no estate to delete."<<endl;
        exit(1);
      }
    }
    //assert(checkEStateSet());
    //assert(checkTransitionGraph());
#else
    cout << "STATUS: NOT folding estates: from " <<estateSet.size()<< " to "<< estateSet.size()-xestates.size() << " ... "<<flush;
#endif
    if(boolOptions["report-semantic-fold"] && tg_size_before_folding!=tg_size_after_folding)
      cout << "STATUS: Folded transition graph from "<<tg_size_before_folding<<" to "<<tg_size_after_folding<<" transitions."<<endl;
    
  } // end of omp pragma
}

void Analyzer::runSolver2() {
  size_t prevStateSetSize=_displayDiff+1; // force immediate report at start
#ifdef _OPENMP
  omp_set_num_threads(_numberOfThreadsToUse);
  omp_set_dynamic(1);
#endif
  int threadNum;
  vector<const EState*> workVector(_numberOfThreadsToUse);
  int analyzedSemanticFoldingNode=0;
  while(1) {
    int workers;
    for(workers=0;workers<_numberOfThreadsToUse;++workers) {
      if(!(workVector[workers]=popWorkList()))
        break;
    }
    if(workers==0)
      break; // we are done

#pragma omp parallel for private(threadNum),shared(workVector,analyzedSemanticFoldingNode)
    for(int j=0;j<workers;++j) {
#ifdef _OPENMP
      threadNum=omp_get_thread_num();
#endif
      const EState* currentEStatePtr=workVector[j];
      if(!currentEStatePtr) {
        cerr<<"Error: thread "<<threadNum<<" finished prematurely. Bailing out. "<<endl;
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
        exit(1);
      }
      assert(currentEStatePtr);
      
      Flow edgeSet=flow.outEdges(currentEStatePtr->label());
#ifdef PARALLELIZE_BRANCHES
      // we can simplify this by adding the proper function to Flow.
      Flow::iterator i=edgeSet.begin();
      int edgeNum=edgeSet.size();
      vector<const Edge*> edgeVec(edgeNum);
      for(int edge_i=0;edge_i<edgeNum;++edge_i) {
        edgeVec[edge_i]=&(*i++);
      }
#pragma omp parallel for
      for(int edge_i=0; edge_i<edgeNum;edge_i++) {
        Edge e=*edgeVec[edge_i]; // *i
#else
        for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
          Edge e=*i;
#endif
          list<EState> newEStateList;
          newEStateList=transferFunction(e,currentEStatePtr);
          if(isTerminationRelevantLabel(e.source)) {
            #pragma omp atomic
            analyzedSemanticFoldingNode++;
          }
          for(list<EState>::iterator nesListIter=newEStateList.begin();
              nesListIter!=newEStateList.end();
              ++nesListIter) {
            EState newEState=*nesListIter;
            if(newEState.label()!=Labeler::NO_LABEL && (!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState))) {
              HSetMaintainer<EState,EStateHashFun>::ProcessingResult pres=process(newEState);
              const EState* newEStatePtr=pres.second;
              if(pres.first==true)
                addToWorkList(newEStatePtr);            
              recordTransition(currentEStatePtr,e,newEStatePtr);
            }
            if(newEState.label()!=Labeler::NO_LABEL && (!newEState.constraints()->disequalityExists()) && (isFailedAssertEState(&newEState))) {
              // failed-assert end-state: do not add to work list but do add it to the transition graph
              const EState* newEStatePtr;
              newEStatePtr=processNewOrExisting(newEState);
              recordTransition(currentEStatePtr,e,newEStatePtr);        
              
              if(boolOptions["report-failed-assert"]) {
#pragma omp critical
                {
                  cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
                }
              }
              if(boolOptions["abstract-interpreter"]) {
                cerr<<"CodeThorn-abstract-interpreter> failed assert";
                string name=labelNameOfAssertLabel(currentEStatePtr->label());
                if(name!="") { cout << " @ Label: "<<name;}
                cout <<endl;
              }
              if(_csv_assert_live_file.size()>0) {
                string name=labelNameOfAssertLabel(currentEStatePtr->label());
                if(name=="globalError")
                  name="error_60";
                name=name.substr(6,name.size()-6);
                std::ofstream fout;
                // csv_assert_live_file is the member-variable of analyzer
#pragma omp critical
                {
                  fout.open(_csv_assert_live_file.c_str(),ios::app);    // open file for appending
                  assert (!fout.fail( ));
                  fout << name << ",yes,9"<<endl;
                  //cout << "REACHABLE ASSERT FOUND: "<< name << ",yes,9"<<endl;
                  
                  fout.close(); 
                }
              } // if
            }
            if(newEState.label()==Labeler::NO_LABEL) {
              //cerr << "INFO: found final state."<<endl;
            }
          } // end of loop on transfer function return-estates
#ifdef PARALLELIZE_BRANCHES
        } // edgeset-parallel for
#else
      } // just for proper auto-formatting in emacs
#endif
    } // worklist-parallel for
    if(boolOptions["semantic-fold"]) {
      if(analyzedSemanticFoldingNode>_semanticFoldThreshold) {
        semanticFoldingOfTransitionGraph();
        analyzedSemanticFoldingNode=0;
        prevStateSetSize=estateSet.size();
      }
    }
    if(_displayDiff && (estateSet.size()>(prevStateSetSize+_displayDiff))) {
      printStatusMessage(true);
      prevStateSetSize=estateSet.size();
    }
  } // while
  if(boolOptions["semantic-fold"]) {
    semanticFoldingOfTransitionGraph(); // final fold
  }
  printStatusMessage(true);
  cout << "analysis finished (worklist is empty)."<<endl;
  assert(checkTransitionGraph());
}
