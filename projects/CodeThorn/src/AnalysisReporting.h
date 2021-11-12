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
    void generateVerificationReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer,bool reportDetectedErrorLines);
    void generateAstNodeStats(CodeThornOptions& ctOpt, SgProject* sageProject);
    void generateNullPointerAnalysisStats(CodeThorn::CTAnalysis* analyzer);
    void generateConstantConditionVerificationReport(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, AnalysisSelector);
    void generateAnalysisLocationCSVReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer);
    void generateAnalyzedFunctionsAndFilesReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer);
    void generateVerificationCallGraphDotFile(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, string analysisName, ProgramLocationsReport& report);
    void generateVerificationFunctionsCsvFileAndOverview(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, string analysisName, string overviewFileName, ProgramLocationsReport& report, bool violationReporting);
    void generateInternalAnalysisReport(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer);
    CodeThorn::LabelSet functionLabels(CodeThorn::CTAnalysis* analyzer);
    void generateDeadCodeLocationsVerificationReport(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, LabelSet& unreachable);
    void generateUnusedVariablesReport(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer);

  private:
    /* utility functions */ 
    bool isSystemHeaderLabel(CodeThorn::CTAnalysis* analyzer, Label lab);
    void printSeparationLine();
    string separationLine();
    void calculatefMap(std::map<Label,VerificationResult>& fMap,CTAnalysis* analyzer, LabelSet& functionEntryLabels, Flow& flow, ProgramLocationsReport& report);
  };
}
#endif
