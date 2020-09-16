#ifndef CODETHORN_LIB_H
#define CODETHORN_LIB_H

#include "Diagnostics.h"
#include "Normalization.h"
#include "IOAnalyzer.h"

namespace CodeThorn {

  struct TimingCollector {
    TimeMeasurement timer;
    double frontEndRunTime=0.0;
    double normalizationRunTime=0.0;
    double determinePrefixDepthTime=0.0;
    double extractAssertionTracesTime=0.0;
    double totalInputTracesTime=0.0;
    double initRunTime=0.0;
    double analysisRunTime=0.0;
    void startTimer() { timer.start(); }
    void stopTimer() { timer.stop(); }
    void stopFrontEndTimer() { frontEndRunTime=timer.getTimeDurationAndStop().milliSeconds(); }
    void stopNormalizationTimer() { normalizationRunTime=timer.getTimeDurationAndStop().milliSeconds(); }
  };

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

  void processCtOptGenerateAssertions(CodeThornOptions& ctOpt, Analyzer* analyzer, SgProject* root);
  IOAnalyzer* createAnalyzer(CodeThornOptions& ctOpt, LTLOptions& ltlOpt);
  void optionallyRunInternalChecks(CodeThornOptions& ctOpt, int argc, char * argv[]);
  void optionallyRunInliner(CodeThornOptions& ctOpt, Normalization& normalization, SgProject* sageProject);
  void optionallyRunVisualizer(CodeThornOptions& ctOpt, Analyzer* analyzer, SgNode* root);
  void optionallyGenerateExternalFunctionsFile(CodeThornOptions& ctOpt, SgProject* sageProject);
  void optionallyGenerateAstStatistics(CodeThornOptions& ctOpt, SgProject* sageProject);
  void optionallyGenerateSourceProgramAndExit(CodeThornOptions& ctOpt, SgProject* sageProject);
  void optionallyGenerateTraversalInfoAndExit(CodeThornOptions& ctOpt, SgProject* sageProject);
  void optionallyRunRoseAstChecksAndExit(CodeThornOptions& ctOpt, SgProject* sageProject);
  void optionallyRunIOSequenceGenerator(CodeThornOptions& ctOpt, IOAnalyzer* analyzer);
  void optionallyAnnotateTermsAndUnparse(CodeThornOptions& ctOpt, SgProject* sageProject, Analyzer* analyzer);
  void optionallyRunDataRaceDetection(CodeThornOptions& ctOpt, Analyzer* analyzer);
  SgProject* runRoseFrontEnd(int argc, char * argv[], CodeThornOptions& ctOpt, TimingCollector& timingCollector);
  void optionallyPrintProgramInfos(CodeThornOptions& ctOpt, Analyzer* analyzer);
  void optionallyRunNormalization(CodeThornOptions& ctOpt,SgProject* sageProject, TimingCollector& timingCollector);
  void setAssertConditionVariablesInAnalyzer(SgNode* root,Analyzer* analyzer);
  void optionallyEliminateCompoundStatements(CodeThornOptions& ctOpt, Analyzer* analyzer, SgNode* root);
  void optionallyEliminateRersArraysAndExit(CodeThornOptions& ctOpt, SgProject* sageProject, Analyzer* analyzer);
  void optionallyPrintFunctionIdMapping(CodeThornOptions& ctOpt,Analyzer* analyzer);
  void optionallyWriteSVCompWitnessFile(CodeThornOptions& ctOpt, Analyzer* analyzer);
  void optionallyAnalyzeAssertions(CodeThornOptions& ctOpt, LTLOptions& ltlOpt, IOAnalyzer* analyzer, TimingCollector& tc);
  void optionallyGenerateVerificationReports(CodeThornOptions& ctOpt,Analyzer* analyzer);
  void optionallyGenerateCallGraphDotFile(CodeThornOptions& ctOpt,Analyzer* analyzer);
  void initializeSolverWithStartFunction(CodeThornOptions& ctOpt,Analyzer* analyzer,SgNode* root, TimingCollector& tc);
  void runSolver(CodeThornOptions& ctOpt,Analyzer* analyzer, SgProject* sageProject,TimingCollector& tc);
  void exprEvalTest(int argc, char* argv[],CodeThornOptions& ctOpt);
} // end of namespace CodeThorn


#endif
