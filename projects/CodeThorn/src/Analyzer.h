#ifndef ANALYZER_H
#define ANALYZER_H

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <utility>

#include <omp.h>

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <unordered_map>

#include "TimeMeasurement.h"
#include "AstTerm.h"
#include "Labeler.h"
#include "CFAnalysis.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "ExprAnalyzer.h"
#include "EState.h"
#include "TransitionGraph.h"
#include "TransitionGraphReducer.h"
#include "PropertyValueTable.h"
#include "CTIOLabeler.h"
#include "VariableValueMonitor.h"
#include "Solver.h"
#include "AnalysisParameters.h"
#include "CounterexampleGenerator.h"

#include "VariableIdMapping.h"
#include "VariableIdMappingExtended.h"
#include "FunctionIdMapping.h"
#include "FunctionCallMapping.h"

// we use INT_MIN, INT_MAX
#include "limits.h"
#include "AstNodeInfo.h"
#include "TypeSizeMapping.h"
#include "CallString.h"
#include "CodeThornOptions.h"
#include "LTLOptions.h"
#include "ltlthorn-lib/ParProOptions.h"
#include "DFAnalysisBase.h"
#include "EStateTransferFunctions.h"
#include "EStateWorkList.h"

namespace CodeThorn {

  typedef std::pair<int, const EState*> FailedAssertion;
  typedef std::pair<PState,  std::list<int> > PStatePlusIOHistory;
  enum AnalyzerMode { AM_ALL_STATES, AM_LTL_STATES };

  class SpotConnection;

  struct hash_pair {
    template <class T1, class T2>
      size_t operator()(const pair<T1, T2>& p) const
    {
      auto hash1 = hash<T1>{}(p.first);
      auto hash2 = hash<T2>{}(p.second);
      return hash1 ^ hash2;
    }
  };

  /*!
   * \author Markus Schordan
   * \date 2012.
   */


    class EStatePriorityWorkList {
  public:
    typedef std::list<const EState*>::iterator iterator;
    bool empty();
    void push_front(const EState* el);
    void pop_front();
    const EState* front();
    void push_back(const EState*);
    size_t size();
    void clear();
    iterator begin();
    iterator end();
  private:
    std::list<const EState*> _list;
  };

  class Analyzer : public DFAnalysisBase {
    friend class Solver;
    friend class Solver5;
    friend class Solver8;
    friend class Solver10;
    friend class Solver11;
    friend class Solver12;
    friend class Visualizer;
    friend class VariableValueMonitor;
    friend class ExprAnalyzer;
  public:
    static void initDiagnostics();
    Analyzer();
    virtual ~Analyzer();

    void initAstNodeInfo(SgNode* node);
    virtual void initializeSolver(std::string functionToStartAt,SgNode* root, bool oneFunctionOnly);
    void initLabeledAssertNodes(SgProject* root);
    
    void setExplorationMode(ExplorationMode em) { _explorationMode=em; }
    ExplorationMode getExplorationMode() { return _explorationMode; }

    void setSolver(Solver* solver);
    Solver* getSolver();

    //! requires init
    virtual void runSolver();

    // experimental: analysis reset and/or backup
    virtual void resetAnalysis();
    //stores a backup of the created transitionGraph
    void storeStgBackup();
    //load previous backup of the transitionGraph, storing the current version as a backup instead
    void swapStgWithBackup();

    // reductions based on a nested BFS from the STG's start state
    void reduceStgToInOutStates();
    void reduceStgToInOutAssertStates();
    void reduceStgToInOutAssertErrStates();
    void reduceStgToInOutAssertWorklistStates();

    // initialize command line arguments provided by option "--cl-options" in PState
    void initializeCommandLineArgumentsInState(PState& initialPState);
    void initializeVariableIdMapping(SgProject*);
    void initializeStringLiteralInState(PState& initialPState,SgStringVal* stringValNode, VariableId stringVarId);
    void initializeStringLiteralsInState(PState& initialPState);

    // set the size of an element determined by this type
    void setElementSize(VariableId variableId, SgType* elementType);

    int computeNumberOfElements(SgVariableDeclaration* decl);
    // modifies PState with written initializers
    PState analyzeSgAggregateInitializer(VariableId initDeclVarId, SgAggregateInitializer* aggregateInitializer, PState pState, EState currentEState);
    // modifies PState with written initializers
    EState analyzeVariableDeclaration(SgVariableDeclaration* nextNodeToAnalyze1, EState currentEState, Label targetLabel);

