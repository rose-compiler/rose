/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

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
#include "ltlthorn-lib/SpotConnection.h"
#include "ltlthorn-lib/CounterexampleAnalyzer.h"
#include "AstUtility.h"
#include "ArrayElementAccessData.h"
#include "PragmaHandler.h"
#include "Miscellaneous2.h"
#include "FIConstAnalysis.h"
#include "ReachabilityAnalysis.h"
#include "EquivalenceChecking.h"
#include "Solver5.h"
#include "Solver8.h"
#include "ltlthorn-lib/Solver10.h"
#include "ltlthorn-lib/Solver11.h"
#include "ltlthorn-lib/Solver12.h"
#include "ReadWriteAnalyzer.h"
#include "AnalysisParameters.h"
#include "CodeThornException.h"
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

// Z3-based analyser / SSA 
#include "z3-prover-connection/SSAGenerator.h"
#include "z3-prover-connection/ReachabilityAnalyzerZ3.h"

#if defined(__unix__) || defined(__unix) || defined(unix)
#include <sys/resource.h>
#endif

#include "CodeThornLib.h"
#include "LTLThornLib.h"
#include "CppStdUtilities.h"

//BOOST includes
#include "boost/lexical_cast.hpp"

using namespace std;

using namespace CodeThorn;
using namespace boost;

#include "Diagnostics.h"
using namespace Sawyer::Message;

// experimental
#include "IOSequenceGenerator.C"

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "Solver5.h"
#include "Solver8.h"
#include "ltlthorn-lib/Solver10.h"
#include "ltlthorn-lib/Solver11.h"
#include "ltlthorn-lib/Solver12.h"

const std::string versionString="1.12.12";



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

Solver* createSolver(CodeThornOptions& ctOpt) {
  Solver* solver = nullptr;
  // solver "factory"
  switch(ctOpt.solver) {
  case 5 :  {  
    solver = new Solver5(); break;
  }
  case 8 :  {  
    solver = new Solver8(); break;
  }
  case 10 :  {  
    solver = new Solver10(); break;
  }
  case 11 :  {  
    solver = new Solver11(); break;
  }
  case 12 :  {  
    solver = new Solver12(); break;
  }
  default :  { 
    logger[ERROR] <<"Unknown solver ID: "<<ctOpt.solver<<endl;
    exit(1);
  }
  }
  return solver;
}

void optionallyRunZ3AndExit(CodeThornOptions& ctOpt,Analyzer* analyzer) {
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
#endif	
}
void optionallyRunSSAGeneratorAndExit(CodeThornOptions& ctOpt, Analyzer* analyzer) {
  if(ctOpt.ssa) {
    SSAGenerator* ssaGen = new SSAGenerator(analyzer, &logger);
    ssaGen->generateSSAForm();
    exit(0);
  }
}

void runLTLAnalysis(CodeThornOptions& ctOpt, LTLOptions& ltlOpt,IOAnalyzer* analyzer, TimingCollector& tc) {
  long pstateSetSize=analyzer->getPStateSet()->size();
  long pstateSetBytes=analyzer->getPStateSet()->memorySize();
  long pstateSetMaxCollisions=analyzer->getPStateSet()->maxCollisions();
  long pstateSetLoadFactor=analyzer->getPStateSet()->loadFactor();
  long eStateSetSize=analyzer->getEStateSet()->size();
  long eStateSetBytes=analyzer->getEStateSet()->memorySize();
  long eStateSetMaxCollisions=analyzer->getEStateSet()->maxCollisions();
  double eStateSetLoadFactor=analyzer->getEStateSet()->loadFactor();
  long transitionGraphSize=analyzer->getTransitionGraph()->size();
  long transitionGraphBytes=transitionGraphSize*sizeof(Transition);
  long numOfconstraintSets=analyzer->getConstraintSetMaintainer()->numberOf();
  long constraintSetsBytes=analyzer->getConstraintSetMaintainer()->memorySize();
  long constraintSetsMaxCollisions=analyzer->getConstraintSetMaintainer()->maxCollisions();
  double constraintSetsLoadFactor=analyzer->getConstraintSetMaintainer()->loadFactor();
  long numOfStdinEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDIN_VAR));
  long numOfStdoutVarEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_VAR));
  long numOfStdoutConstEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_CONST));
  long numOfStderrEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDERR_VAR));
  long numOfFailedAssertEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::FAILED_ASSERT));
  long numOfConstEStates=0;//(analyzer->getEStateSet()->numberOfConstEStates(analyzer->getVariableIdMapping()));
  long numOfStdoutEStates=numOfStdoutVarEStates+numOfStdoutConstEStates;

