#ifndef CODETHORN_OPTIONS_H
#define CODETHORN_OPTIONS_H

#include "Options.h"
#include <vector>
#include <string>
#include <list>
#include <limits>
#include <cstdlib>

namespace CodeThorn {
  enum AnalysisSelector { ANALYSIS_NULL_POINTER, ANALYSIS_OUT_OF_BOUNDS, ANALYSIS_UNINITIALIZED, ANALYSIS_DEAD_CODE, ANALYSIS_OPAQUE_PREDICATE, ANALYSIS_NUM };
}


class CodeThornOptions : public CodeThorn::Options {
public:
  CodeThornOptions();

  // hidden options
  int maxTransitionsForcedTop1=-1;
  int maxTransitionsForcedTop2=-1;
  int maxTransitionsForcedTop3=-1;
  int maxTransitionsForcedTop4=-1;
  int maxTransitionsForcedTop5=-1;
  int solver=5;

  // pass on to ROSE
  std::vector<std::string> includeDirs;
  std::vector<std::string> preProcessorDefines;
  bool edgNoWarningsFlag=true;
  std::string roseAstReadFileName;
  bool roseAstWrite=false;
  bool roseAstMerge=false;

  bool checkCLanguage=true; // check for CLanguage subset (C++ tools need to set this to 'false')

  // visualization
  struct Visualization {
    bool rwClusters=false;
    bool rwData=false;
    bool rwHighlightRaces=false;
    std::string dotIOStg;
    std::string dotIOStgForcedTop;
    bool tg1EStateAddress=false;
    bool tg1EStateId=true;
    bool tg1EStateProperties=true;
    bool tg1EStatePredicate=false;
    bool tg1EStateMemorySubgraphs=false;
    bool tg2EStateAddress=false;
    bool tg2EStateId=true;
    bool tg2EStateProperties=false;
    bool tg2EStatePredicate=false;
    bool visualizeRWSets=false;
    bool vis=false;
    bool visTg2=false;
    std::string icfgFileName;
    std::string callGraphFileName;
    std::string callGraphFileName2;
    bool displayPassThroughLabel=true;
  } visualization;

  // experimental options
  bool ompAst=false;
  int normalizeLevel=0;
  bool normalizePhaseInfo=false;
  bool extendedNormalizedCppFunctionCalls=false; // support for CPP method calls (virtual, constructors etc.)
  bool traceMode=false; // trace mode in solver 16
  std::string stgTraceFileName;
  bool arraysNotInState=false;
  bool strictChecking=false; // only used for testing when a certain level of precision is enforced, does not impact correctness
  int32_t arrayAbstractionIndex=-1; // all elements at and beyond this index are summarized in one summary object. -1 indicates no abstraction

  bool inlineFunctions=false;
  int inlineFunctionsDepth=10;
  bool annotateTerms=false; // unparsing
  bool eliminateSTGBackEdges=false;
  bool generateAssertions=false; // unparsing
  bool precisionExactConstraints=false; // obsolete

  bool z3BasedReachabilityAnalysis=false;
  int z3UpperInputBound=-1;
  int z3VerifierErrorNumber=-1;
  bool ssa=false; // transformation

  bool nullPointerAnalysis=false;
  bool outOfBoundsAnalysis=false;
  bool uninitializedMemoryAnalysis=false;
  bool deadCodeAnalysis=false;
  bool constantConditionAnalysis=false;
  bool reduceStg=false; // calls analyzer->reduceStgToInOutStates();

  std::string reportFilePath=".";
  std::string nullPointerAnalysisFileName="null-pointer.csv"; // used if different to default
  std::string outOfBoundsAnalysisFileName="out-of-bounds.csv"; //used if different to default
  std::string uninitializedMemoryAnalysisFileName="uninitialized.csv";//used if different to default
  std::string deadCodeAnalysisFileName="dead-code-locations.csv"; // only checked to be !=0
  std::string constantConditionAnalysisFileName="opaque-predicate.csv"; // only checked to be !=0
  std::string filePathPrefixToRemove;
  bool generateReports=false;

  bool programStatsOnly=false;
  bool programStats=false;
  std::string programStatsFileName;
  std::string internalAnalysisReportFileName="internal-analysis-report.txt";
  std::string unusedVariablesReportFileName="unused-variables-report.csv";
  
  bool inStateStringLiterals=false;
  bool stdFunctions=false;
  bool ignoreFunctionPointers=false;
  bool ignoreUndefinedDereference=false;
  bool ignoreUnknownFunctions=true;
  bool nullPointerDereferenceKeepGoing=false;
  int functionResolutionMode=4;
  bool contextSensitive=false; // abitrary length call strings
  int abstractionMode=0;
  int interpreterMode=0;

  bool initialStateFilterUnusedVariables=true;

  bool externalSoundness=false; // if set to true, calls to external functions invalidate all variables
  
  std::string interpreterModeOuputFileName;
  bool printWarnings=false;
  //bool printViolations=false;
  int optionsSet=0; // obsolete
  int callStringLength=-1;
  bool byteMode=false; // switches between byte-addresses and index-based addresses in PState
  int testSelector=0;
  bool intraProcedural=false;
  int precisionLevel=0; // keeps default configuration (if =0 configurePrecisionOption does not change any settings)
  bool pointerSetsEnabled=false; // used in more precise pointer analysis
  int domainAbstractionVariant=0; // default
  std::string csvReportModeString="generate";

  bool forkFunctionEnabled=false;
  std::string forkFunctionName="";

