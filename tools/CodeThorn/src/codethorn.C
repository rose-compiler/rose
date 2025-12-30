/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "rose.h"
#include "ROSE_UNUSED.h"

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
#include "AnalysisParameters.h"
#include "CodeThornException.h"
#include "CodeThornException.h"
#include "ProgramInfo.h"
#include "FunctionCallMapping.h"
#include "AstStatistics.h"

#include "Normalization.h"
#include "DataDependenceVisualizer.h" // also used for clustered ICFG
#include "Evaluator.h" // CppConstExprEvaluator
#include "CtxCallStrings.h" // for setting call string options
#include "AnalysisReporting.h"

// Z3-based analyser / SSA
#if HAVE_Z3
#include "z3-prover-connection/SSAGenerator.h"
#include "z3-prover-connection/ReachabilityAnalyzerZ3.h"
#endif

#include "ConstantConditionAnalysis.h"
#include "MemoryViolationAnalysis.h"
#include "ReadWriteTraceAnalysis.h"
#include "CodeThornLib.h"
#include "LTLThornLib.h"
#include "CppStdUtilities.h"

// only required for LTL verification
#include "ltlthorn-lib/Solver10.h"
#include "ltlthorn-lib/Solver11.h"
#include "ltlthorn-lib/Solver12.h"

using namespace std;

using namespace CodeThorn;
using namespace boost;

#include "Rose/Diagnostics.h"
using namespace Sawyer::Message;
using namespace CodeThornLib;

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

void optionallyRunZ3AndExit(CodeThornOptions& ctOpt,CTAnalysis* analyzer) {
#ifdef HAVE_Z3
  if(ctOpt.z3BasedReachabilityAnalysis)
    {
      assert(ctOpt.z3UpperInputBound!=-1 && ctOpt.z3VerifierErrorNumber!=-1);
      int RERSUpperBoundForInput=ctOpt.z3UpperInputBound;
      int RERSVerifierErrorNumber=ctOpt.z3VerifierErrorNumber;
      cout << "generateSSAForm()" << endl;
      ReachabilityAnalyzerZ3* reachAnalyzer = new ReachabilityAnalyzerZ3(RERSUpperBoundForInput, RERSVerifierErrorNumber, analyzer, &logger);
      cout << "checkReachability()" << endl;
      reachAnalyzer->checkReachability();

      exit(0);
    }
#else
  ROSE_UNUSED(ctOpt); ROSE_UNUSED(analyzer);
  cerr<<"optionallyRunZ3AndExit: Z3 not installed."<<endl;
  exit(1);
#endif
}

void optionallyRunSSAGeneratorAndExit(CodeThornOptions& ctOpt, CTAnalysis* analyzer) {
#ifdef HAVE_Z3
  if(ctOpt.ssa) {
    SSAGenerator* ssaGen = new SSAGenerator(analyzer, &logger);
    ssaGen->generateSSAForm();
    exit(0);
  }
#else
  ROSE_UNUSED(ctOpt); ROSE_UNUSED(analyzer);
  cerr<<"optionallyRunSSAGeneratorAndExit: Z3 not installed."<<endl;
  exit(1);
#endif
}

