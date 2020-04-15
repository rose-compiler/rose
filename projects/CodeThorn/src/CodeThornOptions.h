#ifndef CODETHORN_OPTIONS_H
#define CODETHORN_OPTIONS_H

#include "Options.h"
#include <vector>
#include <string>

struct CodeThornOptions : public Options {
  // hidden options
  int maxTransitionsForcedTop1;
  int maxTransitionsForcedTop2;
  int maxTransitionsForcedTop3;
  int maxTransitionsForcedTop4;
  int maxTransitionsForcedTop5;
  int solver;

  // pass on to ROSE
  std::vector<std::string> includeDirs;
  std::vector<std::string> preProcessorDefines;
  std::string languageStandard;
  bool edgNoWarningsFlag;

  // visualization
  struct Visualization {
    bool rwClusters;
    bool rwData;
    bool rwHighlightRaces;
    std::string dotIOStg;
    std::string dotIOStgForcedTop;
    bool tg1EStateAddress;
    bool tg1EStateId;
    bool tg1EStateProperties;
    bool tg1EStatePredicate;
    bool tg1EStateMemorySubgraphs;
    bool tg2EStateAddress;
    bool tg2EStateId;
    bool tg2EStateProperties;
    bool tg2EStatePredicate;
    bool visualizeRWSets;
    bool viz;
    bool vizTg2;
    std::string icfgFileName;
  } visualization;
  
  // experimental options
  bool ompAst;
  bool normalizeAll;
  bool normalizeFCalls;
  bool inlineFunctions;
  int inlineFunctionsDepth;
  bool eliminateCompoundStatements;
  bool annotateTerms; // unparsing
  bool eliminateSTGBackEdges;
  bool generateAssertions; // unparsing
  bool precisionExactConstraints; // obsolete
  std::string stgTraceFileName;
  bool explicitArrays;
  bool z3BasedReachabilityAnalysis;
  int z3UpperInputBound;
  int z3VerifierErrorNumber;
  bool ssa; // transformation
  bool nullPointerAnalysis;
  bool outOfBoundsAnalysis;
  bool uninitializedMemoryAnalysis;
  std::string nullPointerAnalysisFileName;
  std::string outOfBoundsAnalysisFileName;
  std::string uninitializedMemoryAnalysisFileName;
  bool programStatsOnly;
  bool programStats;
  bool inStateStringLiterals;
  bool stdFunctions;
  bool ignoreUnknownFunctions;
  bool ignoreUndefinedDereference;
  int functionResolutionMode;
  bool contextSensitive; // abitrary length call strings
  int abstractionMode;
  int interpreterMode;
  std::string interpreterModeOuputFileName;
  bool printWarnings;
  bool printViolations;
  int optionsSet;
  int callStringLength; // not used yet

  // RERS C-subset program options
  struct Rers {
    std::string assertResultsOutputFileName;
    bool eliminateArrays;
    std::string iSeqFile;
    int iSeqLength;
    int iSeqRandomNum;
    bool rersBinary;
    bool rersNumeric;
    bool rersMode;
    bool stdErrLikeFailedAssert;
  } rers;

    // sv-comp options
  struct SVCOMP {
    bool svcompMode;
    std::string detectedErrorFunctionName;
    std::string witnessFileName;
  } svcomp;

  struct EquivalenceChecking {
    std::string dumpSortedFileName;
    std::string dumpNonSortedFileName;
    bool rewriteSSA;
    bool printRewriteTrace;
    bool printUpdateInfos;
    bool ruleConstSubst;
    bool ruleCommutativeSort;
    int maxExtractedUpdates;
    std::string specializeFunName;
    std::vector<int> specializeFunParamList;
    std::vector<int> specializeFunConstList;
    std::vector<std::string> specializeFunVarInitList;
    std::vector<int> specializeFunVarInitConstList;
  } equiCheck;

  struct PatternSearch {
    int maxDepth;
    int repetitions;
    int maxSuffix;
    std::string explorationMode;
  } patSearch;

  struct DataRace {
    bool detection;
    bool checkShuffleAlgorithm;
    std::string csvResultsFile;
    bool failOnError;
  } dr;

  // visible options
  std::string configFileName;
  bool colors;
  std::string csvStatsFileName;
  int displayDiff;
  std::string explorationMode;
  bool quiet;
  std::string startFunctionName;
  std::string externalFunctionCallsFileName;
  bool status;
  bool reduceCfg;
  bool internalChecks;
  std::string analyzedProgramCLArgs;
  std::string inputValues;
  bool inputValuesAsConstraints; // obsolete
  std::string inputSequence;
  std::string logLevel;
  int maxTransitions;
  int maxIterations;
  long maxMemory;
  long maxTime;
  int maxTransitionsForcedTop;
  int maxIterationsForcedTop;
  long maxMemoryForcedTop;
  long maxTimeForcedTop;
  int resourceLimitDiff;
  bool rewrite;
  bool runRoseAstChecks;
  std::string analyzedFunctionsCSVFileName;
  std::string analyzedFilesCSVFileName;
  std::string externalFunctionsCSVFileName;  
  int threads;
  bool unparse;
  bool displayVersion;

  struct Info {
    bool printVariableIdMapping;
    bool printFunctionIdMapping;
    bool printAstNodeStats;
    std::string astNodeStatsCSVFileName;
    bool printTypeSizeMapping;
    std::string typeSizeMappingCSVFileName;

  } info;
  
};

#endif
