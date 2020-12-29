#ifndef ANALYSIS_REPORTING
#define ANALYSIS_REPORTING

#include "CodeThornOptions.h"
#include "CTAnalysis.h"
#include "Labeler.h"

namespace CodeThorn {
  class AnalysisReporting {
  public:
    // generates reports, if result files are specified in ctopt
    static void generateVerificationReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer,bool reportDetectedErrorLines);
    static void generateAstNodeStats(CodeThornOptions& ctOpt, SgProject* sageProject);
    static void generateNullPointerAnalysisStats(CodeThorn::CTAnalysis* analyzer);
    static void generateAnalysisStatsRawData(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer);
    static void generateAnalyzedFunctionsAndFilesReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer);
    static void generateVerificationCallGraphDotFile(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, string analysisName, ProgramLocationsReport& report);
    static void generateVerificationFunctionsCsvFile(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, string analysisName, ProgramLocationsReport& report);
    static CodeThorn::LabelSet functionLabels(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer);
  };
}
#endif
