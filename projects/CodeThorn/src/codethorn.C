/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
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
#include "InternalChecks.h"
#include "AstAnnotator.h"
#include "AstTerm.h"
#include "AbstractValue.h"
#include "AstMatching.h"
#include "RewriteSystem.h"
#include "SpotConnection.h"
#include "CounterexampleAnalyzer.h"
#include "AnalysisAbstractionLayer.h"
#include "ArrayElementAccessData.h"
#include "PragmaHandler.h"
#include "Miscellaneous2.h"
#include "FIConstAnalysis.h"
#include "ReachabilityAnalysis.h"
#include "EquivalenceChecking.h"
#include "Solver5.h"
#include "Solver8.h"
#include "Solver10.h"
#include "Solver11.h"
#include "Solver12.h"
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

// Z3-based analyser / SSA 
#include "SSAGenerator.h"
#include "ReachabilityAnalyzerZ3.h"

// ParProAutomata
#include "ParProAutomata.h"

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

const std::string versionString="1.11.7";

// handler for generating backtrace
void handler(int sig) {
  void *array[10];
  size_t size;

  size = backtrace (array, 10);
  printf ("Obtained %zd stack frames.\n", size);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

bool isExprRoot(SgNode* node) {
  if(SgExpression* exp=isSgExpression(node)) {
    return isSgStatement(exp->get_parent());
  }
  return false;
}

list<SgExpression*> exprRootList(SgNode *node) {
  RoseAst ast(node);
  list<SgExpression*> exprList;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(isExprRoot(*i)) {
      SgExpression* expr=isSgExpression(*i);
      ROSE_ASSERT(expr);
      exprList.push_back(expr);
      i.skipChildrenOnForward();
    }
  }
  return exprList;
}

static IOAnalyzer* global_analyzer=0;

set<AbstractValue> determineSetOfCompoundIncVars(VariableIdMapping* vim, SgNode* root) {
  ROSE_ASSERT(vim);
  ROSE_ASSERT(root);
  RoseAst ast(root) ;
  set<AbstractValue> compoundIncVarsSet;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgCompoundAssignOp* compoundAssignOp=isSgCompoundAssignOp(*i)) {
      SgVarRefExp* lhsVar=isSgVarRefExp(SgNodeHelper::getLhs(compoundAssignOp));
      if(lhsVar) {
        compoundIncVarsSet.insert(vim->variableId(lhsVar));
      }
    }
  }
  return compoundIncVarsSet;
}

set<VariableId> determineSetOfConstAssignVars2(VariableIdMapping* vim, SgNode* root) {
  ROSE_ASSERT(vim);
  ROSE_ASSERT(root);
  RoseAst ast(root) ;
  set<VariableId> constAssignVars;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgAssignOp* assignOp=isSgAssignOp(*i)) {
      SgVarRefExp* lhsVar=isSgVarRefExp(SgNodeHelper::getLhs(assignOp));
      SgIntVal* rhsIntVal=isSgIntVal(SgNodeHelper::getRhs(assignOp));
      if(lhsVar && rhsIntVal) {
        constAssignVars.insert(vim->variableId(lhsVar));
      }
    }
  }
  return constAssignVars;
}

AbstractValueSet determineVarsInAssertConditions(SgNode* node, VariableIdMapping* variableIdMapping) {
  AbstractValueSet usedVarsInAssertConditions;
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgIfStmt* ifstmt=isSgIfStmt(*i)) {
      SgNode* cond=SgNodeHelper::getCond(ifstmt);
      if(cond) {
        int errorLabelCode=-1;
        errorLabelCode=ReachabilityAnalysis::isConditionOfIfWithLabeledAssert(cond);
        if(errorLabelCode>=0) {
          //cout<<"Assertion cond: "<<cond->unparseToString()<<endl;
          //cout<<"Stmt: "<<ifstmt->unparseToString()<<endl;
          std::vector<SgVarRefExp*> vars=SgNodeHelper::determineVariablesInSubtree(cond);
          //cout<<"Num of vars: "<<vars.size()<<endl;
          for(std::vector<SgVarRefExp*>::iterator j=vars.begin();j!=vars.end();++j) {
            VariableId varId=variableIdMapping->variableId(*j);
            usedVarsInAssertConditions.insert(AbstractValue(varId));
          }
        }
      }
    }
  }
  return usedVarsInAssertConditions;
}

void analyzerSetup(IOAnalyzer* analyzer, Sawyer::Message::Facility logger) {
  analyzer->setOptionOutputWarnings(args.getBool("print-warnings"));
  analyzer->setPrintDetectedViolations(args.getBool("print-violations"));

  if(args.getBool("explicit-arrays")==false) {
    analyzer->setSkipArrayAccesses(true);
  }
  
  // this must be set early, as subsequent initialization depends on this flag
  if (args.getBool("ltl-driven")) {
    analyzer->setModeLTLDriven(true);
  }

  if (args.isDefined("cegpra-ltl") || args.getBool("cegpra-ltl-all")) {
    analyzer->setMaxTransitionsForcedTop(1); //initial over-approximated model
    args.setOption("no-input-input",true);
    args.setOption("with-ltl-counterexamples",true);
    args.setOption("counterexamples-with-output",true);
    cout << "STATUS: CEGPRA activated (with it LTL counterexamples that include output states)." << endl;
    cout << "STATUS: CEGPRA mode: will remove input state --> input state transitions in the approximated STG." << endl;
  }

  if (args.getBool("counterexamples-with-output")) {
    args.setOption("with-ltl-counterexamples",true);
  }

  if(args.isDefined("stg-trace-file")) {
    analyzer->setStgTraceFileName(args.getString("stg-trace-file"));
  }

  if (args.isDefined("cl-args")) {
    string clOptions=args.getString("cl-args");
    vector<string> clOptionsVector=Parse::commandLineArgs(clOptions);
    analyzer->setCommandLineOptions(clOptionsVector);
  }

  if(args.isDefined("input-values")) {
    string setstring=args.getString("input-values");
    cout << "STATUS: input-values="<<setstring<<endl;

    set<int> intSet=Parse::integerSet(setstring);
    for(set<int>::iterator i=intSet.begin();i!=intSet.end();++i) {
      analyzer->insertInputVarValue(*i);
    }
  }

  if(args.isDefined("input-sequence")) {
    string liststring=args.getString("input-sequence");
    cout << "STATUS: input-sequence="<<liststring<<endl;

    list<int> intList=Parse::integerList(liststring);
    for(list<int>::iterator i=intList.begin();i!=intList.end();++i) {
      analyzer->addInputSequenceValue(*i);
    }
  }

  if(args.isDefined("exploration-mode")) {
    string explorationMode=args.getString("exploration-mode");
    if(explorationMode=="depth-first") {
      analyzer->setExplorationMode(EXPL_DEPTH_FIRST);
    } else if(explorationMode=="breadth-first") {
      analyzer->setExplorationMode(EXPL_BREADTH_FIRST);
    } else if(explorationMode=="loop-aware") {
      analyzer->setExplorationMode(EXPL_LOOP_AWARE);
    } else if(explorationMode=="loop-aware-sync") {
      analyzer->setExplorationMode(EXPL_LOOP_AWARE_SYNC);
    } else if(explorationMode=="random-mode1") {
      analyzer->setExplorationMode(EXPL_RANDOM_MODE1);
    } else {
      logger[ERROR] <<"unknown state space exploration mode specified with option --exploration-mode."<<endl;
      exit(1);
    }
  } else {
    // default value
    analyzer->setExplorationMode(EXPL_BREADTH_FIRST);
  }

  if (args.isDefined("max-iterations") || args.isDefined("max-iterations-forced-top")) {
    bool notSupported=false;
    if (!args.isDefined("exploration-mode")) {
      notSupported=true;
    } else {
      string explorationMode=args.getString("exploration-mode");
      if(explorationMode!="loop-aware" && explorationMode!="loop-aware-sync") {
        notSupported=true;
      }
    }
    if(notSupported) {
      cout << "Error: \"max-iterations[-forced-top]\" modes currently require \"--exploration-mode=loop-aware[-sync]\"." << endl;
      exit(1);
    }
  }

  if(args.isDefined("abstraction-mode")) {
    analyzer->setAbstractionMode(args.getInt("abstraction-mode"));
  }

  if(args.isDefined("max-transitions")) {
    analyzer->setMaxTransitions(args.getInt("max-transitions"));
  }

  if(args.isDefined("max-iterations")) {
    analyzer->setMaxIterations(args.getInt("max-iterations"));
  }

  if(args.isDefined("max-iterations-forced-top")) {
    analyzer->setMaxIterationsForcedTop(args.getInt("max-iterations-forced-top"));
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  }

  if(args.isDefined("max-transitions-forced-top")) {
    analyzer->setMaxTransitionsForcedTop(args.getInt("max-transitions-forced-top"));
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  } else if(args.isDefined("max-transitions-forced-top1")) {
    analyzer->setMaxTransitionsForcedTop(args.getInt("max-transitions-forced-top1"));
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  } else if(args.isDefined("max-transitions-forced-top2")) {
    analyzer->setMaxTransitionsForcedTop(args.getInt("max-transitions-forced-top2"));
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IOCF);
  } else if(args.isDefined("max-transitions-forced-top3")) {
    analyzer->setMaxTransitionsForcedTop(args.getInt("max-transitions-forced-top3"));
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IOCFPTR);
  } else if(args.isDefined("max-transitions-forced-top4")) {
    analyzer->setMaxTransitionsForcedTop(args.getInt("max-transitions-forced-top4"));
    analyzer->setGlobalTopifyMode(Analyzer::GTM_COMPOUNDASSIGN);
  } else if(args.isDefined("max-transitions-forced-top5")) {
    analyzer->setMaxTransitionsForcedTop(args.getInt("max-transitions-forced-top5"));
    analyzer->setGlobalTopifyMode(Analyzer::GTM_FLAGS);
  }

  if (args.isDefined("max-memory")) {
    analyzer->setMaxBytes(args.getLongInt("max-memory"));
  }
  if (args.isDefined("max-time")) {
    analyzer->setMaxSeconds(args.getLongInt("max-time"));
  }
  if (args.isDefined("max-memory-forced-top")) {
    analyzer->setMaxBytesForcedTop(args.getLongInt("max-memory-forced-top"));
  }
  if (args.isDefined("max-time-forced-top")) {
    analyzer->setMaxSecondsForcedTop(args.getLongInt("max-time-forced-top"));
  }

  if(args.isDefined("display-diff")) {
    int displayDiff=args.getInt("display-diff");
    analyzer->setDisplayDiff(displayDiff);
  }
  if(args.isDefined("resource-limit-diff")) {
    int resourceLimitDiff=args.getInt("resource-limit-diff");
    analyzer->setResourceLimitDiff(resourceLimitDiff);
  }

  Solver* solver = nullptr;
  // overwrite solver ID based on other options
  if(analyzer->getModeLTLDriven()) {
    args.setOption("solver", 11);
  }
  ROSE_ASSERT(args.isDefined("solver")); // Options should contain a default solver
  int solverId=args.getInt("solver");
  // solverId sanity checks
  if(analyzer->getExplorationMode() == EXPL_LOOP_AWARE_SYNC &&
     solverId != 12) {
    logger[ERROR] <<"Exploration mode loop-aware-sync requires solver 12, but solver "<<solverId<<" was selected."<<endl;
    exit(1);
  }
  if(analyzer->getModeLTLDriven() &&
     solverId != 11) {
    logger[ERROR] <<"Ltl-driven mode requires solver 11, but solver "<<solverId<<" was selected."<<endl;
    exit(1);
  }
  // solver "factory"
  switch(solverId) {
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
    logger[ERROR] <<"Unknown solver ID: "<<solverId<<endl;
    exit(1);
  }
  }
  analyzer->setSolver(solver);
}

