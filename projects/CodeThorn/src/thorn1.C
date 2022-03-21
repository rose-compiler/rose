
#include "rose.h"

//#include "rose_config.h"

#include "codethorn.h"
#include "SgNodeHelper.h"
#include "Labeler.h"
#include "VariableIdMapping.h"
#include "EState.h"
#include "TimeMeasurement.h"
#include <cstdio>
#include <cstring>
#include <map>

#include "CodeThornCommandLineOptions.h"
#include "RewriteSystem.h"

#include "InternalChecks.h"
#include "AstAnnotator.h"
#include "AstTerm.h"
#include "AbstractValue.h"
#include "AstMatching.h"
#include "AstUtility.h"
#include "ArrayElementAccessData.h"
#include "PragmaHandler.h"
#include "Miscellaneous2.h"
#include "FIConstAnalysis.h"
#include "ReachabilityAnalysis.h"
//#include "EquivalenceChecking.h"
#include "Solver5.h"
#include "Solver16.h"
#include "Solver8.h"
#include "ltlthorn-lib/Solver10.h"
#include "ltlthorn-lib/Solver11.h"
#include "ltlthorn-lib/Solver12.h"
#include "AnalysisParameters.h"
#include "CodeThornException.h"
#include "ProgramInfo.h"
#include "FunctionCallMapping.h"
#include "AstStatistics.h"

#include "DataRaceDetection.h"
#include "AstTermRepresentation.h"
#include "Normalization.h"
#include "DataDependenceVisualizer.h" // also used for clustered ICFG
#include "Evaluator.h" // CppConstExprEvaluator
#include "CtxCallStrings.h" // for setting call string options
#include "AnalysisReporting.h"

#include "MemoryViolationAnalysis.h"
#include "ConstantConditionAnalysis.h"
#include "CodeThornLib.h"
#include "LTLThornLib.h"
#include "CppStdUtilities.h"

using namespace std;

using namespace CodeThorn;
using namespace boost;

#include "Rose/Diagnostics.h"
using namespace Sawyer::Message;

// required for createSolver function
#include "Solver5.h"
#include "Solver16.h"
#include "Solver8.h"
#include "ltlthorn-lib/Solver10.h"
#include "ltlthorn-lib/Solver11.h"
#include "ltlthorn-lib/Solver12.h"


const std::string versionString="0.9.8";

void configureRersSpecialization() {
#ifdef RERS_SPECIALIZATION
  // only included in hybrid RERS analyzers.
  // Init external function pointers for generated property state
  // marshalling functions (5 function pointers named:
  // RERS_Problem::...FP, are initialized in the following external
  // function.
  // An implementation of this function is linked with the hybrid analyzer
  extern void RERS_Problem_FunctionPointerInit();
  RERS_Problem_FunctionPointerInit();
#endif
}

