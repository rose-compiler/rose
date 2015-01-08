#ifndef ANALYZER_H
#define ANALYZER_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <sstream>

#include <omp.h>

#include <boost/unordered_set.hpp>

#include "AstTerm.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "ExprAnalyzer.h"
#include "StateRepresentations.h"
#include "PropertyValueTable.h"

// we use INT_MIN, INT_MAX
#include "limits.h"

using namespace std;

namespace CodeThorn {

#define DEBUGPRINT_STMT 0x1
#define DEBUGPRINT_STATE 0x2
#define DEBUGPRINT_STATEMOD 0x4
#define DEBUGPRINT_INFO 0x8
  
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
  class AstNodeInfo : public AstAttribute {
  public:
  AstNodeInfo():label(0),initialLabel(0){}
    string toString() { stringstream ss;
      ss<<"\\n lab:"<<label<<" ";
      ss<<"init:"<<initialLabel<<" ";
      ss<<"final:"<<finalLabelsSet.toString();
      return ss.str(); 
    }
    void setLabel(Label l) { label=l; }
    void setInitialLabel(Label l) { initialLabel=l; }
    void setFinalLabels(LabelSet lset) { finalLabelsSet=lset; }
  private:
    Label label;
    Label initialLabel;
    LabelSet finalLabelsSet;
  };

  typedef list<const EState*> EStateWorkList;
  typedef pair<int, const EState*> FailedAssertion;
  enum AnalyzerMode { AM_ALL_STATES, AM_LTL_STATES };

  class Analyzer;

  class VariableValueMonitor {
  public:
    enum VariableMode { VARMODE_FORCED_TOP, VARMODE_ADAPTIVE_TOP, VARMODE_PRECISE, VARMODE_FORCED_PRECISE};
    VariableValueMonitor();
    void setThreshold(size_t threshold);
    size_t getThreshold();
    bool isActive();
    // the init function only uses the variableIds of a given estate (not its values) for initialization
    void init(const EState* estate);
    void init(const PState* pstate);
    VariableIdSet getHotVariables(Analyzer* analyzer, const EState* estate);
    VariableIdSet getHotVariables(Analyzer* analyzer, const PState* pstate);
    VariableIdSet getVariables();
    void setVariableMode(VariableMode,VariableId);
    VariableMode getVariableMode(VariableId);
    void update(Analyzer* analyzer, EState* estate);
    bool isHotVariable(Analyzer* analyzer, VariableId varId);
    string toString(VariableIdMapping* variableIdMapping);
#if 0
    bool isVariableBeyondTreshold(Analyzer* analyzer, VariableId varId);
#endif
  private:
    map<VariableId,set<int>* > _variablesMap;
    map<VariableId,VariableMode> _variablesModeMap;
    long int _threshold;
  };

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
  class Analyzer {
    friend class Visualizer;
    friend class VariableValueMonitor;
  public:
    
    Analyzer();
    ~Analyzer();
    
    void initAstNodeInfo(SgNode* node);
    bool isActiveGlobalTopify();
    static string nodeToString(SgNode* node);
    void initializeSolver1(std::string functionToStartAt,SgNode* root);
    void initializeTraceSolver(std::string functionToStartAt,SgNode* root);
    void continueAnalysisFrom(EState* newStartEState);
    
    PState analyzeAssignRhs(PState currentPState,VariableId lhsVar, SgNode* rhs,ConstraintSet& cset);
    EState analyzeVariableDeclaration(SgVariableDeclaration* nextNodeToAnalyze1,EState currentEState, Label targetLabel);
    list<EState> transferFunction(Edge edge, const EState* estate);
    
    void addToWorkList(const EState* estate);
    const EState* addToWorkListIfNew(EState estate);
    const EState* takeFromWorkList();
    bool isInWorkList(const EState* estate);
    bool isEmptyWorkList();
    const EState* topWorkList();
    const EState* popWorkList();
    