int main( int argc, char * argv[] ) {
  try {
    ROSE_INITIALIZE;
    CodeThorn::initDiagnostics();
    CodeThorn::initDiagnosticsLTL();
    CodeThorn::CodeThornLib::configureRose();
    configureRersSpecialization();


    TimingCollector tc;

    tc.startTimer();
    CodeThornOptions ctOpt;
    ctOpt.sharedPStates=false; // experimental

    LTLOptions ltlOpt; // to be moved into separate tool
    ParProOptions parProOpt; // options only available in parprothorn
    parseCommandLine(argc, argv, logger,CodeThorn::CodeThornLib::getCodeThornLibraryVersionNumber(),ctOpt,ltlOpt,parProOpt);
    mfacilities.control(ctOpt.logLevel); SAWYER_MESG(logger[TRACE]) << "Log level is " << ctOpt.logLevel << endl;
    ctOpt.configurePrecisionOption();

    IOAnalyzer* analyzer=CodeThornLib::createAnalyzer(ctOpt,ltlOpt); // sets ctOpt,ltlOpt in analyzer
    CodeThornLib::optionallyRunInternalChecks(ctOpt,argc,argv);
    CodeThornLib::optionallyRunExprEvalTestAndExit(ctOpt,argc,argv);
    analyzer->configureOptions(ctOpt,ltlOpt,parProOpt);

    Solver* solver=nullptr;
    // ltl verification specific solvers
    switch(ctOpt.solver) {
    case 10 :  {
      solver = new Solver10(); break; // only available in LTLThornLib
    }
    case 11 :  {
      solver = new Solver11(); break; // only available in LTLThornLib
    }
    case 12 :  {
      solver = new Solver12(); break; // only available in LTLThornLib
    }
    default:
      solver = CodeThornLib::createSolver(ctOpt); // available in CodeThornLib
    }

    ROSE_ASSERT(solver);
    analyzer->setSolver(solver);
    analyzer->setOptionContextSensitiveAnalysis(ctOpt.contextSensitive);
    optionallySetRersMapping(ctOpt,ltlOpt,analyzer);
    tc.stopTimer();

    if(ctOpt.generateReports) {
      // set fixed set of analyses
      CodeThornOptions::AnalysisListType
        analysisList={
                      {CodeThorn::ANALYSIS_NULL_POINTER,"null-pointer"},
                      {CodeThorn::ANALYSIS_OUT_OF_BOUNDS,"out-of-bounds"},
                      {CodeThorn::ANALYSIS_UNINITIALIZED,"uninitialized"},
                      {CodeThorn::ANALYSIS_OPAQUE_PREDICATE,"opaque-predicate"},
                      {CodeThorn::ANALYSIS_DEAD_CODE,"dead-code"},
      };
      ctOpt.setAnalysisList(analysisList);
      if(ctOpt.status) cout<<"STATUS: codethorn: Number of activated analyses: "<<ctOpt.analysisList().size()<<endl;
    }

    SgProject* project=runRoseFrontEnd(argc,argv,ctOpt,tc);
    if(ctOpt.status) cout << "STATUS: Parsing and creating AST finished."<<endl;
    optionallyRunRoseAstChecks(ctOpt, project);

    if(ctOpt.info.printVariableIdMapping) {
      cout<<"VariableIdMapping:"<<endl;
      VariableIdMappingExtended* vim=CodeThorn::CodeThornLib::createVariableIdMapping(ctOpt,project); // print varid mapping and exit
      //AbstractValue::setVariableIdMapping(vim);
      vim->toStream(cout);
      delete vim;
      exit(0);
    }

    optionallyGenerateAstStatistics(ctOpt, project);
    optionallyGenerateTraversalInfoAndExit(ctOpt, project);
    if(ctOpt.status) cout<<"STATUS: analysis started."<<endl;

    VariableIdMappingExtended* vimOrig=CodeThorn::CodeThornLib::createVariableIdMapping(ctOpt,project); // only used for program statistics of original non-normalized program
    //AbstractValue::setVariableIdMapping(vim);
    logger[TRACE]<<"VIM (orig) created."<<endl;

    ProgramInfo originalProgramInfo(project,vimOrig);
    originalProgramInfo.compute();
    logger[TRACE]<<"Program info (orig) computed."<<endl;

    if(ctOpt.programStatsFileName.size()>0) {
      originalProgramInfo.toCsvFileDetailed(ctOpt.programStatsFileName,ctOpt.csvReportModeString);
    }
    if(ctOpt.programStatsOnly) {
      cout<<"=================================="<<endl;
      cout<<"Language Feature Usage Overview"<<endl;
      cout<<"=================================="<<endl;
      cout<<CodeThornLib::programStatsToString(&originalProgramInfo,vimOrig);
      exit(0);
    }

    analyzer->runAnalysisPhase1(project,tc);
    optionallyPrintProgramInfos(ctOpt, analyzer);

    if(ctOpt.programStats) {
      ProgramInfo normalizedProgramInfo(project,analyzer->getVariableIdMapping());
      normalizedProgramInfo.compute();
      analyzer->printStatusMessageLine("==============================================================");
      originalProgramInfo.printCompared(&normalizedProgramInfo);
      analyzer->getVariableIdMapping()->typeSizeOverviewtoStream(cout);
      cout<<CodeThornLib::programStatsToString(&originalProgramInfo,&normalizedProgramInfo, vimOrig);
      CodeThornLib::generateProgramStats(ctOpt,&originalProgramInfo,&normalizedProgramInfo, vimOrig);
    }

    tc.startTimer();
    optionallyGenerateCallGraphDotFile(ctOpt,analyzer);
    tc.stopTimer(TimingCollector::callGraphDotFile);


    optionallyGenerateSourceProgramAndExit(ctOpt, project);
    optionallyGenerateExternalFunctionsFile(ctOpt, analyzer->getFunctionCallMapping());
    optionallyGenerateLineColumnCsv(ctOpt,project);

    // ensure time was not run yet
    tc.startTimer();tc.stopTimer();

    setAssertConditionVariablesInAnalyzer(project,analyzer);
    optionallyEliminateRersArraysAndExit(ctOpt,project,analyzer);
    if(analyzer->getFlow()->getStartLabelSet().size()==0) {
      // exit early
      if(ctOpt.status) cout<<color("normal")<<"done."<<endl;
      exit(0);
    }
    SAWYER_MESG(logger[INFO])<<"registered string literals: "<<analyzer->getVariableIdMapping()->numberOfRegisteredStringLiterals()<<endl;
    analyzer->initLabeledAssertNodes(project);
    CodeThorn::optionallyInitializePatternSearchSolver(ctOpt,analyzer,tc);
    AbstractValue::pointerSetsEnabled=ctOpt.pointerSetsEnabled;

    // register RW listeners
    ROSE_ASSERT(analyzer->getLabeler());
    if(ctOpt.generateReports) {
      auto memViolationAnalysis=new MemoryViolationAnalysis();
      memViolationAnalysis->setEStateTransferFunctions(analyzer->getEStateTransferFunctions());
      memViolationAnalysis->setLabeler(analyzer->getLabeler());
      analyzer->getEStateTransferFunctions()->registerReadWriteListener(memViolationAnalysis,"memory-violation");
    }
    if(ctOpt.constantConditionAnalysisFileName.size()>0) {
      auto ccAna=new ConstantConditionAnalysis();
      ccAna->setLabeler(analyzer->getLabeler());
      analyzer->getEStateTransferFunctions()->registerReadWriteListener(ccAna,"constant-condition");
    }
    if(ctOpt.readWriteTrace) {
      auto rwTrace=new ReadWriteTraceAnalysis();
      rwTrace->setLabeler(analyzer->getLabeler());
      analyzer->getEStateTransferFunctions()->registerReadWriteListener(rwTrace,"read-write-trace");
    }

    if(ctOpt.runSolver) {
      analyzer->runAnalysisPhase2(tc);
    } else {
      cout<<"STATUS: skipping solver run."<<endl;
    }

    analyzer->printStatusMessageLine("==============================================================");

    optionallyWriteSVCompWitnessFile(ctOpt, analyzer);
    if(analyzer->getSolver()->checksAssertions())
      optionallyAnalyzeAssertions(ctOpt, ltlOpt, analyzer, tc);

#if HAVE_Z3
    optionallyRunZ3AndExit(ctOpt,analyzer);
#endif

    tc.startTimer();
    optionallyGenerateVerificationReports(ctOpt,analyzer);
    tc.stopTimer(TimingCollector::reportGeneration);

    if(analyzer->getSolver()->createsTransitionSystem()) {
      runLTLAnalysis(ctOpt,ltlOpt,analyzer,tc);
      processCtOptGenerateAssertions(ctOpt, analyzer, project);

      if(ctOpt.reduceStg) {
        analyzer->reduceStgToInOutStates();
      }
    }

    tc.startTimer();
    optionallyRunVisualizer(ctOpt,analyzer,project);
    tc.stopTimer(TimingCollector::visualization);

    optionallyRunIOSequenceGenerator(ctOpt, analyzer);

    delete analyzer;

    // generate report after analyzer is deleted, to see amount of
    // deleted memory in report
    optionallyPrintRunTimeAndMemoryUsageReport(ctOpt,tc);
    generateRunTimeAndMemoryUsageReport(ctOpt,tc);
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