    // thread save; only prints if option status messages is enabled.
    void printStatusMessage(bool);
    void printStatusMessage(string s);
    void printStatusMessageLine(string s);

    void generateAstNodeInfo(SgNode* node);

    void writeWitnessToFile(std::string filename);

    // consistency checks
    bool checkEStateSet();
    bool isConsistentEStatePtrSet(std::set<const EState*> estatePtrSet);
    bool checkTransitionGraph();

    //! The analyzer requires a CFAnalysis to obtain the ICFG.
    void setCFAnalyzer(CFAnalysis* cf);
    CFAnalysis* getCFAnalyzer() const;

    ExprAnalyzer* getExprAnalyzer();

    // access  functions for computed information
    VariableIdMappingExtended* getVariableIdMapping() override;
    FunctionIdMapping* getFunctionIdMapping() override;
    FunctionCallMapping* getFunctionCallMapping();
    FunctionCallMapping2* getFunctionCallMapping2();
    Label getFunctionEntryLabel(SgFunctionRefExp* funRefExp);
    CTIOLabeler* getLabeler() const override;
    Flow* getFlow(); // this is NOT overriding 'DFAnalysis::getFlow() const'
    CodeThorn::PStateSet* getPStateSet();
    EStateSet* getEStateSet();
    TransitionGraph* getTransitionGraph();
    ConstraintSetMaintainer* getConstraintSetMaintainer();
    std::list<FailedAssertion> getFirstAssertionOccurences();

    void setSkipUnknownFunctionCalls(bool defer);
    void setSkipArrayAccesses(bool skip);
    bool getSkipArrayAccesses();
    void setIgnoreUndefinedDereference(bool);
    bool getIgnoreUndefinedDereference();
    void setIgnoreFunctionPointers(bool);
    bool getIgnoreFunctionPointers();

    // specific to the loop-aware exploration modes
    int getIterations() { return _iterations; }
    int getApproximatedIterations() { return _approximated_iterations; }

    // used by the hybrid analyzer (state marshalling)
    void mapGlobalVarInsert(std::string name, int* addr);

    VariableId globalVarIdByName(std::string varName) { return globalVarName2VarIdMapping[varName]; }

    typedef std::list<SgVariableDeclaration*> VariableDeclarationList;
    VariableDeclarationList computeUnusedGlobalVariableDeclarationList(SgProject* root);
    VariableDeclarationList computeUsedGlobalVariableDeclarationList(SgProject* root);

    void insertInputVarValue(int i) { _inputVarValues.insert(i); }
    void addInputSequenceValue(int i) { _inputSequence.push_back(i); }
    void resetToEmptyInputSequence() { _inputSequence.clear(); }
    void resetInputSequenceIterator() { _inputSequenceIterator=_inputSequence.begin(); }

    void setStgTraceFileName(std::string filename);

    void setAnalyzerMode(AnalyzerMode am) { _analyzerMode=am; } // not used
    // 0: concrete, 1: abstract, 2: strict abstract (does not try to approximate all non-supported operators, rejects program instead)
    void setAbstractionMode(int mode) { _abstractionMode=mode; }
    int getAbstractionMode() { return _abstractionMode; }
    void setInterpreterMode(CodeThorn::InterpreterMode mode);
    CodeThorn::InterpreterMode getInterpreterMode();
    void setInterpreterModeOutputFileName(string);
    string getInterpreterModeOutputFileName();

    bool getPrintDetectedViolations();
    void setPrintDetectedViolations(bool flag);

