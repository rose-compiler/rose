#include "sage3basic.h"
#include "AnalysisReporting.h"
#include <iostream>
#include <string>
#include "Analyzer.h"
#include "AstStatistics.h"
#include "CppStdUtilities.h"
#include "ProgramLocationsAnalysis.h"

using namespace std;
using namespace CodeThorn;

namespace CodeThorn {

  void AnalysisReporting::generateVerificationReports(CodeThornOptions& ctOpt, CodeThorn::Analyzer* analyzer, bool reportDetectedErrorLines) {
    for(auto analysisInfo : ctOpt.analysisList()) {
      AnalysisSelector analysisSel=analysisInfo.first;
      string analysisName=analysisInfo.second;
      if(ctOpt.getAnalysisSelectionFlag(analysisSel)) {
        cout<<"\nAnalysis results for "<<analysisName<<" analysis:"<<endl;
        ProgramLocationsReport report=analyzer->getExprAnalyzer()->getProgramLocationsReport(analysisSel);
        report.setAllLocationsOfInterest(analyzer->getCFAnalyzer()->labelsOfIntersetSet());
        report.writeLocationsVerificationReport(cout,analyzer->getLabeler());
        //report->writeFunctionsVerificationReport(cout,analyzer->getLabeler());
        if(reportDetectedErrorLines) {
          LabelSet violatingLabels=report.falsifiedLocations();
          if(report.numDefinitiveLocations()>0) {
            cout<<"Proven errors in program:"<<endl;
            report.writeAllDefinitiveLocationsToStream(cout,analyzer->getLabeler(),false,true,true);
          } else {
            LabelSet u=report.unverifiedLocations();
            if(u.size()==0) {
              cout<<"No violations exist (program verified)."<<endl;
            } else {
              cout<<"No violations proven (violations may exist in unverified portion of program)."<<endl;
            }
          }
        }
      }
    }
  }
  
  void AnalysisReporting::generateAnalysisStatsRawData(CodeThornOptions& ctOpt, CodeThorn::Analyzer* analyzer) {
    for(auto analysisInfo : ctOpt.analysisList()) {
      AnalysisSelector analysisSel=analysisInfo.first;
      string analysisName=analysisInfo.second;
      if(ctOpt.getAnalysisReportFileName(analysisSel).size()>0) {
        ProgramLocationsReport locations=analyzer->getExprAnalyzer()->getProgramLocationsReport(analysisSel);
        string fileName=ctOpt.getAnalysisReportFileName(analysisSel);
        if(!ctOpt.quiet)
          cout<<"Writing "<<analysisName<<" analysis results to file "<<fileName<<endl;
        locations.writeResultFile(fileName,analyzer->getLabeler());
      }
    }
  }
                             
  void AnalysisReporting::generateNullPointerAnalysisStats(CodeThorn::Analyzer* analyzer) {
    ProgramLocationsAnalysis pla;
    LabelSet pdlSet=pla.pointerDereferenceLocations(*analyzer->getLabeler());
    cout<<"Found "<<pdlSet.size()<<" pointer dereference locations."<<endl;
    
  }
  void AnalysisReporting::generateAstNodeStats(CodeThornOptions& ctOpt, SgProject* sageProject) {
    if(ctOpt.info.printAstNodeStats||ctOpt.info.astNodeStatsCSVFileName.size()>0) {
      // from: src/midend/astDiagnostics/AstStatistics.C
      if(ctOpt.info.printAstNodeStats) {
        ROSE_Statistics::AstNodeTraversalStatistics astStats;
        string s=astStats.toString(sageProject);
        cout<<s; // output includes newline at the end
      }
      if(ctOpt.info.astNodeStatsCSVFileName.size()>0) {
        ROSE_Statistics::AstNodeTraversalCSVStatistics astCSVStats;
        string fileName=ctOpt.info.astNodeStatsCSVFileName;
        astCSVStats.setMinCountToShow(1); // default value is 1
        if(!CppStdUtilities::writeFile(fileName, astCSVStats.toString(sageProject))) {
          cerr<<"Error: cannot write AST node statistics to CSV file "<<fileName<<endl;
          exit(1);
        }
      }
    }
  }

  void AnalysisReporting::generateAnalyzedFunctionsAndFilesReports(CodeThornOptions& ctOpt, CodeThorn::Analyzer* analyzer) {
    if(ctOpt.analyzedFunctionsCSVFileName.size()>0) {
      string fileName=ctOpt.analyzedFunctionsCSVFileName;
      if(!ctOpt.quiet)
        cout<<"Writing list of analyzed functions to file "<<fileName<<endl;
      string s=analyzer->analyzedFunctionsToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        cerr<<"Cannot create file "<<fileName<<endl;
        exit(1);
      }
    }

    if(ctOpt.analyzedFilesCSVFileName.size()>0) {
      string fileName=ctOpt.analyzedFilesCSVFileName;
      if(!ctOpt.quiet)
        cout<<"Writing list of analyzed files to file "<<fileName<<endl;
      string s=analyzer->analyzedFilesToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        cerr<<"Cannot create file "<<fileName<<endl;
        exit(1);
      }
    }

    if(ctOpt.externalFunctionsCSVFileName.size()>0) {
      string fileName=ctOpt.externalFunctionsCSVFileName;
      if(!ctOpt.quiet)
        cout<<"Writing list of external functions to file "<<fileName<<endl;
      string s=analyzer->externalFunctionsToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        cerr<<"Cannot create file "<<fileName<<endl;
        exit(1);
      }
    }
  }
} // end of namespace CodeThorn