#if defined(__unix__) || defined(__unix) || defined(unix)
  // Unix-specific solution to finding the peak phyisical memory consumption (rss).
  // Not necessarily supported by every OS.
  struct rusage resourceUsage;
  getrusage(RUSAGE_SELF, &resourceUsage);
  long totalMemory=resourceUsage.ru_maxrss * 1024;
#else
  long totalMemory=pstateSetBytes+eStateSetBytes+transitionGraphBytes+constraintSetsBytes;
#endif

  double totalRunTime=tc.frontEndRunTime+tc.initRunTime+tc.analysisRunTime;

  long pstateSetSizeInf = 0;
  long eStateSetSizeInf = 0;
  long transitionGraphSizeInf = 0;
  long eStateSetSizeStgInf = 0;
  double infPathsOnlyTime = 0;
  double stdIoOnlyTime = 0;

 
  TimeMeasurement& timer=tc.timer;
  if(ltlOpt.stdIOOnly) {
    SAWYER_MESG(logger[TRACE]) << "STATUS: bypassing all non standard I/O states. (P2)"<<endl;
    timer.start();
    if (ltlOpt.keepErrorStates) {
      analyzer->reduceStgToInOutAssertStates();
    } else {
      analyzer->reduceStgToInOutStates();
    }
    if(ltlOpt.inifinitePathsOnly) {
      analyzer->pruneLeaves();
    }
    stdIoOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
  } else {
    if(ltlOpt.inifinitePathsOnly) {
      assert (!ltlOpt.keepErrorStates);
      cout << "recursively removing all leaves (1)."<<endl;
      timer.start();
      analyzer->pruneLeaves();
      infPathsOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
      pstateSetSizeInf=analyzer->getPStateSet()->size();
      eStateSetSizeInf = analyzer->getEStateSet()->size();
      transitionGraphSizeInf = analyzer->getTransitionGraph()->size();
      eStateSetSizeStgInf = (analyzer->getTransitionGraph())->estateSet().size();
    }
  }

  long eStateSetSizeIoOnly = 0;
  long transitionGraphSizeIoOnly = 0;
  double spotLtlAnalysisTime = 0;

  stringstream statisticsSizeAndLtl;
  stringstream statisticsCegpra;

  if (ltlOpt.ltlFormulaeFile.size()>0) {
    logger[INFO] <<"STG size: "<<analyzer->getTransitionGraph()->size()<<endl;
    string ltl_filename = ltlOpt.ltlFormulaeFile;
    if(ctOpt.rers.rersMode) {  //reduce the graph accordingly, if not already done
      if (!ltlOpt.inifinitePathsOnly
          && !ltlOpt.keepErrorStates
          &&!analyzer->getModeLTLDriven()) {
        cout<< "STATUS: recursively removing all leaves (due to RERS-mode (2))."<<endl;
        timer.start();
        analyzer->pruneLeaves();
        infPathsOnlyTime = timer.getTimeDurationAndStop().milliSeconds();

        pstateSetSizeInf=analyzer->getPStateSet()->size();
        eStateSetSizeInf = analyzer->getEStateSet()->size();
        transitionGraphSizeInf = analyzer->getTransitionGraph()->size();
        eStateSetSizeStgInf = (analyzer->getTransitionGraph())->estateSet().size();
      }
      if (!ltlOpt.stdIOOnly &&!analyzer->getModeLTLDriven()) {
        cout << "STATUS: bypassing all non standard I/O states (due to RERS-mode) (P1)."<<endl;
        timer.start();
        analyzer->getTransitionGraph()->printStgSize("before reducing non-I/O states");
        if (ltlOpt.keepErrorStates) {
          analyzer->reduceStgToInOutAssertStates();
        } else {
          analyzer->reduceStgToInOutStates();
        }
        stdIoOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
        analyzer->getTransitionGraph()->printStgSize("after reducing non-I/O states");
      }
    }
    if(ltlOpt.noInputInputTransitions) {  //delete transitions that indicate two input states without an output in between
      analyzer->removeInputInputTransitions();
      analyzer->getTransitionGraph()->printStgSize("after reducing input->input transitions");
    }
    bool withCounterexample = false;
    if(ltlOpt.withCounterExamples || ltlOpt.withLTLCounterExamples) {  //output a counter-example input sequence for falsified formulae
      withCounterexample = true;
    }

    timer.start();
    std::set<int> ltlInAlphabet;// = analyzer->getInputVarValues();
    //take fixed ltl input alphabet if specified, instead of the input values used for stg computation
    if (ltlOpt.ltlInAlphabet.size()>0) {
      string setstring=ltlOpt.ltlInAlphabet;
      ltlInAlphabet=Parse::integerSet(setstring);
      SAWYER_MESG(logger[TRACE]) << "LTL input alphabet explicitly selected: "<< setstring << endl;
    }
    //take ltl output alphabet if specifically described, otherwise take the old RERS specific 21...26 (a.k.a. oU...oZ)
    std::set<int> ltlOutAlphabet;// = Parse::integerSet("{21,22,23,24,25,26}");
    if (ltlOpt.ltlOutAlphabet.size()>0) {
      string setstring=ltlOpt.ltlOutAlphabet;
      ltlOutAlphabet=Parse::integerSet(setstring);
      SAWYER_MESG(logger[TRACE]) << "LTL output alphabet explicitly selected: "<< setstring << endl;
    } else {
      // TODO: fail, if no output alphabet is provided
    }
    PropertyValueTable* ltlResults=nullptr;
    SpotConnection spotConnection(ltl_filename);
    spotConnection.setModeLTLDriven(analyzer->getModeLTLDriven());
    if (analyzer->getModeLTLDriven()) {
      analyzer->setSpotConnection(&spotConnection);
    }

    SAWYER_MESG(logger[TRACE]) << "STATUS: generating LTL results"<<endl;
    bool spuriousNoAnswers = false;
    SAWYER_MESG(logger[TRACE]) << "LTL: check properties."<<endl;
    if (ltlOpt.propertyNrToCheck!=-1) {
      int propertyNum = ltlOpt.propertyNrToCheck;
      spotConnection.checkSingleProperty(propertyNum, *(analyzer->getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample, spuriousNoAnswers);
    } else {
      spotConnection.checkLtlProperties( *(analyzer->getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample, spuriousNoAnswers);
    }
    spotLtlAnalysisTime=timer.getTimeDurationAndStop().milliSeconds();
    SAWYER_MESG(logger[TRACE]) << "LTL: get results from spot connection."<<endl;
    ltlResults = spotConnection.getLtlResults();
    SAWYER_MESG(logger[TRACE]) << "LTL: results computed."<<endl;

    if (ltlOpt.cegpra.ltlPropertyNr!=-1 || ltlOpt.cegpra.checkAllProperties) {
      if (ltlOpt.cegpra.csvStatsFileName.size()>0) {
        statisticsCegpra << "init,";
        analyzer->getTransitionGraph()->printStgSize("initial abstract model");
        analyzer->getTransitionGraph()->csvToStream(statisticsCegpra);
        statisticsCegpra << ",na,na";
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_YES);
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_NO);
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_UNKNOWN);
      }
      CounterexampleAnalyzer ceAnalyzer(analyzer, &statisticsCegpra);
      if (ltlOpt.cegpra.maxIterations!=-1) {
        ceAnalyzer.setMaxCounterexamples(ltlOpt.cegpra.maxIterations);
      }
      if (ltlOpt.cegpra.checkAllProperties) {
        ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(spotConnection, ltlInAlphabet, ltlOutAlphabet);
      } else {
        // cegpra for single LTL property
        //ROSE_ASSERT(ltlOpt.cegpra.ltlPropertyNr!=-1);
        int property = ltlOpt.cegpra.ltlPropertyNr;
        ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(property, spotConnection, ltlInAlphabet, ltlOutAlphabet);
      }
    }

    if(ctOpt.status) {
      ltlResults-> printResults("YES (verified)", "NO (falsified)", "ltl_property_", withCounterexample);
      analyzer->printStatusMessageLine("==============================================================");
      ltlResults->printResultsStatistics();
      analyzer->printStatusMessageLine("==============================================================");
    }
    if (ltlOpt.spotVerificationResultsCSVFileName.size()>0) {  //write results to a file instead of displaying them directly
      std::string csv_filename = ltlOpt.spotVerificationResultsCSVFileName;
      SAWYER_MESG(logger[TRACE]) << "STATUS: writing ltl results to file: " << csv_filename << endl;
      ltlResults->writeFile(csv_filename.c_str(), false, 0, withCounterexample);
    }
    if (ltlOpt.ltlStatisticsCSVFileName.size()>0) {
      analyzer->getTransitionGraph()->printStgSize("final model");
      analyzer->getTransitionGraph()->csvToStream(statisticsSizeAndLtl);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_YES);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_NO);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_UNKNOWN);
    }
