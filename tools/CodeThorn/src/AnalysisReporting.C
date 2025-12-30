#include "sage3basic.h"
#include "AnalysisReporting.h"
#include <iostream>
#include <string>
#include "CTAnalysis.h"
#include "AstStatistics.h"
#include "CppStdUtilities.h"
#include "ProgramLocationsAnalysis.h"
#include "BoolLattice.h"
#include "ConstantConditionAnalysis.h"
#include "MemoryViolationAnalysis.h"
#include <climits>

using namespace std;
using namespace CodeThorn;

namespace CodeThorn {

  LabelSet AnalysisReporting::functionLabels(CodeThorn::CTAnalysis* analyzer) {
    LabelSet allFunctionLabels;
    LabelSet functionEntryLabels=analyzer->functionEntryLabels();
    Flow& flow=*analyzer->getFlow();
    for(auto entryLabel : functionEntryLabels) {
      LabelSet funLabSet=analyzer->getCFAnalyzer()->functionLabelSet(entryLabel,flow);
      allFunctionLabels+=funLabSet;
    }
    return allFunctionLabels;
  }

  bool AnalysisReporting::isSystemHeaderLabel(CodeThorn::CTAnalysis* analyzer, Label lab) {
    ROSE_ASSERT(analyzer->getLabeler());
    ROSE_ASSERT(lab.isValid());
    SgNode* node=analyzer->getLabeler()->getNode(lab);
    if(SgLocatedNode* locNode=isSgLocatedNode(node)) {
      return SageInterface::insideSystemHeader(locNode);
    } else {
      return false;
    }
  }

  void AnalysisReporting::printSeparationLine() {
    cout<<separationLine();
  }

  string AnalysisReporting::separationLine() {
    return "------------------------------------------------\n";
  }

