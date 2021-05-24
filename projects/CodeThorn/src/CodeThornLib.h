#ifndef CODETHORN_LIB_H
#define CODETHORN_LIB_H

#include "Rose/Diagnostics.h"
#include "Normalization.h"
#include "IOAnalyzer.h"
#include "TimingCollector.h"

namespace CodeThorn {

  void initDiagnostics();
  extern Sawyer::Message::Facility logger;
  void turnOffRoseWarnings();
  void configureRose();
  void optionallyRunExprEvalTestAndExit(CodeThornOptions& ctOpt,int argc, char * argv[]);
  void optionallyInitializePatternSearchSolver(CodeThornOptions& ctOpt,IOAnalyzer* analyzer,TimingCollector& timingCollector);
  void optionallySetRersMapping(CodeThornOptions ctOpt,LTLOptions ltlOptions,IOAnalyzer* analyzer);

  /* reads and parses LTL rers mapping file into the alphabet
     sets. Returns false if reading fails. Exits with error message if
     format is wrong. */
  bool readAndParseLTLRersMappingFile(string ltlRersMappingFileName, LtlRersMapping& ltlRersMapping);
  void processCtOptGenerateAssertions(CodeThornOptions& ctOpt, CTAnalysis* analyzer, SgProject* root);
  void optionallyRunInternalChecks(CodeThornOptions& ctOpt, int argc, char * argv[]);
  void optionallyRunInliner(CodeThornOptions& ctOpt, Normalization& normalization, SgProject* sageProject);
  void optionallyRunVisualizer(CodeThornOptions& ctOpt, CTAnalysis* analyzer, SgNode* root);
  void optionallyGenerateExternalFunctionsFile(CodeThornOptions& ctOpt, FunctionCallMapping* funCallMapping);
  void optionallyGenerateAstStatistics(CodeThornOptions& ctOpt, SgProject* sageProject);
  void optionallyGenerateSourceProgramAndExit(CodeThornOptions& ctOpt, SgProject* sageProject);
  void optionallyGenerateTraversalInfoAndExit(CodeThornOptions& ctOpt, SgProject* sageProject);
  void optionallyRunRoseAstChecksAndExit(CodeThornOptions& ctOpt, SgProject* sageProject);
  void optionallyRunIOSequenceGenerator(CodeThornOptions& ctOpt, IOAnalyzer* analyzer);
  void optionallyAnnotateTermsAndUnparse(CodeThornOptions& ctOpt, SgProject* sageProject, CTAnalysis* analyzer);
  void optionallyRunDataRaceDetection(CodeThornOptions& ctOpt, CTAnalysis* analyzer);
  void optionallyPrintProgramInfos(CodeThornOptions& ctOpt, CTAnalysis* analyzer);
  void optionallyRunNormalization(CodeThornOptions& ctOpt,SgProject* sageProject, TimingCollector& timingCollector);
  void setAssertConditionVariablesInAnalyzer(SgNode* root,CTAnalysis* analyzer);
  void optionallyEliminateRersArraysAndExit(CodeThornOptions& ctOpt, SgProject* sageProject, CTAnalysis* analyzer);
  void optionallyWriteSVCompWitnessFile(CodeThornOptions& ctOpt, CTAnalysis* analyzer);
  void optionallyAnalyzeAssertions(CodeThornOptions& ctOpt, LTLOptions& ltlOpt, IOAnalyzer* analyzer, TimingCollector& tc);
  void exprEvalTest(int argc, char* argv[],CodeThornOptions& ctOpt);

  IOAnalyzer* createAnalyzer(CodeThornOptions& ctOpt, LTLOptions& ltlOpt);
  void initializeSolverWithStartFunction(CodeThornOptions& ctOpt,CTAnalysis* analyzer,SgProject* root, TimingCollector& tc);
  void runSolver(CodeThornOptions& ctOpt,CTAnalysis* analyzer, SgProject* sageProject,TimingCollector& tc);

  void optionallyGenerateVerificationReports(CodeThornOptions& ctOpt,CTAnalysis* analyzer);
  void optionallyGenerateCallGraphDotFile(CodeThornOptions& ctOpt,CTAnalysis* analyzer);
  
  SgProject* runRoseFrontEnd(int argc, char * argv[], CodeThornOptions& ctOpt, TimingCollector& timingCollector);
  void normalizationPass(CodeThornOptions& ctOpt, SgProject* sageProject);
  Labeler* createLabeler(SgProject* sageProject, VariableIdMappingExtended* variableIdMapping);
  VariableIdMappingExtended* createVariableIdMapping(CodeThornOptions& ctOpt, SgProject* sageProject);
  CFAnalysis* createControlFlowGraph(CodeThornOptions& ctOpt, SgProject* project, Labeler* labeler);
  //CFAnalysis* createBackwardControlFlowGraph(SgProject* sageProject*, CodeThornOptions& ctOpt, SgProject* sageProject);
  IOAnalyzer* runMemoryAnalysis(CodeThornOptions& ctOpt, VariableIdMapping* vim, Labeler* labeler, CFAnalysis* icfg, TimingCollector& timingCollector);
  //IOAnalyzer* runLTLVerification(CodeThornOptions&, CFAnalysis*,TimingCollector& timingCollector));
  void optionallyPrintRunTimeAndMemoryUsage(CodeThornOptions& ctOpt,TimingCollector& tc);
  
} // end of namespace CodeThorn


#endif