#if 0
    if(ltlResults) {
      delete ltlResults;
      ltlResults = NULL;
    }
#endif
    //temporaryTotalRunTime = totalRunTime + infPathsOnlyTime + stdIoOnlyTime + spotLtlAnalysisTime;
    //printAnalyzerStatistics(analyzer, temporaryTotalRunTime, "LTL check complete. Reduced transition system:");
  }
  double totalLtlRunTime =  infPathsOnlyTime + stdIoOnlyTime + spotLtlAnalysisTime;

  double arrayUpdateExtractionRunTime=0.0;
  double arrayUpdateSsaNumberingRunTime=0.0;
  double sortingAndIORunTime=0.0;
  double verifyUpdateSequenceRaceConditionRunTime=0.0;

  int verifyUpdateSequenceRaceConditionsResult=-1;
  int verifyUpdateSequenceRaceConditionsTotalLoopNum=-1;
  int verifyUpdateSequenceRaceConditionsParLoopNum=-1;

  /* Data race detection */
  optionallyRunDataRaceDetection(ctOpt,analyzer);

  double overallTime=totalRunTime + tc.totalInputTracesTime + totalLtlRunTime;
  analyzer->printAnalyzerStatistics(totalRunTime, "STG generation and assertion analysis complete");

  if(ctOpt.csvStatsFileName.size()>0) {
    string filename=ctOpt.csvStatsFileName;
    stringstream text;
    text<<"Sizes,"<<pstateSetSize<<", "
        <<eStateSetSize<<", "
        <<transitionGraphSize<<", "
        <<numOfconstraintSets<<", "
        << numOfStdinEStates<<", "
        << numOfStdoutEStates<<", "
        << numOfStderrEStates<<", "
        << numOfFailedAssertEStates<<", "
        << numOfConstEStates<<endl;
    text<<"Memory,"<<pstateSetBytes<<", "
        <<eStateSetBytes<<", "
        <<transitionGraphBytes<<", "
        <<constraintSetsBytes<<", "
        <<totalMemory<<endl;
    text<<"Runtime(readable),"
        <<CodeThorn::readableruntime(tc.frontEndRunTime)<<", "
        <<CodeThorn::readableruntime(tc.initRunTime)<<", "
        <<CodeThorn::readableruntime(tc.normalizationRunTime)<<", "
        <<CodeThorn::readableruntime(tc.analysisRunTime)<<", "
        <<CodeThorn::readableruntime(verifyUpdateSequenceRaceConditionRunTime)<<", "
        <<CodeThorn::readableruntime(arrayUpdateExtractionRunTime)<<", "
        <<CodeThorn::readableruntime(arrayUpdateSsaNumberingRunTime)<<", "
        <<CodeThorn::readableruntime(sortingAndIORunTime)<<", "
        <<CodeThorn::readableruntime(totalRunTime)<<", "
        <<CodeThorn::readableruntime(tc.extractAssertionTracesTime)<<", "
        <<CodeThorn::readableruntime(tc.determinePrefixDepthTime)<<", "
        <<CodeThorn::readableruntime(tc.totalInputTracesTime)<<", "
        <<CodeThorn::readableruntime(infPathsOnlyTime)<<", "
        <<CodeThorn::readableruntime(stdIoOnlyTime)<<", "
        <<CodeThorn::readableruntime(spotLtlAnalysisTime)<<", "
        <<CodeThorn::readableruntime(totalLtlRunTime)<<", "
        <<CodeThorn::readableruntime(overallTime)<<endl;
    text<<"Runtime(ms),"
        <<tc.frontEndRunTime<<", "
        <<tc.initRunTime<<", "
        <<tc.normalizationRunTime<<", "
        <<tc.analysisRunTime<<", "
        <<verifyUpdateSequenceRaceConditionRunTime<<", "
        <<arrayUpdateExtractionRunTime<<", "
        <<arrayUpdateSsaNumberingRunTime<<", "
        <<sortingAndIORunTime<<", "
        <<totalRunTime<<", "
        <<tc.extractAssertionTracesTime<<", "
        <<tc.determinePrefixDepthTime<<", "
        <<tc.totalInputTracesTime<<", "
        <<infPathsOnlyTime<<", "
        <<stdIoOnlyTime<<", "
        <<spotLtlAnalysisTime<<", "
        <<totalLtlRunTime<<", "
        <<overallTime<<endl;
    text<<"hashset-collisions,"
        <<pstateSetMaxCollisions<<", "
        <<eStateSetMaxCollisions<<", "
        <<constraintSetsMaxCollisions<<endl;
    text<<"hashset-loadfactors,"
        <<pstateSetLoadFactor<<", "
        <<eStateSetLoadFactor<<", "
        <<constraintSetsLoadFactor<<endl;
    text<<"threads,"<<analyzer->getNumberOfThreadsToUse()<<endl;
    //    text<<"abstract-and-const-states,"
    //    <<"";

    // iterations (currently only supported for sequential analysis)
    text<<"iterations,";
    if(analyzer->getNumberOfThreadsToUse()==1 && analyzer->getSolver()->getId()==5 && analyzer->getExplorationMode()==EXPL_LOOP_AWARE)
      text<<analyzer->getIterations()<<","<<analyzer->getApproximatedIterations();
    else
      text<<"-1,-1";
    text<<endl;

    // -1: test not performed, 0 (no race conditions), >0: race conditions exist
    text<<"parallelism-stats,";
    if(verifyUpdateSequenceRaceConditionsResult==-1) {
      text<<"sequential";
    } else if(verifyUpdateSequenceRaceConditionsResult==0) {
      text<<"pass";
    } else {
      text<<"fail";
    }
    text<<","<<verifyUpdateSequenceRaceConditionsResult;
    text<<","<<verifyUpdateSequenceRaceConditionsParLoopNum;
    text<<","<<verifyUpdateSequenceRaceConditionsTotalLoopNum;
    text<<endl;

    text<<"infinite-paths-size,"<<pstateSetSizeInf<<", "
        <<eStateSetSizeInf<<", "
        <<transitionGraphSizeInf<<", "
        <<eStateSetSizeStgInf<<endl;
    //<<numOfconstraintSetsInf<<", "
    //<< numOfStdinEStatesInf<<", "
    //<< numOfStdoutEStatesInf<<", "
    //<< numOfStderrEStatesInf<<", "
    //<< numOfFailedAssertEStatesInf<<", "
    //<< numOfConstEStatesInf<<endl;
    text<<"states & transitions after only-I/O-reduction,"
        <<eStateSetSizeIoOnly<<", "
        <<transitionGraphSizeIoOnly<<endl;
    //  text<<"input length coverage"
    // <<inputSeqLengthCovered<<endl; // MS 2020: this value is not computed

    write_file(filename,text.str());
    cout << "generated "<<filename<<endl;
  }

  if(ltlOpt.ltlStatisticsCSVFileName.size()>0) {
    // content of a line in the .csv file:
    // <#transitions>,<#states>,<#input_states>,<#output_states>,<#error_states>,<#verified_LTL>,<#falsified_LTL>,<#unknown_LTL>
    string filename = ltlOpt.ltlStatisticsCSVFileName;
    write_file(filename,statisticsSizeAndLtl.str());
    cout << "generated "<<filename<<endl;
  }

  if (ltlOpt.cegpra.csvStatsFileName.size()>0) {
    // content of a line in the .csv file:
    // <analyzed_property>,<#transitions>,<#states>,<#input_states>,<#output_states>,<#error_states>,
    // <#analyzed_counterexamples>,<analysis_result(y/n/?)>,<#verified_LTL>,<#falsified_LTL>,<#unknown_LTL>
    string filename = ltlOpt.cegpra.csvStatsFileName;
    write_file(filename,statisticsCegpra.str());
    cout << "generated "<<filename<<endl;
  }
}
void optionallyInitializePatternSearchSolver(CodeThornOptions& ctOpt,IOAnalyzer* analyzer,TimingCollector& timingCollector) {
    // pattern search: requires that exploration mode is set,
    // otherwise no pattern search is performed
    if(ctOpt.patSearch.explorationMode.size()>0) {
      logger[INFO] << "Pattern search exploration mode was set. Choosing solver 10." << endl;
      analyzer->setSolver(new Solver10());
      analyzer->setStartPState(*analyzer->popWorkList()->pstate());
    }
}

