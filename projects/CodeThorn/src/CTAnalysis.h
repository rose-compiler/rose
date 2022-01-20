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

#include <unordered_set>
#include <unordered_map>

#include "TimeMeasurement.h"
#include "AstTerm.h"
#include "Labeler.h"
#include "CFAnalysis.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include "EStateTransferFunctions.h"
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
#include "FunctionCallMapping.h"
#include "TopologicalSort.h"

// we use INT_MIN, INT_MAX
#include "limits.h"
#include "AstNodeInfo.h"
#include "TypeSizeMapping.h"
#include "CallString.h"
#include "CodeThornOptions.h"
#include "LTLOptions.h"
#include "ParProOptions.h"
#include "LTLRersMapping.h"
#include "DFAnalysisBase.h"
#include "CSDFAnalysisBaseWithoutData.h"
#include "EStateTransferFunctions.h"
#include "EStateWorkList.h"
#include "EStatePriorityWorkList.h"


namespace CodeThorn {

  typedef CallString Context;

  typedef std::pair<int, EStatePtr> FailedAssertion;
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


  class CTAnalysis {
    friend class Solver12;
    friend class Visualizer;
    friend class VariableValueMonitor;
  public:
    static void initDiagnostics();
    CTAnalysis();
    virtual ~CTAnalysis();
    void deleteAllStates();
    virtual void run();

  public:

    void initAstNodeInfo(SgNode* node);

    // overridden in IOAnalyzer
    virtual void runAnalysisPhase1(SgProject* root, TimingCollector& tc);
    virtual void runAnalysisPhase2(TimingCollector& tc);
  protected:
    EStatePtr createInitialEState(SgProject* root, Label slab);
    void initializeSolverWithInitialEState(SgProject* root);
    virtual void postInitializeSolver();
    void runAnalysisPhase1Sub1(SgProject* root, TimingCollector& tc);
    void runAnalysisPhase2Sub1(TimingCollector& tc);
  public:
    virtual void runSolver();

    void initLabeledAssertNodes(SgProject* root);

    void setExplorationMode(ExplorationMode em);
    ExplorationMode getExplorationMode();

    void setSolver(Solver* solver);
    Solver* getSolver();

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

    // thread save; only prints if option status messages is enabled.
    void printStatusMessage(bool);
    void printStatusMessage(string s);
    void printStatusMessageLine(string s);

    // adds attributes for dot generation of AST with labels
    void generateAstNodeInfo(SgNode* node);

    void writeWitnessToFile(std::string filename);

    // consistency checks
    bool checkEStateSet();
    bool isConsistentEStatePtrSet(std::set<EStatePtr> estatePtrSet);
    bool checkTransitionGraph();

    EStateTransferFunctions* getEStateTransferFunctions();

    // access  functions for computed information
    FunctionCallMapping* getFunctionCallMapping();
    FunctionCallMapping2* getFunctionCallMapping2();
    //Label getFunctionEntryLabel(SgFunctionRefExp* funRefExp);

    Labeler* getLabeler() const;
    CTIOLabeler* getIOLabeler() const;
    Flow* getFlow();
    InterFlow* getInterFlow();
    VariableIdMappingExtended* getVariableIdMapping();
    void setVariableIdMapping(VariableIdMappingExtended* vid);
    CFAnalysis* getCFAnalyzer();
    TopologicalSort* getTopologicalSort();

    CodeThorn::PStateSet* getPStateSet();
    EStateSet* getEStateSet();
    TransitionGraph* getTransitionGraph();
    std::list<FailedAssertion> getFirstAssertionOccurences();

    void setSkipUnknownFunctionCalls(bool defer);
    void setSkipArrayAccesses(bool skip);
    bool getSkipArrayAccesses();

    // obtained from ctopt
    bool getIgnoreUndefinedDereference();
    bool getIgnoreFunctionPointers();

