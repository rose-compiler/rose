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
#include <list>
#include <vector>

#include <omp.h>

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

#include "Timer.h"
#include "AstTerm.h"
#include "Labeler.h"
#include "CFAnalysis.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "ExprAnalyzer.h"
#include "EState.h"
#include "TransitionGraph.h"
#include "PropertyValueTable.h"
#include "CTIOLabeler.h"
#include "VariableValueMonitor.h"

// we use INT_MIN, INT_MAX
#include "limits.h"
#include "AstNodeInfo.h"
#include "SgTypeSizeMapping.h"

namespace CodeThorn {

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
  typedef std::list<const EState*> EStateWorkList;
  typedef std::pair<int, const EState*> FailedAssertion;
  typedef std::pair<PState,  std::list<int> > PStatePlusIOHistory;
  enum AnalyzerMode { AM_ALL_STATES, AM_LTL_STATES };

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

  protected:
    static Sawyer::Message::Facility logger;

  public:
    static void initDiagnostics();
    static std::string nodeToString(SgNode* node);
    void initAstNodeInfo(SgNode* node);
    bool isInExplicitStateMode();
    bool isActiveGlobalTopify();
    void initializeSolver1(std::string functionToStartAt,SgNode* root, bool oneFunctionOnly);
    void initializeTraceSolver(std::string functionToStartAt,SgNode* root);
    void continueAnalysisFrom(EState* newStartEState);
    
    EState analyzeVariableDeclaration(SgVariableDeclaration* nextNodeToAnalyze1,EState currentEState, Label targetLabel);

    PState analyzeAssignRhsExpr(PState currentPState,VariableId lhsVar, SgNode* rhs,ConstraintSet& cset);
    // to be replaced by above function
    PState analyzeAssignRhs(PState currentPState,VariableId lhsVar, SgNode* rhs,ConstraintSet& cset);
    
    void addToWorkList(const EState* estate);
    const EState* addToWorkListIfNew(EState estate);
    const EState* takeFromWorkList();
    bool isInWorkList(const EState* estate);
    bool isEmptyWorkList();
    const EState* topWorkList();
    const EState* popWorkList();
    void swapWorkLists();
    size_t memorySizeContentEStateWorkLists();
    
    void setOptionStatusMessages(bool flag);
    bool getOptionStatusMessages();
    // thread save; only prints if option status messages is enabled.
    void printStatusMessage(string s);
    void printStatusMessageLine(string s);
    void printStatusMessage(string s, bool newLineFlag);
    static string lineColSource(SgNode* node);
    void recordTransition(const EState* sourceEState, Edge e, const EState* targetEState);
    void printStatusMessage(bool);
    bool isStartLabel(Label label);

    // determines whether lab is a function call label of a function
    // call of the form 'x=f(...)' and returns the varible-id of the
    // lhs, if a valid pointer is provided
    bool isFunctionCallWithAssignment(Label lab,VariableId* varId=0);