    void recordTransition(const EState* sourceEState, Edge e, const EState* targetEState);
    void printStatusMessage(bool);
    bool isLTLRelevantLabel(Label label);
    bool isStdIOLabel(Label label);
    bool isStartLabel(Label label);
    set<const EState*> nonLTLRelevantEStates();
    bool isTerminationRelevantLabel(Label label);
    
    // 6 experimental functions
    // reduces all states different to stdin and stdout.
    void stdIOFoldingOfTransitionGraph();
    void semanticFoldingOfTransitionGraph();
    void semanticEliminationOfTransitions();
    int semanticEliminationOfSelfInInTransitions();
    // eliminates only input states
    int semanticEliminationOfDeadStates();
    int semanticFusionOfInInTransitions();
    // requires semantically reduced STG
    int semanticExplosionOfInputNodesFromOutputNodeConstraints();
    bool checkEStateSet();
    bool isConsistentEStatePtrSet(set<const EState*> estatePtrSet);
    bool checkTransitionGraph();
    // this function requires that no LTL graph is computed
    void deleteNonRelevantEStates();

    // bypasses and removes all states that are not standard I/O states
    void removeNonIOStates();
    // bypasses and removes all states that are not stdIn/stdOut/stdErr/failedAssert states
    void reduceToObservableBehavior();
    // erases transitions that lead directly from one output state to another output state
    void removeOutputOutputTransitions();
    // erases transitions that lead directly from one input state to another input state
    void removeInputInputTransitions();
    // cuts off all paths in the transition graph that lead to leaves 
    // (recursively until only paths of infinite length remain)
    void pruneLeavesRec();
    // connects start, input, output and worklist states according to possible paths in the transition graph. 
    // removes all states and transitions that are not necessary for the graph that only consists of these new transitions. The two parameters allow to select input and/or output states to remain in the STG.
    void reduceGraphInOutWorklistOnly(bool reduceIn=true, bool reduceOut=true);
    // extracts input sequences leading to each discovered failing assertion where discovered for the first time.
    // stores results in PropertyValueTable "reachabilityResults".
    // returns length of the longest of these sequences if it can be guaranteed that all processed traces are the
    // shortest ones leading to the individual failing assertion (returns -1 otherwise).
    int extractAssertionTraces();
    
  private:
    /*! if state exists in stateSet, a pointer to the existing state is returned otherwise 
      a new state is entered into stateSet and a pointer to it is returned.
    */
    const PState* processNew(PState& s);
    const PState* processNewOrExisting(PState& s);
    const EState* processNew(EState& s);
    const EState* processNewOrExisting(EState& s);
    const EState* processCompleteNewOrExisting(const EState* es);
    void topifyVariable(PState& pstate, ConstraintSet& cset, VariableId varId);
    
    EStateSet::ProcessingResult process(EState& s);
    EStateSet::ProcessingResult process(Label label, PState pstate, ConstraintSet cset, InputOutput io);
    const ConstraintSet* processNewOrExisting(ConstraintSet& cset);
    
    EState createEState(Label label, PState pstate, ConstraintSet cset);
    EState createEState(Label label, PState pstate, ConstraintSet cset, InputOutput io);

    //returns a list of transitions representing existing paths from "startState" to all possible input/output/worklist states (no output -> output)
    // the returned set has to be deleted by the calling function.
    boost::unordered_set<Transition*>* transitionsToInOutAndWorklist( const EState* startState);                                                          
    boost::unordered_set<Transition*>* transitionsToInOutAndWorklist( const EState* currentState, const EState* startState, 
                                                            boost::unordered_set<Transition*>* results, boost::unordered_set<const EState*>* visited);
    // adds a string representation of the shortest input path from start state to assertEState to reachabilityResults. returns the length of the 
    // counterexample input sequence.
    int addCounterexample(int assertCode, const EState* assertEState);
    // returns a list of EStates from source to target. Target has to come before source in the STG (reversed trace). 
    list<const EState*>reverseInOutSequenceBreadthFirst(const EState* source, const EState* target, bool counterexampleWithOutput = false);
    // returns a list of EStates from source to target (shortest input path). 
    // please note: target has to be a predecessor of source (reversed trace)
    list<const EState*> reverseInOutSequenceDijkstra(const EState* source, const EState* target, bool counterexampleWithOutput = false);
    list<const EState*> filterStdInOutOnly(list<const EState*>& states, bool counterexampleWithOutput = false) const;
    string reversedInOutRunToString(list<const EState*>& run);
    //returns the shortest possible number of input states on the path leading to "target".
    int inputSequenceLength(const EState* target);
    