int main( int argc, char * argv[] ) {
  try {
    ROSE_INITIALIZE;
    CodeThorn::initDiagnostics();
    CodeThorn::initDiagnosticsLTL();

    Rose::Diagnostics::mprefix->showProgramName(false);
    Rose::Diagnostics::mprefix->showThreadId(false);
    Rose::Diagnostics::mprefix->showElapsedTime(false);

    string turnOffRoseWarnings=string("Rose(none,>=error),Rose::EditDistance(none,>=error),Rose::FixupAstDeclarationScope(none,>=error),")
      +"Rose::FixupAstSymbolTablesToSupportAliasedSymbols(none,>=error),"
      +"Rose::EditDistance(none,>=error),"
      +"Rose::TestChildPointersInMemoryPool(none,>=error),Rose::UnparseLanguageIndependentConstructs(none,>=error),"
      +"rose_ir_node(none,>=error)";
    // result string must be checked
    string result=Rose::Diagnostics::mfacilities.control(turnOffRoseWarnings); 
    if(result!="") {
      cerr<<result<<endl;
      cerr<<"Error in logger initialization."<<endl;
      exit(1);
    }

    Rose::global_options.set_frontend_notes(false);
    Rose::global_options.set_frontend_warnings(false);
    Rose::global_options.set_backend_warnings(false);

    signal(SIGSEGV, handler);   // install handler for backtrace

    Sawyer::Message::Facility logger;
    Rose::Diagnostics::initAndRegister(&logger, "CodeThorn");

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

    TimeMeasurement timer;
    timer.start();

    parseCommandLine(argc, argv, logger,versionString);

    // Check if chosen options are available
#ifndef HAVE_SPOT
    // display error message and exit in case SPOT is not avaiable, but related options are selected
    if (args.isDefined("csv-stats-cegpra") ||
	args.isDefined("cegpra-ltl") ||
	args.getBool("cegpra-ltl-all") ||
	args.isDefined("cegpra-max-iterations") ||
	args.isDefined("viz-cegpra-detailed") ||
	args.isDefined("csv-spot-ltl") ||
	args.isDefined("check-ltl") ||
	args.isDefined("single-property") ||
	args.isDefined("ltl-in-alphabet") ||
	args.isDefined("ltl-out-alphabet") ||
	args.getBool("ltl-driven") ||
	args.getBool("with-ltl-counterexamples") ||
	args.isDefined("mine-num-verifiable") ||
	args.isDefined("mine-num-falsifiable") ||
	args.isDefined("ltl-mode") ||
	args.isDefined("ltl-properties-output") ||
	args.isDefined("promela-output") ||
	args.getBool("promela-output-only") ||
	args.getBool("output-with-results") ||
	args.getBool("output-with-annotations")) {
      cerr << "Error: Options selected that require the SPOT library, however SPOT was not selected during configuration." << endl;
      exit(1);
    }
#endif

#ifndef HAVE_Z3
    if (args.isDefined("z3") ||
	args.isDefined("rers-upper-input-bound") ||
	args.isDefined("rers-verifier-error-number")){
      cerr << "Error: Options selected that require the Z3 library, however Z3 was not selected during configuration." << endl;
      exit(1);
    }
#endif	

    // Start execution
    mfacilities.control(args.getString("log-level"));
    SAWYER_MESG(logger[TRACE]) << "Log level is " << args.getString("log-level") << endl;

    // ParPro command line options
    if(CodeThorn::ParProAutomata::handleCommandLineArguments(args,logger)) {
      exit(0);
    }

    IOAnalyzer* analyzer;
    if(args.getBool("data-race-check-shuffle")) {
      analyzer = new ReadWriteAnalyzer();
    } else {
      analyzer = new IOAnalyzer();
    }
    global_analyzer=analyzer;

#if 0
    string option_pragma_name;
    if (args.isDefined("limit-to-fragment")) {
      option_pragma_name = args.getString("limit-to-fragment");
    }
#endif

    if (args.isDefined("internal-checks")) {
      mfacilities.shutdown();
      if(CodeThorn::internalChecks(argc,argv)==false)
        return 1;
      else
        return 0;
    }

    string optionName="options-set";
    int optionValue=args.getInt(optionName);
    switch(optionValue) {
    case 0:
      // fall-through for default
      break;
    case 1:
      args.setOption("explicit-arrays",true);
      args.setOption("in-state-string-literals",true);
      args.setOption("ignore-unknown-functions",true);
      args.setOption("ignore-function-pointers",true);
      args.setOption("std-functions",true);
      args.setOption("context-sensitive",true);
      args.setOption("normalize-all",true);
      args.setOption("abstraction-mode",1);
      break;
    case 2:
      args.setOption("explicit-arrays",true);
      args.setOption("in-state-string-literals",true);
      args.setOption("ignore-unknown-functions",true);
      args.setOption("ignore-function-pointers",false);
      args.setOption("std-functions",true);
      args.setOption("context-sensitive",true);
      args.setOption("normalize-all",true);
      args.setOption("abstraction-mode",1);
      break;
    case 3:
      args.setOption("explicit-arrays",true);
      args.setOption("in-state-string-literals",true);
      args.setOption("ignore-unknown-functions",true);
      args.setOption("ignore-function-pointers",false);
      args.setOption("std-functions",true);
      args.setOption("context-sensitive",true);
      args.setOption("normalize-all",true);
      args.setOption("abstraction-mode",0);
      break;
    default:
      cerr<<"Error: unsupported "<<optionName<<" value: "<<optionValue<<endl;
      exit(1);
    }

    analyzer->optionStringLiteralsInState=args.getBool("in-state-string-literals");
    analyzer->setSkipUnknownFunctionCalls(args.getBool("ignore-unknown-functions"));
    analyzer->setIgnoreFunctionPointers(args.getBool("ignore-function-pointers"));
    analyzer->setStdFunctionSemantics(args.getBool("std-functions"));

    analyzerSetup(analyzer, logger);

    switch(int mode=args.getInt("interpreter-mode")) {
    case 0: analyzer->setInterpreterMode(IM_ABSTRACT); break;
    case 1: analyzer->setInterpreterMode(IM_CONCRETE); break;
    default:
      cerr<<"Unknown interpreter mode "<<mode<<" provided on command line (supported: 0..1)."<<endl;
      exit(1);
    }
    string outFileName=args.getString("interpreter-mode-file");
    if(outFileName!="") {
      analyzer->setInterpreterModeOutputFileName(outFileName);
      CppStdUtilities::writeFile(outFileName,""); // touch file
    }
    {
      switch(int argVal=args.getInt("function-resolution-mode")) {
      case 1: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_TRANSLATION_UNIT;break;
      case 2: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_WHOLE_AST_LOOKUP;break;
      case 3: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_FUNCTION_ID_MAPPING;break;
      case 4: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_FUNCTION_CALL_MAPPING;break;
      default: 
        cerr<<"Error: unsupported argument value of "<<argVal<<" for function-resolution-mode.";
        exit(1);
      }
    }
    // analyzer->setFunctionResolutionMode(args.getInt("function-resolution-mode")); xxx
    // needs to set CFAnalysis functionResolutionMode

    if(args.isDefined("threads")) {
      int numThreads=args.getInt("threads");
      if(numThreads<=0) {
        cerr<<"Error: number of threads must be greater or equal 1."<<endl;
        exit(1);
      }
      analyzer->setNumberOfThreadsToUse(numThreads);
    } else {
      analyzer->setNumberOfThreadsToUse(1);
    }

    string option_start_function="main";
    if(args.isDefined("start-function")) {
      option_start_function = args.getString("start-function");
    }

    string option_specialize_fun_name="";
    vector<int> option_specialize_fun_param_list;
    vector<int> option_specialize_fun_const_list;
    vector<string> option_specialize_fun_varinit_list;
    vector<int> option_specialize_fun_varinit_const_list;
    if(args.isDefined("specialize-fun-name")) {
      option_specialize_fun_name = args.getString("specialize-fun-name");
      // logger[DEBUG] << "option_specialize_fun_name: "<< option_specialize_fun_name<<endl;
    } else {
      // logger[DEBUG] << "option_specialize_fun_name: NONE"<< option_specialize_fun_name<<endl;
    }

    if(args.isDefined("specialize-fun-param")) {
      option_specialize_fun_param_list=args.getIntVector("specialize-fun-param");
      option_specialize_fun_const_list=args.getIntVector("specialize-fun-const");
    }

    if(args.isDefined("specialize-fun-varinit")) {
      option_specialize_fun_varinit_list=args.getStringVector("specialize-fun-varinit");
      option_specialize_fun_varinit_const_list=args.getIntVector("specialize-fun-varinit-const");
    }

    // logger[DEBUG] << "specialize-params:"<<option_specialize_fun_const_list.size()<<endl;

    if(args.isDefined("specialize-fun-name")) {
      if((args.isDefined("specialize-fun-param")||args.isDefined("specialize-fun-const"))
          && !(args.isDefined("specialize-fun-name")&&args.isDefined("specialize-fun-param")&&args.isDefined("specialize-fun-param"))) {
        logger[ERROR] <<"options --specialize-fun-name=NAME --specialize-fun-param=NUM --specialize-fun-const=NUM must be used together."<<endl;
        exit(1);
      }
      if((args.isDefined("specialize-fun-varinit")||args.isDefined("specialize-fun-varinit-const"))
          && !(args.isDefined("specialize-fun-varinit")&&args.isDefined("specialize-fun-varinit-const"))) {
        logger[ERROR] <<"options --specialize-fun-name=NAME --specialize-fun-varinit=NAME --specialize-fun-const=NUM must be used together."<<endl;
        exit(1);
      }
    }

    if((args.getBool("print-update-infos")||args.isDefined("equivalence-check"))&&(args.isDefined("dump-sorted")==0 && args.isDefined("dump-non-sorted")==0)) {
      logger[ERROR] <<"option print-update-infos/equivalence-check must be used together with option --dump-non-sorted or --dump-sorted."<<endl;
      exit(1);
    }
    RewriteSystem rewriteSystem;
    if(args.getBool("print-rewrite-trace")) {
      rewriteSystem.setTrace(true);
    }
    if(args.getBool("ignore-undefined-dereference")) {
      analyzer->setIgnoreUndefinedDereference(true);
    }
    if(args.isDefined("dump-sorted")>0 || args.isDefined("dump-non-sorted")>0 || args.isDefined("equivalence-check")>0) {
      analyzer->setSkipUnknownFunctionCalls(true);
      analyzer->setSkipArrayAccesses(true);
      args.setOption("explicit-arrays",false);
      if(analyzer->getNumberOfThreadsToUse()>1) {
        logger[ERROR] << "multi threaded rewrite not supported yet."<<endl;
        exit(1);
      }
    }

    // parse command line options for data race detection
    DataRaceDetection dataRaceDetection;
    dataRaceDetection.handleCommandLineOptions(*analyzer);
    dataRaceDetection.setVisualizeReadWriteAccesses(args.getBool("visualize-read-write-sets"));

    // handle RERS mode: reconfigure options
    if(args.getBool("rersmode")) {
      SAWYER_MESG(logger[TRACE]) <<"RERS MODE activated [stderr output is treated like a failed assert]"<<endl;
      args.setOption("stderr-like-failed-assert",true);
    }

    if(args.getBool("svcomp-mode")) {
      analyzer->enableSVCompFunctionSemantics();
      string errorFunctionName="__VERIFIER_error";
      analyzer->setExternalErrorFunctionName(errorFunctionName);
    }

    if(args.isDefined("external-function-semantics")) {
      // obsolete
    }

    if(args.isDefined("error-function")) {
      string errorFunctionName=args.getString("error-function");
      analyzer->setExternalErrorFunctionName(errorFunctionName);
    }

    analyzer->setTreatStdErrLikeFailedAssert(args.getBool("stderr-like-failed-assert"));

    // Build the AST used by ROSE
    if(!args.isDefined("quiet")) {
      cout<< "STATUS: Parsing and creating AST started."<<endl;
    }

    timer.stop();
    timer.start();
    SgProject* sageProject = 0;
    vector<string> argvList(argv,argv+argc);
    //string turnOffRoseLoggerWarnings="-rose:log none";
    //    argvList.push_back(turnOffRoseLoggerWarnings);
    if(args.getBool("omp-ast")||args.getBool("data-race")) {
      SAWYER_MESG(logger[TRACE])<<"selected OpenMP AST."<<endl;
      argvList.push_back("-rose:OpenMP:ast_only");
    }
    sageProject=frontend(argvList);

    if(!args.isDefined("quiet")) {
      cout << "STATUS: Parsing and creating AST finished."<<endl;
    }
    double frontEndRunTime=timer.getTimeDurationAndStop().milliSeconds();

    /* perform inlining before variable ids are computed, because
       variables are duplicated by inlining. */
    Normalization lowering;
    if(args.getBool("normalize-fcalls")) {
      lowering.normalizeAst(sageProject,1);
      SAWYER_MESG(logger[TRACE])<<"STATUS: normalized expressions with fcalls (if not a condition)"<<endl;
    }

    if(args.getBool("normalize-all")||args.getInt("options-set")==1) {
      if(!args.isDefined("quiet")) {
        cout<<"STATUS: normalizing program."<<endl;
      }
      //SAWYER_MESG(logger[INFO])<<"STATUS: normalizing program."<<endl;
      lowering.normalizeAst(sageProject,2);
    }

    /* Context sensitive analysis using call strings.
     */
    {
      analyzer->setOptionContextSensitiveAnalysis(args.getBool("context-sensitive"));
      //Call strings length abrivation is not supported yet.
      //CodeThorn::CallString::setMaxLength((args.getInt("callstring-length")));
    }

    /* perform inlining before variable ids are computed, because
     * variables are duplicated by inlining. */
    if(args.getBool("inline")) {
      InlinerBase* inliner=lowering.getInliner();
      if(RoseInliner* roseInliner=dynamic_cast<CodeThorn::RoseInliner*>(inliner)) {
        roseInliner->inlineDepth=args.getInt("inlinedepth");
      }
      inliner->inlineFunctions(sageProject);
      size_t numInlined=inliner->getNumInlinedFunctions();
      SAWYER_MESG(logger[TRACE])<<"inlined "<<numInlined<<" functions"<<endl;
    }

    {
      bool unknownFunctionsFile=args.isUserProvided("unknown-functions-file");
      bool showProgramStats=args.getBool("program-stats");
      bool showProgramStatsOnly=args.getBool("program-stats-only");
      if(unknownFunctionsFile||showProgramStats||showProgramStatsOnly) {
        ProgramInfo programInfo(sageProject);
        programInfo.compute();
        if(unknownFunctionsFile) {
          ROSE_ASSERT(analyzer);
          string unknownFunctionsFileName=args.getString("unknown-functions-file");
          programInfo.writeFunctionCallNodesToFile(unknownFunctionsFileName);
        }
        if(showProgramStats||showProgramStatsOnly) {
          programInfo.printDetailed();
        }
        if(showProgramStatsOnly) {
          exit(0);
        }
      }
    }

    if(args.getBool("unparse")) {
      sageProject->unparse(0,0);
      return 0;
    }

    if(args.isUserProvided("ast-stats-print")||args.isUserProvided("ast-stats-csv")) {
      // from: src/midend/astDiagnostics/AstStatistics.C
      if(args.getBool("ast-stats-print")) {
        ROSE_Statistics::AstNodeTraversalStatistics astStats;
        string s=astStats.toString(sageProject);
        cout<<s; // output includes newline at the end
      }
      if(args.isUserProvided("ast-stats-csv")) {
        ROSE_Statistics::AstNodeTraversalCSVStatistics astCSVStats;
        string fileName=args.getString("ast-stats-csv");
        astCSVStats.setMinCountToShow(1); // default value is 1
        if(!CppStdUtilities::writeFile(fileName, astCSVStats.toString(sageProject))) {
          cerr<<"Error: cannot write AST node statistics to CSV file "<<fileName<<endl;
          exit(1);
        }
      }
    }

    if(!args.isDefined("quiet")) {
      cout<<"STATUS: analysis started."<<endl;
    }
    // TODO: introduce ProgramAbstractionLayer
    analyzer->initializeVariableIdMapping(sageProject);
    logger[INFO]<<"registered string literals: "<<analyzer->getVariableIdMapping()->numberOfRegisteredStringLiterals()<<endl;

    if(args.getBool("print-variable-id-mapping")) {
      analyzer->getVariableIdMapping()->toStream(cout);
    }
  
    if(args.isUserProvided("type-size-mapping-print")||args.isUserProvided("type-size-mapping-csv")) {
      // from: src/midend/astDiagnostics/AstStatistics.C
      string s=analyzer->typeSizeMappingToString();
      if(args.getBool("type-size-mapping-print")) {
        cout<<"Type size mapping:"<<endl;
        cout<<s; // output includes newline at the end
      }
      if(args.isUserProvided("type-size-mapping-csv")) {
        string fileName=args.getString("type-size-mapping-csv");
        if(!CppStdUtilities::writeFile(fileName, s)) {
          cerr<<"Error: cannot write type-size mapping to CSV file "<<fileName<<endl;
          exit(1);
        }
      }
    }
    
    if(args.isDefined("run-rose-tests")) {
      cout << "ROSE tests started."<<endl;
      // Run internal consistency tests on AST
      AstTests::runAllTests(sageProject);

      // test: constant expressions
      {
        SAWYER_MESG(logger[TRACE]) <<"STATUS: testing constant expressions."<<endl;
        CppConstExprEvaluator* evaluator=new CppConstExprEvaluator();
        list<SgExpression*> exprList=exprRootList(sageProject);
        logger[INFO] <<"found "<<exprList.size()<<" expressions."<<endl;
        for(list<SgExpression*>::iterator i=exprList.begin();i!=exprList.end();++i) {
          EvalResult r=evaluator->traverse(*i);
          if(r.isConst()) {
            SAWYER_MESG(logger[TRACE])<<"Found constant expression: "<<(*i)->unparseToString()<<" eq "<<r.constValue()<<endl;
          }
        }
        delete evaluator;
      }
      cout << "ROSE tests finished."<<endl; 
      mfacilities.shutdown();
      return 0;
    }

    // TODO: exit here if no analysis option is selected
    // exit(0);

    SgNode* root=sageProject;
    ROSE_ASSERT(root);

    // only handle pragmas if fun_name is not set on the command line
    if(option_specialize_fun_name=="") {
      SAWYER_MESG(logger[TRACE])<<"STATUS: handling pragmas started."<<endl;
      PragmaHandler pragmaHandler;
      pragmaHandler.handlePragmas(sageProject,analyzer);
      // TODO: requires more refactoring
      option_specialize_fun_name=pragmaHandler.option_specialize_fun_name;
      // unparse specialized code
      //sageProject->unparse(0,0);
      SAWYER_MESG(logger[TRACE])<<"STATUS: handling pragmas finished."<<endl;
    } else {
      // do specialization and setup data structures
      analyzer->setSkipUnknownFunctionCalls(true);
      analyzer->setSkipArrayAccesses(true);
      args.setOption("explicit-arrays",false);

      //TODO1: refactor into separate function
      int numSubst=0;
      if(option_specialize_fun_name!="") {
        Specialization speci;
        SAWYER_MESG(logger[TRACE])<<"STATUS: specializing function: "<<option_specialize_fun_name<<endl;

        string funNameToFind=option_specialize_fun_name;

        for(size_t i=0;i<option_specialize_fun_param_list.size();i++) {
          int param=option_specialize_fun_param_list[i];
          int constInt=option_specialize_fun_const_list[i];
          numSubst+=speci.specializeFunction(sageProject,funNameToFind, param, constInt, analyzer->getVariableIdMapping());
        }
        SAWYER_MESG(logger[TRACE])<<"STATUS: specialization: number of variable-uses replaced with constant: "<<numSubst<<endl;
        int numInit=0;
        //logger[DEBUG]<<"var init spec: "<<endl;
        for(size_t i=0;i<option_specialize_fun_varinit_list.size();i++) {
          string varInit=option_specialize_fun_varinit_list[i];
          int varInitConstInt=option_specialize_fun_varinit_const_list[i];
          //logger[DEBUG]<<"checking for varInitName nr "<<i<<" var:"<<varInit<<" Const:"<<varInitConstInt<<endl;
          numInit+=speci.specializeFunction(sageProject,funNameToFind, -1, 0, varInit, varInitConstInt,analyzer->getVariableIdMapping());
        }
        SAWYER_MESG(logger[TRACE])<<"STATUS: specialization: number of variable-inits replaced with constant: "<<numInit<<endl;
      }
    }

    if(args.isDefined("rewrite")) {
      SAWYER_MESG(logger[TRACE])<<"STATUS: rewrite started."<<endl;
      rewriteSystem.resetStatistics();
      rewriteSystem.setRewriteCondStmt(false); // experimental: supposed to normalize conditions
      rewriteSystem.rewriteAst(root,analyzer->getVariableIdMapping(), false, true/*eliminate compound assignments*/);
      // TODO: Outputs statistics
      cout <<"Rewrite statistics:"<<endl<<rewriteSystem.getStatistics().toString()<<endl;
      sageProject->unparse(0,0);
      SAWYER_MESG(logger[TRACE])<<"STATUS: generated rewritten program."<<endl;
      exit(0);
    }

    {
      // TODO: refactor this into class Analyzer after normalization has been moved to class Analyzer->
      set<AbstractValue> compoundIncVarsSet=determineSetOfCompoundIncVars(analyzer->getVariableIdMapping(),root);
      analyzer->setCompoundIncVarsSet(compoundIncVarsSet);
      SAWYER_MESG(logger[TRACE])<<"STATUS: determined "<<compoundIncVarsSet.size()<<" compound inc/dec variables before normalization."<<endl;
    }
    {
      AbstractValueSet varsInAssertConditions=determineVarsInAssertConditions(root,analyzer->getVariableIdMapping());
      SAWYER_MESG(logger[TRACE])<<"STATUS: determined "<<varsInAssertConditions.size()<< " variables in (guarding) assert conditions."<<endl;
      analyzer->setAssertCondVarsSet(varsInAssertConditions);
    }

    if(args.getBool("eliminate-compound-assignments")) {
      SAWYER_MESG(logger[TRACE])<<"STATUS: Elimination of compound assignments started."<<endl;
      rewriteSystem.resetStatistics();
      rewriteSystem.rewriteCompoundAssignmentsInAst(root,analyzer->getVariableIdMapping());
      SAWYER_MESG(logger[TRACE])<<"STATUS: Elimination of compound assignments finished."<<endl;
    }

    timer.start();

#if 0
    if(!analyzer->getVariableIdMapping()->isUniqueVariableSymbolMapping()) {
      logger[WARN] << "Variable<->Symbol mapping not bijective."<<endl;
      //varIdMap.reportUniqueVariableSymbolMappingViolations();
    }
#endif
#if 0
    analyzer->getVariableIdMapping()->toStream(cout);
#endif

    if(args.getBool("eliminate-arrays")) {
      Specialization speci;
      speci.transformArrayProgram(sageProject, analyzer);
      sageProject->unparse(0,0);
      exit(0);
    }

    SAWYER_MESG(logger[TRACE])<< "INIT: creating solver "<<analyzer->getSolver()->getId()<<"."<<endl;

    if(option_specialize_fun_name!="") {
      analyzer->initializeSolver(option_specialize_fun_name,root,true);
    } else {
      // if main function exists, start with main-function
      // if a single function exist, use this function
      // in all other cases exit with error.
      RoseAst completeAst(root);
      string startFunction=option_start_function;
      SgNode* startFunRoot=completeAst.findFunctionByName(startFunction);
      if(startFunRoot==0) {
        // no main function exists. check if a single function exists in the translation unit
        SgProject* project=isSgProject(root);
        ROSE_ASSERT(project);
        std::list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(project);
        if(funDefs.size()==1) {
          // found exactly one function. Analyse this function.
          SgFunctionDefinition* functionDef=*funDefs.begin();
          startFunction=SgNodeHelper::getFunctionName(functionDef);
        } else if(funDefs.size()>1) {
          cerr<<"Error: no main function and more than one function in translation unit."<<endl;
          exit(1);
        } else if(funDefs.size()==0) {
          cerr<<"Error: no function in translation unit."<<endl;
          exit(1);
        }
      }
      ROSE_ASSERT(startFunction!="");
      analyzer->initializeSolver(startFunction,root,false);
    }
    analyzer->initLabeledAssertNodes(sageProject);

    // function-id-mapping is initialized in initializeSolver.
    if(args.getBool("print-function-id-mapping")) {
      ROSE_ASSERT(analyzer->getCFAnalyzer());
      ROSE_ASSERT(analyzer->getCFAnalyzer()->getFunctionIdMapping());
      analyzer->getCFAnalyzer()->getFunctionIdMapping()->toStream(cout);
    }


    if(args.isUserProvided("pattern-search-max-depth") || args.isUserProvided("pattern-search-max-suffix")
       || args.isUserProvided("pattern-search-repetitions") || args.isUserProvided("pattern-search-exploration")) {
      logger[INFO] << "at least one of the parameters of mode \"pattern search\" was set. Choosing solver 10." << endl;
      analyzer->setSolver(new Solver10());
      analyzer->setStartPState(*analyzer->popWorkList()->pstate());
    }

    double initRunTime=timer.getTimeDurationAndStop().milliSeconds();

    timer.start();
    analyzer->printStatusMessageLine("==============================================================");
    if(!analyzer->getModeLTLDriven() && args.isDefined("z3") == 0 && !args.getBool("ssa")) {
      analyzer->runSolver();
    }
    double analysisRunTime=timer.getTimeDurationAndStop().milliSeconds();
    analyzer->printStatusMessageLine("==============================================================");

    if (args.getBool("svcomp-mode") && args.isDefined("witness-file")) {
      analyzer->writeWitnessToFile(args.getString("witness-file"));
    }

    double extractAssertionTracesTime= 0;
    if ( args.getBool("with-counterexamples") || args.getBool("with-assert-counterexamples")) {
      SAWYER_MESG(logger[TRACE]) << "STATUS: extracting assertion traces (this may take some time)"<<endl;
      timer.start();
      analyzer->extractRersIOAssertionTraces();
      extractAssertionTracesTime = timer.getTimeDurationAndStop().milliSeconds();
    }

    double determinePrefixDepthTime= 0; // MJ: Determination of prefix depth currently deactivated.
    int inputSeqLengthCovered = -1;
    double totalInputTracesTime = extractAssertionTracesTime + determinePrefixDepthTime;

    bool withCe = args.getBool("with-counterexamples") || args.getBool("with-assert-counterexamples");
    if(args.getBool("status")) {
      analyzer->printStatusMessageLine("==============================================================");
      analyzer->reachabilityResults.printResults("YES (REACHABLE)", "NO (UNREACHABLE)", "error_", withCe);
    }
    if (args.isDefined("csv-assert")) {
      string filename=args.getString("csv-assert").c_str();
      analyzer->reachabilityResults.writeFile(filename.c_str(), false, 0, withCe);
      if(args.getBool("status")) {
        cout << "Reachability results written to file \""<<filename<<"\"." <<endl;
        cout << "=============================================================="<<endl;
      }
    }
    if(args.getBool("eliminate-stg-back-edges")) {
      int numElim=analyzer->getTransitionGraph()->eliminateBackEdges();
      SAWYER_MESG(logger[TRACE])<<"STATUS: eliminated "<<numElim<<" STG back edges."<<endl;
    }

    if(args.getBool("status")) {
      analyzer->reachabilityResults.printResultsStatistics();
      analyzer->printStatusMessageLine("==============================================================");
    }

#ifdef HAVE_Z3
    if(args.isDefined("z3"))
    {
	assert(args.isDefined("rers-upper-input-bound") != 0 &&  args.isDefined("rers-verifier-error-number") != 0);	
	int RERSUpperBoundForInput = args.getInt("rers-upper-input-bound");
	int RERSVerifierErrorNumber = args.getInt("rers-verifier-error-number");
	cout << "generateSSAForm()" << endl;
	ReachabilityAnalyzerZ3* reachAnalyzer = new ReachabilityAnalyzerZ3(RERSUpperBoundForInput, RERSVerifierErrorNumber, analyzer, &logger);	
	cout << "checkReachability()" << endl;
	reachAnalyzer->checkReachability();

	exit(0);
    }
#endif	

    if(args.getBool("ssa")) {
      SSAGenerator* ssaGen = new SSAGenerator(analyzer, &logger);
      ssaGen->generateSSAForm();
      exit(0);
    }

    list<pair<CodeThorn::AnalysisSelector,string> > analysisNames={
      {ANALYSIS_NULL_POINTER,"null-pointer"},
      {ANALYSIS_OUT_OF_BOUNDS,"out-of-bounds"},
      {ANALYSIS_UNINITIALIZED,"uninitialized"}
    };
    for(auto analysisInfo : analysisNames) {
      AnalysisSelector analysisSel=analysisInfo.first;
      string analysisName=analysisInfo.second;
      string analysisOption=analysisName+"-analysis";
      string analysisOutputFileOption=analysisName+"-analysis-file";
      if(args.isDefined(analysisOption)>0||args.isDefined(analysisOutputFileOption)) {
        ProgramLocationsReport locations=analyzer->getExprAnalyzer()->getViolatingLocations(analysisSel);
        if(args.isDefined(analysisOption)>0) {
          cout<<"\nResults for "<<analysisName<<" analysis:"<<endl;
          if(locations.numTotalLocations()>0) {
            locations.writeResultToStream(cout,analyzer->getLabeler());
          } else {
            cout<<"No violations detected."<<endl;
          }
        }
        if(args.isDefined(analysisOutputFileOption)) {
          string fileName=args.getString(analysisOutputFileOption);
          cout<<"Writing "<<analysisName<<" analysis results to file "<<fileName<<endl;
          locations.writeResultFile(fileName,analyzer->getLabeler());
        }
      }
    }

    if(args.isUserProvided("analyzed-functions-csv")) {
      string fileName=args.getString("analyzed-functions-csv");
      cout<<"Writing list of analyzed functions to file "<<fileName<<endl;
      string s=analyzer->analyzedFunctionsToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        logger[ERROR]<<"Cannot create file "<<fileName<<endl;
      }
    }

    if(args.isUserProvided("analyzed-files-csv")) {
      string fileName=args.getString("analyzed-files-csv");
      cout<<"Writing list of analyzed files to file "<<fileName<<endl;
      string s=analyzer->analyzedFilesToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        logger[ERROR]<<"Cannot create file "<<fileName<<endl;
      }
    }

    if(args.isUserProvided("external-functions-csv")) {
      string fileName=args.getString("external-functions-csv");
      cout<<"Writing list of external functions to file "<<fileName<<endl;
      string s=analyzer->externalFunctionsToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        logger[ERROR]<<"Cannot create file "<<fileName<<endl;
      }
    }

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

    double totalRunTime=frontEndRunTime+initRunTime+analysisRunTime;

    long pstateSetSizeInf = 0;
    long eStateSetSizeInf = 0;
    long transitionGraphSizeInf = 0;
    long eStateSetSizeStgInf = 0;
    double infPathsOnlyTime = 0;
    double stdIoOnlyTime = 0;

    if(args.getBool("inf-paths-only")) {
      assert (!args.getBool("keep-error-states"));
      cout << "recursively removing all leaves (1)."<<endl;
      timer.start();
      infPathsOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
      pstateSetSizeInf=analyzer->getPStateSet()->size();
      eStateSetSizeInf = analyzer->getEStateSet()->size();
      transitionGraphSizeInf = analyzer->getTransitionGraph()->size();
      eStateSetSizeStgInf = (analyzer->getTransitionGraph())->estateSet().size();
    }
    
    if(args.getBool("std-io-only")) {
      SAWYER_MESG(logger[TRACE]) << "STATUS: bypassing all non standard I/O states. (P2)"<<endl;
      timer.start();
      if (args.getBool("keep-error-states")) {
        analyzer->reduceStgToInOutAssertStates();
      } else {
        analyzer->reduceStgToInOutStates();
      }
      if(args.getBool("inf-paths-only")) {
        analyzer->pruneLeaves();
      }
      stdIoOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
    }

    long eStateSetSizeIoOnly = 0;
    long transitionGraphSizeIoOnly = 0;
    double spotLtlAnalysisTime = 0;

    stringstream statisticsSizeAndLtl;
    stringstream statisticsCegpra;

    if (args.isDefined("check-ltl")) {
      logger[INFO] <<"STG size: "<<analyzer->getTransitionGraph()->size()<<endl;
      string ltl_filename = args.getString("check-ltl");
      if(args.getBool("rersmode")) {  //reduce the graph accordingly, if not already done
        if (!args.getBool("inf-paths-only") && !args.getBool("keep-error-states") &&!analyzer->getModeLTLDriven()) {
          cout<< "STATUS: recursively removing all leaves (due to RERS-mode (2))."<<endl;
          timer.start();
          analyzer->pruneLeaves();
          infPathsOnlyTime = timer.getTimeDurationAndStop().milliSeconds();

          pstateSetSizeInf=analyzer->getPStateSet()->size();
          eStateSetSizeInf = analyzer->getEStateSet()->size();
          transitionGraphSizeInf = analyzer->getTransitionGraph()->size();
          eStateSetSizeStgInf = (analyzer->getTransitionGraph())->estateSet().size();
        }
        if (!args.getBool("std-io-only") &&!analyzer->getModeLTLDriven()) {
          cout << "STATUS: bypassing all non standard I/O states (due to RERS-mode) (P1)."<<endl;
          timer.start();
          analyzer->getTransitionGraph()->printStgSize("before reducing non-I/O states");
          if (args.getBool("keep-error-states")) {
            analyzer->reduceStgToInOutAssertStates();
          } else {
            analyzer->reduceStgToInOutStates();
          }
          stdIoOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
          analyzer->getTransitionGraph()->printStgSize("after reducing non-I/O states");
        }
      }
      if(args.getBool("no-input-input")) {  //delete transitions that indicate two input states without an output in between
        analyzer->removeInputInputTransitions();
        analyzer->getTransitionGraph()->printStgSize("after reducing input->input transitions");
      }
      bool withCounterexample = false;
      if(args.getBool("with-counterexamples") || args.getBool("with-ltl-counterexamples")) {  //output a counter-example input sequence for falsified formulae
        withCounterexample = true;
      }

      timer.start();
      std::set<int> ltlInAlphabet = analyzer->getInputVarValues();
      //take fixed ltl input alphabet if specified, instead of the input values used for stg computation
      if (args.isDefined("ltl-in-alphabet")) {
        string setstring=args.getString("ltl-in-alphabet");
        ltlInAlphabet=Parse::integerSet(setstring);
        SAWYER_MESG(logger[TRACE]) << "LTL input alphabet explicitly selected: "<< setstring << endl;
      }
      //take ltl output alphabet if specifically described, otherwise take the old RERS specific 21...26 (a.k.a. oU...oZ)
      std::set<int> ltlOutAlphabet = Parse::integerSet("{21,22,23,24,25,26}");
      if (args.isDefined("ltl-out-alphabet")) {
        string setstring=args.getString("ltl-out-alphabet");
        ltlOutAlphabet=Parse::integerSet(setstring);
        SAWYER_MESG(logger[TRACE]) << "LTL output alphabet explicitly selected: "<< setstring << endl;
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
      if (args.isDefined("single-property")) {
	int propertyNum = args.getInt("single-property");
	spotConnection.checkSingleProperty(propertyNum, *(analyzer->getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample, spuriousNoAnswers);
      } else {
	spotConnection.checkLtlProperties( *(analyzer->getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample, spuriousNoAnswers);
      }
      spotLtlAnalysisTime=timer.getTimeDurationAndStop().milliSeconds();
      SAWYER_MESG(logger[TRACE]) << "LTL: get results from spot connection."<<endl;
      ltlResults = spotConnection.getLtlResults();
      SAWYER_MESG(logger[TRACE]) << "LTL: results computed."<<endl;

      if (args.isDefined("cegpra-ltl") || (args.isDefined("cegpra-ltl-all")&&args.getBool("cegpra-ltl-all"))) {
        if (args.isDefined("csv-stats-cegpra")) {
          statisticsCegpra << "init,";
          analyzer->getTransitionGraph()->printStgSize("initial abstract model");
          analyzer->getTransitionGraph()->csvToStream(statisticsCegpra);
          statisticsCegpra << ",na,na";
          statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_YES);
          statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_NO);
          statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_UNKNOWN);
        }
        CounterexampleAnalyzer ceAnalyzer(analyzer, &statisticsCegpra);
        if (args.isDefined("cegpra-max-iterations")) {
          ceAnalyzer.setMaxCounterexamples(args.getInt("cegpra-max-iterations"));
        }
        if (args.getBool("cegpra-ltl-all")) {
          ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(spotConnection, ltlInAlphabet, ltlOutAlphabet);
        } else {  // cegpra for single LTL property
          int property = args.getInt("cegpra-ltl");
          ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(property, spotConnection, ltlInAlphabet, ltlOutAlphabet);
        }
      }

      if(args.getBool("status")) {
        ltlResults-> printResults("YES (verified)", "NO (falsified)", "ltl_property_", withCounterexample);
        analyzer->printStatusMessageLine("==============================================================");
        ltlResults->printResultsStatistics();
        analyzer->printStatusMessageLine("==============================================================");
      }
      if (args.isDefined("csv-spot-ltl")) {  //write results to a file instead of displaying them directly
        std::string csv_filename = args.getString("csv-spot-ltl");
        SAWYER_MESG(logger[TRACE]) << "STATUS: writing ltl results to file: " << csv_filename << endl;
        ltlResults->writeFile(csv_filename.c_str(), false, 0, withCounterexample);
      }
      if (args.isDefined("csv-stats-size-and-ltl")) {
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

    // TEST
    if (args.getBool("generate-assertions")) {
      AssertionExtractor assertionExtractor(analyzer);
      assertionExtractor.computeLabelVectorOfEStates();
      assertionExtractor.annotateAst();
      AstAnnotator ara(analyzer->getLabeler());
      ara.annotateAstAttributesAsCommentsBeforeStatements  (sageProject,"ctgen-pre-condition");
      SAWYER_MESG(logger[TRACE]) << "STATUS: Generated assertions."<<endl;
    }

    double arrayUpdateExtractionRunTime=0.0;
    double arrayUpdateSsaNumberingRunTime=0.0;
    double sortingAndIORunTime=0.0;
    double verifyUpdateSequenceRaceConditionRunTime=0.0;

    int verifyUpdateSequenceRaceConditionsResult=-1;
    int verifyUpdateSequenceRaceConditionsTotalLoopNum=-1;
    int verifyUpdateSequenceRaceConditionsParLoopNum=-1;

    /* Data race detection */
    if(dataRaceDetection.run(*analyzer)) {
      exit(0);
    }

    if(args.isDefined("dump-sorted")>0 || args.isDefined("dump-non-sorted")>0) {
      SAR_MODE sarMode=SAR_SSA;
      if(args.getBool("rewrite-ssa")) {
	sarMode=SAR_SUBSTITUTE;
      }
      Specialization speci;
      ArrayUpdatesSequence arrayUpdates;
      SAWYER_MESG(logger[TRACE]) <<"STATUS: performing array analysis on STG."<<endl;
      SAWYER_MESG(logger[TRACE]) <<"STATUS: identifying array-update operations in STG and transforming them."<<endl;

      bool useRuleConstSubstitution=args.getBool("rule-const-subst");
      bool useRuleCommutativeSort=args.getBool("rule-commutative-sort");
      
      timer.start();
      speci.extractArrayUpdateOperations(analyzer,
          arrayUpdates,
          rewriteSystem,
          useRuleConstSubstitution
          );

      //cout<<"DEBUG: Rewrite1:"<<rewriteSystem.getStatistics().toString()<<endl;
      speci.substituteArrayRefs(arrayUpdates, analyzer->getVariableIdMapping(), sarMode, rewriteSystem);
      //cout<<"DEBUG: Rewrite2:"<<rewriteSystem.getStatistics().toString()<<endl;

      rewriteSystem.setRuleCommutativeSort(useRuleCommutativeSort); // commutative sort only used in substituteArrayRefs
      //cout<<"DEBUG: Rewrite3:"<<rewriteSystem.getStatistics().toString()<<endl;
      speci.substituteArrayRefs(arrayUpdates, analyzer->getVariableIdMapping(), sarMode, rewriteSystem);
      arrayUpdateExtractionRunTime=timer.getTimeDurationAndStop().milliSeconds();

      if(args.getBool("print-update-infos")) {
        speci.printUpdateInfos(arrayUpdates,analyzer->getVariableIdMapping());
      }
      SAWYER_MESG(logger[TRACE]) <<"STATUS: establishing array-element SSA numbering."<<endl;
      timer.start();
      speci.createSsaNumbering(arrayUpdates, analyzer->getVariableIdMapping());
      arrayUpdateSsaNumberingRunTime=timer.getTimeDurationAndStop().milliSeconds();

      if(args.isDefined("dump-non-sorted")) {
        string filename=args.getString("dump-non-sorted");
        speci.writeArrayUpdatesToFile(arrayUpdates, filename, sarMode, false);
      }
      if(args.isDefined("dump-sorted")) {
        timer.start();
        string filename=args.getString("dump-sorted");
        speci.writeArrayUpdatesToFile(arrayUpdates, filename, sarMode, true);
        sortingAndIORunTime=timer.getTimeDurationAndStop().milliSeconds();
      }
      totalRunTime+=arrayUpdateExtractionRunTime+verifyUpdateSequenceRaceConditionRunTime+arrayUpdateSsaNumberingRunTime+sortingAndIORunTime;
    }

    double overallTime =totalRunTime + totalInputTracesTime + totalLtlRunTime;

    analyzer->printAnalyzerStatistics(totalRunTime, "STG generation and assertion analysis complete");

    if(args.isDefined("csv-stats")) {
      string filename=args.getString("csv-stats").c_str();
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
        <<CodeThorn::readableruntime(frontEndRunTime)<<", "
        <<CodeThorn::readableruntime(initRunTime)<<", "
        <<CodeThorn::readableruntime(analysisRunTime)<<", "
        <<CodeThorn::readableruntime(verifyUpdateSequenceRaceConditionRunTime)<<", "
        <<CodeThorn::readableruntime(arrayUpdateExtractionRunTime)<<", "
        <<CodeThorn::readableruntime(arrayUpdateSsaNumberingRunTime)<<", "
        <<CodeThorn::readableruntime(sortingAndIORunTime)<<", "
        <<CodeThorn::readableruntime(totalRunTime)<<", "
        <<CodeThorn::readableruntime(extractAssertionTracesTime)<<", "
        <<CodeThorn::readableruntime(determinePrefixDepthTime)<<", "
        <<CodeThorn::readableruntime(totalInputTracesTime)<<", "
        <<CodeThorn::readableruntime(infPathsOnlyTime)<<", "
        <<CodeThorn::readableruntime(stdIoOnlyTime)<<", "
        <<CodeThorn::readableruntime(spotLtlAnalysisTime)<<", "
        <<CodeThorn::readableruntime(totalLtlRunTime)<<", "
        <<CodeThorn::readableruntime(overallTime)<<endl;
      text<<"Runtime(ms),"
        <<frontEndRunTime<<", "
        <<initRunTime<<", "
        <<analysisRunTime<<", "
        <<verifyUpdateSequenceRaceConditionRunTime<<", "
        <<arrayUpdateExtractionRunTime<<", "
        <<arrayUpdateSsaNumberingRunTime<<", "
        <<sortingAndIORunTime<<", "
        <<totalRunTime<<", "
        <<extractAssertionTracesTime<<", "
        <<determinePrefixDepthTime<<", "
        <<totalInputTracesTime<<", "
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

      text<<"rewrite-stats, "<<rewriteSystem.getRewriteStatistics().toCsvString()<<endl;
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
      text<<"input length coverage"
	<<inputSeqLengthCovered<<endl;

      write_file(filename,text.str());
      cout << "generated "<<filename<<endl;
    }

    if (args.isDefined("csv-stats-size-and-ltl")) {
      // content of a line in the .csv file:
      // <#transitions>,<#states>,<#input_states>,<#output_states>,<#error_states>,<#verified_LTL>,<#falsified_LTL>,<#unknown_LTL>
      string filename = args.getString("csv-stats-size-and-ltl");
      write_file(filename,statisticsSizeAndLtl.str());
      cout << "generated "<<filename<<endl;
    }

    if (args.isDefined("csv-stats-cegpra")) {
      // content of a line in the .csv file:
      // <analyzed_property>,<#transitions>,<#states>,<#input_states>,<#output_states>,<#error_states>,
      // <#analyzed_counterexamples>,<analysis_result(y/n/?)>,<#verified_LTL>,<#falsified_LTL>,<#unknown_LTL>
      string filename = args.getString("csv-stats-cegpra");
      write_file(filename,statisticsCegpra.str());
      cout << "generated "<<filename<<endl;
    }


    {
      Visualizer visualizer(analyzer->getLabeler(),analyzer->getVariableIdMapping(),analyzer->getFlow(),analyzer->getPStateSet(),analyzer->getEStateSet(),analyzer->getTransitionGraph());
      if (args.isDefined("cfg")) {
        string cfgFileName=args.getString("cfg");
        DataDependenceVisualizer ddvis(analyzer->getLabeler(),analyzer->getVariableIdMapping(),"none");
        ddvis.setDotGraphName("CFG");
        ddvis.generateDotFunctionClusters(root,analyzer->getCFAnalyzer(),cfgFileName,false);
        cout << "generated "<<cfgFileName<<endl;
      }
      if(args.getBool("viz")) {
        cout << "generating graphviz files:"<<endl;
        visualizer.setOptionMemorySubGraphs(args.getBool("tg1-estate-memory-subgraphs"));
        string dotFile="digraph G {\n";
        dotFile+=visualizer.transitionGraphToDot();
        dotFile+="}\n";
        write_file("transitiongraph1.dot", dotFile);
        cout << "generated transitiongraph1.dot."<<endl;
        string dotFile3=visualizer.foldedTransitionGraphToDot();
        write_file("transitiongraph2.dot", dotFile3);
        cout << "generated transitiongraph2.dot."<<endl;

        string datFile1=(analyzer->getTransitionGraph())->toString();
        write_file("transitiongraph1.dat", datFile1);
        cout << "generated transitiongraph1.dat."<<endl;

        assert(analyzer->startFunRoot);
        //analyzer->generateAstNodeInfo(analyzer->startFunRoot);
        //dotFile=astTermWithNullValuesToDot(analyzer->startFunRoot);
        SAWYER_MESG(logger[TRACE]) << "Option VIZ: generate ast node info."<<endl;
        analyzer->generateAstNodeInfo(sageProject);
        cout << "generating AST node info ... "<<endl;
        dotFile=AstTerm::functionAstTermsWithNullValuesToDot(sageProject);
        write_file("ast.dot", dotFile);
        cout << "generated ast.dot."<<endl;

        SAWYER_MESG(logger[TRACE]) << "Option VIZ: generating cfg dot file ..."<<endl;
        write_file("cfg_non_clustered.dot", analyzer->getFlow()->toDot(analyzer->getCFAnalyzer()->getLabeler()));
        DataDependenceVisualizer ddvis(analyzer->getLabeler(),analyzer->getVariableIdMapping(),"none");
        ddvis.generateDotFunctionClusters(root,analyzer->getCFAnalyzer(),"cfg.dot",false);
        cout << "generated cfg.dot, cfg_non_clustered.dot"<<endl;
        cout << "=============================================================="<<endl;
      }
      if(args.getBool("viz-tg2")) {
        string dotFile3=visualizer.foldedTransitionGraphToDot();
        write_file("transitiongraph2.dot", dotFile3);
        cout << "generated transitiongraph2.dot."<<endl;
      }

      if (args.isDefined("dot-io-stg")) {
        string filename=args.getString("dot-io-stg");
        cout << "generating dot IO graph file:"<<filename<<endl;
        string dotFile="digraph G {\n";
        dotFile+=visualizer.transitionGraphWithIOToDot();
        dotFile+="}\n";
        write_file(filename, dotFile);
        cout << "=============================================================="<<endl;
      }

      if (args.isDefined("dot-io-stg-forced-top")) {
        string filename=args.getString("dot-io-stg-forced-top");
        cout << "generating dot IO graph file for an abstract STG:"<<filename<<endl;
        string dotFile="digraph G {\n";
        dotFile+=visualizer.abstractTransitionGraphToDot();
        dotFile+="}\n";
        write_file(filename, dotFile);
        cout << "=============================================================="<<endl;
      }
    }
    // InputPathGenerator
#if 1
    {
      if(args.isDefined("iseq-file")) {
        int iseqLen=0;
        if(args.isDefined("iseq-length")) {
          iseqLen=args.getInt("iseq-length");
        } else {
          logger[ERROR] <<"input-sequence file specified, but no sequence length."<<endl;
          exit(1);
        }
        string fileName=args.getString("iseq-file");
        SAWYER_MESG(logger[TRACE]) <<"STATUS: computing input sequences of length "<<iseqLen<<endl;
        IOSequenceGenerator iosgen;
        if(args.isDefined("iseq-random-num")) {
          int randomNum=args.getInt("iseq-random-num");
          SAWYER_MESG(logger[TRACE]) <<"STATUS: reducing input sequence set to "<<randomNum<<" random elements."<<endl;
          iosgen.computeRandomInputPathSet(iseqLen,*analyzer->getTransitionGraph(),randomNum);
        } else {
          iosgen.computeInputPathSet(iseqLen,*analyzer->getTransitionGraph());
        }
        SAWYER_MESG(logger[TRACE]) <<"STATUS: generating input sequence file "<<fileName<<endl;
        iosgen.generateFile(fileName);
      } else {
        if(args.isDefined("iseq-length")) {
          logger[ERROR] <<"input sequence length specified without also providing a file name (use option --iseq-file)."<<endl;
          exit(1);
        }
      }
    }
#endif

#if 0
    {
      cout << "EStateSet:\n"<<analyzer->getEStateSet()->toString(analyzer->getVariableIdMapping())<<endl;
      cout << "ConstraintSet:\n"<<analyzer->getConstraintSetMaintainer()->toString()<<endl;
      if(analyzer->variableValueMonitor.isActive())
        cout << "VariableValueMonitor:\n"<<analyzer->variableValueMonitor.toString(analyzer->getVariableIdMapping())<<endl;
      cout << "MAP:"<<endl;
      cout << analyzer->getLabeler()->toString();
    }
#endif

    if (args.getBool("annotate-terms")) {
      // TODO: it might be useful to be able to select certain analysis results to be only annotated
      logger[INFO] << "Annotating term representations."<<endl;
      AstTermRepresentationAttribute::attachAstTermRepresentationAttributes(sageProject);
      AstAnnotator ara(analyzer->getLabeler());
      ara.annotateAstAttributesAsCommentsBeforeStatements(sageProject,"codethorn-term-representation");
    }

    if (args.getBool("annotate-terms")||args.getBool("generate-assertions")) {
      logger[INFO] << "Generating annotated program."<<endl;
      //backend(sageProject);
      sageProject->unparse(0,0);
    }

    // reset terminal
    if(!args.isDefined("quiet"))
      cout<<color("normal")<<"done."<<endl;

    // main function try-catch
  } catch(const CodeThorn::Exception& e) {
    cerr << "CodeThorn::Exception raised: " << e.what() << endl;
    mfacilities.shutdown();
    return 1;
  } catch(const std::exception& e) {
    cerr<< "std::exception raised: " << e.what() << endl;
    mfacilities.shutdown();
    return 1;
  } catch(char const* str) {
    cerr<< "*Exception raised: " << str << endl;
    mfacilities.shutdown();
    return 1;
  } catch(string str) {
    cerr<< "Exception raised: " << str << endl;
    mfacilities.shutdown();
    return 1;
  } catch(...) {
    cerr<< "Unknown exception raised." << endl;
    mfacilities.shutdown();
    return 1;
  }
  mfacilities.shutdown();
  return 0;
}

