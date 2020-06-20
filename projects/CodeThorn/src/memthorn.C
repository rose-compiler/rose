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

#include "VxThornCommandLineOptions.h"
#include "RewriteSystem.h"

#include "InternalChecks.h"
#include "AstAnnotator.h"
#include "AstTerm.h"
#include "AbstractValue.h"
#include "AstMatching.h"
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
#include "MemAnalysis.h"

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

const std::string versionString="0.8.0";

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

set<AbstractValue> determineSetOfCompoundIncVars(VariableIdMapping* vim, SgNode* astRoot) {
  ROSE_ASSERT(vim);
  ROSE_ASSERT(astRoot);
  RoseAst ast(astRoot) ;
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

set<VariableId> determineSetOfConstAssignVars2(VariableIdMapping* vim, SgNode* astRoot) {
  ROSE_ASSERT(vim);
  ROSE_ASSERT(astRoot);
  RoseAst ast(astRoot) ;
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

int main( int argc, char * argv[] ) {
  try {
    configureRose();
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

    if(ctOpt.internalChecks) {
      if(CodeThorn::internalChecks(argc,argv)==false) {
        mfacilities.shutdown();
        return 1;
      } else {
        mfacilities.shutdown();
        return 0;
      }
    }

    // Build the AST used by ROSE
    if(ctOpt.status) {
      cout<< "STATUS: Parsing and creating AST started!"<<endl;
    }

    SgProject* astRoot = 0;
    vector<string> argvList(argv,argv+argc);

    if(ctOpt.roseAstReadFileName.size()>0) {
      // add ROSE option as required non-standard single dash long option
      argvList.push_back("-rose:ast:read");
      argvList.push_back(ctOpt.roseAstReadFileName);
    }
    timer.stop();

    timer.start();
    astRoot=frontend(argvList);
    double frontEndRunTime=timer.getTimeDurationAndStop().milliSeconds();

    if(ctOpt.status) {
      cout << "STATUS: Parsing and creating AST finished."<<endl;
    }

    /* perform inlining before variable ids are computed, because
       variables are duplicated by inlining. */
    timer.start();
    Normalization lowering;
    if(ctOpt.normalizeFCalls) {
      lowering.normalizeAst(astRoot,1);
      SAWYER_MESG(logger[TRACE])<<"STATUS: normalized expressions with fcalls (if not a condition)"<<endl;
    }

    if(ctOpt.normalizeAll) {
      if(ctOpt.quiet==false) {
        cout<<"STATUS: normalizing program."<<endl;
      }
      //SAWYER_MESG(logger[INFO])<<"STATUS: normalizing program."<<endl;
      lowering.normalizeAst(astRoot,2);
    }

#if 0
    if(ctOpt.eliminateCompoundStatements) {
      RewriteSystem rewriteSystem;
      SAWYER_MESG(logger[TRACE])<<"STATUS: Elimination of compound assignments started."<<endl;
      set<AbstractValue> compoundIncVarsSet=determineSetOfCompoundIncVars(memAnalysis->getVariableIdMapping(),astRoot);
      SAWYER_MESG(logger[TRACE])<<"STATUS: determined "<<compoundIncVarsSet.size()<<" compound inc/dec variables before normalization."<<endl;
      rewriteSystem.resetStatistics();
      rewriteSystem.rewriteCompoundAssignmentsInAst(astRoot,memAnalysis->getVariableIdMapping());
      SAWYER_MESG(logger[TRACE])<<"STATUS: Elimination of compound assignments finished."<<endl;
    }
#endif
    double normalizationRunTime=timer.getTimeDurationAndStop().milliSeconds();

    if(ctOpt.unparse) {
      astRoot->unparse(0,0);
      return 0;
    }

    if(ctOpt.status) {
      cout<<"STATUS: analysis started."<<endl;
    }

    // if main function exists, start with main-function
    // if a single function exist, use this function
    // in all other cases exit with error.
    RoseAst completeAst(astRoot);
    string startFunction="main";
    
    SgNode* startFunRoot=completeAst.findFunctionByName(startFunction);
    if(startFunRoot==0) {
      // no main function exists. check if a single function exists in the translation unit
      SgProject* project=isSgProject(astRoot);
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

    MemAnalysis* memAnalysis=new MemAnalysis();
    memAnalysis->initialize(astRoot);
    ROSE_ASSERT(memAnalysis->getVariableIdMapping());
    ROSE_ASSERT(memAnalysis->getFunctionIdMapping());
    ROSE_ASSERT(memAnalysis->getFunctionIdMapping());

    memAnalysis->determineExtremalLabels(startFunRoot,false);
    memAnalysis->initializeTransferFunctions();
    memAnalysis->initializeGlobalVariables(astRoot);
    memAnalysis->run();
    
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