  public:
    SgNode* getCond(SgNode* node);
    void generateAstNodeInfo(SgNode* node);
    string generateSpotSTG();
  private:
    void generateSpotTransition(stringstream& ss, const Transition& t);
    //less than comarisions on two states according to (#input transitions * #output transitions)
    bool indegreeTimesOutdegreeLessThan(const EState* a, const EState* b);
  public:
    //stores a backup of the created transitionGraph
    void storeStgBackup();
    //load previous backup of the transitionGraph, storing the current version as a backup instead
    void swapStgWithBackup();
    //reset the analyzer to now use solver 8 (includes choosing solver8-specific analyzer settings)
    void resetAnalyzerToSolver8(EState* startEState);
    //! requires init
    void runSolver1();
    void runSolver2();
    void runSolver3();
    void runSolver4();
    void runSolver5();
    void runSolver6();
    void runSolver7();
    void runSolver8();
    void runSolver();
    //! The analyzer requires a CFAnalyzer to obtain the ICFG.
    void setCFAnalyzer(CFAnalyzer* cf) { cfanalyzer=cf; }
    CFAnalyzer* getCFAnalyzer() const { return cfanalyzer; }
    
    //void initializeVariableIdMapping(SgProject* project) { variableIdMapping.computeVariableSymbolMapping(project); }

    // access  functions for computed information
    VariableIdMapping* getVariableIdMapping() { return &variableIdMapping; }
    SPRAY::IOLabeler* getLabeler() const {
      SPRAY::IOLabeler* ioLabeler=dynamic_cast<SPRAY::IOLabeler*>(cfanalyzer->getLabeler());
      ROSE_ASSERT(ioLabeler);
      return ioLabeler;
    }
    Flow* getFlow() { return &flow; }
    PStateSet* getPStateSet() { return &pstateSet; }
    EStateSet* getEStateSet() { return &estateSet; }
    TransitionGraph* getTransitionGraph() { return &transitionGraph; }
    ConstraintSetMaintainer* getConstraintSetMaintainer() { return &constraintSetMaintainer; }
    
    //private: TODO
    Flow flow;
    SgNode* startFunRoot;
    CFAnalyzer* cfanalyzer;
    VariableValueMonitor variableValueMonitor;
    void setVariableValueThreshold(int threshold) { variableValueMonitor.setThreshold(threshold); }
  public:
    //! compute the VariableIds of variable declarations
    VariableIdMapping::VariableIdSet determineVariableIdsOfVariableDeclarations(set<SgVariableDeclaration*> decls);
    //! compute the VariableIds of SgInitializedNamePtrList
    VariableIdMapping::VariableIdSet determineVariableIdsOfSgInitializedNames(SgInitializedNamePtrList& namePtrList);
    
    set<string> variableIdsToVariableNames(VariableIdMapping::VariableIdSet);
    typedef list<SgVariableDeclaration*> VariableDeclarationList;
    VariableDeclarationList computeUnusedGlobalVariableDeclarationList(SgProject* root);
    VariableDeclarationList computeUsedGlobalVariableDeclarationList(SgProject* root);
    