int main( int argc, char * argv[] ) {
  try {
    ROSE_INITIALIZE;
    CodeThorn::configureRose();
    configureRersSpecialization();
    CodeThorn::initDiagnosticsLTL();

    TimingCollector tc;
    TimeMeasurement timer;

    tc.startTimer();
    CodeThornOptions ctOpt;
    LTLOptions ltlOpt; // to be moved into separate tool
    ParProOptions parProOpt; // options only available in parprothorn
    parseCommandLine(argc, argv, logger,versionString,ctOpt,ltlOpt,parProOpt);
    mfacilities.control(ctOpt.logLevel); SAWYER_MESG(logger[TRACE]) << "Log level is " << ctOpt.logLevel << endl;
    IOAnalyzer* analyzer=createAnalyzer(ctOpt,ltlOpt);
    optionallyRunInternalChecks(ctOpt,argc,argv);
    optionallyRunExprEvalTestAndExit(ctOpt,argc,argv);
    analyzer->configureOptions(ctOpt,ltlOpt,parProOpt);
    analyzer->setSolver(createSolver(ctOpt));
    analyzer->setOptionContextSensitiveAnalysis(ctOpt.contextSensitive);
    tc.stopTimer();

    SgProject* sageProject=runRoseFrontEnd(argc,argv,ctOpt,tc);
    if(ctOpt.status) cout << "STATUS: Parsing and creating AST finished."<<endl;
    optionallyRunNormalization(ctOpt,sageProject,tc);
    optionallyGenerateExternalFunctionsFile(ctOpt, sageProject);
    optionallyGenerateAstStatistics(ctOpt, sageProject);
    optionallyGenerateTraversalInfoAndExit(ctOpt, sageProject);
    optionallyGenerateSourceProgramAndExit(ctOpt, sageProject);
    if(ctOpt.status) cout<<"STATUS: analysis started."<<endl;
    analyzer->initializeVariableIdMapping(sageProject);
    logger[INFO]<<"registered string literals: "<<analyzer->getVariableIdMapping()->numberOfRegisteredStringLiterals()<<endl;
    optionallyPrintProgramInfos(ctOpt, analyzer);
    optionallyRunRoseAstChecksAndExit(ctOpt, sageProject);
    SgNode* root=sageProject;ROSE_ASSERT(root);
    setAssertConditionVariablesInAnalyzer(root,analyzer);
    optionallyEliminateCompoundStatements(ctOpt, analyzer, root);
    optionallyEliminateRersArraysAndExit(ctOpt,sageProject,analyzer);
    initializeSolverWithStartFunction(ctOpt,analyzer,root,tc);
    analyzer->initLabeledAssertNodes(sageProject);
    optionallyPrintFunctionIdMapping(ctOpt,analyzer);
    optionallyInitializePatternSearchSolver(ctOpt,analyzer,tc);
    runSolver(ctOpt,analyzer,sageProject,tc);
    analyzer->printStatusMessageLine("==============================================================");
    optionallyWriteSVCompWitnessFile(ctOpt, analyzer);
    optionallyAnalyzeAssertions(ctOpt, ltlOpt, analyzer, tc);
    optionallyRunZ3AndExit(ctOpt,analyzer);
    optionallyGenerateVerificationReports(ctOpt,analyzer);
    runLTLAnalysis(ctOpt,ltlOpt,analyzer,tc);
    processCtOptGenerateAssertions(ctOpt, analyzer, sageProject);
    optionallyRunVisualizer(ctOpt,analyzer,root);
    optionallyRunIOSequenceGenerator(ctOpt, analyzer);
    optionallyAnnotateTermsAndUnparse(ctOpt, sageProject, analyzer);
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