    void setMaxTransitions(size_t maxTransitions) { _maxTransitions=maxTransitions; }
    void setMaxIterations(size_t maxIterations) { _maxIterations=maxIterations; }
    void setMaxTransitionsForcedTop(size_t maxTransitions) { _maxTransitionsForcedTop=maxTransitions; }
    void setMaxIterationsForcedTop(size_t maxIterations) { _maxIterationsForcedTop=maxIterations; }
    void setMaxBytes(long int maxBytes) { _maxBytes=maxBytes; }
    void setMaxBytesForcedTop(long int maxBytesForcedTop) { _maxBytesForcedTop=maxBytesForcedTop; }
    void setMaxSeconds(long int maxSeconds) { _maxSeconds=maxSeconds; }
    void setMaxSecondsForcedTop(long int maxSecondsForcedTop) { _maxSecondsForcedTop=maxSecondsForcedTop; }
    void setResourceLimitDiff(int diff) { _resourceLimitDiff=diff; }
    void setDisplayDiff(int diff) { _displayDiff=diff; }
    void setNumberOfThreadsToUse(int n) { _numberOfThreadsToUse=n; }
    int getNumberOfThreadsToUse() { return _numberOfThreadsToUse; }
    void setTreatStdErrLikeFailedAssert(bool x) { _treatStdErrLikeFailedAssert=x; }
    void setCompoundIncVarsSet(set<AbstractValue> ciVars);
    void setSmallActivityVarsSet(set<AbstractValue> ciVars);
    void setAssertCondVarsSet(set<AbstractValue> acVars);
    /** allows to enable context sensitive analysis. Currently only
        call strings of arbitrary length are supported (recursion is
        not supported yet) */
    void setOptionContextSensitiveAnalysis(bool flag);
    bool getOptionContextSensitiveAnalysis();
  protected:
    void configureOptionSets(CodeThornOptions& ctOpt);
  public:
    enum GlobalTopifyMode {GTM_IO, GTM_IOCF, GTM_IOCFPTR, GTM_COMPOUNDASSIGN, GTM_FLAGS};
    void setGlobalTopifyMode(GlobalTopifyMode mode);
    void setExternalErrorFunctionName(std::string externalErrorFunctionName);
    // enables external function semantics
    void enableSVCompFunctionSemantics();
    void disableSVCompFunctionSemantics();
    bool svCompFunctionSemantics();
    bool getStdFunctionSemantics();
    void setStdFunctionSemantics(bool flag);

    /* command line options provided to analyzed application
       if set they are used to initialize the initial state with argv and argc domain abstractions
    */
    void setCommandLineOptions(vector<string> clOptions);
  protected:
    void setFunctionResolutionModeInCFAnalysis(CodeThornOptions& ctOpt);
  public:
    // TODO: move to flow analyzer (reports label,init,final sets)
    static std::string astNodeInfoAttributeAndNodeToString(SgNode* node);
    SgNode* startFunRoot;
    PropertyValueTable reachabilityResults;
    boost::unordered_map <std::string,int*> mapGlobalVarAddress;
    boost::unordered_map <int*,std::string> mapAddressGlobalVar;
    // only used temporarily for binary-binding prototype
    std::map<std::string,VariableId> globalVarName2VarIdMapping;
    std::vector<bool> binaryBindingAssert;

    // functions related to abstractions during the analysis
    void eventGlobalTopifyTurnedOn();
    bool isActiveGlobalTopify();
    bool isIncompleteSTGReady();
    bool isPrecise();

    //EState createEState(Label label, PState pstate, ConstraintSet cset);
    EState createEStateInternal(Label label, PState pstate, ConstraintSet cset);
    //EState createEState(Label label, PState pstate, ConstraintSet cset, InputOutput io);
    EState createEState(Label label, CallString cs, PState pstate, ConstraintSet cset);
    EState createEState(Label label, CallString cs, PState pstate, ConstraintSet cset, InputOutput io);

    // temporary option
    bool optionStringLiteralsInState=false;
    void reduceStg(function<bool(const EState*)> predicate);

    void initializeSummaryStates(const PState* initialPStateStored, const ConstraintSet* emptycsetstored);
    const CodeThorn::EState* getSummaryState(CodeThorn::Label lab, CallString cs);
    void setSummaryState(CodeThorn::Label lab, CallString cs, CodeThorn::EState const* estate);
    std::string programPositionInfo(CodeThorn::Label);

    bool isApproximatedBy(const EState* es1, const EState* es2);
    EState combine(const EState* es1, const EState* es2);

    void setOptionOutputWarnings(bool flag);
    bool getOptionOutputWarnings();

    // first: list of new states (worklist), second: set of found existing states
    typedef pair<EStateWorkList,std::set<const EState*> > SubSolverResultType;
    SubSolverResultType subSolver(const EState* currentEStatePtr);
    std::string typeSizeMappingToString();
    void setModeLTLDriven(bool ltlDriven) { transitionGraph.setModeLTLDriven(ltlDriven); }
    bool getModeLTLDriven() { return transitionGraph.getModeLTLDriven(); }

