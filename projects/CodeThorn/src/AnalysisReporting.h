#ifndef ANALYSIS_REPORTING
#define ANALYSIS_REPORTING

#include "CodeThornOptions.h"
#include "Analyzer.h"
#include "Labeler.h"

namespace CodeThorn {
  class AnalysisReporting {
  public:
    // generates reports, if result files are specified in ctopt
    static void generateVerificationReports(CodeThornOptions& ctOpt, CodeThorn::Analyzer* analyzer,bool reportDetectedErrorLines);
    static void generateAstNodeStats(CodeThornOptions& ctOpt, SgProject* sageProject);
    static void generateNullPointerAnalysisStats(CodeThorn::Analyzer* analyzer);
    static void generateAnalysisStatsRawData(CodeThornOptions& ctOpt, CodeThorn::Analyzer* analyzer);
    static void generateAnalyzedFunctionsAndFilesReports(CodeThornOptions& ctOpt, CodeThorn::Analyzer* analyzer);
    static void generateVerificationCallGraphDotFile(CodeThornOptions& ctOpt, CodeThorn::Analyzer* analyzer, string analysisName, ProgramLocationsReport& report);
    static void generateVerificationFunctionsCsvFile(CodeThornOptions& ctOpt, CodeThorn::Analyzer* analyzer, string analysisName, ProgramLocationsReport& report);
    static CodeThorn::LabelSet functionLabels(CodeThornOptions& ctOpt, CodeThorn::Analyzer* analyzer);
  };
}
#endif
