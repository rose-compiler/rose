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
#include "ltlthorn-lib/SpotConnection.h"
#include "ltlthorn-lib/CounterexampleAnalyzer.h"
#include "AnalysisAbstractionLayer.h"
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

// Z3-based analyser / SSA 
#include "z3-prover-connection/SSAGenerator.h"
#include "z3-prover-connection/ReachabilityAnalyzerZ3.h"

// ParProAutomata
#include "ltlthorn-lib/ParProAutomata.h"

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

const std::string versionString="1.12.1";

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

void analyzerSetup(IOAnalyzer* analyzer, Sawyer::Message::Facility logger,
                   CodeThornOptions& ctOpt, LTLOptions& ltlOpt, ParProOptions& parProOpt) {
  analyzer->setOptionOutputWarnings(ctOpt.printWarnings);
  analyzer->setPrintDetectedViolations(ctOpt.printViolations);

  // this must be set early, as subsequent initialization depends on this flag
  if (ltlOpt.ltlDriven) {
    analyzer->setModeLTLDriven(true);
  }

  if (ltlOpt.cegpra.ltlPropertyNrIsSet() || ltlOpt.cegpra.checkAllProperties) {
    analyzer->setMaxTransitionsForcedTop(1); //initial over-approximated model
    ltlOpt.noInputInputTransitions=true;
    ltlOpt.withLTLCounterExamples=true;
    ltlOpt.counterExamplesWithOutput=true;
    cout << "STATUS: CEGPRA activated (with it LTL counterexamples that include output states)." << endl;
    cout << "STATUS: CEGPRA mode: will remove input state --> input state transitions in the approximated STG." << endl;
  }

  if (ltlOpt.counterExamplesWithOutput) {
    ltlOpt.withLTLCounterExamples=true;
  }

  if(ctOpt.stgTraceFileName.size()>0) {
    analyzer->setStgTraceFileName(ctOpt.stgTraceFileName);
  }

  if(ctOpt.analyzedProgramCLArgs.size()>0) {
    string clOptions=ctOpt.analyzedProgramCLArgs;
    vector<string> clOptionsVector=Parse::commandLineArgs(clOptions);
    analyzer->setCommandLineOptions(clOptionsVector);
  }

  if(ctOpt.inputValues.size()>0) {
    cout << "STATUS: input-values="<<ctOpt.inputValues<<endl;
    set<int> intSet=Parse::integerSet(ctOpt.inputValues);
    for(set<int>::iterator i=intSet.begin();i!=intSet.end();++i) {
      analyzer->insertInputVarValue(*i);
    }
    cout << "STATUS: input-values stored."<<endl;
  }

  if(ctOpt.inputSequence.size()>0) {
    cout << "STATUS: input-sequence="<<ctOpt.inputSequence<<endl;
    list<int> intList=Parse::integerList(ctOpt.inputSequence);
    for(list<int>::iterator i=intList.begin();i!=intList.end();++i) {
      analyzer->addInputSequenceValue(*i);
    }
  }

  if(ctOpt.explorationMode.size()>0) {
    string explorationMode=ctOpt.explorationMode;
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

  if (ctOpt.maxIterations!=-1 || ctOpt.maxIterationsForcedTop!=-1) {
    if(ctOpt.explorationMode!="loop-aware" && ctOpt.explorationMode!="loop-aware-sync") {
      cout << "Error: \"max-iterations[-forced-top]\" modes currently require \"--exploration-mode=loop-aware[-sync]\"." << endl;
      exit(1);
    }
  }

  analyzer->setAbstractionMode(ctOpt.abstractionMode);
  analyzer->setMaxTransitions(ctOpt.maxTransitions);
  analyzer->setMaxIterations(ctOpt.maxIterations);

  if(ctOpt.maxIterationsForcedTop!=-1) {
    analyzer->setMaxIterationsForcedTop(ctOpt.maxIterationsForcedTop);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  }

  // TODO: Analyzer::GTM_IO is only mode used now, all others are deprecated
  if(ctOpt.maxTransitionsForcedTop!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  } else if(ctOpt.maxTransitionsForcedTop1!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop1);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  } else if(ctOpt.maxTransitionsForcedTop2!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop2);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IOCF);
  } else if(ctOpt.maxTransitionsForcedTop3!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop3);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IOCFPTR);
  } else if(ctOpt.maxTransitionsForcedTop4!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop4);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_COMPOUNDASSIGN);
  } else if(ctOpt.maxTransitionsForcedTop5!=-1) {
    analyzer->setMaxTransitionsForcedTop(ctOpt.maxTransitionsForcedTop5);
    analyzer->setGlobalTopifyMode(Analyzer::GTM_FLAGS);
  }

  if (ctOpt.maxMemory!=1) {
    analyzer->setMaxBytes(ctOpt.maxMemory);
  }
  if (ctOpt.maxTime!=1) {
    analyzer->setMaxSeconds(ctOpt.maxTime);
  }
  if (ctOpt.maxMemoryForcedTop!=-1) {
    analyzer->setMaxBytesForcedTop(ctOpt.maxMemoryForcedTop);
  }
  if (ctOpt.maxTimeForcedTop!=-1) {
    analyzer->setMaxSecondsForcedTop(ctOpt.maxTimeForcedTop);
  }

  if(ctOpt.displayDiff!=-1) {
    analyzer->setDisplayDiff(ctOpt.displayDiff);
  }
  if(ctOpt.resourceLimitDiff!=-1) {
    analyzer->setResourceLimitDiff(ctOpt.resourceLimitDiff);
  }

  Solver* solver = nullptr;
  // overwrite solver ID based on other options
  if(analyzer->getModeLTLDriven()) {
    ctOpt.solver=11;
  }
  int solverId=ctOpt.solver;
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