    void recordAnalyzedFunction(SgFunctionDefinition* funDef);
    std::string analyzedFunctionsToString();
    std::string analyzedFilesToString();
    void recordExternalFunctionCall(SgFunctionCallExp* funCall);
    std::string externalFunctionsToString();
    void setOptions(CodeThornOptions options);
    CodeThornOptions& getOptionsRef();
    void setLtlOptions(LTLOptions ltlOptions);
    LTLOptions& getLtlOptionsRef();
    //protected:
    /* these functions are used for the internal timer for resource management
       this function is protected to ensure it is not used from outside. It is supposed to be used
       only for internal timing managing the max-time option resource.
    */
    void startAnalysisTimer();
    void stopAnalysisTimer();
    long analysisRunTimeInSeconds();

    static Sawyer::Message::Facility logger;
    void printStatusMessage(string s, bool newLineFlag);

    std::string analyzerStateToString();

    void addToWorkList(const EState* estate); 
    bool isEmptyWorkList();
    const EState* popWorkList();
    const EState* topWorkList();
    void swapWorkLists();

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
    const ConstraintSet* processNewOrExisting(ConstraintSet& cset);

    void recordTransition(const EState* sourceEState, Edge e, const EState* targetEState);

    void set_finished(std::vector<bool>& v, bool val);
    bool all_false(std::vector<bool>& v);

    // determines whether lab is a function call label of a function
    // call of the form 'x=f(...)' and returns the varible-id of the
    // lhs, if a valid pointer is provided
    bool isFunctionCallWithAssignment(Label lab,VariableId* varId=0);
    // this function uses the respective function of ExprAnalyzer and
    // extracts the result from the ExprAnalyzer data structure.
    list<EState> evaluateFunctionCallArguments(Edge edge, SgFunctionCallExp* funCall, EState estate, bool useConstraints);

    // functions for handling callstring contexts
    CallString transferFunctionCallContext(CallString cs, Label lab);
    bool isFeasiblePathContext(CallString& cs,Label lab);

    std::list<EState> transferEdgeEState(Edge edge, const EState* estate);