int main( int argc, char * argv[] ) {
  try {
    ROSE_INITIALIZE;
    CodeThorn::CodeThornLib::configureRose();
    configureRersSpecialization();
    CodeThorn::initDiagnosticsLTL();

    TimingCollector tc;

    CodeThornOptions ctOpt;
    LTLOptions ltlOpt; // to be moved into separate tool
    ParProOptions parProOpt; // options only available in parprothorn
    parseCommandLine(argc, argv, logger,versionString,ctOpt,ltlOpt,parProOpt);
    mfacilities.control(ctOpt.logLevel); SAWYER_MESG(logger[TRACE]) << "Log level is " << ctOpt.logLevel << endl;

    // thorn 1 configuration
    ctOpt.intraProcedural=true; // do not check for start function
    ctOpt.runSolver=false; // do not run solver

    SgProject* project=CodeThorn::CodeThornLib::runRoseFrontEnd(argc,argv,ctOpt,tc);
    if(ctOpt.status) cout << "STATUS: Parsing and creating AST finished."<<endl;
    CodeThorn::CodeThornLib::runRoseAstChecks(project); // if this check fails, the program exits

    bool allChecksPassed=true; // if any check fails, this variable goes to false
    
    if(ctOpt.info.astSymbolPointerCheckReport) {
      bool checkOK=CodeThornLib::astSymbolPointerCheck(ctOpt,project);
      if(!checkOK) {
	cout<<"AST symbol pointer check: FAIL"<<endl;
	cout<<"see file ast-symbol-pointer-check.txt for details."<<endl;
      } else {
	cout<<"AST symbol pointer check: PASS"<<endl;
      }
      allChecksPassed=allChecksPassed&&checkOK;
    } else {
      cout<<"AST symbol pointer check: OFF"<<endl;
    }

    tc.startTimer();

    IOAnalyzer* analyzer=CodeThorn::CodeThornLib::createAnalyzer(ctOpt,ltlOpt); // sets ctOpt,ltlOpt in analyzer
    CodeThorn::CodeThornLib::optionallyRunInternalChecks(ctOpt,argc,argv);
    analyzer->configureOptions(ctOpt,ltlOpt,parProOpt);
    analyzer->setSolver(CodeThornLib::createSolver(ctOpt));
    analyzer->setOptionContextSensitiveAnalysis(ctOpt.contextSensitive);
    CodeThorn::CodeThornLib::optionallySetRersMapping(ctOpt,ltlOpt,analyzer);
    tc.stopTimer();

    CodeThorn::CodeThornLib::optionallyGenerateAstStatistics(ctOpt, project);

    CodeThorn::CodeThornLib::optionallyPrintProgramInfos(ctOpt, analyzer);
    

    VariableIdMappingExtended* vimOrig=CodeThorn::CodeThornLib::createVariableIdMapping(ctOpt,project); // only used for program statistics of original non-normalized program
    //AbstractValue::setVariableIdMapping(vim);
    
    ProgramInfo originalProgramInfo(project,vimOrig);
    originalProgramInfo.compute();
    
    if(ctOpt.programStatsFileName.size()>0) {
      originalProgramInfo.toCsvFileDetailed(ctOpt.programStatsFileName,ctOpt.csvReportModeString);
    }

    if(ctOpt.info.printVariableIdMapping) {
      cout<<"VariableIdMapping:"<<endl;
      vimOrig->toStream(cout);
      exit(0);
    }

    if(ctOpt.programStatsOnly) {
      cout<<"=================================="<<endl;
      cout<<"Language Feature Usage Overview"<<endl;
      cout<<"=================================="<<endl;
      originalProgramInfo.printDetailed();
      cout<<endl;
      vimOrig->typeSizeOverviewtoStream(cout);
      exit(0);
    }

    if(ctOpt.status) cout<<"STATUS: analysis phase 1 started."<<endl;
    analyzer->runAnalysisPhase1(project,tc);

    if(!allChecksPassed) {
      cerr<<"Some consistency checks failed. Bailing out."<<endl;
      mfacilities.shutdown();
      exit(1);
    }
    
    if(ctOpt.programStats) {
      analyzer->printStatusMessageLine("==============================================================");
      ProgramInfo normalizedProgramInfo(project,analyzer->getVariableIdMapping());
      normalizedProgramInfo.compute();
      originalProgramInfo.printCompared(&normalizedProgramInfo);
      analyzer->getVariableIdMapping()->typeSizeOverviewtoStream(cout);
    }
    
    ctOpt.generateReports=true;
    CodeThorn::CodeThornLib::optionallyGenerateExternalFunctionsFile(ctOpt, analyzer->getFunctionCallMapping());
    AnalysisReporting anaRep;
    anaRep.generateUnusedVariablesReport(ctOpt,analyzer);

    tc.startTimer();tc.stopTimer();

    if(ctOpt.status) cout<<color("normal")<<"done."<<endl;

    // main function try-catch
  } catch(const CodeThorn::Exception& e) {
    cerr << "Error: " << e.what() << endl;
    mfacilities.shutdown();
    return 1;
  } catch(const std::exception& e) {
    cerr<< "Error: " << e.what() << endl;
    mfacilities.shutdown();
    return 1;
  } catch(char const* str) {
    cerr<< "Error: " << str << endl;
    mfacilities.shutdown();
    return 1;
  } catch(string str) {
    cerr<< "Error: " << str << endl;
    mfacilities.shutdown();
    return 1;
  } catch(...) {
    cerr<< "Error: Unknown exception raised." << endl;
    mfacilities.shutdown();
    return 1;
  }
  mfacilities.shutdown();
  return 0;
}