    // specific to the loop-aware exploration modes
    size_t getIterations() { return _iterations; }
    size_t getApproximatedIterations() { return _approximated_iterations; }

    // used by the hybrid analyzer (state marshalling)
    void mapGlobalVarInsert(std::string name, int* addr);

    typedef std::list<SgVariableDeclaration*> VariableDeclarationList;
    // deprecated
    VariableDeclarationList computeUnusedGlobalVariableDeclarationList(SgProject* root);
    // deprecated
    VariableDeclarationList computeUsedGlobalVariableDeclarationList(SgProject* root);

    void insertInputVarValue(int i);
    std::set<int> getInputVarValues();
    void setLtlRersMapping(CodeThorn::LtlRersMapping m); // also sets inputvarvalues
    CodeThorn::LtlRersMapping getLtlRersMapping();

    void addInputSequenceValue(int i) { _inputSequence.push_back(i); }
    void resetToEmptyInputSequence() { _inputSequence.clear(); }
    void resetInputSequenceIterator() { _inputSequenceIterator=_inputSequence.begin(); }

    void openStgTraceFile();

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
    size_t getMaxTransitions() { return _maxTransitions; }
    void setMaxIterations(size_t maxIterations) { _maxIterations=maxIterations; }
    size_t getMaxIterations() { return _maxIterations; }
    void setMaxTransitionsForcedTop(size_t maxTransitions) { _maxTransitionsForcedTop=maxTransitions; }
    void setMaxIterationsForcedTop(size_t maxIterations) { _maxIterationsForcedTop=maxIterations; }
    void setMaxBytesForcedTop(long int maxBytesForcedTop) { _maxBytesForcedTop=maxBytesForcedTop; }
    void setMaxSecondsForcedTop(long int maxSecondsForcedTop) { _maxSecondsForcedTop=maxSecondsForcedTop; }
    void setResourceLimitDiff(int diff) { _resourceLimitDiff=diff; }
    void setDisplayDiff(int diff) { _displayDiff=diff; }
    int getDisplayDiff() { return _displayDiff; }
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
    bool isReachableLabel(Label lab);
    bool isUnreachableLabel(Label lab);
  protected:
    using super = CTAnalysis; // allows use of super like in Java without repeating the class name
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
    vector<string> getCommandLineOptions();
    SgNode* getStartFunRoot();
  protected:
    void setFunctionResolutionModeInCFAnalysis(CodeThornOptions& ctOpt);
    void deleteWorkLists();

    // creates topologically order list and initializes work lists
    void setWorkLists(ExplorationMode explorationMode);

    SgNode* _startFunRoot;
  public:
    // TODO: move to flow analyzer (reports label,init,final sets)
    static std::string astNodeInfoAttributeAndNodeToString(SgNode* node);
    PropertyValueTable reachabilityResults;
    std::unordered_map <std::string,int*> mapGlobalVarAddress;
    std::unordered_map <int*,std::string> mapAddressGlobalVar;

    // only used temporarily for binary-binding prototype
    std::vector<bool> binaryBindingAssert;
    // only used in binary-prototype binding
    VariableId globalVarIdByName(std::string varName);

    // functions related to abstractions during the analysis
    void eventGlobalTopifyTurnedOn();
    bool isActiveGlobalTopify();
    bool isIncompleteSTGReady();
    bool isPrecise();

    void reduceStg(function<bool(EStatePtr)> predicate);

    virtual Lattice* getPreInfo(Label lab, CallString context);
    virtual Lattice* getPostInfo(Label lab, CallString context);
    virtual void setPreInfo(Label lab, CallString context, Lattice*);
    virtual void setPostInfo(Label lab, CallString context, Lattice*);

    void initializeSummaryStates(PStatePtr initialPStateStored);
    EStatePtr getSummaryState(CodeThorn::Label lab, CallString cs);
    void setSummaryState(CodeThorn::Label lab, CallString cs, EStatePtr estate);
    std::string programPositionInfo(CodeThorn::Label);

