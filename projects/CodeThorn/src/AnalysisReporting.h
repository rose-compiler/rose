#ifndef ANALYSIS_REPORTING
#define ANALYSIS_REPORTING

#include "CodeThornOptions.h"
#include "Analyzer.h"

namespace CodeThorn {
  class AnalysisReporting {
  public:
    // generates reports, if result files are specified in ctopt
    static void generateAnalyzedFunctionsAndFilesReports(CodeThornOptions& ctOpt, CodeThorn::Analyzer* analyzer);
    static void generateAstNodeStats(CodeThornOptions& ctOpt, SgProject* sageProject);
    static void generateNullPointerAnalysisStats(CodeThorn::Analyzer* analyzer);
    static void generateAnalysisStatsRawData(CodeThornOptions& ctOpt, CodeThorn::Analyzer* analyzer);
  };
}
#endif