  void AnalysisReporting::generateVerificationReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, bool reportDetectedErrorLines) {
    for(auto analysisInfo : ctOpt.analysisList()) {
      stringstream ss;
      AnalysisSelector analysisSel=analysisInfo.first;
      string analysisName=analysisInfo.second;
      if(true||ctOpt.getAnalysisSelectionFlag(analysisSel)) {
        ss<<endl;
        ss<<separationLine();
        ss<<"Analysis results for "<<analysisName<<" analysis:"<<endl;
        ss<<separationLine();
        ProgramLocationsReport report=analyzer->getEStateTransferFunctions()->getProgramLocationsReport(analysisSel);
        LabelSet labelsOfInterest2=AnalysisReporting::functionLabels(analyzer);
        // compute partioning
        LabelSet reachableLabels;
        LabelSet unreachableLabels;
        for(auto lab : labelsOfInterest2) {
          if(analyzer->isUnreachableLabel(lab)) {
            unreachableLabels.insert(lab);
          } else {
            reachableLabels.insert(lab);
          }
        }
        report.setReachableLocations(reachableLabels);
        report.setUnreachableLocations(unreachableLabels);

        switch(analysisSel) {
        case ANALYSIS_NULL_POINTER:
        case ANALYSIS_OUT_OF_BOUNDS:
        case ANALYSIS_UNINITIALIZED: {
          string overviewFileName=ctOpt.reportFilePath+"/"+analysisName+"-"+ctOpt.analysisReportOverviewFileName;
          report.filterDefinitiveFromPotentialLocations();
          report.writeLocationsVerificationOverview(ctOpt,ss,analyzer->getLabeler()); // overview
          ss<<separationLine();
          if(reportDetectedErrorLines) {
            LabelSet violatingLabels=report.falsifiedLocations();
            if(report.numDefinitiveLocations()>0) {
              ss<<"Proven errors in program:"<<endl;
              report.writeAllDefinitiveLocationsToStream(cout,analyzer->getLabeler(),false,true);
            } else {
              LabelSet u=report.unverifiedLocations();
              if(u.size()==0) {
                ss<<"No violations exist - program verified."<<endl;
              } else {
                ss<<"No violations proven - violations may exist in 'undecided' portion of program."<<endl;
              }
            }
          }
          ss<<separationLine();
          ss<<getVerificationFunctionsOverview(ctOpt,analyzer,analysisName,report);
          { /* generate analysis results overview in file */
            if(!CppStdUtilities::writeFile(ctOpt.csvReportModeString, overviewFileName, ss.str())) {
              cerr<<"Error: cannot write file "<<overviewFileName<<endl;
            } else {
              if(!ctOpt.quiet)
                cout<<"Generated analysis results overview in file "<<overviewFileName<<endl;
            }
          }
          if(ctOpt.status) cout<<ss.str();
          generateAnalysisLocationCsvReports(ctOpt,analyzer);
          generateVerificationFunctionsCsvFile(ctOpt,analyzer,analysisName,report);
          generateVerificationCallGraphDotFile(ctOpt,analyzer,analysisName,report);
          break;
        }
        case ANALYSIS_DEAD_CODE: {
          string overviewFileName=ctOpt.reportFilePath+"/"+analysisName+"-"+ctOpt.analysisReportOverviewFileName;
          generateDeadCodeLocationsVerificationReport(ctOpt, analyzer, unreachableLabels, reachableLabels);
          generateVerificationFunctionsCsvFile(ctOpt,analyzer,analysisName,report);
          generateVerificationCallGraphDotFile(ctOpt,analyzer,analysisName,report);
          ss<<separationLine();
          break;
        }
        case ANALYSIS_OPAQUE_PREDICATE:
          generateConstantConditionVerificationReport(ctOpt, analyzer,analysisSel);
          break;
        default:
          cout<<"Error: generateVerificationReports: unknown analysis: "<<analysisSel<<endl;
          exit(1);
        }

      }
    }
  }

  void AnalysisReporting::generateDeadCodeLocationsVerificationReport(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, LabelSet& unreachable, LabelSet& reachable) {
    if(ctOpt.deadCodeAnalysisFileName.size()>0) {
      stringstream locationsCSVFileData;
      std::set<string> deadLocations; // use set to get sorted unique locations
      std::set<string> tmpDeadLocations; // use set to get sorted unique locations
      std::set<string> liveLocations; // use set to get sorted unique locations
      ROSE_ASSERT(analyzer->getLabeler());

      for(auto lab : reachable) {
	ROSE_ASSERT(lab.isValid());
        SgNode* node=analyzer->getLabeler()->getNode(lab);
        string loc;
        if(node) {
          loc=ProgramLocationsReport::findOriginalProgramLocationOfLabel(analyzer->getLabeler(),lab);
        }
        liveLocations.insert(loc);
      }

      for(auto lab : unreachable) {
	ROSE_ASSERT(lab.isValid());
        SgNode* node=analyzer->getLabeler()->getNode(lab);
        string loc;
        if(node) {
          loc=ProgramLocationsReport::findOriginalProgramLocationOfLabel(analyzer->getLabeler(),lab);
        }
        tmpDeadLocations.insert(loc);
      }
      // live and dead locations can be mapped to the same original program location. Only report as dead if no live loc is mapped to a dead loc
      for(auto loc : tmpDeadLocations) {
        if(liveLocations.find(loc)==liveLocations.end()) {
          deadLocations.insert(loc);
        }
      }

      for(auto sloc : deadLocations) {
        locationsCSVFileData<<sloc<<endl;
      }
      string fileName=ctOpt.reportFilePath+"/"+"dead-code-locations.csv";
      if(!CppStdUtilities::writeFile(ctOpt.csvReportModeString, fileName, locationsCSVFileData.str())) {
        cerr<<"Error: cannot write file "<<fileName<<endl;
      } else {
        if(!ctOpt.quiet)
          cout<<"Generated analysis results in file "<<fileName<<endl;
      }
    }
  }

  void AnalysisReporting::generateConstantConditionVerificationReport(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, AnalysisSelector analysisSel) {
    ROSE_ASSERT(analyzer->getEStateTransferFunctions());
    if(ReadWriteListener* readWriteListener=analyzer->getEStateTransferFunctions()->getReadWriteListener("constant-condition")) {
      ROSE_ASSERT(readWriteListener);
      ConstantConditionAnalysis* constCondAnalysis=dynamic_cast<ConstantConditionAnalysis*>(readWriteListener);
      ROSE_ASSERT(constCondAnalysis);
      ConstantConditionAnalysis::ConstConditionsMap& map=*constCondAnalysis->getResultMapPtr();
      stringstream locationsCSVFileData;
      std::uint32_t constTrueCnt=0;
      std::uint32_t constFalseCnt=0;

      std::set<string> resultSet; // use set to get sorted unique locations
      for(auto p : map) {
        string entry;
        Label lab=p.first;
        BoolLattice value=p.second;
        if(value.isTop()) {
          continue;
        }
        ROSE_ASSERT(analyzer->getLabeler());
	ROSE_ASSERT(lab.isValid());
        SgNode* node=analyzer->getLabeler()->getNode(lab);
        SgNode* parent=node->get_parent();
        if(isSgWhileStmt(parent))
          continue;
        entry="opaque-predicate,";
        if(value.isTrue()) {
          constTrueCnt++;
          entry+="true,";
        } else if(value.isFalse()){
          constFalseCnt++;
          entry+="false,";
        } else {
          // bot?
          continue;
        }
        if(node) {
          //cout<<lab.toString()<<","<<value<<endl;
          entry+=ProgramLocationsReport::findOriginalProgramLocationOfLabel(analyzer->getLabeler(),lab);
        } else {
          entry+="unknown-location";
        }
        resultSet.insert(entry);
      }
      // generate file
      for(auto s : resultSet) {
        locationsCSVFileData<<s<<endl;
      }

      if(ctOpt.status) {
        cout<<"constant true        locations: "<<setw(6)<<constTrueCnt<<endl;
        cout<<"constant false       locations: "<<setw(6)<<constFalseCnt<<endl;
        printSeparationLine();
      }
      string fileName=ctOpt.reportFilePath+"/"+ctOpt.getAnalysisReportFileName(analysisSel);
      if(fileName.size()>0) {
        if(!CppStdUtilities::writeFile(ctOpt.csvReportModeString,fileName, locationsCSVFileData.str())) {
          cerr<<"Error: cannot write file "<<fileName<<endl;
        } else {
          if(!ctOpt.quiet)
            cout<<"Generated analysis results in file "<<fileName<<endl;
        }

      }

    } else {
      cout<<"WARNING: Opaque Predicate Analysis report generation: plug-in not registered."<<endl;
    }
  }

  void AnalysisReporting::generateAnalysisLocationCsvReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer) {
    for(auto analysisInfo : ctOpt.analysisList()) {
      AnalysisSelector analysisSel=analysisInfo.first;
      // exception: skip some analysis, because they use their own format
      if(analysisSel==ANALYSIS_OPAQUE_PREDICATE||analysisSel==ANALYSIS_DEAD_CODE)
        continue;
      string analysisName=analysisInfo.second;
      ProgramLocationsReport locations=analyzer->getEStateTransferFunctions()->getProgramLocationsReport(analysisSel);
      string fileName=ctOpt.reportFilePath+"/"+analysisName+"-locations.csv";

      // override with user-provided name
      if(analysisSel==ANALYSIS_NULL_POINTER && ctOpt.nullPointerAnalysisFileName.size()>0 && ctOpt.nullPointerAnalysisFileName!="null-pointer.csv") {
	fileName=ctOpt.nullPointerAnalysisFileName;
      } else if(analysisSel==ANALYSIS_OUT_OF_BOUNDS && ctOpt.outOfBoundsAnalysisFileName.size()>0 && ctOpt.outOfBoundsAnalysisFileName!="out-of-bounds.csv") {
	fileName=ctOpt.outOfBoundsAnalysisFileName;
      } else if(analysisSel==ANALYSIS_UNINITIALIZED && ctOpt.uninitializedMemoryAnalysisFileName.size()>0 && ctOpt.uninitializedMemoryAnalysisFileName!="uninitialized.csv") {
	fileName=ctOpt.uninitializedMemoryAnalysisFileName;
      }

      if(!ctOpt.quiet)
	cout<<"Writing "<<analysisName<<" analysis results to file "<<fileName<<endl;
      locations.writeResultFile(ctOpt,fileName,analyzer->getLabeler());
    }
  }

  void AnalysisReporting::generateNullPointerAnalysisStats(CodeThorn::CTAnalysis* analyzer) {
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
        }
      }
    }
  }

  void AnalysisReporting::generateAnalyzedFunctionsAndFilesReports(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer) {
    if(ctOpt.analyzedFunctionsCSVFileName.size()>0) {
      string fileName=ctOpt.reportFilePath+"/"+ctOpt.analyzedFunctionsCSVFileName;
      if(!ctOpt.quiet)
        cout<<"Writing list of analyzed functions to file "<<fileName<<endl;
      string s=analyzer->analyzedFunctionsToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        cerr<<"Cannot create file "<<fileName<<endl;
        exit(1);
      }
    }

    if(ctOpt.analyzedFilesCSVFileName.size()>0) {
      string fileName=ctOpt.reportFilePath+"/"+ctOpt.analyzedFilesCSVFileName;
      if(!ctOpt.quiet)
        cout<<"Writing list of analyzed files to file "<<fileName<<endl;
      string s=analyzer->analyzedFilesToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        cerr<<"Cannot create file "<<fileName<<endl;
        exit(1);
      }
    }