    void setOptionOutputWarnings(bool flag);
    bool getOptionOutputWarnings();

    // first: list of new states (worklist), second: set of found existing states
    typedef pair<EStateWorkList,std::set<EStatePtr> > SubSolverResultType;
    SubSolverResultType subSolver(EStatePtr currentEStatePtr);
    std::string typeSizeMappingToString();
    void setModeLTLDriven(bool ltlDriven) { transitionGraph.setModeLTLDriven(ltlDriven); }
    bool getModeLTLDriven() { return transitionGraph.getModeLTLDriven(); }

    LabelSet functionEntryLabels();
    LabelSet reachableFunctionEntryLabels();
    SgFunctionDefinition* getFunctionDefinitionOfEntryLabel(Label lab);
    std::string analyzedFunctionsToString();
    std::string analyzedFilesToString();

    void recordExternalFunctionCall(SgFunctionCallExp* funCall);
    std::string externalFunctionsToString();
    void setOptions(CodeThornOptions options);
    CodeThornOptions& getOptionsRef();
    CodeThornOptions getOptions();
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

    void addToWorkList(EStatePtr estate);
    bool isEmptyWorkList();
    EStatePtr popWorkList();
    EStatePtr topWorkList();
    void swapWorkLists();
    void eraseWorkList();

    /*! if state exists in stateSet, a pointer to the existing state is returned otherwise
      a new state is entered into stateSet and a pointer to it is returned.
    */

    PStatePtr processNew(PState& s);
    PStatePtr processNewOrExisting(PState& s);

    PStatePtr processNew(PState* s);
    PStatePtr processNewOrExisting(PState* s);

    EStatePtr processNew(EStateRef s);
    EStatePtr processNewOrExisting(EStateRef s);
    EStateSet::ProcessingResult process(EStateRef s);

    EStatePtr processNew(EStatePtr s);
    EStatePtr processNewOrExisting(EStatePtr s);
    EStateSet::ProcessingResult process(EStatePtr s);

    
    void topifyVariable(PState& pstate, AbstractValue varId);
    bool isTopified(EState& s);

    void recordTransition(EStatePtr sourceEState, Edge e, EStatePtr targetEState);

    void set_finished(std::vector<bool>& v, bool val);
    bool all_false(std::vector<bool>& v);

    std::list<EStatePtr> transferEdgeEStateInPlace(Edge edge, EStatePtr estate);
    std::list<EStatePtr> transferEdgeEState(Edge edge, EStatePtr estate);

    // forwarding functions for EStateTransferFunctions (backward compatibility)
    std::list<EStatePtr> elistify();
    std::list<EStatePtr> elistify(EState res);

    std::set<std::string> variableIdsToVariableNames(CodeThorn::VariableIdSet);

    bool isStartLabel(Label label);
    int reachabilityAssertCode(EStatePtr currentEStatePtr);

    bool isFailedAssertEState(EStatePtr estate);
    bool isVerificationErrorEState(EStatePtr estate);
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

    VariableValueMonitor* getVariableValueMonitor();

    // to be moved to IOAnalyzer
    std::list<std::pair<SgLabelStatement*,SgNode*> > _assertNodes;
    GlobalTopifyMode _globalTopifyMode;
    set<AbstractValue> _compoundIncVarsSet;
    set<AbstractValue> _smallActivityVarsSet;
    set<AbstractValue> _assertCondVarsSet;
    set<int> _inputVarValues;
    LtlRersMapping _ltlRersMapping; // only used for LTL verification
    std::list<int> _inputSequence;
    std::list<int>::iterator _inputSequenceIterator;
    size_t getEStateSetSize();
    size_t getTransitionGraphSize();

    // returns an overview of computed statistics in readable format
    std::string internalAnalysisReportToString();