    // forwarding functions for EStateTransferFunctions (backward compatibility)
    std::list<EState> transferFunctionCall(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallLocalEdge(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallExternal(Edge edge, const EState* estate);
    std::list<EState> transferFunctionCallReturn(Edge edge, const EState* estate);
    std::list<EState> transferFunctionExit(Edge edge, const EState* estate);
    std::list<EState> transferReturnStmt(Edge edge, const EState* estate);
    std::list<EState> transferCaseOptionStmt(SgCaseOptionStmt* stmt,Edge edge, const EState* estate);
    std::list<EState> transferDefaultOptionStmt(SgDefaultOptionStmt* stmt,Edge edge, const EState* estate);
    std::list<EState> transferVariableDeclaration(SgVariableDeclaration* decl,Edge edge, const EState* estate);
    std::list<EState> transferExprStmt(SgNode* nextNodeToAnalyze1, Edge edge, const EState* estate);
    std::list<EState> transferIdentity(Edge edge, const EState* estate);
    std::list<EState> transferAssignOp(SgAssignOp* assignOp, Edge edge, const EState* estate);
    std::list<EState> transferIncDecOp(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate);
    std::list<EState> transferTrueFalseEdge(SgNode* nextNodeToAnalyze2, Edge edge, const EState* estate);
    std::list<EState> elistify();
    std::list<EState> elistify(EState res);
    
    // used by transferAssignOp to seperate evaluation from memory updates (i.e. state modifications)
    typedef std::pair<AbstractValue,AbstractValue> MemoryUpdatePair;
    typedef std::list<std::pair<EState,MemoryUpdatePair> > MemoryUpdateList;
    MemoryUpdateList  evalAssignOp(SgAssignOp* assignOp, Edge edge, const EState* estate);

    // uses ExprAnalyzer to compute the result. Limits the number of results to one result only. Does not permit state splitting.
    // requires normalized AST
    AbstractValue singleValevaluateExpression(SgExpression* expr,EState currentEState);

    std::set<std::string> variableIdsToVariableNames(CodeThorn::VariableIdSet);

    bool isStartLabel(Label label);
    int reachabilityAssertCode(const EState* currentEStatePtr);

    bool isFailedAssertEState(const EState* estate);
    bool isVerificationErrorEState(const EState* estate);
    //! adds a specific code to the io-info of an estate which is checked by isFailedAsserEState and determines a failed-assert estate. Note that the actual assert (and its label) is associated with the previous estate (this information can therefore be obtained from a transition-edge in the transition graph).
    EState createFailedAssertEState(const EState estate, Label target);
    EState createVerificationErrorEState(const EState estate, Label target);

    //! list of all asserts in a program
    //! rers-specific error_x: assert(0) version
    std::list<std::pair<SgLabelStatement*,SgNode*> > listOfLabeledAssertNodes(SgProject *root);
    size_t getNumberOfErrorLabels();
    std::string labelNameOfAssertLabel(Label lab);
    bool isCppLabeledAssertLabel(Label lab);
    std::list<FailedAssertion> _firstAssertionOccurences;

    // specific to the loop-aware exploration modes
    bool isLoopCondLabel(Label lab);
    void incIterations();

    Flow flow;
    CFAnalysis* cfanalyzer;
    std::list<std::pair<SgLabelStatement*,SgNode*> > _assertNodes;
    GlobalTopifyMode _globalTopifyMode;
    set<AbstractValue> _compoundIncVarsSet;
    set<AbstractValue> _smallActivityVarsSet;
    set<AbstractValue> _assertCondVarsSet;
    set<int> _inputVarValues;
    std::list<int> _inputSequence;
    std::list<int>::iterator _inputSequenceIterator;
    ExprAnalyzer exprAnalyzer;
    VariableIdMappingExtended* variableIdMapping;
    FunctionIdMapping functionIdMapping;
    FunctionCallMapping functionCallMapping;
    FunctionCallMapping2 functionCallMapping2;
    // EStateWorkLists: Current and Next should point to One and Two (or swapped)
    EStateWorkList* estateWorkListCurrent;
    EStateWorkList* estateWorkListNext;
    EStateWorkList estateWorkListOne;
    EStateWorkList estateWorkListTwo;
    EStateSet estateSet;
    PStateSet pstateSet;
    ConstraintSetMaintainer constraintSetMaintainer;
    TransitionGraph transitionGraph;
    TransitionGraph backupTransitionGraph;
    TransitionGraphReducer _stgReducer;
    CounterexampleGenerator _counterexampleGenerator;
    int _displayDiff;
    int _resourceLimitDiff;
    int _numberOfThreadsToUse;
    VariableIdMapping::VariableIdSet _variablesToIgnore;
    Solver* _solver;
    AnalyzerMode _analyzerMode;
    long int _maxTransitions;
    long int _maxIterations;
    long int _maxBytes;
    long int _maxSeconds;
    long int _maxTransitionsForcedTop;
    long int _maxIterationsForcedTop;
    long int _maxBytesForcedTop;
    long int _maxSecondsForcedTop;

    VariableValueMonitor variableValueMonitor;
    CodeThornOptions _ctOpt;
    LTLOptions _ltlOpt;
    bool _treatStdErrLikeFailedAssert;
    bool _skipSelectedFunctionCalls;
    ExplorationMode _explorationMode;
    bool _topifyModeActive;
    int _abstractionMode=0; // 0=no abstraction, >=1: different abstraction modes.

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

    std::string _stg_trace_filename;

    TimeMeasurement _analysisTimer;
    bool _timerRunning = false;

    std::vector<string> _commandLineOptions;
    bool _contextSensitiveAnalysis;
    // this is used in abstract mode to hold a pointer to the
    // *current* summary state (more than one may be created to allow
    // to represent multiple summary states in the transition system)
    size_t getSummaryStateMapSize();
    const EState* getBottomSummaryState(Label lab, CallString cs);
    bool isLTLRelevantEState(const EState* estate);

    size_t _prevStateSetSizeDisplay = 0;
    size_t _prevStateSetSizeResource = 0;

    typedef std::unordered_set<SgFunctionDefinition*> AnalyzedFunctionsContainerType;
    AnalyzedFunctionsContainerType analyzedFunctions;
    typedef std::unordered_set<SgFunctionCallExp*> ExternalFunctionsContainerType;
    ExternalFunctionsContainerType externalFunctions;

  private:
    //std::unordered_map<int,const EState*> _summaryStateMap;
    std::unordered_map< pair<int, CallString> ,const EState*, hash_pair> _summaryCSStateMap;
    const CodeThorn::PState* _initialPStateStored=nullptr;
    const CodeThorn::ConstraintSet* _emptycsetstored=nullptr;
    CodeThorn::EStateTransferFunctions* _estateTransferFunctions=nullptr;
  }; // end of class Analyzer
} // end of namespace CodeThorn

#include "RersSpecialization.h"

#endif