#if 1
    if(ctOpt.analyzedExternalFunctionCallsCSVFileName.size()>0) {
      string fileName=ctOpt.reportFilePath+"/"+ctOpt.analyzedExternalFunctionCallsCSVFileName;
      if(!ctOpt.quiet)
        cout<<"Writing list of called external functions to file "<<fileName<<endl;
      string s=analyzer->externalFunctionsToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        cerr<<"Error: cannot create file "<<fileName<<endl;
      }
    }
#endif
  }

  void AnalysisReporting::generateVerificationCallGraphDotFile(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, string analysisName, ProgramLocationsReport& report) {
    string fileName1=ctOpt.reportFilePath+"/"+analysisName+"-cg1.dot";
    string fileName2=ctOpt.reportFilePath+"/"+analysisName+"-cg2.dot";
    //if(ctOpt.status) cout<<"Generating verification call graph for "<<analysisName<<" analysis."<<endl;
    Flow& flow=*analyzer->getFlow();
    LabelSet functionEntryLabels=analyzer->getCFAnalyzer()->functionEntryLabels(flow);
    std::map<Label,VerificationResult> fMap;

    calculatefMap(fMap,analyzer,functionEntryLabels,flow,report);

    InterFlow::LabelToFunctionMap map=analyzer->getCFAnalyzer()->labelToFunctionMap(flow);

    std::string cgBegin="digraph G {\n concentrate=true\n overlap=false\n ";
    std::string cgEnd="}\n";
    std::string cgEdges=analyzer->getInterFlow()->dotCallGraphEdges(map);
    // generate colored nodes
    std::string nodeColor;
    stringstream cgNodes;
    int numFalsifiedFunctions=0;
    int numUnverifiedFunctions=0;
    int numVerifiedFunctions=0;
    for (auto entryLabel : functionEntryLabels ) {
      switch(fMap[entryLabel]) {
      case FALSIFIED: nodeColor="red";numFalsifiedFunctions++;break;
      case UNVERIFIED: nodeColor="orange";numUnverifiedFunctions++;break;
      case VERIFIED: nodeColor="green";numVerifiedFunctions++;break;
      case INCONSISTENT: nodeColor="orchid1";break;
      case UNREACHABLE: nodeColor="gray";break;
      }
      ROSE_ASSERT(entryLabel.isValid());
      std::string functionName=SgNodeHelper::getFunctionName(analyzer->getLabeler()->getNode(entryLabel));
      std::string dotFunctionName="label=\""+entryLabel.toString()+":"+functionName+"\"";
      //if(nodeColor!="gray")
        cgNodes<<entryLabel.toString()<<" [style=filled, fillcolor="<<nodeColor<<","<<dotFunctionName<<"]"<<endl;
    }

    // without function name, only label id
    stringstream cgNodes2;
    for (auto entryLabel : functionEntryLabels ) {
      switch(fMap[entryLabel]) {
      case FALSIFIED: nodeColor="red";break;
      case UNVERIFIED: nodeColor="orange";break;
      case VERIFIED: nodeColor="green";break;
      case INCONSISTENT: nodeColor="orchid1";break;
      case UNREACHABLE: nodeColor="gray";break;
      }
      std::string functionName=SgNodeHelper::getFunctionName(analyzer->getLabeler()->getNode(entryLabel));
      std::string dotFunctionName="label=\""+entryLabel.toString()+"\"";
      if(nodeColor!="orchid1")
        cgNodes2<<entryLabel.toString()<<" [style=filled, fillcolor="<<nodeColor<<","<<dotFunctionName<<"]"<<endl;
    }

    std::string dotFileString1=cgBegin+cgNodes2.str()+cgEdges+cgEnd;
    if(!CppStdUtilities::writeFile(fileName1, dotFileString1)) {
      cerr<<"Error: could not generate callgraph dot file "<<fileName1<<endl;
      exit(1);
    } else {
      if(!ctOpt.quiet) cout<<"Generated verification call graph "<<fileName1<<endl;
    }

    std::string dotFileString2=cgBegin+cgNodes.str()+cgEdges+cgEnd;
    if(!CppStdUtilities::writeFile(fileName2, dotFileString2)) {
      cerr<<"Error: could not generate callgraph dot file "<<fileName2<<endl;
      exit(1);
    } else {
      if(!ctOpt.quiet) cout<<"Generated verification call graph "<<fileName2<<endl;
    }

  }

  void AnalysisReporting::calculatefMap(std::map<Label,VerificationResult>& fMap,CTAnalysis* analyzer, LabelSet& functionEntryLabels, Flow& flow, ProgramLocationsReport& report) {
    LabelSet verified=report.verifiedLocations();
    LabelSet falsified=report.falsifiedLocations();
    LabelSet unverified=report.unverifiedLocations();

    for(auto entryLabel : functionEntryLabels) {
      if(analyzer->isUnreachableLabel(entryLabel)) {
        fMap[entryLabel]=UNREACHABLE;
        continue;
      }
      LabelSet funLabSet=analyzer->getCFAnalyzer()->functionLabelSet(entryLabel,flow);
      //cout<<"Function label set:"<<funLabSet.toString()<<endl;
      //for(auto lab : funLabSet) {
      //  cout<<lab.toString()<<":"<<analyzer->getLabeler()->getNode(lab)->class_name()<<endl;
      //}
      VerificationResult funVer=INCONSISTENT;
      size_t count=0;
      size_t unreachableCount=0;
      for(auto lab : funLabSet) {
        if(falsified.isElement(lab)) {
          funVer=FALSIFIED;
          break;
        } else if(analyzer->isUnreachableLabel(lab)) {
          unreachableCount++;
        } else if(unverified.isElement(lab)) {
          funVer=UNVERIFIED;
        } else if(verified.isElement(lab)) {
          count++;
        }
      }
      //cout<<"DEBUG: Function verification: "<<SgNodeHelper::getFunctionName(analyzer->getLabeler()->getNode(entryLabel))<<":"<<funLabSet.size()<<" vs "<<count<<" + "<<unreachableCount<<endl;
      if(funLabSet.size()==count+unreachableCount) {
        funVer=VERIFIED;
      }
      fMap[entryLabel]=funVer;
      //cout<<"DEBUG:entrylabel:"<<entryLabel.toString()<<" : "<<funVer<<" : "<< analyzer->getLabeler()->getNode(entryLabel)->unparseToString()<<endl;
    }
  }

  void AnalysisReporting::computeFunctionReportData(CodeThorn::CTAnalysis* analyzer, string /*analysisName*/, ProgramLocationsReport& report) {
    Flow& flow=*analyzer->getFlow();
    std::map<Label,VerificationResult> fMap;
    LabelSet functionEntryLabels=analyzer->getCFAnalyzer()->functionEntryLabels(flow);
    calculatefMap(fMap,analyzer,functionEntryLabels,flow,report);
    InterFlow::LabelToFunctionMap map=analyzer->getCFAnalyzer()->labelToFunctionMap(flow);
    numFalsifiedFunctions=0;
    numUnverifiedFunctions=0;
    numVerifiedFunctions=0;
    numInconsistentFunctions=0;
    numUnreachableFunctions=0;
    for (auto entryLabel : functionEntryLabels ) {
      switch(fMap[entryLabel]) {
      case FALSIFIED: numFalsifiedFunctions++;break;
      case UNVERIFIED: numUnverifiedFunctions++;break;
      case VERIFIED: numVerifiedFunctions++;break;
      case INCONSISTENT: numInconsistentFunctions++;break;
      case UNREACHABLE: numUnreachableFunctions++;break;
      }
    }
  }

  void AnalysisReporting::generateVerificationFunctionsCsvFile(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer, string analysisName, ProgramLocationsReport& report) {
    string csvFileName=ctOpt.reportFilePath+"/"+analysisName+"-functions.csv";
    Flow& flow=*analyzer->getFlow();
    std::map<Label,VerificationResult> fMap;
    LabelSet functionEntryLabels=analyzer->getCFAnalyzer()->functionEntryLabels(flow);
    calculatefMap(fMap,analyzer,functionEntryLabels,flow,report);
    //InterFlow::LabelToFunctionMap map=analyzer->getCFAnalyzer()->labelToFunctionMap(flow);

    stringstream csvFileEntries;
    string csvEntryType;
    for(auto entryLabel : functionEntryLabels ) {
      switch(fMap[entryLabel]) {
      case FALSIFIED: csvEntryType="violated";break;
      case UNVERIFIED: csvEntryType="undecided";break;
      case VERIFIED: csvEntryType="verified";break;
      case INCONSISTENT: csvEntryType="inconsistent";break;
      case UNREACHABLE: csvEntryType="dead";break;
        // intentionally no default case to trigger compiler warning
      }
      ROSE_ASSERT(entryLabel.isValid());
      SgNode* node=analyzer->getLabeler()->getNode(entryLabel);
      string fileName=SgNodeHelper::sourceFilenameToString(node);
      std::string functionName=SgNodeHelper::getFunctionName(node);
      //      if(csvEntryType!="inconsistent")
      if(analysisName!="dead-code") {
        csvFileEntries<<fileName<<","<<functionName<<","<<csvEntryType<<endl;
      } else {
        if(csvEntryType=="dead")
          csvFileEntries<<fileName<<","<<functionName<<","<<csvEntryType<<endl;
      }
    }

    if(!CppStdUtilities::writeFile(ctOpt.csvReportModeString, csvFileName, csvFileEntries.str())) {
      cerr<<"Error: could not generate function verification CSV file "<<csvFileName<<endl;
      exit(1);
    } else {
      if(!ctOpt.quiet) cout<<"Generated function verification CSV file "<<csvFileName<<endl;
    }
  }


  string AnalysisReporting::getVerificationFunctionsOverview(CodeThornOptions& /*ctOpt*/, CodeThorn::CTAnalysis* analyzer, string analysisName, ProgramLocationsReport& report) {
    computeFunctionReportData(analyzer,analysisName,report);
    // compute stats
    int numProvenFunctions=numVerifiedFunctions+numFalsifiedFunctions;
    int numTotalReachableFunctions=numProvenFunctions+numUnverifiedFunctions+numInconsistentFunctions;
    int numTotalUnreachableFunctions=numUnreachableFunctions;
    int numTotalFunctions=numTotalReachableFunctions+numTotalUnreachableFunctions;
    stringstream ss;
    // ss<<"Reachable verified   functions: "<<numProvenFunctions<<" [ "<<numProvenFunctions/(double)numTotalReachableFunctions*100<<"%]"<<endl;
    ss<<"Verified  (definitely safe)    functions: "<<setw(6)<<numVerifiedFunctions<<" ["<<setw(6)<<numVerifiedFunctions/(double)numTotalReachableFunctions*100<<"%]"<<endl;
    ss<<"Violated  (definitely unsafe)  functions: "<<setw(6)<<numFalsifiedFunctions<<" ["<<setw(6)<<numFalsifiedFunctions/(double)numTotalReachableFunctions*100<<"%]"<<endl ;
    ss<<"Undecided (potentially unsafe) functions: "<<setw(6)<<numUnverifiedFunctions<<" ["<<setw(6)<<numUnverifiedFunctions/(double)numTotalReachableFunctions*100<<"%]"<<endl;
    // ss<<"Total reachable      functions: "<<setw(6)<<numTotalReachableFunctions<<" ["<<setw(6)<<numTotalReachableFunctions/(double)numTotalFunctions*100<<"%]"<<endl;
    ss<<"Dead (unreachable)             functions: "<<setw(6)<<numTotalUnreachableFunctions<<" ["<<setw(6)<<numTotalUnreachableFunctions/(double)numTotalFunctions*100<<"%]"<<endl;
    ss<<"Total                          functions: "<<setw(6)<<numTotalFunctions<<endl;
    if(numInconsistentFunctions>0)
      ss<<"Inconsistent                   functions: "<<setw(6)<<numInconsistentFunctions<<" ["<<setw(6)<<numInconsistentFunctions/(double)numTotalUnreachableFunctions*100<<"%]"<<endl;
    ss<<separationLine();
    return ss.str();
  }

  void AnalysisReporting::generateInternalAnalysisReport(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer) {
    ROSE_ASSERT(analyzer);
    string report=analyzer->internalAnalysisReportToString();
    string fileName=ctOpt.reportFilePath+"/"+ctOpt.internalAnalysisReportFileName;
    if(!CppStdUtilities::writeFile(ctOpt.csvReportModeString, fileName, report)) {
      cerr<<"Error: cannot write file "<<fileName<<endl;
    } else {
      if(!ctOpt.quiet)
	cout<<"Generated analysis results in file "<<fileName<<endl;
    }
  }

  void AnalysisReporting::generateUnusedVariablesReport(CodeThornOptions& ctOpt, CodeThorn::CTAnalysis* analyzer) {
    string fileName=ctOpt.reportFilePath+"/"+ctOpt.unusedVariablesReportFileName;
    VariableIdMappingExtended* vim=analyzer->getVariableIdMapping();
    ROSE_ASSERT(vim);
    string report=vim->unusedVariablesCsvReport();
    if(!CppStdUtilities::writeFile(ctOpt.csvReportModeString, fileName, report)) {
      cerr<<"Error: cannot write file "<<fileName<<endl;
    } else {
      if(!ctOpt.quiet)
	cout<<"Generated unused-variables report in file "<<fileName<<endl;
    }
  }

} // end of namespace CodeThorn