    std::list<EState> transferEdgeEState(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCall(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallLocalEdge(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallExternal(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallReturn(Edge edge, const EState* estate);
    std::list<EState> transferFunctionExit(Edge edge, const EState* estate);
    std::list<EState> transferReturnStmt(Edge edge, const EState* estate);
    std::list<EState> transferVariableDeclaration(SgVariableDeclaration* decl,Edge edge, const EState* estate);
    std::list<EState> transferExprStmt(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate);
    std::list<EState> transferIdentity(Edge edge, const EState* estate);
    std::list<EState> transferAssignOp(SgAssignOp* assignOp, Edge edge, const EState* estate);
    std::list<EState> transferIncDecOp(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate);
    std::list<EState> transferTrueFalseEdge(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate);

    SgNode* getCond(SgNode* node);
    void generateAstNodeInfo(SgNode* node);
    bool checkEStateSet();
    bool isConsistentEStatePtrSet(std::set<const EState*> estatePtrSet);
    bool checkTransitionGraph();

    //! requires init
    void runSolver4();
    void runSolver5();
    void runSolver8();
    void runSolver9();
    void runSolver10();
    void runSolver11();
    void runSolver12();
    void runSolver();
    //! The analyzer requires a CFAnalysis to obtain the ICFG.
    void setCFAnalyzer(CFAnalysis* cf) { cfanalyzer=cf; }
    CFAnalysis* getCFAnalyzer() const { return cfanalyzer; }
    
    //void initializeVariableIdMapping(SgProject* project) { variableIdMapping.computeVariableSymbolMapping(project); }

    // access  functions for computed information
    VariableIdMapping* getVariableIdMapping() { return &variableIdMapping; }
    CTIOLabeler* getLabeler() const {
      CTIOLabeler* ioLabeler=dynamic_cast<CTIOLabeler*>(cfanalyzer->getLabeler());
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
    CFAnalysis* cfanalyzer;

    enum ExplorationMode { EXPL_DEPTH_FIRST, EXPL_BREADTH_FIRST, EXPL_LOOP_AWARE, EXPL_LOOP_AWARE_SYNC, EXPL_RANDOM_MODE1 };
    void eventGlobalTopifyTurnedOn();
    bool isIncompleteSTGReady();
    bool isPrecise();
    PropertyValueTable reachabilityResults;
    int reachabilityAssertCode(const EState* currentEStatePtr);
    void setExplorationMode(ExplorationMode em) { _explorationMode=em; }
    ExplorationMode getExplorationMode() { return _explorationMode; }
    void setSkipSelectedFunctionCalls(bool defer) {
      _skipSelectedFunctionCalls=true; 
      exprAnalyzer.setSkipSelectedFunctionCalls(true);
    }
    void setSkipArrayAccesses(bool skip) {
      exprAnalyzer.setSkipArrayAccesses(skip);
    }
    bool getSkipArrayAccesses() {
      return exprAnalyzer.getSkipArrayAccesses();
    }
    ExprAnalyzer* getExprAnalyzer();
    std::list<FailedAssertion> getFirstAssertionOccurences(){return _firstAssertionOccurences;}
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
    string getVarNameByIdCode(int varIdCode) {return variableIdMapping.variableName(variableIdMapping.variableIdFromCode(varIdCode));};
    void mapGlobalVarInsert(std::string name, int* addr);

    //! compute the VariableIds of variable declarations
    VariableIdMapping::VariableIdSet determineVariableIdsOfVariableDeclarations(set<SgVariableDeclaration*> decls);
    //! compute the VariableIds of SgInitializedNamePtrList
    VariableIdMapping::VariableIdSet determineVariableIdsOfSgInitializedNames(SgInitializedNamePtrList& namePtrList);
    
    std::set<std::string> variableIdsToVariableNames(CodeThorn::AbstractValueSet);
    std::set<std::string> variableIdsToVariableNames(SPRAY::VariableIdSet);
    typedef std::list<SgVariableDeclaration*> VariableDeclarationList;
    VariableDeclarationList computeUnusedGlobalVariableDeclarationList(SgProject* root);
    VariableDeclarationList computeUsedGlobalVariableDeclarationList(SgProject* root);
    
    bool isFailedAssertEState(const EState* estate);
    bool isVerificationErrorEState(const EState* estate);
    //! adds a specific code to the io-info of an estate which is checked by isFailedAsserEState and determines a failed-assert estate. Note that the actual assert (and its label) is associated with the previous estate (this information can therefore be obtained from a transition-edge in the transition graph).
    EState createFailedAssertEState(const EState estate, Label target);
    EState createVerificationErrorEState(const EState estate, Label target);
    //! list of all asserts in a program
    std::list<SgNode*> listOfAssertNodes(SgProject *root);
    //! rers-specific error_x: assert(0) version 
    std::list<std::pair<SgLabelStatement*,SgNode*> > listOfLabeledAssertNodes(SgProject *root);
    void initLabeledAssertNodes(SgProject* root) {
      _assertNodes=listOfLabeledAssertNodes(root);
    }
    size_t getNumberOfErrorLabels();
    std::string labelNameOfAssertLabel(Label lab) {
      std::string labelName;
      for(std::list<std::pair<SgLabelStatement*,SgNode*> >::iterator i=_assertNodes.begin();i!=_assertNodes.end();++i)
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
    void setResourceLimitDiff(int diff) { _resourceLimitDiff=diff; }
    void setSolver(int solver);
    int getSolver();
    void setSemanticFoldThreshold(int t) { _semanticFoldThreshold=t; }
    void setNumberOfThreadsToUse(int n) { _numberOfThreadsToUse=n; }
    int getNumberOfThreadsToUse() { return _numberOfThreadsToUse; }
    std::list<std::pair<SgLabelStatement*,SgNode*> > _assertNodes;
    VariableId globalVarIdByName(std::string varName) { return globalVarName2VarIdMapping[varName]; }
    void setTreatStdErrLikeFailedAssert(bool x) { _treatStdErrLikeFailedAssert=x; }

    boost::unordered_map <std::string,int*> mapGlobalVarAddress;
    boost::unordered_map <int*,std::string> mapAddressGlobalVar;
    void setCompoundIncVarsSet(set<AbstractValue> ciVars);
    void setSmallActivityVarsSet(set<AbstractValue> ciVars);
    void setAssertCondVarsSet(set<AbstractValue> acVars);
    enum GlobalTopifyMode {GTM_IO, GTM_IOCF, GTM_IOCFPTR, GTM_COMPOUNDASSIGN, GTM_FLAGS};
    void setGlobalTopifyMode(GlobalTopifyMode mode);
    void setExternalErrorFunctionName(std::string externalErrorFunctionName);
    // enables external function semantics 
    void enableSVCompFunctionSemantics();
    void disableSVCompFunctionSemantics();
    bool svCompFunctionSemantics() { return _svCompFunctionSemantics; }
    bool stdFunctionSemantics() { return _stdFunctionSemantics; }
    void setModeLTLDriven(bool ltlDriven) { transitionGraph.setModeLTLDriven(ltlDriven); }
    bool getModeLTLDriven() { return transitionGraph.getModeLTLDriven(); }
    long analysisRunTimeInSeconds(); 

    void set_finished(std::vector<bool>& v, bool val);
    bool all_false(std::vector<bool>& v);

    void setTypeSizeMapping(SgTypeSizeMapping* typeSizeMapping);
    SgTypeSizeMapping* getTypeSizeMapping();
  private:
    GlobalTopifyMode _globalTopifyMode;
    set<AbstractValue> _compoundIncVarsSet;
    set<AbstractValue> _smallActivityVarsSet;
    set<AbstractValue> _assertCondVarsSet;
    set<int> _inputVarValues;
    std::list<int> _inputSequence;
    std::list<int>::iterator _inputSequenceIterator;
    ExprAnalyzer exprAnalyzer;
    VariableIdMapping variableIdMapping;
    EStateWorkList* estateWorkListCurrent;
    EStateWorkList* estateWorkListNext;
    EStateWorkList estateWorkListOne;
    EStateWorkList estateWorkListTwo;
    EStateSet estateSet;
    PStateSet pstateSet;
    ConstraintSetMaintainer constraintSetMaintainer;
    TransitionGraph transitionGraph;
    TransitionGraph backupTransitionGraph;
    set<const EState*> transitionSourceEStateSetOfLabel(Label lab);
    int _displayDiff;
    int _resourceLimitDiff;
    int _numberOfThreadsToUse;
    int _semanticFoldThreshold;
    VariableIdMapping::VariableIdSet _variablesToIgnore;
    int _solver;
    AnalyzerMode _analyzerMode;
    set<const EState*> _newNodesToFold;
    long int _maxTransitions;
    long int _maxIterations;
    long int _maxBytes;
    long int _maxSeconds;
    long int _maxTransitionsForcedTop;
    long int _maxIterationsForcedTop;
    long int _maxBytesForcedTop;
    long int _maxSecondsForcedTop;
    PState _startPState;
    bool _optionStatusMessages;

    std::list<EState> elistify();
    std::list<EState> elistify(EState res);

    // only used in LTL-driven mode
    void setStartEState(const EState* estate);

    /*! if state exists in stateSet, a pointer to the existing state is returned otherwise 
      a new state is entered into stateSet and a pointer to it is returned.
    */
    const PState* processNew(PState& s);
    const PState* processNewOrExisting(PState& s);
    const EState* processNew(EState& s);
    const EState* processNewOrExisting(EState& s);
    const EState* processCompleteNewOrExisting(const EState* es);
    void topifyVariable(PState& pstate, ConstraintSet& cset, AbstractValue varId);
    bool isTopified(EState& s);
    EStateSet::ProcessingResult process(EState& s);
    EStateSet::ProcessingResult process(Label label, PState pstate, ConstraintSet cset, InputOutput io);
    const ConstraintSet* processNewOrExisting(ConstraintSet& cset);
    
    EState createEState(Label label, PState pstate, ConstraintSet cset);
    EState createEState(Label label, PState pstate, ConstraintSet cset, InputOutput io);
    
    VariableValueMonitor variableValueMonitor;

    bool _treatStdErrLikeFailedAssert;
    bool _skipSelectedFunctionCalls;
    ExplorationMode _explorationMode;
    bool _topifyModeActive;
    bool _explicitArrays;

    // loop-aware mode
    int _swapWorkListsCount; // currently only used for debugging purposes

    int _iterations;
    int _approximated_iterations;
    int _curr_iteration_cnt;
    int _next_iteration_cnt;

    bool _stdFunctionSemantics=true;

    bool _svCompFunctionSemantics;
    string _externalErrorFunctionName; // the call of this function causes termination of analysis
    string _externalNonDetIntFunctionName;
    string _externalNonDetLongFunctionName;
    string _externalExitFunctionName;

    Timer _analysisTimer;

    // =======================================================================
    // ========================== LTLAnalyzer ================================
    // =======================================================================
  public:
    bool isTerminationRelevantLabel(Label label);
    bool isLTLRelevantEState(const EState* estate);
    bool isLTLRelevantLabel(Label label);
    bool isStdIOLabel(Label label);
    std::set<const EState*> nonLTLRelevantEStates();

    std::string generateSpotSTG();

    // reduces all states different to stdin and stdout.
    void stdIOFoldingOfTransitionGraph();
    void semanticFoldingOfTransitionGraph();

    // bypasses and removes all states that are not standard I/O states
    // (old version, works correctly, but has a long execution time)
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
    void reduceGraphInOutWorklistOnly(bool includeIn=true, bool includeOut=true, bool includeErr=false);
    // extracts input sequences leading to each discovered failing assertion where discovered for the first time.
    // stores results in PropertyValueTable "reachabilityResults".
    // returns length of the longest of these sequences if it can be guaranteed that all processed traces are the
    // shortest ones leading to the individual failing assertion (returns -1 otherwise).
    int extractAssertionTraces();

    // LTLAnalyzer
  private:
    //returns a list of transitions representing existing paths from "startState" to all possible input/output/error states (no output -> output)
    // collection of transitions to worklist states currently disabled. the returned set has to be deleted by the calling function.
    boost::unordered_set<Transition*>* transitionsToInOutErrAndWorklist( const EState* startState, 
                                                                         bool includeIn, 
                                                                         bool includeOut, 
                                                                         bool includeErr);                                                          
    boost::unordered_set<Transition*>* transitionsToInOutErrAndWorklist( const EState* currentState, 
                                                                         const EState* startState, 
                                                                         boost::unordered_set<Transition*>* results,
                                                                         boost::unordered_set<const EState*>* visited,
                                                                         bool includeIn, bool includeOut, bool includeErr);
    // adds a string representation of the shortest input path from start state to assertEState to reachabilityResults. returns the length of the 
    // counterexample input sequence.
    int addCounterexample(int assertCode, const EState* assertEState);
    // returns a list of EStates from source to target. Target has to come before source in the STG (reversed trace). 
    std::list<const EState*>reverseInOutSequenceBreadthFirst(const EState* source, const EState* target, bool counterexampleWithOutput = false);
    // returns a list of EStates from source to target (shortest input path). 
    // please note: target has to be a predecessor of source (reversed trace)
    std::list<const EState*> reverseInOutSequenceDijkstra(const EState* source, const EState* target, bool counterexampleWithOutput = false);
    std::list<const EState*> filterStdInOutOnly(std::list<const EState*>& states, bool counterexampleWithOutput = false) const;
    std::string reversedInOutRunToString(std::list<const EState*>& run);
    //returns the shortest possible number of input states on the path leading to "target".
    int inputSequenceLength(const EState* target);

    // begin of solver 9 functions
    bool searchForIOPatterns(PState* startPState, int assertion_id, std::list<int>& inputSuffix, std::list<int>* partialTrace = NULL, int* inputPatternLength=NULL);
    bool containsPatternTwoRepetitions(std::list<int>& sequence);
    bool containsPatternTwoRepetitions(std::list<int>& sequence, int startIndex, int endIndex);
    bool computePStateAfterInputs(PState& pState, std::list<int>& inputs, int thread_id, std::list<int>* iOSequence=NULL);
    bool computePStateAfterInputs(PState& pState, int input, int thread_id, std::list<int>* iOSequence=NULL);
    bool searchPatternPath(int assertion_id, PState& pState, std::list<int>& inputPattern, std::list<int>& inputSuffix, int thread_id,std::list<int>* iOSequence=NULL);
    std::list<int> inputsFromPatternTwoRepetitions(std::list<int> pattern2r);
    string convertToCeString(std::list<int>& ceAsIntegers, int maxInputVal);
    int pStateDepthFirstSearch(PState* startPState, int maxDepth, int thread_id, std::list<int>* partialTrace, int maxInputVal, int patternLength, int PatternIterations);
    // end of solver 9 functions

    void generateSpotTransition(std::stringstream& ss, const Transition& t);
    //less than comarisions on two states according to (#input transitions * #output transitions)
    bool indegreeTimesOutdegreeLessThan(const EState* a, const EState* b);

  public:
    //stores a backup of the created transitionGraph
    void storeStgBackup();
    //load previous backup of the transitionGraph, storing the current version as a backup instead
    void swapStgWithBackup();
    //solver 8 becomes the active solver used by the analyzer. Deletion of previous data iff "resetAnalyzerData" is set to true.
    void setAnalyzerToSolver8(EState* startEState, bool resetAnalyzerData);

    // first: list of new states (worklist), second: set of found existing states
    typedef pair<EStateWorkList,EStateSet> SubSolverResultType;
    SubSolverResultType subSolver(const EState* currentEStatePtr);

    PropertyValueTable* loadAssertionsToReconstruct(string filePath);
    void insertInputVarValue(int i) { _inputVarValues.insert(i); }
    void addInputSequenceValue(int i) { _inputSequence.push_back(i); }
    void resetToEmptyInputSequence() { _inputSequence.clear(); }
    void resetInputSequenceIterator() { _inputSequenceIterator=_inputSequence.begin(); }
    const EState* getEstateBeforeMissingInput() {return _estateBeforeMissingInput;}
    const EState* getLatestErrorEState() {return _latestErrorEState;}
    int numberOfInputVarValues() { return _inputVarValues.size(); }
    std::set<int> getInputVarValues() { return _inputVarValues; }
    void setStgTraceFileName(std::string filename) {
      _stg_trace_filename=filename;
      std::ofstream fout;
      fout.open(_stg_trace_filename.c_str());    // create new file/overwrite existing file
      fout<<"START"<<endl;
      fout.close();    // close. Will be used with append.
    }
  private:
    std::string _stg_trace_filename;

 public:
    // only used temporarily for binary-binding prototype
    std::map<std::string,VariableId> globalVarName2VarIdMapping;
    std::vector<bool> binaryBindingAssert;
    void setAnalyzerMode(AnalyzerMode am) { _analyzerMode=am; }
    void setMaxTransitions(size_t maxTransitions) { _maxTransitions=maxTransitions; }
    void setMaxIterations(size_t maxIterations) { _maxIterations=maxIterations; }
    void setMaxTransitionsForcedTop(size_t maxTransitions) { _maxTransitionsForcedTop=maxTransitions; }
    void setMaxIterationsForcedTop(size_t maxIterations) { _maxIterationsForcedTop=maxIterations; }
    void setMaxBytes(long int maxBytes) { _maxBytes=maxBytes; }
    void setMaxBytesForcedTop(long int maxBytesForcedTop) { _maxBytesForcedTop=maxBytesForcedTop; }
    void setMaxSeconds(long int maxSeconds) { _maxSeconds=maxSeconds; }
    void setMaxSecondsForcedTop(long int maxSecondsForcedTop) { _maxSecondsForcedTop=maxSecondsForcedTop; }
    void setStartPState(PState startPState) { _startPState=startPState; }
    void setReconstructMaxInputDepth(size_t inputDepth) { _reconstructMaxInputDepth=inputDepth; }
    void setReconstructMaxRepetitions(size_t repetitions) { _reconstructMaxRepetitions=repetitions; }
    void setReconstructPreviousResults(PropertyValueTable* previousResults) { _reconstructPreviousResults = previousResults; };
    void setPatternSearchMaxDepth(size_t iODepth) { _patternSearchMaxDepth=iODepth; }
    void setPatternSearchRepetitions(size_t patternReps) { _patternSearchRepetitions=patternReps; }
    void setPatternSearchMaxSuffixDepth(size_t suffixDepth) { _patternSearchMaxSuffixDepth=suffixDepth; }
    void setPatternSearchAssertTable(PropertyValueTable* patternSearchAsserts) { _patternSearchAssertTable = patternSearchAsserts; };
    void setPatternSearchExploration(ExplorationMode explorationMode) { _patternSearchExplorationMode = explorationMode; };

  private:
    int _reconstructMaxInputDepth;
    int _reconstructMaxRepetitions;
    PropertyValueTable* _reconstructPreviousResults;
    PropertyValueTable*  _patternSearchAssertTable;
    int _patternSearchMaxDepth;
    int _patternSearchRepetitions;
    int _patternSearchMaxSuffixDepth;
    ExplorationMode _patternSearchExplorationMode;
    std::list<FailedAssertion> _firstAssertionOccurences;
    const EState* _estateBeforeMissingInput;
    const EState* _latestOutputEState;
    const EState* _latestErrorEState;
  }; // end of class Analyzer

} // end of namespace CodeThorn

#include "RersSpecialization.h"

#endif