    //bool isAssertExpr(SgNode* node);
    bool isFailedAssertEState(const EState* estate);
    //! adds a specific code to the io-info of an estate which is checked by isFailedAsserEState and determines a failed-assert estate. Note that the actual assert (and its label) is associated with the previous estate (this information can therefore be obtained from a transition-edge in the transition graph).
    EState createFailedAssertEState(const EState estate, Label target);
    //! list of all asserts in a program
    list<SgNode*> listOfAssertNodes(SgProject *root);
    //! rers-specific error_x: assert(0) version 
    list<pair<SgLabelStatement*,SgNode*> > listOfLabeledAssertNodes(SgProject *root);
    void initLabeledAssertNodes(SgProject* root) {
      _assertNodes=listOfLabeledAssertNodes(root);
    }
    size_t getNumberOfErrorLabels();
    string labelNameOfAssertLabel(Label lab) {
      string labelName;
      for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=_assertNodes.begin();i!=_assertNodes.end();++i)
        if(lab==getLabeler()->getLabel((*i).second))
          labelName=SgNodeHelper::getLabelName((*i).first);
      //assert(labelName.size()>0);
      return labelName;
    }
    bool isCppLabeledAssertLabel(Label lab) {
      return labelNameOfAssertLabel(lab).size()>0;
    }
    
    InputOutput::OpType ioOp(const EState* estate) const;
    
    void setDisplayDiff(int diff) { _displayDiff=diff; }
    void setSolver(int solver) { _solver=solver; }
    int getSolver() { return _solver;}
    void setSemanticFoldThreshold(int t) { _semanticFoldThreshold=t; }
    void setLTLVerifier(int v) { _ltlVerifier=v; }
    int getLTLVerifier() { return _ltlVerifier; }
    void setNumberOfThreadsToUse(int n) { _numberOfThreadsToUse=n; }
    int getNumberOfThreadsToUse() { return _numberOfThreadsToUse; }
    void insertInputVarValue(int i) { _inputVarValues.insert(i); }
    void addInputSequenceValue(int i) { _inputSequence.push_back(i); }
    void resetToEmptyInputSequence() { _inputSequence.clear(); }
    void resetInputSequenceIterator() { _inputSequenceIterator=_inputSequence.begin(); }
    const EState* getEstateBeforeMissingInput() {return _estateBeforeMissingInput;}
    void setTreatStdErrLikeFailedAssert(bool x) { _treatStdErrLikeFailedAssert=x; }
    int numberOfInputVarValues() { return _inputVarValues.size(); }
    std::set<int> getInputVarValues() { return _inputVarValues; }
    list<pair<SgLabelStatement*,SgNode*> > _assertNodes;
    void setCsvAssertLiveFileName(string filename) { _csv_assert_live_file=filename; }
    VariableId globalVarIdByName(string varName) { return globalVarName2VarIdMapping[varName]; }
    void setStgTraceFileName(string filename) {
      _stg_trace_filename=filename;
      std::ofstream fout;
      fout.open(_stg_trace_filename.c_str());    // create new file/overwrite existing file
      fout<<"START"<<endl;
      fout.close();    // close. Will be used with append.
    }
    string _csv_assert_live_file; // to become private
  private:
    string _stg_trace_filename;
 public:
    // only used temporarily for binary-binding prototype
    map<string,VariableId> globalVarName2VarIdMapping;
    vector<bool> binaryBindingAssert;
    void setAnalyzerMode(AnalyzerMode am) { _analyzerMode=am; }
    void setMaxTransitions(size_t maxTransitions) { _maxTransitions=maxTransitions; }
    void setMaxTransitionsForcedTop(size_t maxTransitions) { _maxTransitionsForcedTop=maxTransitions; }
    void eventGlobalTopifyTurnedOn();
    void setMinimizeStates(bool minimizeStates) { _minimizeStates=minimizeStates; }
    bool isIncompleteSTGReady();
    bool isPrecise();
    PropertyValueTable reachabilityResults;
    int reachabilityAssertCode(const EState* currentEStatePtr);
    enum ExplorationMode { EXPL_DEPTH_FIRST, EXPL_BREADTH_FIRST, EXPL_LOOP_AWARE };
    void setExplorationMode(ExplorationMode em) { _explorationMode=em; }
    ExplorationMode getExplorationMode() { return _explorationMode; }
    void setSkipSelectedFunctionCalls(bool defer) {
      _skipSelectedFunctionCalls=true; 
      exprAnalyzer.setSkipSelectedFunctionCalls(true);
    }
    ExprAnalyzer* getExprAnalyzer();
    list<FailedAssertion> getFirstAssertionOccurences(){return _firstAssertionOccurences;}
    void incIterations() {
      if(isPrecise()) {
#pragma omp atomic
        _iterations+=1;
      } else {
#pragma omp atomic
        _approximated_iterations+=1;
      }
    }
    bool isLoopCondLabel(Label lab);
    int getApproximatedIterations() { return _approximated_iterations; }
    int getIterations() { return _iterations; }
  private:
    set<int> _inputVarValues;
    list<int> _inputSequence;
    list<int>::iterator _inputSequenceIterator;
    ExprAnalyzer exprAnalyzer;
    VariableIdMapping variableIdMapping;
    EStateWorkList estateWorkList;
    EStateSet estateSet;
    PStateSet pstateSet;
    ConstraintSetMaintainer constraintSetMaintainer;
    TransitionGraph transitionGraph;
    TransitionGraph backupTransitionGraph;
    set<const EState*> transitionSourceEStateSetOfLabel(Label lab);
    int _displayDiff;
    int _numberOfThreadsToUse;
    int _ltlVerifier;
    int _semanticFoldThreshold;
    VariableIdMapping::VariableIdSet _variablesToIgnore;
    int _solver;
    AnalyzerMode _analyzerMode;
    set<const EState*> _newNodesToFold;
    long int _maxTransitions;
    long int _maxTransitionsForcedTop;
    bool _treatStdErrLikeFailedAssert;
    bool _skipSelectedFunctionCalls;
    ExplorationMode _explorationMode;
    list<FailedAssertion> _firstAssertionOccurences;
    const EState* _estateBeforeMissingInput;
    const EState* _latestOutputEstate;
    bool _minimizeStates;
    bool _topifyModeActive;
    int _iterations;
    int _approximated_iterations;
    int _curr_iteration_cnt;
    int _next_iteration_cnt;
  }; // end of class Analyzer
  
} // end of namespace CodeThorn