void configureRose() {
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

  // see class Options in src/roseSupport/utility_functions.h
  Rose::global_options.set_frontend_notes(false);
  Rose::global_options.set_frontend_warnings(false);
  Rose::global_options.set_backend_warnings(false);

  signal(SIGSEGV, handler);   // install handler for backtrace
}

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

void configureOptionSets(CodeThornOptions& ctOpt) {
  string optionName="options-set";  // only used for error reporting
  int optionValue=ctOpt.optionsSet; // only used for error reporting
  switch(optionValue) {
  case 0:
    // fall-through for default
    break;
  case 1:
    ctOpt.explicitArrays=true;
    ctOpt.inStateStringLiterals=true;
    ctOpt.ignoreUnknownFunctions=true;
    ctOpt.ignoreFunctionPointers=true;
    ctOpt.stdFunctions=true;
    ctOpt.contextSensitive=true;
    ctOpt.normalizeAll=true;
    ctOpt.abstractionMode=1;
    AbstractValue::strictChecking=false;
    break;
  case 2:
    ctOpt.explicitArrays=true;
    ctOpt.inStateStringLiterals=true;
    ctOpt.ignoreUnknownFunctions=true;
    ctOpt.ignoreFunctionPointers=false;
    ctOpt.stdFunctions=true;
    ctOpt.contextSensitive=true;
    ctOpt.normalizeAll=true;
    ctOpt.abstractionMode=1;
    AbstractValue::strictChecking=false;
    break;
  case 3:
    ctOpt.explicitArrays=true;
    ctOpt.inStateStringLiterals=false;
    ctOpt.ignoreUnknownFunctions=true;
    ctOpt.ignoreFunctionPointers=false;
    ctOpt.stdFunctions=false;
    ctOpt.contextSensitive=true;
    ctOpt.normalizeAll=true;
    ctOpt.abstractionMode=1;
    AbstractValue::strictChecking=false;
    break;
  case 4:
    ctOpt.explicitArrays=true;
    ctOpt.inStateStringLiterals=false;
    ctOpt.ignoreUnknownFunctions=true;
    ctOpt.ignoreFunctionPointers=false;
    ctOpt.stdFunctions=false;
    ctOpt.contextSensitive=true;
    ctOpt.normalizeAll=true;
    ctOpt.abstractionMode=1;
    AbstractValue::strictChecking=true;
    break;
  case 11:
    ctOpt.explicitArrays=true;
    ctOpt.inStateStringLiterals=true;
    ctOpt.ignoreUnknownFunctions=true;
    ctOpt.ignoreFunctionPointers=false;
    ctOpt.stdFunctions=false;
    ctOpt.contextSensitive=true;
    ctOpt.normalizeAll=true;
    ctOpt.abstractionMode=0;
    break;
  default:
    cerr<<"Error: unsupported "<<optionName<<" value: "<<optionValue<<endl;
    exit(1);
  }
}