    uint32_t getTotalNumberOfFunctions();
    void setTotalNumberOfFunctions(uint32_t num);
    std::string hashSetConsistencyReport();

    EStateWorkList* getWorkList();
    EStateWorkList* getWorkListNext(); // only used in Solver12
  protected:

    // EStateWorkLists: Current and Next should point to One and Two (or swapped)
    EStateWorkList* estateWorkListCurrent=0;
    EStateWorkList* estateWorkListNext=0;

    EStateSet estateSet;
    PStateSet pstateSet;
    TransitionGraph transitionGraph;
    TransitionGraph backupTransitionGraph;
    TransitionGraphReducer _stgReducer;
    CounterexampleGenerator _counterexampleGenerator;
    int _displayDiff;
    int _resourceLimitDiff;
    int _numberOfThreadsToUse;
    VariableIdMapping::VariableIdSet _variablesToIgnore;
    AnalyzerMode _analyzerMode;
    long int _maxTransitions;
    long int _maxIterations;
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

    size_t _iterations;
    size_t _approximated_iterations;
    size_t _curr_iteration_cnt;
    size_t _next_iteration_cnt;

    bool _stdFunctionSemantics=true;

    bool _svCompFunctionSemantics;
    string _externalErrorFunctionName; // the call of this function causes termination of analysis
    string _externalNonDetIntFunctionName;
    string _externalNonDetLongFunctionName;
    string _externalExitFunctionName;

    TimeMeasurement _analysisTimer;
    bool _timerRunning = false;

    std::vector<string> _commandLineOptions;
    bool _contextSensitiveAnalysis;
    // this is used in abstract mode to hold a pointer to the
    // *current* summary state (more than one may be created to allow
    // to represent multiple summary states in the transition system)
    //size_t getSummaryStateMapSize();
    EStatePtr getBottomSummaryState(Label lab, CallString cs);
    bool isLTLRelevantEState(EStatePtr estate);

    size_t _prevStateSetSizeDisplay = 0;
    size_t _prevStateSetSizeResource = 0;

    typedef std::unordered_set<SgFunctionDefinition*> AnalyzedFunctionsContainerType;
    AnalyzedFunctionsContainerType analyzedFunctions;
    typedef std::unordered_set<SgFunctionCallExp*> ExternalFunctionsContainerType;
    ExternalFunctionsContainerType externalFunctions;

    SgProject* _root=0; // AST root node, set by phase 1, also used in phase 2.
    uint32_t _statsIntraFinishedFunctions=0;
    uint32_t _statsIntraUnfinishedFunctions=0;
    uint32_t _totalNumberOfFunctions=0; // for status reporting only

  private:

    //std::unordered_map<int,EStatePtr> _summaryStateMap;
    //std::unordered_map< pair<int, CallString> ,EStatePtr, hash_pair> _summaryCSStateMap;
    typedef std::unordered_map <CallString ,EStatePtr> SummaryCSStateMap;
    std::unordered_map< int, SummaryCSStateMap > _summaryCSStateMapMap;

    Labeler* _labeler=nullptr;
    VariableIdMappingExtended* _variableIdMapping=nullptr;
    FunctionCallMapping* _functionCallMapping=nullptr;
    FunctionCallMapping2* _functionCallMapping2=nullptr;
    //~ ClassHierarchyWrapper* _classHierarchy=nullptr;
    ClassAnalysis* _classHierarchy=nullptr;
    VirtualFunctionAnalysis* _virtualFunctions=nullptr;
    CFAnalysis* _cfAnalysis=nullptr;
    Solver* _solver;
    TopologicalSort* _topologicalSort=nullptr;

    PStatePtr _initialPStateStored=nullptr;
    CodeThorn::EStateTransferFunctions* _estateTransferFunctions=nullptr;

  }; // end of class CTAnalysis
} // end of namespace CodeThorn

#include "RersSpecialization.h"

#endif