  bool sharedPStates=false;
  bool fastPointerHashing=true;
  bool exitOnHashError=false;
  bool readWriteTrace=false;
  
  bool keepErrorStates=false;
  bool abstractionConsistencyCheck=false; // currently only supported in Solver18
  bool passThroughOptimization=true;
  bool temporaryLocalVarOptFlag=false;
  std::string registerAddressStartString="0x10000";
  std::string registerAddressEndString=""; // defaults to max value if empty
  
  // RERS C-subset program options
  struct Rers {
    std::string assertResultsOutputFileName;
    bool eliminateArrays=false;
    std::string iSeqFile;
    int iSeqLength=-1;
    int iSeqRandomNum=-1;
    bool rersBinary=false;
    bool rersNumeric=false;
    bool rersMode=false;
    bool stdErrLikeFailedAssert=false;
  } rers;

    // sv-comp options
  struct SVCOMP {
    bool svcompMode=false;
    std::string detectedErrorFunctionName;
    std::string witnessFileName;
  } svcomp;

  struct EquivalenceChecking {
    std::string dumpSortedFileName;
    std::string dumpNonSortedFileName;
    bool rewriteSSA=false;
    bool printRewriteTrace=false;
    bool printUpdateInfos=false;
    bool ruleConstSubst=false;
    bool ruleCommutativeSort=false;
    int maxExtractedUpdates=5000;
    std::string specializeFunName;
    std::vector<int> specializeFunParamList;
    std::vector<int> specializeFunConstList;
    std::vector<std::string> specializeFunVarInitList;
    std::vector<int> specializeFunVarInitConstList;
  } equiCheck;

  struct PatternSearch {
    int maxDepth=10;
    int repetitions=100;
    int maxSuffix=5;
    std::string explorationMode;
  } patSearch;

  struct DataRace {
    bool detection=false;
    bool checkShuffleAlgorithm=false;
    std::string csvResultsFile;
    bool failOnError=false;
  } dr;

  struct MultiSelectors {
    std::string analysisMode="none"; // values: {"none", "abstract", "concrete"} used to set multiple options in processMultiSelectors
  } multiSelectors;

  // visible options
  std::string configFileName;
  bool colors=true;
  std::string csvStatsFileName;
  int displayDiff=10000;
  std::string explorationMode="breadth-first";
  bool quiet=true; // turns off printing of names of successfully written files
  std::string startFunctionName;
  bool status=false;
  bool reduceCfg=true;
  bool internalChecks=false;
  std::string analyzedProgramCLArgs;
  std::string inputValues="{}";
  bool inputValuesAsConstraints=false; // obsolete
  std::string inputSequence;
  std::string logLevel;
  int maxTransitions=-1;
  int maxIterations=-1;
  long maxMemory=-1;
  long maxTime=-1;
  int maxTransitionsForcedTop=-1;
  int maxIterationsForcedTop=-1;
  long maxMemoryForcedTop=-1;
  long maxTimeForcedTop=-1;
  int resourceLimitDiff=-1;
  bool rewrite=false;
  bool runRoseAstChecks=false;
  std::string analyzedFunctionsCSVFileName="analyzed-functions.csv"; // generated by analysis+reporting
  std::string analyzedFilesCSVFileName="analyzed-files.csv"; // generated by analys+reporting
  std::string externalFunctionCallsCSVFileName="external-function-calls.csv";  // generated by analysis+reporting
  std::string externalFunctionsCSVFileName="external-functions1.csv"; // generated by FunctionCallMapping
  std::string analysisReportOverviewFileName="overview.txt"; // generated by AnalysisReporting
  std::string analyzedExternalFunctionCallsCSVFileName="external-functions2.csv"; // generated by analysis
  int threads=1;
  bool unparse=false;
  bool displayVersion=false;
  bool runSolver=true;

  // consistency checking options
  bool astSymbolCheckFlag=false; // performed by VariableIdMapping, checks consistency of symbol pointers and memory pool
  std::string vimReportFileName="vim-report.txt";
  
  struct Info {
    bool printVariableIdMapping=false;
    bool printAstNodeStats=false;
    std::string astNodeStatsCSVFileName;
    std::string astTraversalCSVFileName;
    int astTraversalCSVMode=1;
    bool printTypeSizeMapping=false;
    std::string typeSizeMappingCSVFileName;
    bool printTransferFunctionInfo=false;
    std::string astTraversalLineColumnCSVFileName;
    bool astSymbolPointerCheckReport=false; // checks if symbol pointers are non-zero or contain memory pool delete fill byte
    std::string astSymbolPointerCheckReportFileName="ast-symbol-pointer-check-report.txt";
    bool dumpFunctionCallMapping=false;
  } info;

  void configurePrecisionOption();
  bool getInterProceduralFlag();
  bool getIntraProceduralFlag();
  bool activeOptionsRequireZ3Library();
  bool getAnalysisSelectionFlag(CodeThorn::AnalysisSelector asel);
  void setAnalysisSelectionFlag(CodeThorn::AnalysisSelector asel, bool flag);
  std::string getAnalysisReportFileName(CodeThorn::AnalysisSelector asel);
  typedef std::list<std::pair<CodeThorn::AnalysisSelector,std::string> > AnalysisListType;
  AnalysisListType analysisList() const;
  void setAnalysisList(CodeThornOptions::AnalysisListType list);
  // default hard code init values
  int maxExactMemorySizeRepresentation=10;
  bool exprEvalTest=false;
  uint64_t getRegisterRangeStart();
  uint64_t getRegisterRangeEnd();
private:
  CodeThornOptions::AnalysisListType _analysisList;
};

#endif