int main( int argc, char * argv[] ) {
  try {
    configureRose();
    Sawyer::Message::Facility logger;
    Rose::Diagnostics::initAndRegister(&logger, "CodeThorn");
    configureRersSpecialization();

    TimeMeasurement timer;
    timer.start();

    CodeThornOptions ctOpt;
    LTLOptions ltlOpt; // to be moved into separate tool
    ParProOptions parProOpt; // to be moved into separate tool
    parseCommandLine(argc, argv, logger,versionString,ctOpt,ltlOpt,parProOpt);

    // Start execution
    mfacilities.control(ctOpt.logLevel);
    SAWYER_MESG(logger[TRACE]) << "Log level is " << ctOpt.logLevel << endl;

    // ParPro command line options
    bool exitRequest=CodeThorn::ParProAutomata::handleCommandLineArguments(parProOpt,ctOpt,ltlOpt,logger);
    if(exitRequest) {
      exit(0);
    }

    IOAnalyzer* analyzer;
    if(ctOpt.dr.checkShuffleAlgorithm) {
      analyzer = new ReadWriteAnalyzer();
    } else {
      analyzer = new IOAnalyzer();
    }
    analyzer->setOptions(ctOpt);
    analyzer->setLtlOptions(ltlOpt);
    global_analyzer=analyzer;

    if(ctOpt.internalChecks) {
      mfacilities.shutdown();
      if(CodeThorn::internalChecks(argc,argv)==false)
        return 1;
      else
        return 0;
    }

    configureOptionSets(ctOpt);

    analyzer->optionStringLiteralsInState=ctOpt.inStateStringLiterals;
    analyzer->setSkipUnknownFunctionCalls(ctOpt.ignoreUnknownFunctions);
    analyzer->setIgnoreFunctionPointers(ctOpt.ignoreFunctionPointers);
    analyzer->setStdFunctionSemantics(ctOpt.stdFunctions);

    analyzerSetup(analyzer, logger, ctOpt, ltlOpt, parProOpt);

    switch(int mode=ctOpt.interpreterMode) {
    case 0: analyzer->setInterpreterMode(IM_ABSTRACT); break;
    case 1: analyzer->setInterpreterMode(IM_CONCRETE); break;
    default:
      cerr<<"Unknown interpreter mode "<<mode<<" provided on command line (supported: 0..1)."<<endl;
      exit(1);
    }
    string outFileName=ctOpt.interpreterModeOuputFileName;
    if(outFileName!="") {
      analyzer->setInterpreterModeOutputFileName(outFileName);
      CppStdUtilities::writeFile(outFileName,""); // touch file
    }
    {
      switch(int argVal=ctOpt.functionResolutionMode) {
      case 1: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_TRANSLATION_UNIT;break;
      case 2: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_WHOLE_AST_LOOKUP;break;
      case 3: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_FUNCTION_ID_MAPPING;break;
      case 4: CFAnalysis::functionResolutionMode=CFAnalysis::FRM_FUNCTION_CALL_MAPPING;break;
      default: 
        cerr<<"Error: unsupported argument value of "<<argVal<<" for function-resolution-mode.";
        exit(1);
      }
    }
    // analyzer->setFunctionResolutionMode(ctOpt.functionResolutionMode);
    // needs to set CFAnalysis functionResolutionMode

    int numThreads=ctOpt.threads; // default is 1
    if(numThreads<=0) {
      cerr<<"Error: number of threads must be greater or equal 1."<<endl;
      exit(1);
    }
    analyzer->setNumberOfThreadsToUse(numThreads);

    string option_start_function="main";
    if(ctOpt.startFunctionName.size()>0) {
      option_start_function = ctOpt.startFunctionName;
    }

    string option_specialize_fun_name="";
    vector<int> option_specialize_fun_param_list;
    vector<int> option_specialize_fun_const_list;
    vector<string> option_specialize_fun_varinit_list;
    vector<int> option_specialize_fun_varinit_const_list;
    if(ctOpt.equiCheck.specializeFunName.size()>0) {
      option_specialize_fun_name = ctOpt.equiCheck.specializeFunName;
      // logger[DEBUG] << "option_specialize_fun_name: "<< option_specialize_fun_name<<endl;
    } else {
      // logger[DEBUG] << "option_specialize_fun_name: NONE"<< option_specialize_fun_name<<endl;
    }

    if(ctOpt.equiCheck.specializeFunParamList.size()>0) {
      option_specialize_fun_param_list=ctOpt.equiCheck.specializeFunParamList;
      option_specialize_fun_const_list=ctOpt.equiCheck.specializeFunConstList;
    }

    if(ctOpt.equiCheck.specializeFunVarInitList.size()>0) {
      option_specialize_fun_varinit_list=ctOpt.equiCheck.specializeFunVarInitList;
      option_specialize_fun_varinit_const_list=ctOpt.equiCheck.specializeFunVarInitConstList;
    }

    // logger[DEBUG] << "specialize-params:"<<option_specialize_fun_const_list.size()<<endl;

    if(ctOpt.equiCheck.specializeFunName.size()>0) {
      if( ((ctOpt.equiCheck.specializeFunParamList.size()>0)||ctOpt.equiCheck.specializeFunConstList.size()>0)
        && !(ctOpt.equiCheck.specializeFunName.size()>0&&ctOpt.equiCheck.specializeFunParamList.size()>0)) {
        logger[ERROR] <<"options --specialize-fun-name=NAME --specialize-fun-param=NUM --specialize-fun-const=NUM must be used together."<<endl;
        exit(1);
      }
    if((ctOpt.equiCheck.specializeFunVarInitList.size()>0||ctOpt.equiCheck.specializeFunVarInitConstList.size()>0)
       && !(ctOpt.equiCheck.specializeFunVarInitList.size()>0&&ctOpt.equiCheck.specializeFunVarInitConstList.size()>0)) {
        logger[ERROR] <<"options --specialize-fun-name=NAME --specialize-fun-varinit=NAME --specialize-fun-const=NUM must be used together."<<endl;
        exit(1);
      }
    }

    if((ctOpt.equiCheck.printUpdateInfos)&&(ctOpt.equiCheck.dumpSortedFileName.size()==0 && ctOpt.equiCheck.dumpNonSortedFileName.size()==0)) {
      logger[ERROR] <<"option print-update-infos/equivalence-check must be used together with option --dump-non-sorted or --dump-sorted."<<endl;
      exit(1);
    }
    RewriteSystem rewriteSystem;
    if(ctOpt.equiCheck.printRewriteTrace) {
      rewriteSystem.setTrace(true);
    }
    if(ctOpt.ignoreUndefinedDereference) {
      analyzer->setIgnoreUndefinedDereference(true);
    }
    if(ctOpt.equiCheck.dumpSortedFileName.size()>0 || ctOpt.equiCheck.dumpNonSortedFileName.size()>0) {
      analyzer->setSkipUnknownFunctionCalls(true);
      analyzer->setSkipArrayAccesses(true);
      ctOpt.explicitArrays=false;
      if(analyzer->getNumberOfThreadsToUse()>1) {
        logger[ERROR] << "multi threaded rewrite not supported yet."<<endl;
        exit(1);
      }
    }

    // parse command line options for data race detection
    DataRaceDetection dataRaceDetection;
    dataRaceDetection.setOptions(ctOpt);
    dataRaceDetection.handleCommandLineOptions(*analyzer);
    dataRaceDetection.setVisualizeReadWriteAccesses(ctOpt.visualization.visualizeRWSets);

    // handle RERS mode: reconfigure options
    if(ctOpt.rers.rersMode) {
      SAWYER_MESG(logger[TRACE]) <<"RERS MODE activated [stderr output is treated like a failed assert]"<<endl;
      ctOpt.rers.stdErrLikeFailedAssert=true;
    }

    if(ctOpt.svcomp.svcompMode) {
      analyzer->enableSVCompFunctionSemantics();
      string errorFunctionName="__VERIFIER_error";
      analyzer->setExternalErrorFunctionName(errorFunctionName);
    }

    if(ctOpt.svcomp.detectedErrorFunctionName.size()>0) {
      analyzer->setExternalErrorFunctionName(ctOpt.svcomp.detectedErrorFunctionName);
    }

    analyzer->setTreatStdErrLikeFailedAssert(ctOpt.rers.stdErrLikeFailedAssert);

    // Build the AST used by ROSE
    if(ctOpt.status) {
      cout<< "STATUS: Parsing and creating AST started!"<<endl;
    }

    SgProject* sageProject = 0;
    vector<string> argvList(argv,argv+argc);
    //string turnOffRoseLoggerWarnings="-rose:log none";
    //    argvList.push_back(turnOffRoseLoggerWarnings);
    if(ctOpt.ompAst||ctOpt.dr.detection) {
      SAWYER_MESG(logger[TRACE])<<"selected OpenMP AST."<<endl;
      argvList.push_back("-rose:OpenMP:ast_only");
    }
    timer.stop();

    timer.start();
    sageProject=frontend(argvList);
    double frontEndRunTime=timer.getTimeDurationAndStop().milliSeconds();

    if(ctOpt.status) {
      cout << "STATUS: Parsing and creating AST finished."<<endl;
    }

    /* perform inlining before variable ids are computed, because
       variables are duplicated by inlining. */
    timer.start();
    Normalization lowering;
    if(ctOpt.normalizeFCalls) {
      lowering.normalizeAst(sageProject,1);
      SAWYER_MESG(logger[TRACE])<<"STATUS: normalized expressions with fcalls (if not a condition)"<<endl;
    }

    if(ctOpt.normalizeAll) {
      if(ctOpt.quiet==false) {
        cout<<"STATUS: normalizing program."<<endl;
      }
      //SAWYER_MESG(logger[INFO])<<"STATUS: normalizing program."<<endl;
      lowering.normalizeAst(sageProject,2);
    }
    double normalizationRunTime=timer.getTimeDurationAndStop().milliSeconds();

    /* Context sensitive analysis using call strings.
     */
    {
      analyzer->setOptionContextSensitiveAnalysis(ctOpt.contextSensitive);
      //Call strings length abrivation is not supported yet.
      //CodeThorn::CallString::setMaxLength(_ctOpt.callStringLength);
    }

    /* perform inlining before variable ids are computed, because
     * variables are duplicated by inlining. */
    if(ctOpt.inlineFunctions) {
      InlinerBase* inliner=lowering.getInliner();
      if(RoseInliner* roseInliner=dynamic_cast<CodeThorn::RoseInliner*>(inliner)) {
        roseInliner->inlineDepth=ctOpt.inlineFunctionsDepth;
      }
      inliner->inlineFunctions(sageProject);
      size_t numInlined=inliner->getNumInlinedFunctions();
      SAWYER_MESG(logger[TRACE])<<"inlined "<<numInlined<<" functions"<<endl;
    }

    {
      bool unknownFunctionsFile=ctOpt.externalFunctionsCSVFileName.size()>0;
      bool showProgramStats=ctOpt.programStats;
      bool showProgramStatsOnly=ctOpt.programStatsOnly;
      if(unknownFunctionsFile||showProgramStats||showProgramStatsOnly) {
        ProgramInfo programInfo(sageProject);
        programInfo.compute();
        if(unknownFunctionsFile) {
          ROSE_ASSERT(analyzer);
          programInfo.writeFunctionCallNodesToFile(ctOpt.externalFunctionsCSVFileName);
        }
        if(showProgramStats||showProgramStatsOnly) {
          programInfo.printDetailed();
        }
        if(showProgramStatsOnly) {
          exit(0);
        }
      }
    }

    if(ctOpt.unparse) {
      sageProject->unparse(0,0);
      return 0;
    }

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

    if(ctOpt.status) {
      cout<<"STATUS: analysis started."<<endl;
    }
    // TODO: introduce ProgramAbstractionLayer
    analyzer->initializeVariableIdMapping(sageProject);
    logger[INFO]<<"registered string literals: "<<analyzer->getVariableIdMapping()->numberOfRegisteredStringLiterals()<<endl;

    if(ctOpt.info.printVariableIdMapping) {
      analyzer->getVariableIdMapping()->toStream(cout);
    }
  
    if(ctOpt.info.printTypeSizeMapping||ctOpt.info.typeSizeMappingCSVFileName.size()>0) {
      // from: src/midend/astDiagnostics/AstStatistics.C
      string s=analyzer->typeSizeMappingToString();
      if(ctOpt.info.printTypeSizeMapping) {
        cout<<"Type size mapping:"<<endl;
        cout<<s; // output includes newline at the end
      }
      if(ctOpt.info.typeSizeMappingCSVFileName.size()>0) {
        string fileName=ctOpt.info.typeSizeMappingCSVFileName;
        if(!CppStdUtilities::writeFile(fileName, s)) {
          cerr<<"Error: cannot write type-size mapping to CSV file "<<fileName<<endl;
          exit(1);
        }
      }
    }
    
    if(ctOpt.runRoseAstChecks) {
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
      analyzer->getOptionsRef().explicitArrays=false;
      analyzer->setOptions(ctOpt);
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

    if(ctOpt.rewrite) {
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
      AbstractValueSet varsInAssertConditions=determineVarsInAssertConditions(root,analyzer->getVariableIdMapping());
      SAWYER_MESG(logger[TRACE])<<"STATUS: determined "<<varsInAssertConditions.size()<< " variables in (guarding) assert conditions."<<endl;
      analyzer->setAssertCondVarsSet(varsInAssertConditions);
    }

    if(ctOpt.eliminateCompoundStatements) {
      SAWYER_MESG(logger[TRACE])<<"STATUS: Elimination of compound assignments started."<<endl;
      set<AbstractValue> compoundIncVarsSet=determineSetOfCompoundIncVars(analyzer->getVariableIdMapping(),root);
      analyzer->setCompoundIncVarsSet(compoundIncVarsSet);
      SAWYER_MESG(logger[TRACE])<<"STATUS: determined "<<compoundIncVarsSet.size()<<" compound inc/dec variables before normalization."<<endl;
      rewriteSystem.resetStatistics();
      rewriteSystem.rewriteCompoundAssignmentsInAst(root,analyzer->getVariableIdMapping());
      SAWYER_MESG(logger[TRACE])<<"STATUS: Elimination of compound assignments finished."<<endl;
    }

    if(ctOpt.rers.eliminateArrays) {
      Specialization speci;
      speci.transformArrayProgram(sageProject, analyzer);
      sageProject->unparse(0,0);
      exit(0);
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
    if(ctOpt.info.printFunctionIdMapping) {
      ROSE_ASSERT(analyzer->getCFAnalyzer());
      ROSE_ASSERT(analyzer->getCFAnalyzer()->getFunctionIdMapping());
      analyzer->getCFAnalyzer()->getFunctionIdMapping()->toStream(cout);
    }
    // pattern search: requires that exploration mode is set,
    // otherwise no pattern search is performed
    if(ctOpt.patSearch.explorationMode.size()>0) {
      logger[INFO] << "Pattern search exploration mode was set. Choosing solver 10." << endl;
      analyzer->setSolver(new Solver10());
      analyzer->setStartPState(*analyzer->popWorkList()->pstate());
    }
    double initRunTime=timer.getTimeDurationAndStop().milliSeconds();
    
    timer.start();
    analyzer->printStatusMessageLine("==============================================================");
    if(!analyzer->getModeLTLDriven() && ctOpt.z3BasedReachabilityAnalysis==false && ctOpt.ssa==false) {
      analyzer->runSolver();
    }
    double analysisRunTime=timer.getTimeDurationAndStop().milliSeconds();

    analyzer->printStatusMessageLine("==============================================================");

    if (ctOpt.svcomp.svcompMode && ctOpt.svcomp.witnessFileName.size()>0) {
      analyzer->writeWitnessToFile(ctOpt.svcomp.witnessFileName);
    }

    double extractAssertionTracesTime= 0;
    bool withCe=ltlOpt.withCounterExamples || ltlOpt.withAssertCounterExamples;
    if(withCe) {
      SAWYER_MESG(logger[TRACE]) << "STATUS: extracting assertion traces (this may take some time)"<<endl;
      timer.start();
      analyzer->extractRersIOAssertionTraces();
      extractAssertionTracesTime = timer.getTimeDurationAndStop().milliSeconds();
    }

    double determinePrefixDepthTime= 0; // MJ: Determination of prefix depth currently deactivated.
    int inputSeqLengthCovered = -1;
    double totalInputTracesTime = extractAssertionTracesTime + determinePrefixDepthTime;

    if(ctOpt.status) {
      analyzer->printStatusMessageLine("==============================================================");
      analyzer->reachabilityResults.printResults("YES (REACHABLE)", "NO (UNREACHABLE)", "error_", withCe);
    }
    if (ctOpt.rers.assertResultsOutputFileName.size()>0) {
      analyzer->reachabilityResults.writeFile(ctOpt.rers.assertResultsOutputFileName.c_str(),
                                              false, 0, withCe);
      if(ctOpt.status) {
        cout << "Reachability results written to file \""<<ctOpt.rers.assertResultsOutputFileName<<"\"." <<endl;
        cout << "=============================================================="<<endl;
      }
    }
    // deprecated?
    if(ctOpt.eliminateSTGBackEdges) {
      int numElim=analyzer->getTransitionGraph()->eliminateBackEdges();
      SAWYER_MESG(logger[TRACE])<<"STATUS: eliminated "<<numElim<<" STG back edges."<<endl;
    }

    if(ctOpt.status) {
      analyzer->reachabilityResults.printResultsStatistics();
      analyzer->printStatusMessageLine("==============================================================");
    }

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

    if(ctOpt.ssa) {
      SSAGenerator* ssaGen = new SSAGenerator(analyzer, &logger);
      ssaGen->generateSSAForm();
      exit(0);
    }

    for(auto analysisInfo : ctOpt.analysisList()) {
      AnalysisSelector analysisSel=analysisInfo.first;
      string analysisName=analysisInfo.second;
      if(ctOpt.getAnalysisSelectionFlag(analysisSel)||ctOpt.getAnalysisReportFileName(analysisSel).size()>0) {
        ProgramLocationsReport locations=analyzer->getExprAnalyzer()->getViolatingLocations(analysisSel);
        if(ctOpt.getAnalysisSelectionFlag(analysisSel)) {
          cout<<"\nResults for "<<analysisName<<" analysis:"<<endl;
          if(locations.numTotalLocations()>0) {
            locations.writeResultToStream(cout,analyzer->getLabeler());
          } else {
            cout<<"No violations detected."<<endl;
          }
        }
        if(ctOpt.getAnalysisReportFileName(analysisSel).size()>0) {
          string fileName=ctOpt.getAnalysisReportFileName(analysisSel);
          cout<<"Writing "<<analysisName<<" analysis results to file "<<fileName<<endl;
          locations.writeResultFile(fileName,analyzer->getLabeler());
        }
      }
    }

    if(ctOpt.analyzedFunctionsCSVFileName.size()>0) {
      string fileName=ctOpt.analyzedFunctionsCSVFileName;
      cout<<"Writing list of analyzed functions to file "<<fileName<<endl;
      string s=analyzer->analyzedFunctionsToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        logger[ERROR]<<"Cannot create file "<<fileName<<endl;
      }
    }

    if(ctOpt.analyzedFilesCSVFileName.size()>0) {
      string fileName=ctOpt.analyzedFilesCSVFileName;
      cout<<"Writing list of analyzed files to file "<<fileName<<endl;
      string s=analyzer->analyzedFilesToString();
      if(!CppStdUtilities::writeFile(fileName, s)) {
        logger[ERROR]<<"Cannot create file "<<fileName<<endl;
      }
    }

    if(ctOpt.externalFunctionsCSVFileName.size()>0) {
      string fileName=ctOpt.externalFunctionsCSVFileName;
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
      std::set<int> ltlInAlphabet = analyzer->getInputVarValues();
      //take fixed ltl input alphabet if specified, instead of the input values used for stg computation
      if (ltlOpt.ltlInAlphabet.size()>0) {
        string setstring=ltlOpt.ltlInAlphabet;
        ltlInAlphabet=Parse::integerSet(setstring);
        SAWYER_MESG(logger[TRACE]) << "LTL input alphabet explicitly selected: "<< setstring << endl;
      }
      //take ltl output alphabet if specifically described, otherwise take the old RERS specific 21...26 (a.k.a. oU...oZ)
      std::set<int> ltlOutAlphabet = Parse::integerSet("{21,22,23,24,25,26}");
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
        } else {  // cegpra for single LTL property
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

    // TEST
    if (ctOpt.generateAssertions) {
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

    if(ctOpt.equiCheck.dumpSortedFileName.size()>0 || ctOpt.equiCheck.dumpNonSortedFileName.size()>0) {
      SAR_MODE sarMode=SAR_SSA;
      if(ctOpt.equiCheck.rewriteSSA) {
	sarMode=SAR_SUBSTITUTE;
      }
      Specialization speci;
      ArrayUpdatesSequence arrayUpdates;
      SAWYER_MESG(logger[TRACE]) <<"STATUS: performing array analysis on STG."<<endl;
      SAWYER_MESG(logger[TRACE]) <<"STATUS: identifying array-update operations in STG and transforming them."<<endl;

      bool useRuleConstSubstitution=ctOpt.equiCheck.ruleConstSubst;
      bool useRuleCommutativeSort=ctOpt.equiCheck.ruleCommutativeSort;
      
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

      if(ctOpt.equiCheck.printUpdateInfos) {
        speci.printUpdateInfos(arrayUpdates,analyzer->getVariableIdMapping());
      }
      SAWYER_MESG(logger[TRACE]) <<"STATUS: establishing array-element SSA numbering."<<endl;
      timer.start();
      speci.createSsaNumbering(arrayUpdates, analyzer->getVariableIdMapping());
      arrayUpdateSsaNumberingRunTime=timer.getTimeDurationAndStop().milliSeconds();

      if(ctOpt.equiCheck.dumpNonSortedFileName.size()>0) {
        string filename=ctOpt.equiCheck.dumpNonSortedFileName;
        speci.writeArrayUpdatesToFile(arrayUpdates, filename, sarMode, false);
      }
      if(ctOpt.equiCheck.dumpSortedFileName.size()>0) {
        timer.start();
        string filename=ctOpt.equiCheck.dumpSortedFileName;
        speci.writeArrayUpdatesToFile(arrayUpdates, filename, sarMode, true);
        sortingAndIORunTime=timer.getTimeDurationAndStop().milliSeconds();
      }
      totalRunTime+=arrayUpdateExtractionRunTime+verifyUpdateSequenceRaceConditionRunTime+arrayUpdateSsaNumberingRunTime+sortingAndIORunTime;
    }

    double overallTime =totalRunTime + totalInputTracesTime + totalLtlRunTime;

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
          <<CodeThorn::readableruntime(frontEndRunTime)<<", "
          <<CodeThorn::readableruntime(initRunTime)<<", "
          <<CodeThorn::readableruntime(normalizationRunTime)<<", "
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
          <<normalizationRunTime<<", "
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


    {
      Visualizer visualizer(analyzer->getLabeler(),analyzer->getVariableIdMapping(),analyzer->getFlow(),analyzer->getPStateSet(),analyzer->getEStateSet(),analyzer->getTransitionGraph());
      if (ctOpt.visualization.icfgFileName.size()>0) {
        string cfgFileName=ctOpt.visualization.icfgFileName;
        DataDependenceVisualizer ddvis(analyzer->getLabeler(),analyzer->getVariableIdMapping(),"none");
        ddvis.setDotGraphName("CFG");
        ddvis.generateDotFunctionClusters(root,analyzer->getCFAnalyzer(),cfgFileName,false);
        cout << "generated "<<cfgFileName<<endl;
      }
      if(ctOpt.visualization.viz) {
        cout << "generating graphviz files:"<<endl;
        visualizer.setOptionMemorySubGraphs(ctOpt.visualization.tg1EStateMemorySubgraphs);
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
      if(ctOpt.visualization.vizTg2) {
        string dotFile3=visualizer.foldedTransitionGraphToDot();
        write_file("transitiongraph2.dot", dotFile3);
        cout << "generated transitiongraph2.dot."<<endl;
      }

      if (ctOpt.visualization. dotIOStg.size()>0) {
        string filename=ctOpt.visualization. dotIOStg;
        cout << "generating dot IO graph file:"<<filename<<endl;
        string dotFile="digraph G {\n";
        dotFile+=visualizer.transitionGraphWithIOToDot();
        dotFile+="}\n";
        write_file(filename, dotFile);
        cout << "=============================================================="<<endl;
      }

      if (ctOpt.visualization.dotIOStgForcedTop.size()>0) {
        string filename=ctOpt.visualization.dotIOStgForcedTop;
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
      if(ctOpt.rers.iSeqFile.size()>0) {
        int iseqLen=0;
        if(ctOpt.rers.iSeqLength!=-1) {
          iseqLen=ctOpt.rers.iSeqLength;
        } else {
          logger[ERROR] <<"input-sequence file specified, but no sequence length."<<endl;
          exit(1);
        }
        string fileName=ctOpt.rers.iSeqFile;
        SAWYER_MESG(logger[TRACE]) <<"STATUS: computing input sequences of length "<<iseqLen<<endl;
        IOSequenceGenerator iosgen;
        if(ctOpt.rers.iSeqRandomNum!=-1) {
          int randomNum=ctOpt.rers.iSeqRandomNum;
          SAWYER_MESG(logger[TRACE]) <<"STATUS: reducing input sequence set to "<<randomNum<<" random elements."<<endl;
          iosgen.computeRandomInputPathSet(iseqLen,*analyzer->getTransitionGraph(),randomNum);
        } else {
          iosgen.computeInputPathSet(iseqLen,*analyzer->getTransitionGraph());
        }
        SAWYER_MESG(logger[TRACE]) <<"STATUS: generating input sequence file "<<fileName<<endl;
        iosgen.generateFile(fileName);
      } else {
        if(ctOpt.rers.iSeqLength!=-1) {
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

    if (ctOpt.annotateTerms) {
      // TODO: it might be useful to be able to select certain analysis results to be annotated only
      logger[INFO] << "Annotating term representations."<<endl;
      AstTermRepresentationAttribute::attachAstTermRepresentationAttributes(sageProject);
      AstAnnotator ara(analyzer->getLabeler());
      ara.annotateAstAttributesAsCommentsBeforeStatements(sageProject,"codethorn-term-representation");
    }

    if (ctOpt.annotateTerms||ctOpt.generateAssertions) {
      logger[INFO] << "Generating annotated program."<<endl;
      //backend(sageProject);
      sageProject->unparse(0,0);
    }

    // reset terminal
    if(ctOpt.status)
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

