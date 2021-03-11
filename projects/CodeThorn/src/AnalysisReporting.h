#ifndef ANALYSIS_REPORTING
#define ANALYSIS_REPORTING

#include "CodeThornOptions.h"
#include "CTAnalysis.h"
#include "Labeler.h"

namespace CodeThorn {
  enum VerificationResult { INCONSISTENT, UNVERIFIED, VERIFIED, FALSIFIED, UNREACHABLE };

  class AnalysisReporting {
  public:
    // generates reports, if result files are specified in ctopt
    static void generateVerificationReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer,bool reportDetectedErrorLines);
    static void generateAstNodeStats(CodeThornOptions& ctOpt, SgProject* sageProject);
    static void generateNullPointerAnalysisStats(CodeThorn::CTAnalysis* analyzer);
    static void generateConstantConditionVerificationReport(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, AnalysisSelector);
    static void generateAnalysisStatsRawData(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer);
    static void generateAnalyzedFunctionsAndFilesReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer);
    static void generateVerificationCallGraphDotFile(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, string analysisName, ProgramLocationsReport& report);
    static void generateVerificationFunctionsCsvFile(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, string analysisName, ProgramLocationsReport& report, bool violationReporting);
    static CodeThorn::LabelSet functionLabels(CodeThorn::CTAnalysis* analyzer);
    static void generateDeadCodeLocationsVerificationReport(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, LabelSet& unreachable);
  private:
    static bool isSystemHeaderLabel(CodeThorn::CTAnalysis* analyzer, Label lab);
    static void printSeparationLine();
    static void calculatefMap(std::map<Label,VerificationResult>& fMap,CTAnalysis* analyzer, LabelSet& functionEntryLabels, Flow& flow, ProgramLocationsReport& report);
  };
}
#endif
