#ifndef CODETHORN_OPTIONS_H
#define CODETHORN_OPTIONS_H

#include "Options.h"
#include <vector>
#include <string>
#include <list>

namespace CodeThorn {
  enum AnalysisSelector { ANALYSIS_NULL_POINTER, ANALYSIS_OUT_OF_BOUNDS, ANALYSIS_UNINITIALIZED, ANALYSIS_NUM };
}


struct CodeThornOptions : public CodeThorn::Options {
  // hidden options
  int maxTransitionsForcedTop1=-1;
  int maxTransitionsForcedTop2=-1;
  int maxTransitionsForcedTop3=-1;
  int maxTransitionsForcedTop4=-1;
  int maxTransitionsForcedTop5=-1;
  int solver;

  // pass on to ROSE
  std::vector<std::string> includeDirs;
  std::vector<std::string> preProcessorDefines;
  bool edgNoWarningsFlag;
  std::string roseAstReadFileName;
  bool roseAstWrite;
  bool roseAstMerge;
  
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
    bool viz=false;
    bool vizTg2=false;
    std::string icfgFileName;
    std::string callGraphFileName;
  } visualization;
  
  // experimental options
  bool ompAst=false;
  bool normalizeAll=false;
  bool normalizeFCalls=false;
  bool normalizePhaseInfo=false;
  bool extendedNormalizedCppFunctionCalls=false; // support for CPP method calls (virtual etc.)
  bool strictChecking=false; // only used for testing when a certain level of precision is enforced, does not impact correctness
  bool inlineFunctions=false;
  int inlineFunctionsDepth=10;
  bool eliminateCompoundStatements=false;
  bool annotateTerms=false; // unparsing
  bool eliminateSTGBackEdges=false;
  bool generateAssertions=false; // unparsing
  bool precisionExactConstraints=false; // obsolete
  std::string stgTraceFileName;
  bool arraysNotInState=false; // exceptional case (double negation intentional)
  bool z3BasedReachabilityAnalysis=false;
  int z3UpperInputBound=-1;
  int z3VerifierErrorNumber=-1;
  bool ssa=false; // transformation
  bool nullPointerAnalysis=false;
  bool outOfBoundsAnalysis=false;
  bool uninitializedMemoryAnalysis=false;
  std::string nullPointerAnalysisFileName;
  std::string outOfBoundsAnalysisFileName;
  std::string uninitializedMemoryAnalysisFileName;
  bool programStatsOnly=false;
  bool programStats=false;
  bool inStateStringLiterals=false;
  bool stdFunctions=false;
  bool ignoreFunctionPointers=false;
  bool ignoreUndefinedDereference=false;
  bool ignoreUnknownFunctions=true;
  int functionResolutionMode=4;
  bool contextSensitive=false; // abitrary length call strings
  int abstractionMode=0;
  int interpreterMode=0;
  std::string interpreterModeOuputFileName;
  bool printWarnings=false;
  bool printViolations=false;
  int optionsSet=0;
  int callStringLength=-1; // not used yet
  bool byteMode=false; // switches between byte-addresses and index-based addresses in PState
  int testSelector=0;
  bool intraProcedural=false;
  int precisionLevel=1;

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

  // visible options
  std::string configFileName;
  bool colors=true;
  std::string csvStatsFileName;
  int displayDiff=10000;
  std::string explorationMode="breadth-first";
  bool quiet=false;
  std::string startFunctionName;
  std::string externalFunctionCallsFileName;
  bool status=false;
  bool reduceCfg=true;
  bool internalChecks=false;
  std::string analyzedProgramCLArgs;
  std::string inputValues;
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
  std::string analyzedFunctionsCSVFileName;
  std::string analyzedFilesCSVFileName;
  std::string externalFunctionsCSVFileName;  
  int threads=1;
  bool unparse=false;
  bool displayVersion=false;

  struct Info {
    bool printVariableIdMapping=false;
    bool printFunctionIdMapping=false;
    bool printAstNodeStats=false;
    std::string astNodeStatsCSVFileName;
    std::string astTraversalCSVFileName;
    int astTraversalCSVMode=1;
    bool printTypeSizeMapping=false;
    std::string typeSizeMappingCSVFileName;

  } info;

  bool getInterProceduralFlag();
  bool activeOptionsRequireZ3Library();
  bool getAnalysisSelectionFlag(CodeThorn::AnalysisSelector asel);
  std::string getAnalysisReportFileName(CodeThorn::AnalysisSelector asel);
  typedef std::list<std::pair<CodeThorn::AnalysisSelector,std::string> > AnalysisListType;
  AnalysisListType analysisList() const;

  // default hard code init values
  int maxExactMemorySizeRepresentation=10;
  bool exprEvalTest=false;
   
};

#endif