#define RERS_SPECIALIZATION
#ifdef RERS_SPECIALIZATION
// RERS-binary-binding-specific declarations
#define STR_VALUE(arg) #arg
#define COPY_PSTATEVAR_TO_GLOBALVAR(VARNAME) VARNAME[thread_id] = pstate[analyzer->globalVarIdByName(STR_VALUE(VARNAME))].getValue().getIntValue();

//cout<<"PSTATEVAR:"<<pstate[analyzer->globalVarIdByName(STR_VALUE(VARNAME))].toString()<<"="<<pstate[analyzer->globalVarIdByName(STR_VALUE(VARNAME))].getValue().toString()<<endl;

#define COPY_GLOBALVAR_TO_PSTATEVAR(VARNAME) pstate[analyzer->globalVarIdByName(STR_VALUE(VARNAME))]=CodeThorn::AType::CppCapsuleConstIntLattice(VARNAME[thread_id]);

// macro used to generate the initialization of global variables in the hybrid analyzer (linked binary with threads)
#define INIT_GLOBALVAR(VARNAME) VARNAME = new int[numberOfThreads];

namespace RERS_Problem {
  void rersGlobalVarsCallInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate, int thread_id);
  void rersGlobalVarsCallReturnInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate, int thread_id);
  void rersGlobalVarsArrayInit(int numberOfThreads);
#if 0
  // input variable passed as a parameter (obsolete since transformation of "input" into a global varialbe)
  void calculate_output(int);
#endif
  void calculate_output(int numberOfThreads);
  extern int* output;
}
// END OF RERS-binary-binding-specific declarations
#endif

#endif
