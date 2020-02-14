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

#include "CommandLineOptions.h"
#ifdef USE_SAWYER_COMMANDLINE
#include "Sawyer/CommandLineBoost.h"
#else
#include <boost/program_options.hpp>
#endif

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

// test
#include "SSAGenerator.h"
#include "ReachabilityAnalyzerZ3.h"
#include "DotGraphCfgFrontend.h"
#include "ParProAnalyzer.h"
#include "PromelaCodeGenerator.h"
#include "ParProLtlMiner.h"
#include "ParProExplorer.h"
#include "ParallelAutomataGenerator.h"
#if defined(__unix__) || defined(__unix) || defined(unix)
#include <sys/resource.h>
#endif

#include "CodeThornLib.h"
#include "LTLThornLib.h"
#include "CppStdUtilities.h"

//BOOST includes
#include "boost/lexical_cast.hpp"

using namespace std;

#ifdef USE_SAWYER_COMMANDLINE
namespace po = Sawyer::CommandLine::Boost;
#else
namespace po = boost::program_options;
#endif

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

void CodeThornLanguageRestrictor::initialize() {
  LanguageRestrictorCppSubset1::initialize();
  // RERS 2013 (required for some system headers)
  setAstNodeVariant(V_SgBitOrOp, true);
  setAstNodeVariant(V_SgBitAndOp, true);
  setAstNodeVariant(V_SgBitComplementOp, true);
  setAstNodeVariant(V_SgRshiftOp, true);
  setAstNodeVariant(V_SgLshiftOp, true);
  setAstNodeVariant(V_SgAggregateInitializer, true);
  setAstNodeVariant(V_SgNullExpression, true);
  // Polyhedral test codes and RERS 2015
  setAstNodeVariant(V_SgPlusAssignOp, true);
  setAstNodeVariant(V_SgMinusAssignOp, true);
  setAstNodeVariant(V_SgMultAssignOp, true);
  setAstNodeVariant(V_SgDivAssignOp, true);
  setAstNodeVariant(V_SgPntrArrRefExp, true);
  setAstNodeVariant(V_SgPragmaDeclaration, true);
  setAstNodeVariant(V_SgPragma, true);

  // floating point types
  setAstNodeVariant(V_SgFloatVal, true);
  setAstNodeVariant(V_SgDoubleVal, true);
  setAstNodeVariant(V_SgLongDoubleVal, true);

  // all accepted number types (restricted to int range)
  setAstNodeVariant(V_SgEnumVal, true);
  setAstNodeVariant(V_SgCharVal, true);
  setAstNodeVariant(V_SgUnsignedCharVal, true);
  setAstNodeVariant(V_SgBoolValExp, true);
  setAstNodeVariant(V_SgShortVal, true);
  setAstNodeVariant(V_SgIntVal, true);
  setAstNodeVariant(V_SgLongIntVal, true);
  setAstNodeVariant(V_SgLongLongIntVal, true);
  setAstNodeVariant(V_SgUnsignedShortVal, true);
  setAstNodeVariant(V_SgUnsignedIntVal, true);
  setAstNodeVariant(V_SgUnsignedLongVal, true);
  setAstNodeVariant(V_SgUnsignedLongLongIntVal, true);

  setAstNodeVariant(V_SgComplexVal, true);
  setAstNodeVariant(V_SgNullptrValExp, true);
  setAstNodeVariant(V_SgStringVal, true);

  //more general test codes
  setAstNodeVariant(V_SgPointerDerefExp, true);
  setAstNodeVariant(V_SgNullExpression, true);
  setAstNodeVariant(V_SgSizeOfOp,true);

}

#ifdef POLYOPT_CHECK
typedef std::list<SgFunctionDefinition*,SgFunctionDefinition> PolyOptFunctionPairList;
typedef std::list<SgFunctionDefinition*> FunctionList;
PolyOptFunctionPairList findPolyOptGeneratedFunctions(SgProject* project) {
  PolyOptFunctionPairList funPairlist;
  FunctionList funList;
  std::list<SgFunctionDefinition*> allFunDefs=SgNodeHelper::listOfFunctionDefinitions(project);
  for(auto funDef : allFunDefs) {
    // TODO
  }
  return list;
}
#endif

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

CommandLineOptions& parseCommandLine(int argc, char* argv[], Sawyer::Message::Facility logger) {
  // Command line option handling.
  po::options_description visibleOptions("Supported options");
  po::options_description hiddenOptions("Hidden options");
  po::options_description passOnToRose("Options passed on to ROSE frontend");
  po::options_description cegpraOptions("CEGPRA options");
  po::options_description ltlOptions("LTL options");
  po::options_description svcompOptions("SV-Comp options");
  po::options_description rersOptions("RERS options");
  po::options_description patternSearchOptions("RERS options");
  po::options_description equivalenceCheckingOptions("Equivalence checking options");
  po::options_description parallelProgramOptions("Analysis options for parallel programs");
  po::options_description dataRaceOptions("Data race detection options");
  po::options_description experimentalOptions("Experimental options");
  po::options_description visualizationOptions("Visualization options");
  po::options_description infoOptions("Program information options");

  ltlOptions.add_options()
    ("csv-spot-ltl", po::value< string >(), "Output SPOT's LTL verification results into a CSV file <arg>.")
    ("csv-stats-size-and-ltl",po::value< string >(),"Output statistics regarding the final model size and results for LTL properties into a CSV file <arg>.")
    ("check-ltl", po::value< string >(), "Take a text file of LTL I/O formulae <arg> and check whether or not the analyzed program satisfies these formulae. Formulae should start with '('. Use \"csv-spot-ltl\" option to specify an output csv file for the results.")
    ("single-property", po::value< int >(), "Number (ID) of the property that is supposed to be analyzed. All other LTL properties will be ignored. ( Use \"check-ltl\" option to specify a input property file).")
    ("counterexamples-with-output", po::value< bool >()->default_value(false)->implicit_value(true), "Reported counterexamples for LTL or reachability properties also include output values.")
    ("inf-paths-only", po::value< bool >()->default_value(false)->implicit_value(true), "Recursively prune the transition graph so that only infinite paths remain when checking LTL properties.")
    ("io-reduction", po::value< int >(), "(work in progress) Reduce the transition system to only input/output/worklist states after every <arg> computed EStates.")
    ("keep-error-states",  po::value< bool >()->default_value(false)->implicit_value(true), "Do not reduce error states for the LTL analysis.")      
    ("ltl-in-alphabet",po::value< string >(),"Specify an input alphabet used by the LTL formulae. (e.g. \"{1,2,3}\")")
    ("ltl-out-alphabet",po::value< string >(),"Specify an output alphabet used by the LTL formulae. (e.g. \"{19,20,21,22,23,24,25,26}\")")
    ("ltl-driven", po::value< bool >()->default_value(false)->implicit_value(true), "Select mode to verify LTLs driven by SPOT's access to the state transitions.")
    ("reset-analyzer", po::value< bool >()->default_value(true)->implicit_value(true), "Reset the analyzer and therefore the state transition graph before checking the next property. Only affects ltl-driven mode.")
    ("no-input-input",  po::value< bool >()->default_value(false)->implicit_value(true), "remove transitions where one input states follows another without any output in between. Removal occurs before the LTL check. [yes|=no]")
    ("std-io-only", po::value< bool >()->default_value(false)->implicit_value(true), "Bypass and remove all states that are not standard I/O.")
    ("with-counterexamples", po::value< bool >()->default_value(false)->implicit_value(true), "Add counterexample I/O traces to the analysis results. Applies to reachable assertions and falsified LTL properties (uses RERS-specific alphabet).")
    ("with-assert-counterexamples", po::value< bool >()->default_value(false)->implicit_value(true), "Report counterexamples leading to failing assertion states.")
    ("with-ltl-counterexamples", po::value< bool >()->default_value(false)->implicit_value(true), "Report counterexamples that violate LTL properties.")
    ;

  hiddenOptions.add_options()
    ("max-transitions-forced-top1",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output).")
    ("max-transitions-forced-top2",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output,df).")
    ("max-transitions-forced-top3",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output,df,ptr-vars).")
    ("max-transitions-forced-top4",po::value< int >(),"Performs approximation after <arg> transitions (exact for all but inc-vars).")
    ("max-transitions-forced-top5",po::value< int >(),"Performs approximation after <arg> transitions (exact for input,output,df and vars with 0 to 2 assigned values)).")
    ("solver",po::value< int >()->default_value(5),"Set solver <arg> to use (one of 1,2,3,...).")
    ("relop-constraints", po::value< bool >()->default_value(false)->implicit_value(true),"Flag for the expression analyzer .")
    ;

  passOnToRose.add_options()
    (",I", po::value< vector<string> >(),"Include directories.")
    (",D", po::value< vector<string> >(),"Define constants for preprocessor.")
    (",std", po::value< string >(),"Compilation standard.")
    ("edg:no_warnings", po::bool_switch(),"EDG frontend flag.")
    ;

  cegpraOptions.add_options()
    ("csv-stats-cegpra",po::value< string >(),"Output statistics regarding the counterexample-guided prefix refinement analysis (CEGPRA) into a CSV file <arg>.")
    ("cegpra-ltl",po::value< int >(),"Select the ID of an LTL property that should be checked using cegpra (between 0 and 99).")
    ("cegpra-ltl-all", po::value< bool >()->default_value(false)->implicit_value(true),"Check all specified LTL properties using CEGPRA.")
    ("cegpra-max-iterations",po::value< int >(),"Select a maximum number of counterexamples anaylzed by CEGPRA.")
    ("viz-cegpra-detailed",po::value< string >(),"Generate visualization (.dot) output files with prefix <arg> for different stages within each loop of CEGPRA.")
    ;

  visualizationOptions.add_options()
    ("rw-clusters", po::value< bool >()->default_value(false)->implicit_value(true), "Draw boxes around data elements from the same array (read/write-set graphs).")      
    ("rw-data", po::value< bool >()->default_value(false)->implicit_value(true), "Display names of data elements (read/write-set graphs).") 
    ("rw-highlight-races", po::value< bool >()->default_value(false)->implicit_value(true), "Highlight data races as large red dots (read/write-set graphs).") 
    ("dot-io-stg", po::value< string >(), "Output STG with explicit I/O node information in dot file <arg>.")
    ("dot-io-stg-forced-top", po::value< string >(), "Output STG with explicit I/O node information in dot file <arg>. Groups abstract states together.")
    ("tg1-estate-address", po::value< bool >()->default_value(false)->implicit_value(true), "Transition graph 1: Visualize address.")
    ("tg1-estate-id", po::value< bool >()->default_value(true)->implicit_value(true), "Transition graph 1: Visualize estate-id.")
    ("tg1-estate-properties", po::value< bool >()->default_value(true)->implicit_value(true), "Transition graph 1: Visualize all estate-properties.")
    ("tg1-estate-predicate", po::value< bool >()->default_value(false)->implicit_value(true), "Transition graph 1: Show estate as predicate.")
    ("tg1-estate-memory-subgraphs", po::value< bool >()->default_value(false)->implicit_value(true), "Transition graph 1: Show estate as memory graphs.")
    ("tg2-estate-address", po::value< bool >()->default_value(false)->implicit_value(true), "Transition graph 2: Visualize address.")
    ("tg2-estate-id", po::value< bool >()->default_value(true)->implicit_value(true), "Transition graph 2: Visualize estate-id.")
    ("tg2-estate-properties", po::value< bool >()->default_value(false)->implicit_value(true),"Transition graph 2: Visualize all estate-properties.")
    ("tg2-estate-predicate", po::value< bool >()->default_value(false)->implicit_value(true), "Transition graph 2: Show estate as predicate.")
    ("visualize-read-write-sets", po::value< bool >()->default_value(false)->implicit_value(true), "Generate a read/write-set graph that illustrates the read and write accesses of the involved threads.")
    ("viz", po::value< bool >()->default_value(false)->implicit_value(true),"Generate visualizations of AST, CFG, and transition system as dot files (ast.dot, cfg.dot, transitiongraph1/2.dot.")
    ("viz-tg2", po::value< bool >()->default_value(false)->implicit_value(true),"Generate transition graph 2 (.dot).")
    ("cfg", po::value< string >(), "Generate inter-procedural cfg as dot file. Each function is visualized as one dot cluster.")
    ;

  parallelProgramOptions.add_options()
    ("seed",po::value< int >(),"Seed value for randomly selected integers (concurrency-related non-determinism might still affect results).")
    ("generate-automata",po::value< string >(),"Generate random control flow automata (file <arg>) that can be interpreted and analyzed as a parallel program.")
    ("num-automata",po::value< int >(),"Select the number of parallel automata to generate.")
    ("num-syncs-range",po::value< string >(),"Select a range for the number of random synchronizations between the generated automata (csv pair of integers).")
    ("num-circles-range",po::value< string >(),"Select a range for the number of circles that a randomly generated automaton consists of (csv pair of integers).")
    ("circle-length-range",po::value< string >(),"Select a range for the length of circles that are used to construct an automaton (csv pair of integers).")
    ("num-intersections-range",po::value< string >(),"Select a range for the number of intersections of a newly added circle with existing circles in the automaton (csv pair of integers).")
    ("automata-dot-input",po::value< string >(),"Reads in parallel automata with synchronized transitions from a given .dot file.")
    ("keep-systems", po::value< bool >()->default_value(false)->implicit_value(true),"Store computed parallel systems (over- and under-approximated STGs) during exploration  so that they do not need to be recomputed.")
    ("use-components",po::value< string >(),"Selects which parallel components are chosen for analyzing the (approximated) state space ([all] | subsets-fixed | subsets-random).")
    ("fixed-subsets",po::value< string >(),"A list of sets of parallel component IDs used for analysis (e.g. \"{1,2},{4,7}\"). Use only with \"--use-components=subsets-fixed\".")
    ("num-random-components",po::value< int >(),"Number of different random components used for the analysis. Use only with \"--use-components=subsets-random\". Default: min(3, <num-parallel-components>)")
    ("parallel-composition-only", po::value< bool >()->default_value(false)->implicit_value(true),"If set to \"yes\", then no approximation will take place. Instead, the parallel compositions of the respective sub-systems will be expanded (sequentialized). Skips any LTL analysis. ([yes|no])")
    ("num-components-ltl",po::value< int >(),"Number of different random components used to generate a random LTL property. Default: value of option --num-random-components (a.k.a. all analyzed components)")
    ("minimum-components",po::value< int >(),"Number of different parallel components that need to be explored together in order to be able to analyze the mined properties. (Default: 3).")
    ("different-component-subsets",po::value< int >(),"Number of random component subsets. The solver will be run for each of the random subsets. Use only with \"--use-components=subsets-random\" (Default: no termination).")
    ("ltl-mode",po::value< string >(),"\"check\" checks the properties passed to option \"--check-ltl=<filename>\". \"mine\" searches for automatically generated properties that adhere to certain criteria. \"none\" means no LTL analysis (default).")
    ("mine-num-verifiable",po::value< int >(),"Number of verifiable properties satisfying given requirements that should be collected (Default: 10).")
    ("mine-num-falsifiable",po::value< int >(),"Number of falsifiable properties satisfying given requirements that should be collected (Default: 10).")
    ("minings-per-subsets",po::value< int >(),"Number of randomly generated properties that are evaluated based on one subset of parallel components (Default: 50).")
    ("ltl-properties-output",po::value< string >(),"Writes the analyzed LTL properties to file <arg>.")
    ("promela-output",po::value< string >(),"Writes a promela program reflecting the synchronized automata of option \"--automata-dot-input\" to file <arg>. Includes LTL properties if analyzed.")
    ("promela-output-only", po::value< bool >()->default_value(false)->implicit_value(true),"Only generate Promela code, skip analysis of the input .dot graphs.")
    ("output-with-results", po::value< bool >()->default_value(false)->implicit_value(true),"Include results for the LTL properties in generated promela code and LTL property files .")
    ("output-with-annotations", po::value< bool >()->default_value(false)->implicit_value(true),"Include annotations for the LTL properties in generated promela code and LTL property files .")
    ("verification-engine",po::value< string >(),"Choose which backend verification engine is used (ltsmin|[spot]).")
    ;

  experimentalOptions.add_options()
    ("omp-ast", po::value< bool >()->default_value(false)->implicit_value(true),"Flag for using the OpenMP AST - useful when visualizing the ICFG.")
    ("normalize-all", po::value< bool >()->default_value(false)->implicit_value(true),"Normalize all expressions before analysis.")
    ("normalize-fcalls", po::value< bool >()->default_value(false)->implicit_value(true),"Normalize only expressions with function calls.")
    ("inline", po::value< bool >()->default_value(false)->implicit_value(false),"inline functions before analysis .")
    ("inlinedepth",po::value< int >()->default_value(10),"Default value is 10. A higher value inlines more levels of function calls.")
    ("eliminate-compound-assignments", po::value< bool >()->default_value(true)->implicit_value(true),"Replace all compound-assignments by assignments.")
    ("annotate-terms", po::value< bool >()->default_value(false)->implicit_value(true),"Annotate term representation of expressions in unparsed program.")
    ("eliminate-stg-back-edges", po::value< bool >()->default_value(false)->implicit_value(true), "Eliminate STG back-edges (STG becomes a tree).")
    ("generate-assertions", po::value< bool >()->default_value(false)->implicit_value(true),"Generate assertions (pre-conditions) in program and output program (using ROSE unparser).")
    ("precision-exact-constraints", po::value< bool >()->default_value(false)->implicit_value(true),"Use precise constraint extraction.")
    ("stg-trace-file", po::value< string >(), "Generate STG computation trace and write to file <arg>.")
    ("explicit-arrays", po::value< bool >()->default_value(true)->implicit_value(true),"Represent all arrays explicitly in every state.")
    ("z3", "RERS specific reachability analysis using z3.")	
    ("rers-upper-input-bound", po::value< int >(), "RERS specific parameter for z3.")
    ("rers-verifier-error-number",po::value< int >(), "RERS specific parameter for z3.")
    ("ssa",  po::value< bool >()->default_value(false)->implicit_value(true), "Generate SSA form (only works for programs without function calls, loops, jumps, pointers and returns).")
    ("null-pointer-analysis","Perform null pointer analysis and print results.")
    ("out-of-bounds-analysis","Perform out-of-bounds analysis and print results.")
    ("uninitialized-analysis","Perform uninitialized analysis and print results.")
    ("null-pointer-analysis-file",po::value< string >(),"Perform null pointer analysis and write results to file [arg].")
    ("out-of-bounds-analysis-file",po::value< string >(),"Perform out-of-bounds analysis and write results to file [arg].")
    ("uninitialized-analysis-file",po::value< string >(),"Perform uninitialized analysis and write results to file [arg].")
    ("program-stats-only",po::value< bool >()->default_value(false)->implicit_value(true),"print some basic program statistics about used language constructs and exit.")
    ("program-stats",po::value< bool >()->default_value(false)->implicit_value(true),"print some basic program statistics about used language constructs.")
    ("in-state-string-literals",po::value< bool >()->default_value(false)->implicit_value(true),"create string literals in initial state.")
    ("std-functions",po::value< bool >()->default_value(true)->implicit_value(true),"model std function semantics (malloc, memcpy, etc). Must be turned off explicitly.")
    ("ignore-unknown-functions",po::value< bool >()->default_value(true)->implicit_value(true), "Unknown functions are assumed to be side-effect free.")
    ("ignore-undefined-dereference",po::value< bool >()->default_value(false)->implicit_value(true), "Ignore pointer dereference of uninitalized value (assume data exists).")
    ("ignore-function-pointers",po::value< bool >()->default_value(false)->implicit_value(true), "Ignore function pointers (functions are not called).")
    ("function-resolution-mode",po::value< int >()->default_value(4),"1:Translation unit only, 2:slow lookup, 3: -, 4: complete resolution (including function pointers)")
    ("context-sensitive",po::value< bool >()->default_value(false)->implicit_value(true),"Perform context sensitive analysis. Uses call strings with arbitrary length, recursion is not supported yet.")
    ("abstraction-mode",po::value< int >()->default_value(0),"Select abstraction mode (0: equality merge (explicit model checking), 1: approximating merge (abstract model checking).")
    ("interpretation-mode",po::value< int >()->default_value(0),"Select interpretation mode. 0: default, 1: execute stdout functions.")
    ("interpretation-mode-file",po::value< string >()->default_value(""),"Select interpretation mode output file (otherwise stdout is used).")
    ("print-warnings",po::value< bool >()->default_value(false)->implicit_value(true),"Print warnings on stdout during analysis (this can slow down the analysis significantly)")
    ("print-violations",po::value< bool >()->default_value(false)->implicit_value(true),"Print detected violations on stdout during analysis (this can slow down the analysis significantly)")
    ("options-set",po::value< int >()->default_value(0)->implicit_value(0),"Use a predefined set of default options (0..3).")
//  ("callstring-length",po::value< int >()->default_value(10),"Set the length of the callstring for context-sensitive analysis. Default value is 10.")
    ;

  rersOptions.add_options()
    ("csv-assert", po::value< string >(), "Output assert reachability results into a CSV file <arg>.")
    ("eliminate-arrays", po::value< bool >()->default_value(false)->implicit_value(true), "Transform all arrays into single variables.")
    ("iseq-file", po::value< string >(), "Compute input sequence and generate file <arg>.")
    ("iseq-length", po::value< int >(), "Set length <arg> of input sequence to be computed.")
    ("iseq-random-num", po::value< int >(), "Select random search and number <arg> of paths.")
    ("rers-binary", po::value< bool >()->default_value(false)->implicit_value(true),"Call RERS binary functions in analysis.")
    ("rers-numeric", po::value< bool >()->default_value(false)->implicit_value(true), "Print RERS I/O values as raw numeric numbers.")
    ("rersmode", po::value< bool >()->default_value(false)->implicit_value(true), "Sets several options such that RERS specifics are utilized and observed.")
    ("stderr-like-failed-assert", po::value< bool >()->default_value(false)->implicit_value(true), "Treat output on stderr similar to a failed assert.")
    ;

  svcompOptions.add_options()
    ("svcomp-mode", po::value< bool >()->default_value(false)->implicit_value(true), "Sets default options for all following SVCOMP-specific options.")
    //("external-function-semantics",  "assumes specific semantics for the external functions: __VERIFIER_error, __VERIFIER_nondet_int, exit, memcpy.")
    ("error-function", po::value< string >(), "Detect a verifier error function with name <arg> (terminates verification).")
    ("witness-file", po::value< string >(), "Write an SV-COMP witness (counterexample) to file <arg>.")
    ;

  equivalenceCheckingOptions.add_options()
    ("dump-sorted",po::value< string >(), " (experimental) Generates sorted array updates in file <file>.")
    ("dump-non-sorted",po::value< string >(), " (experimental) Generates non-sorted array updates in file <file>.")
    ("rewrite-ssa", po::value< bool >()->default_value(false)->implicit_value(true), "Rewrite SSA form: Replace use of SSA variable by rhs of its assignment (only applied outside loops or unrolled loops).")
    ("print-rewrite-trace", po::value< bool >()->default_value(false)->implicit_value(true), "Print trace of rewrite rules.")
    ("print-update-infos", po::value< bool >()->default_value(false)->implicit_value(true), "Print information about array updates on stdout.")
    ("rule-const-subst", po::value< bool >()->default_value(true)->implicit_value(true), "Use const-expr substitution rule.")
    ("rule-commutative-sort", po::value< bool >()->default_value(false)->implicit_value(true), "Apply rewrite rule for commutative sort of expression trees.")
    ("max-extracted-updates",po::value< int >()->default_value(5000)->implicit_value(-1),"Set maximum number of extracted updates. This ends the analysis.")
    ("specialize-fun-name", po::value< string >(), "Function of name <arg> to be specialized.")
    ("specialize-fun-param", po::value< vector<int> >(), "Function parameter number to be specialized (starting at 0).")
    ("specialize-fun-const", po::value< vector<int> >(), "Constant <arg>, the param is to be specialized to.")
    ("specialize-fun-varinit", po::value< vector<string> >(), "Variable name of which the initialization is to be specialized (overrides any initializer expression).")
    ("specialize-fun-varinit-const", po::value< vector<int> >(), "Constant <arg>, the variable initialization is to be specialized to.")
#ifdef POLYOPT_CHECK
    ("check-polyopt-variants", po::value< bool >()->default_value(false)->implicit_value(true), "Select equivalence checking for generated polyopt variants (named '*_orig' and '*_transfo'). Function pairs are automatically detected.")
#endif
    ;
  patternSearchOptions.add_options()
    ("pattern-search-max-depth", po::value< int >()->default_value(10), "Maximum input depth that is searched for cyclic I/O patterns.")
    ("pattern-search-repetitions", po::value< int >()->default_value(100), "Number of unrolled iterations of cyclic I/O patterns.")
    ("pattern-search-max-suffix", po::value< int >()->default_value(5), "Maximum input depth of the suffix that is searched for failing assertions after following an I/O-pattern.")
    ("pattern-search-exploration", po::value< string >(), "Exploration mode for the pattern search. Note: all suffixes will always be checked using depth-first search. ([depth-first]|breadth-first)")
    ;

  dataRaceOptions.add_options()
    ("data-race", po::value< bool >()->default_value(false)->implicit_value(true), "Perform data race detection.")
    ("data-race-check-shuffle", po::value< bool >()->default_value(false)->implicit_value(true), "(work in progress) Perform data race detection using the new \"shuffle\" algorithm.")
    ("data-race-csv",po::value<string >(),"Write data race detection results in specified csv file <arg>. Implicitly enables data race detection.")
    ("data-race-fail", po::value< bool >()->default_value(false)->implicit_value(true), "Perform data race detection and fail on error (codethorn exit status 1). For use in regression verification. Implicitly enables data race detection.")
    ;

  visibleOptions.add_options()            
    ("config,c", po::value< string >(), "Use the configuration specified in file <arg>.")
    ("colors", po::value< bool >()->default_value(true)->implicit_value(true),"Use colors in output.")
    ("csv-stats",po::value< string >(),"Output statistics into a CSV file <arg>.")
    ("display-diff",po::value< int >(),"Print statistics every <arg> computed estates.")
    ("exploration-mode",po::value< string >(), "Set mode in which state space is explored. ([breadth-first]|depth-first|loop-aware|loop-aware-sync)")
    ("quiet", "Produce no output on screen.")
    ("help,h", "Produce this help message.")
    ("help-cegpra", "Show options for CEGRPA.")
    ("help-eq", "Show options for program equivalence checking.")
    ("help-exp", "Show options for experimental features.")
    ("help-pat", "Show options for pattern search mode.")
    ("help-svcomp", "Show options for SV-Comp specific features.")
    ("help-rers", "Show options for RERS specific features")
    ("help-ltl", "Show options for LTL verification.")
    ("help-par", "Show options for analyzing parallel programs.")
    ("help-vis", "Show options for visualization output files.")
    ("help-data-race", "Show options for data race detection.")
    ("help-info", "Show options for program info.")
    ("start-function", po::value< string >(), "Name of function to start the analysis from.")
    ("external-function-calls-file",po::value< string >(), "write a list of all function calls to external functions (functions for which no implementation exists) to a CSV file.")
    ("status", po::value< bool >()->default_value(false)->implicit_value(true), "Show status messages.")
    ("reduce-cfg", po::value< bool >()->default_value(true)->implicit_value(true), "Reduce CFG nodes that are irrelevant for the analysis.")
    ("internal-checks", "Run internal consistency checks (without input program).")
    ("cl-args",po::value< string >(),"Specify command line options for the analyzed program (as one quoted string).")
    ("input-values",po::value< string >(),"Specify a set of input values. (e.g. \"{1,2,3}\")")
    ("input-values-as-constraints", po::value< bool >()->default_value(false)->implicit_value(true),"Represent input var values as constraints (otherwise as constants in PState).")
    ("input-sequence",po::value< string >(),"Specify a sequence of input values. (e.g. \"[1,2,3]\")")
    ("log-level",po::value< string >()->default_value("none"),"Set the log level (\"x,>=y\" with x,y in: (none|info|warn|trace|debug)).")
    ("max-transitions",po::value< int >(),"Passes (possibly) incomplete STG to verifier after <arg> transitions have been computed.")
    ("max-iterations",po::value< int >(),"Passes (possibly) incomplete STG to verifier after <arg> loop iterations have been explored. Currently requires --exploration-mode=loop-aware[-sync].")
    ("max-memory",po::value< long int >(),"Stop computing the STG after a total physical memory consumption of approximately <arg> Bytes has been reached.")
    ("max-time",po::value< long int >(),"Stop computing the STG after an analysis time of approximately <arg> seconds has been reached.")
    ("max-transitions-forced-top",po::value< int >(),"Performs approximation after <arg> transitions.")
    ("max-iterations-forced-top",po::value< int >(),"Performs approximation after <arg> loop iterations. Currently requires --exploration-mode=loop-aware[-sync].")
    ("max-memory-forced-top",po::value< long int >(),"Performs approximation after <arg> bytes of physical memory have been used.")
    ("max-time-forced-top",po::value< long int >(),"Performs approximation after an analysis time of approximately <arg> seconds has been reached.")
    ("resource-limit-diff",po::value< int >(),"Check if the resource limit is reached every <arg> computed estates.")
    ("rewrite","Rewrite AST applying all rewrite system rules.")
    ("run-rose-tests", "Run ROSE AST tests.")
    ("threads",po::value< int >(),"(experimental) Run analyzer in parallel using <arg> threads.")
    ("unparse",po::value< bool >()->default_value(false)->implicit_value(true),"unpare code (only relevant for inlining, normalization, and lowering)")
    ("version,v", "Display the version of CodeThorn.")
    ;

  infoOptions.add_options()
    ("print-variable-id-mapping",po::value< bool >()->default_value(false)->implicit_value(true),"Print variable-id-mapping on stdout.")
    ("print-function-id-mapping",po::value< bool >()->default_value(false)->implicit_value(true),"Print function-id-mapping on stdout.")
    ("ast-stats-print",po::value< bool >()->default_value(false)->implicit_value(true),"Print ast node statistics on stdout.")
    ("ast-stats-csv",po::value< string >(),"Write ast node statistics to CSV file [arg].")
    ("type-size-mapping-print",po::value< bool >()->default_value(false)->implicit_value(true),"Print type-size mapping on stdout.")
    ("type-size-mapping-csv",po::value< bool >()->default_value(false)->implicit_value(true),"Write type-size mapping to CSV file [arg].")
    ;

  po::options_description all("All supported options");
  all.add(visibleOptions)
    .add(hiddenOptions)
    .add(passOnToRose)
    .add(cegpraOptions)
    .add(equivalenceCheckingOptions)
    .add(parallelProgramOptions)
    .add(experimentalOptions)
    .add(ltlOptions)
    .add(patternSearchOptions)
    .add(rersOptions)
    .add(svcompOptions)
    .add(dataRaceOptions)
    .add(visualizationOptions)
    .add(infoOptions)
    ;

  po::options_description configOptions("Configuration file options");
  configOptions.add(visibleOptions)
    .add(hiddenOptions)
    .add(cegpraOptions)
    .add(equivalenceCheckingOptions)
    .add(parallelProgramOptions)
    .add(experimentalOptions)
    .add(ltlOptions)
    .add(patternSearchOptions)
    .add(rersOptions)
    .add(svcompOptions)
    .add(dataRaceOptions)
    .add(visualizationOptions)
    .add(infoOptions)
    ;

  po::store(po::command_line_parser(argc, argv).options(all).run(), args);
  po::notify(args);

  if (args.isDefined("config")) {
    ifstream configStream(args.getString("config").c_str());
    po::store(po::parse_config_file(configStream, configOptions), args);
    po::notify(args);
  } 

  if (args.count("help")) {
    cout << visibleOptions << "\n";
    exit(0);
  } else if(args.count("help-cegpra")) {
    cout << cegpraOptions << "\n";
    exit(0);
  } else if(args.count("help-eq")) {
    cout << equivalenceCheckingOptions << "\n";
    exit(0);
  } else if(args.count("help-exp")) {
    cout << experimentalOptions << "\n";
    exit(0);
  } else if(args.count("help-ltl")) {
    cout << ltlOptions << "\n";
    exit(0);
  } else if(args.count("help-par")) {
    cout << parallelProgramOptions << "\n";
    exit(0);
  } else if(args.count("help-pat")) {
    cout << patternSearchOptions << "\n";
    exit(0);
  } else if(args.count("help-rers")) {
    cout << rersOptions << "\n";
    exit(0);
  } else if(args.count("help-svcomp")) {
    cout << svcompOptions << "\n";
    exit(0);
  } else if(args.count("help-vis")) {
    cout << visualizationOptions << "\n";
    exit(0);
  } else if(args.count("help-data-race")) {
    cout << dataRaceOptions << "\n";
    exit(0);
  } else if(args.count("help-info")) {
    cout << infoOptions << "\n";
    exit(0);
  } else if (args.count("version")) {
    cout << "CodeThorn version 1.11.3\n";
    cout << "Written by Markus Schordan, Marc Jasper, Simon Schroder, Maximilan Fecke, Joshua Asplund, Adrian Prantl\n";
    exit(0);
  }

  // Additional checks for options passed on to the ROSE frontend.
  // "-std" is a short option with long name. Check that it still has an argument if used.
  // deactivated  // "-I" should either be followed by a whitespace or by a slash
  for (int i=1; i < argc; ++i) {
    string currentArg(argv[i]);
    if (currentArg == "-std") {
      logger[ERROR] << "Option \"-std\" requires an argument." << endl;
      ROSE_ASSERT(0);
    }
  }

  // Remove all CodeThorn-specific elements of argv (do not confuse ROSE frontend)
  for (int i=1; i < argc; ++i) {
    string currentArg(argv[i]);
    if (currentArg[0] != '-' ){
      continue;  // not an option      
    }
    // explicitly keep options relevant to the ROSE frontend (white list) 
    else if (currentArg == "-I") {
      assert(i+1<argc);
      ++i;
      continue;
    } else if (currentArg == "--edg:no_warnings") {
      continue;
    } else {
      string iPrefix = "-I/";
      string dPrefix = "-D"; // special case, cannot contain separating space
      string stdPrefix = "-std=";
      if(currentArg.substr(0, iPrefix.size()) == iPrefix) {
	continue;
      }
      if(currentArg.substr(0, dPrefix.size()) == dPrefix) {
	continue;
      }
      if(currentArg.substr(0, stdPrefix.size()) == stdPrefix) {
	continue;
      }
    }
    // No match with elements in the white list above. 
    // Must be a CodeThorn option, therefore remove it from argv.
    argv[i] = strdup("");
  }

  return args;
}

void automataDotInput(Sawyer::Message::Facility logger) {
  if (args.count("seed")) {
    srand(args["seed"].as<int>());
  } else {
    srand(time(NULL));
  }
  DotGraphCfgFrontend dotGraphCfgFrontend;
  string filename = args["automata-dot-input"].as<string>();
  CfgsAndAnnotationMap cfgsAndMap = dotGraphCfgFrontend.parseDotCfgs(filename);
  list<Flow> cfgs = cfgsAndMap.first;
  EdgeAnnotationMap edgeAnnotationMap = cfgsAndMap.second;

  string promelaCode;
  if (args.count("promela-output")) {
    cout << "STATUS: generating PROMELA code (parallel processes based on CFG automata)..." << endl;
    PromelaCodeGenerator codeGenerator;
    promelaCode = codeGenerator.generateCode(cfgsAndMap);
    cout << "STATUS: done (LTLs not added yet)." << endl;
  }

  if (args.getBool("viz")) {
    int counter = 0;
    for(list<Flow>::iterator i=cfgs.begin(); i!=cfgs.end(); i++) {
      Flow cfg = *i;
      cfg.setDotOptionDisplayLabel(false);
      cfg.setDotOptionDisplayStmt(false);
      cfg.setDotOptionEdgeAnnotationsOnly(true);
      string outputFilename = "parallelComponentCfg_" + boost::lexical_cast<string>(counter) + ".dot";
      write_file(outputFilename, cfg.toDot(NULL));
      cout << "generated " << outputFilename <<"."<<endl;
      counter++;
    }
  }

  vector<Flow*> cfgsAsVector(cfgs.size());
  int index = 0;
  for (list<Flow>::iterator i=cfgs.begin(); i!=cfgs.end(); ++i) {
    cfgsAsVector[index] = &(*i);
    ++index;
  }

  ParProExplorer explorer(cfgsAsVector, edgeAnnotationMap);
  if (args.count("verification-engine")) {
    string verificationEngine = args["verification-engine"].as<string>();
    if (verificationEngine == "ltsmin") {
      explorer.setUseLtsMin(true);
    }
  } 
  if (args.getBool("keep-systems")) {
    explorer.setStoreComputedSystems(true);
  } else {
    explorer.setStoreComputedSystems(false);
  }
  if (args.getBool("parallel-composition-only")) {
    explorer.setParallelCompositionOnly(true);
  } else {
    explorer.setStoreComputedSystems(false);
  }
  if (args.count("use-components")) {
    string componentSelection = args["use-components"].as<string>();
    if (componentSelection == "all") {
      explorer.setComponentSelection(PAR_PRO_COMPONENTS_ALL);
      if (args.count("ltl-mode")) {
	string ltlMode= args["ltl-mode"].as<string>();
	if (ltlMode == "mine") {
	  explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
	}
      }
    } else if (componentSelection == "subsets-fixed") {
      explorer.setComponentSelection(PAR_PRO_COMPONENTS_SUBSET_FIXED);
      explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_FINITE);
      if (args.count("fixed-subsets")) {
        string setsstring=args["fixed-subsets"].as<string>();
        list<set<int> > intSets=Parse::integerSetList(setsstring);
        explorer.setFixedComponentSubsets(intSets);
      } else {
        logger[ERROR] << "selected a fixed set of components but no were selected. Please use option \"--fixed-subsets=<csv-id-list>\".";
        ROSE_ASSERT(0);
      }
    } else if (componentSelection == "subsets-random") {
      explorer.setComponentSelection(PAR_PRO_COMPONENTS_SUBSET_RANDOM);
      if (args.count("num-random-components")) {
        explorer.setNumberRandomComponents(args["num-random-components"].as<int>());
      } else {
        explorer.setNumberRandomComponents(std::min(3, (int) cfgsAsVector.size()));
      }
      if (args.count("different-component-subsets")) {
        explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_FINITE);
        explorer.setNumberDifferentComponentSubsets(args["different-component-subsets"].as<int>());
      } else {
        explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
      }
    }
  } else {
    explorer.setComponentSelection(PAR_PRO_COMPONENTS_ALL);
    if (args.count("ltl-mode")) {
      string ltlMode= args["ltl-mode"].as<string>();
      if (ltlMode == "mine") {
	explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
      }
    }
  }

  if ( args.count("check-ltl") ) {
    explorer.setLtlMode(PAR_PRO_LTL_MODE_CHECK);
    explorer.setLtlInputFilename(args["check-ltl"].as<string>());
  } else {
    if ( args.count("ltl-mode") ) {
      string ltlMode= args["ltl-mode"].as<string>();
      if (ltlMode == "check") {
        logger[ERROR] << "ltl mode \"check\" selected but option \"--check-ltl=<filename>\" not used. Please provide LTL property file." << endl;
        ROSE_ASSERT(0);
      } else if (ltlMode == "mine") {
        explorer.setLtlMode(PAR_PRO_LTL_MODE_MINE);
        if (args.count("num-components-ltl")) {
          explorer.setNumberOfComponentsForLtlAnnotations(args["num-components-ltl"].as<int>());
        } else {
          explorer.setNumberOfComponentsForLtlAnnotations(std::min(3, (int) cfgsAsVector.size()));
        }
        if (args.count("minimum-components")) {
          explorer.setMinNumComponents(args["minimum-components"].as<int>());
        }
        if (args.count("mine-num-verifiable")) {
          explorer.setNumRequiredVerifiable(args["mine-num-verifiable"].as<int>());
        } else {
          explorer.setNumRequiredVerifiable(10);
        }
        if (args.count("mine-num-falsifiable")) {
          explorer.setNumRequiredFalsifiable(args["mine-num-falsifiable"].as<int>());
        } else {
          explorer.setNumRequiredFalsifiable(10);
        }
        if (args.count("minings-per-subsets")) {
          explorer.setNumMiningsPerSubset(args["minings-per-subsets"].as<int>());
        } else {
          explorer.setNumMiningsPerSubset(50);
        }
      } else if (ltlMode == "none") {
        explorer.setLtlMode(PAR_PRO_LTL_MODE_NONE);
      }
    } else {
      explorer.setLtlMode(PAR_PRO_LTL_MODE_NONE);
    }
  }

  if (args.getBool("viz")) {
    explorer.setVisualize(true);
  }

  if (!args.getBool("promela-output-only")) {
    explorer.explore();
  }
  
  if (args.count("check-ltl")) {
    PropertyValueTable* ltlResults=nullptr;
    if (args.getBool("promela-output-only")) { // just read the properties into a PropertyValueTable
      SpotConnection spotConnection(args["check-ltl"].as<string>());
      ltlResults = spotConnection.getLtlResults();
    } else {
      ltlResults = explorer.propertyValueTable();
    }
    bool withCounterexamples = false;
    ltlResults-> printResults("YES (verified)", "NO (falsified)", "ltl_property_", withCounterexamples);
    cout << "=============================================================="<<endl;
    ltlResults->printResultsStatistics();
    cout << "=============================================================="<<endl;
  }

  bool withResults = args.getBool("output-with-results");
  bool withAnnotations = args.getBool("output-with-annotations");
#ifdef HAVE_SPOT
  if (args.count("promela-output")) {
    PropertyValueTable* ltlResults;
    if (args.getBool("promela-output-only")) { // just read the properties into a PropertyValueTable
      SpotConnection spotConnection(args["check-ltl"].as<string>());
      ltlResults = spotConnection.getLtlResults();
    } else {
      ltlResults = explorer.propertyValueTable();
    }
    // uses SpotMiscellaneous::spinSyntax as callback to avoid static dependency of ltlResults on SpotMisc.
    string promelaLtlFormulae = ltlResults->getLtlsAsPromelaCode(withResults, withAnnotations,&SpotMiscellaneous::spinSyntax);
    promelaCode += "\n" + promelaLtlFormulae;
    string filename = args["promela-output"].as<string>();
    write_file(filename, promelaCode);
    cout << "generated " << filename  <<"."<<endl;
  }
#endif
  if (args.count("ltl-properties-output")) {
    string ltlFormulae = explorer.propertyValueTable()->getLtlsRersFormat(withResults, withAnnotations);
    string filename = args["ltl-properties-output"].as<string>();
    write_file(filename, ltlFormulae);
    cout << "generated " << filename  <<"."<<endl;
  }
  if(!args.count("quiet"))
    cout << "STATUS: done." << endl;
}

void generateAutomata() {
  if (args.count("seed")) {
    srand(args["seed"].as<int>());
  } else {
    srand(time(NULL));
  }
  ParallelAutomataGenerator automataGenerator;
  int numberOfAutomata = 10;
  if (args.count("num-automata")) {
    numberOfAutomata = args["num-automata"].as<int>();
  }
  pair<int, int> numberOfSyncsRange = pair<int, int>(9, 18);
  if (args.count("num-syncs-range")) {
    numberOfSyncsRange = parseCsvIntPair(args["num-syncs-range"].as<string>());
  }
  pair<int, int> numberOfCirclesPerAutomatonRange = pair<int, int>(2, 4);
  if (args.count("num-circles-range")) {
    numberOfCirclesPerAutomatonRange = parseCsvIntPair(args["num-circles-range"].as<string>());
  }
  pair<int, int> circleLengthRange = pair<int, int>(5, 8);
  if (args.count("circle-length-range")) {
    circleLengthRange = parseCsvIntPair(args["circle-length-range"].as<string>());
  }
  pair<int, int> numIntersectionsOtherCirclesRange = pair<int, int>(1, 2);
  if (args.count("num-intersections-range")) {
    numIntersectionsOtherCirclesRange = parseCsvIntPair(args["num-intersections-range"].as<string>());
  }
  vector<Flow*> automata = automataGenerator.randomlySyncedCircleAutomata(
      numberOfAutomata,
      numberOfSyncsRange,
      numberOfCirclesPerAutomatonRange,
      circleLengthRange,
      numIntersectionsOtherCirclesRange);
  Visualizer visualizer;
  string dotCfas = visualizer.cfasToDotSubgraphs(automata);
  string outputFilename = args["generate-automata"].as<string>();
  write_file(outputFilename, dotCfas);
  cout << "generated " << outputFilename <<"."<<endl;
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

  if(args.count("stg-trace-file")) {
    analyzer->setStgTraceFileName(args["stg-trace-file"].as<string>());
  }

  if (args.isDefined("cl-args")) {
    string clOptions=args.getString("cl-args");
    vector<string> clOptionsVector=Parse::commandLineArgs(clOptions);
    analyzer->setCommandLineOptions(clOptionsVector);
  }

  if(args.count("input-values")) {
    string setstring=args["input-values"].as<string>();
    cout << "STATUS: input-values="<<setstring<<endl;

    set<int> intSet=Parse::integerSet(setstring);
    for(set<int>::iterator i=intSet.begin();i!=intSet.end();++i) {
      analyzer->insertInputVarValue(*i);
    }
  }

  if(args.count("input-sequence")) {
    string liststring=args["input-sequence"].as<string>();
    cout << "STATUS: input-sequence="<<liststring<<endl;

    list<int> intList=Parse::integerList(liststring);
    for(list<int>::iterator i=intList.begin();i!=intList.end();++i) {
      analyzer->addInputSequenceValue(*i);
    }
  }

  if(args.count("exploration-mode")) {
    string explorationMode=args["exploration-mode"].as<string>();
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

  if (args.count("max-iterations") || args.count("max-iterations-forced-top")) {
    bool notSupported=false;
    if (!args.count("exploration-mode")) {
      notSupported=true;
    } else {
      string explorationMode=args["exploration-mode"].as<string>();
      if(explorationMode!="loop-aware" && explorationMode!="loop-aware-sync") {
        notSupported=true;
      }
    }
    if(notSupported) {
      cout << "Error: \"max-iterations[-forced-top]\" modes currently require \"--exploration-mode=loop-aware[-sync]\"." << endl;
      exit(1);
    }
  }

  if(args.count("abstraction-mode")) {
    analyzer->setAbstractionMode(args.getInt("abstraction-mode"));
  }

  if(args.count("max-transitions")) {
    analyzer->setMaxTransitions(args.getInt("max-transitions"));
  }

  if(args.count("max-iterations")) {
    analyzer->setMaxIterations(args.getInt("max-iterations"));
  }

  if(args.count("max-iterations-forced-top")) {
    analyzer->setMaxIterationsForcedTop(args["max-iterations-forced-top"].as<int>());
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  }

  if(args.count("max-transitions-forced-top")) {
    analyzer->setMaxTransitionsForcedTop(args["max-transitions-forced-top"].as<int>());
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  } else if(args.count("max-transitions-forced-top1")) {
    analyzer->setMaxTransitionsForcedTop(args["max-transitions-forced-top1"].as<int>());
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IO);
  } else if(args.count("max-transitions-forced-top2")) {
    analyzer->setMaxTransitionsForcedTop(args["max-transitions-forced-top2"].as<int>());
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IOCF);
  } else if(args.count("max-transitions-forced-top3")) {
    analyzer->setMaxTransitionsForcedTop(args["max-transitions-forced-top3"].as<int>());
    analyzer->setGlobalTopifyMode(Analyzer::GTM_IOCFPTR);
  } else if(args.count("max-transitions-forced-top4")) {
    analyzer->setMaxTransitionsForcedTop(args["max-transitions-forced-top4"].as<int>());
    analyzer->setGlobalTopifyMode(Analyzer::GTM_COMPOUNDASSIGN);
  } else if(args.count("max-transitions-forced-top5")) {
    analyzer->setMaxTransitionsForcedTop(args["max-transitions-forced-top5"].as<int>());
    analyzer->setGlobalTopifyMode(Analyzer::GTM_FLAGS);
  }

  if (args.count("max-memory")) {
    analyzer->setMaxBytes(args["max-memory"].as<long int>());
  }
  if (args.count("max-time")) {
    analyzer->setMaxSeconds(args["max-time"].as<long int>());
  }
  if (args.count("max-memory-forced-top")) {
    analyzer->setMaxBytesForcedTop(args["max-memory-forced-top"].as<long int>());
  }
  if (args.count("max-time-forced-top")) {
    analyzer->setMaxSecondsForcedTop(args["max-time-forced-top"].as<long int>());
  }

  if(args.count("display-diff")) {
    int displayDiff=args["display-diff"].as<int>();
    analyzer->setDisplayDiff(displayDiff);
  }
  if(args.count("resource-limit-diff")) {
    int resourceLimitDiff=args["resource-limit-diff"].as<int>();
    analyzer->setResourceLimitDiff(resourceLimitDiff);
  }

  Solver* solver = nullptr;
  // overwrite solver ID based on other options
  if(analyzer->getModeLTLDriven()) {
    args.setOption("solver", 11);
  }
  ROSE_ASSERT(args.count("solver")); // Options should contain a default solver
  int solverId=args["solver"].as<int>();
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
  ROSE_INITIALIZE;
  CodeThorn::initDiagnostics();
  CodeThorn::initDiagnosticsLTL();

  Rose::Diagnostics::mprefix->showProgramName(false);
  Rose::Diagnostics::mprefix->showThreadId(false);
  Rose::Diagnostics::mprefix->showElapsedTime(false);

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

  try {
    TimeMeasurement timer;
    timer.start();

    parseCommandLine(argc, argv, logger);

    // Check if chosen options are available
#ifndef HAVE_SPOT
    // display error message and exit in case SPOT is not avaiable, but related options are selected
    if (args.count("csv-stats-cegpra") ||
	args.isDefined("cegpra-ltl") ||
	args.getBool("cegpra-ltl-all") ||
	args.count("cegpra-max-iterations") ||
	args.count("viz-cegpra-detailed") ||
	args.count("csv-spot-ltl") ||
	args.count("check-ltl") ||
	args.count("single-property") ||
	args.count("ltl-in-alphabet") ||
	args.count("ltl-out-alphabet") ||
	args.getBool("ltl-driven") ||
	args.getBool("with-ltl-counterexamples") ||
	args.count("mine-num-verifiable") ||
	args.count("mine-num-falsifiable") ||
	args.count("ltl-mode") ||
	args.count("ltl-properties-output") ||
	args.count("promela-output") ||
	args.getBool("promela-output-only") ||
	args.getBool("output-with-results") ||
	args.getBool("output-with-annotations")) {
      cerr << "Error: Options selected that require the SPOT library, however SPOT was not selected during configuration." << endl;
      exit(1);
    }
#endif

#ifndef HAVE_Z3
    if (args.count("z3") ||
	args.count("rers-upper-input-bound") ||
	args.count("rers-verifier-error-number")){
      cerr << "Error: Options selected that require the Z3 library, however Z3 was not selected during configuration." << endl;
      exit(1);
    }
#endif	

    // Start execution
    mfacilities.control(args["log-level"].as<string>());
    SAWYER_MESG(logger[TRACE]) << "Log level is " << args["log-level"].as<string>() << endl;

    if (args.count("generate-automata")) {
      generateAutomata();
      exit(0);
    }

    if (args.count("automata-dot-input")) {
      automataDotInput(logger);
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
    if (args.count("limit-to-fragment")) {
      option_pragma_name = args["limit-to-fragment"].as<string>();
    }
#endif

    if (args.count("internal-checks")) {
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
    analyzer->setSkipSelectedFunctionCalls(args.getBool("ignore-unknown-functions"));
    analyzer->setIgnoreFunctionPointers(args.getBool("ignore-function-pointers"));
    analyzer->setStdFunctionSemantics(args.getBool("std-functions"));

    analyzerSetup(analyzer, logger);

    switch(int mode=args.getInt("interpretation-mode")) {
    case 0: analyzer->setInterpretationMode(IM_ABSTRACT); break;
    case 1: analyzer->setInterpretationMode(IM_CONCRETE); break;
    default:
      cerr<<"Unknown interpretation mode "<<mode<<" provided on command line (supported: 0..1)."<<endl;
      exit(1);
    }
    string outFileName=args.getString("interpretation-mode-file");
    if(outFileName!="") {
      analyzer->setInterpretationModeOutputFileName(outFileName);
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

    if(args.count("threads")) {
      int numThreads=args["threads"].as<int>();
      if(numThreads<=0) {
        cerr<<"Error: number of threads must be greater or equal 1."<<endl;
        exit(1);
      }
      analyzer->setNumberOfThreadsToUse(numThreads);
    } else {
      analyzer->setNumberOfThreadsToUse(1);
    }

    string option_start_function="main";
    if(args.count("start-function")) {
      option_start_function = args["start-function"].as<string>();
    }

    string option_specialize_fun_name="";
    vector<int> option_specialize_fun_param_list;
    vector<int> option_specialize_fun_const_list;
    vector<string> option_specialize_fun_varinit_list;
    vector<int> option_specialize_fun_varinit_const_list;
    if(args.count("specialize-fun-name")) {
      option_specialize_fun_name = args["specialize-fun-name"].as<string>();
      // logger[DEBUG] << "option_specialize_fun_name: "<< option_specialize_fun_name<<endl;
    } else {
      // logger[DEBUG] << "option_specialize_fun_name: NONE"<< option_specialize_fun_name<<endl;
    }

    if(args.count("specialize-fun-param")) {
      option_specialize_fun_param_list=args["specialize-fun-param"].as< vector<int> >();
      option_specialize_fun_const_list=args["specialize-fun-const"].as< vector<int> >();
    }

    if(args.count("specialize-fun-varinit")) {
      option_specialize_fun_varinit_list=args["specialize-fun-varinit"].as< vector<string> >();
      option_specialize_fun_varinit_const_list=args["specialize-fun-varinit-const"].as< vector<int> >();
    }

    // logger[DEBUG] << "specialize-params:"<<option_specialize_fun_const_list.size()<<endl;

    if(args.count("specialize-fun-name")) {
      if((args.count("specialize-fun-param")||args.count("specialize-fun-const"))
          && !(args.count("specialize-fun-name")&&args.count("specialize-fun-param")&&args.count("specialize-fun-param"))) {
        logger[ERROR] <<"options --specialize-fun-name=NAME --specialize-fun-param=NUM --specialize-fun-const=NUM must be used together."<<endl;
        exit(1);
      }
      if((args.count("specialize-fun-varinit")||args.count("specialize-fun-varinit-const"))
          && !(args.count("specialize-fun-varinit")&&args.count("specialize-fun-varinit-const"))) {
        logger[ERROR] <<"options --specialize-fun-name=NAME --specialize-fun-varinit=NAME --specialize-fun-const=NUM must be used together."<<endl;
        exit(1);
      }
    }

    if((args.getBool("print-update-infos")||args.count("equivalence-check"))&&(args.count("dump-sorted")==0 && args.count("dump-non-sorted")==0)) {
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
    if(args.count("dump-sorted")>0 || args.count("dump-non-sorted")>0 || args.count("equivalence-check")>0) {
      analyzer->setSkipSelectedFunctionCalls(true);
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

    if(args.count("external-function-semantics")) {
      // obsolete
    }

    if(args.count("error-function")) {
      string errorFunctionName=args["error-function"].as<string>();
      analyzer->setExternalErrorFunctionName(errorFunctionName);
    }

    analyzer->setTreatStdErrLikeFailedAssert(args.getBool("stderr-like-failed-assert"));

    // Build the AST used by ROSE
    if(!args.count("quiet")) {
      cout<< "STATUS: Parsing and creating AST started."<<endl;
    }
    timer.stop();
    timer.start();

    SgProject* sageProject = 0;
    if(args.getBool("omp-ast")||args.getBool("data-race")) {
      vector<string> argvList(argv,argv+argc);
      SAWYER_MESG(logger[TRACE])<<"selected OpenMP AST."<<endl;
      argvList.push_back("-rose:OpenMP:ast_only");
      sageProject=frontend(argvList);
    } else {
      sageProject=frontend(argc,argv,true);
    }
    
    if(!args.count("quiet")) {
      cout << "STATUS: Parsing and creating AST finished."<<endl;
    }
    double frontEndRunTime=timer.getTimeDuration().milliSeconds();

    /* perform inlining before variable ids are computed, because
       variables are duplicated by inlining. */
    Normalization lowering;
    if(args.getBool("normalize-fcalls")) {
      lowering.normalizeAst(sageProject,1);
      SAWYER_MESG(logger[TRACE])<<"STATUS: normalized expressions with fcalls (if not a condition)"<<endl;
    }

    if(args.getBool("normalize-all")||args.getInt("options-set")==1) {
      if(!args.count("quiet")) {
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

    if(!args.count("quiet")) {
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
      TypeSizeMapping* tsm=analyzer->getTypeSizeMapping();
      string s=tsm->toString();
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
    
    if(args.count("run-rose-tests")) {
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
      analyzer->setSkipSelectedFunctionCalls(true);
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

    if(args.count("rewrite")) {
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
    SAWYER_MESG(logger[TRACE])<< "INIT: Checking input program."<<endl;
    CodeThornLanguageRestrictor lr;
    //lr.checkProgram(root);
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

    double initRunTime=timer.getTimeDuration().milliSeconds();

    timer.start();
    analyzer->printStatusMessageLine("==============================================================");
    if(!analyzer->getModeLTLDriven() && args.count("z3") == 0 && !args.getBool("ssa")) {
      analyzer->runSolver();
    }
    double analysisRunTime=timer.getTimeDuration().milliSeconds();
    analyzer->printStatusMessageLine("==============================================================");

    if (args.getBool("svcomp-mode") && args.isDefined("witness-file")) {
      analyzer->writeWitnessToFile(args.getString("witness-file"));
    }

    double extractAssertionTracesTime= 0;
    if ( args.getBool("with-counterexamples") || args.getBool("with-assert-counterexamples")) {
      SAWYER_MESG(logger[TRACE]) << "STATUS: extracting assertion traces (this may take some time)"<<endl;
      timer.start();
      analyzer->extractRersIOAssertionTraces();
      extractAssertionTracesTime = timer.getTimeDuration().milliSeconds();
    }

    double determinePrefixDepthTime= 0; // MJ: Determination of prefix depth currently deactivated.
    int inputSeqLengthCovered = -1;
    double totalInputTracesTime = extractAssertionTracesTime + determinePrefixDepthTime;

    bool withCe = args.getBool("with-counterexamples") || args.getBool("with-assert-counterexamples");
    if(args.getBool("status")) {
      analyzer->printStatusMessageLine("==============================================================");
      analyzer->reachabilityResults.printResults("YES (REACHABLE)", "NO (UNREACHABLE)", "error_", withCe);
    }
    if (args.count("csv-assert")) {
      string filename=args["csv-assert"].as<string>().c_str();
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
    if(args.count("z3"))
    {
	assert(args.count("rers-upper-input-bound") != 0 &&  args.count("rers-verifier-error-number") != 0);	
	int RERSUpperBoundForInput = args["rers-upper-input-bound"].as<int>();
	int RERSVerifierErrorNumber = args["rers-verifier-error-number"].as<int>();
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
      if(args.count(analysisOption)>0||args.isDefined(analysisOutputFileOption)) {
        ProgramLocationsReport locations=analyzer->getExprAnalyzer()->getViolatingLocations(analysisSel);
        if(args.count(analysisOption)>0) {
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
      infPathsOnlyTime = timer.getTimeDuration().milliSeconds();
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
      stdIoOnlyTime = timer.getTimeDuration().milliSeconds();
    }

    long eStateSetSizeIoOnly = 0;
    long transitionGraphSizeIoOnly = 0;
    double spotLtlAnalysisTime = 0;

    stringstream statisticsSizeAndLtl;
    stringstream statisticsCegpra;

    if (args.count("check-ltl")) {
      logger[INFO] <<"STG size: "<<analyzer->getTransitionGraph()->size()<<endl;
      string ltl_filename = args["check-ltl"].as<string>();
      if(args.getBool("rersmode")) {  //reduce the graph accordingly, if not already done
        if (!args.getBool("inf-paths-only") && !args.getBool("keep-error-states") &&!analyzer->getModeLTLDriven()) {
          cout<< "STATUS: recursively removing all leaves (due to RERS-mode (2))."<<endl;
          timer.start();
          analyzer->pruneLeaves();
          infPathsOnlyTime = timer.getTimeDuration().milliSeconds();

          pstateSetSizeInf=analyzer->getPStateSet()->size();
          eStateSetSizeInf = analyzer->getEStateSet()->size();
          transitionGraphSizeInf = analyzer->getTransitionGraph()->size();
          eStateSetSizeStgInf = (analyzer->getTransitionGraph())->estateSet().size();
        }
        if (!args.getBool("std-io-only") &&!analyzer->getModeLTLDriven()) {
          cout << "STATUS: bypassing all non standard I/O states (due to RERS-mode) (P1)."<<endl;
          timer.start();
          printStgSize(analyzer->getTransitionGraph(), "before reducing non-I/O states");
          if (args.getBool("keep-error-states")) {
            analyzer->reduceStgToInOutAssertStates();
          } else {
            analyzer->reduceStgToInOutStates();
          }
          stdIoOnlyTime = timer.getTimeDuration().milliSeconds();
          printStgSize(analyzer->getTransitionGraph(), "after reducing non-I/O states");
        }
      }
      if(args.getBool("no-input-input")) {  //delete transitions that indicate two input states without an output in between
        analyzer->removeInputInputTransitions();
        printStgSize(analyzer->getTransitionGraph(), "after reducing input->input transitions");
      }
      bool withCounterexample = false;
      if(args.getBool("with-counterexamples") || args.getBool("with-ltl-counterexamples")) {  //output a counter-example input sequence for falsified formulae
        withCounterexample = true;
      }

      timer.start();
      std::set<int> ltlInAlphabet = analyzer->getInputVarValues();
      //take fixed ltl input alphabet if specified, instead of the input values used for stg computation
      if (args.count("ltl-in-alphabet")) {
        string setstring=args["ltl-in-alphabet"].as<string>();
        ltlInAlphabet=Parse::integerSet(setstring);
        SAWYER_MESG(logger[TRACE]) << "LTL input alphabet explicitly selected: "<< setstring << endl;
      }
      //take ltl output alphabet if specifically described, otherwise take the old RERS specific 21...26 (a.k.a. oU...oZ)
      std::set<int> ltlOutAlphabet = Parse::integerSet("{21,22,23,24,25,26}");
      if (args.count("ltl-out-alphabet")) {
        string setstring=args["ltl-out-alphabet"].as<string>();
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
      if (args.count("single-property")) {
	int propertyNum = args["single-property"].as<int>();
	spotConnection.checkSingleProperty(propertyNum, *(analyzer->getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample, spuriousNoAnswers);
      } else {
	spotConnection.checkLtlProperties( *(analyzer->getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample, spuriousNoAnswers);
      }
      spotLtlAnalysisTime=timer.getTimeDuration().milliSeconds();
      SAWYER_MESG(logger[TRACE]) << "LTL: get results from spot connection."<<endl;
      ltlResults = spotConnection.getLtlResults();
      SAWYER_MESG(logger[TRACE]) << "LTL: results computed."<<endl;

      if (args.isDefined("cegpra-ltl") || (args.isDefined("cegpra-ltl-all")&&args.getBool("cegpra-ltl-all"))) {
        if (args.count("csv-stats-cegpra")) {
          statisticsCegpra << "init,";
          printStgSize(analyzer->getTransitionGraph(), "initial abstract model", &statisticsCegpra);
          statisticsCegpra << ",na,na";
          statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_YES);
          statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_NO);
          statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_UNKNOWN);
        }
        CounterexampleAnalyzer ceAnalyzer(analyzer, &statisticsCegpra);
        if (args.count("cegpra-max-iterations")) {
          ceAnalyzer.setMaxCounterexamples(args["cegpra-max-iterations"].as<int>());
        }
        if (args.getBool("cegpra-ltl-all")) {
          ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(spotConnection, ltlInAlphabet, ltlOutAlphabet);
        } else {  // cegpra for single LTL property
          int property = args["cegpra-ltl"].as<int>();
          ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(property, spotConnection, ltlInAlphabet, ltlOutAlphabet);
        }
      }

      if(args.getBool("status")) {
        ltlResults-> printResults("YES (verified)", "NO (falsified)", "ltl_property_", withCounterexample);
        analyzer->printStatusMessageLine("==============================================================");
        ltlResults->printResultsStatistics();
        analyzer->printStatusMessageLine("==============================================================");
      }
      if (args.count("csv-spot-ltl")) {  //write results to a file instead of displaying them directly
        std::string csv_filename = args["csv-spot-ltl"].as<string>();
        SAWYER_MESG(logger[TRACE]) << "STATUS: writing ltl results to file: " << csv_filename << endl;
        ltlResults->writeFile(csv_filename.c_str(), false, 0, withCounterexample);
      }
      if (args.count("csv-stats-size-and-ltl")) {
        printStgSize(analyzer->getTransitionGraph(), "final model", &statisticsSizeAndLtl);
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

#ifdef POLYOPT_CHECK
    if(args.getBool("check-polyopt-variants")) {
      cout<<"STATUS: checking polyopt variants."<<endl;
      analyzer->setSkipSelectedFunctionCalls(true);
      analyzer->setSkipArrayAccesses(true);
      args.setOption("explicit-arrays",false);
      PolyOptFunctionPairList list=findPolyOptGeneratedFunctions();
    }
#endif
    if(args.count("dump-sorted")>0 || args.count("dump-non-sorted")>0) {
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
      arrayUpdateExtractionRunTime=timer.getTimeDuration().milliSeconds();

      if(args.getBool("print-update-infos")) {
        speci.printUpdateInfos(arrayUpdates,analyzer->getVariableIdMapping());
      }
      SAWYER_MESG(logger[TRACE]) <<"STATUS: establishing array-element SSA numbering."<<endl;
      timer.start();
      speci.createSsaNumbering(arrayUpdates, analyzer->getVariableIdMapping());
      arrayUpdateSsaNumberingRunTime=timer.getTimeDuration().milliSeconds();

      if(args.count("dump-non-sorted")) {
        string filename=args["dump-non-sorted"].as<string>();
        speci.writeArrayUpdatesToFile(arrayUpdates, filename, sarMode, false);
      }
      if(args.count("dump-sorted")) {
        timer.start();
        string filename=args["dump-sorted"].as<string>();
        speci.writeArrayUpdatesToFile(arrayUpdates, filename, sarMode, true);
        sortingAndIORunTime=timer.getTimeDuration().milliSeconds();
      }
      totalRunTime+=arrayUpdateExtractionRunTime+verifyUpdateSequenceRaceConditionRunTime+arrayUpdateSsaNumberingRunTime+sortingAndIORunTime;
    }

    double overallTime =totalRunTime + totalInputTracesTime + totalLtlRunTime;

    printAnalyzerStatistics(analyzer, totalRunTime, "STG generation and assertion analysis complete");

    if(args.count("csv-stats")) {
      string filename=args["csv-stats"].as<string>().c_str();
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

    if (args.count("csv-stats-size-and-ltl")) {
      // content of a line in the .csv file:
      // <#transitions>,<#states>,<#input_states>,<#output_states>,<#error_states>,<#verified_LTL>,<#falsified_LTL>,<#unknown_LTL>
      string filename = args["csv-stats-size-and-ltl"].as<string>();
      write_file(filename,statisticsSizeAndLtl.str());
      cout << "generated "<<filename<<endl;
    }

    if (args.count("csv-stats-cegpra")) {
      // content of a line in the .csv file:
      // <analyzed_property>,<#transitions>,<#states>,<#input_states>,<#output_states>,<#error_states>,
      // <#analyzed_counterexamples>,<analysis_result(y/n/?)>,<#verified_LTL>,<#falsified_LTL>,<#unknown_LTL>
      string filename = args["csv-stats-cegpra"].as<string>();
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

      if (args.count("dot-io-stg")) {
        string filename=args["dot-io-stg"].as<string>();
        cout << "generating dot IO graph file:"<<filename<<endl;
        string dotFile="digraph G {\n";
        dotFile+=visualizer.transitionGraphWithIOToDot();
        dotFile+="}\n";
        write_file(filename, dotFile);
        cout << "=============================================================="<<endl;
      }

      if (args.count("dot-io-stg-forced-top")) {
        string filename=args["dot-io-stg-forced-top"].as<string>();
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
      if(args.count("iseq-file")) {
        int iseqLen=0;
        if(args.count("iseq-length")) {
          iseqLen=args["iseq-length"].as<int>();
        } else {
          logger[ERROR] <<"input-sequence file specified, but no sequence length."<<endl;
          exit(1);
        }
        string fileName=args["iseq-file"].as<string>();
        SAWYER_MESG(logger[TRACE]) <<"STATUS: computing input sequences of length "<<iseqLen<<endl;
        IOSequenceGenerator iosgen;
        if(args.count("iseq-random-num")) {
          int randomNum=args["iseq-random-num"].as<int>();
          SAWYER_MESG(logger[TRACE]) <<"STATUS: reducing input sequence set to "<<randomNum<<" random elements."<<endl;
          iosgen.computeRandomInputPathSet(iseqLen,*analyzer->getTransitionGraph(),randomNum);
        } else {
          iosgen.computeInputPathSet(iseqLen,*analyzer->getTransitionGraph());
        }
        SAWYER_MESG(logger[TRACE]) <<"STATUS: generating input sequence file "<<fileName<<endl;
        iosgen.generateFile(fileName);
      } else {
        if(args.count("iseq-length")) {
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
    if(!args.count("quiet"))
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

void CodeThorn::printStgSize(TransitionGraph* model, string optionalComment, stringstream* csvOutput) {
  long inStates = model->numberOfObservableStates(true, false, false);
  long outStates = model->numberOfObservableStates(false, true, false);
  long errStates = model->numberOfObservableStates(false, false, true);
  cout << "STATUS: STG size ";
  if (optionalComment != "") {
    cout << "(" << optionalComment << "): ";
  }
  cout << "#transitions: " << model->size();
  cout << ", #states: " << model->estateSet().size()
    << " (" << inStates << " in / " << outStates << " out / " << errStates << " err)" << endl;
  if (csvOutput) {
    (*csvOutput) << model->size() <<","<< model->estateSet().size() <<","<< inStates <<","<< outStates <<","<< errStates;
  }
}

void CodeThorn::printAnalyzerStatistics(IOAnalyzer* analyzer, double totalRunTime, string title) {
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
  long numOfConstEStates=(analyzer->getEStateSet()->numberOfConstEStates(analyzer->getVariableIdMapping()));
  //long numOfStdoutEStates=numOfStdoutVarEStates+numOfStdoutConstEStates;

  long totalMemory=pstateSetBytes+eStateSetBytes+transitionGraphBytes+constraintSetsBytes;

  stringstream ss;
  ss <<color("white");
  ss << "=============================================================="<<endl;
  ss <<color("normal")<<title<<color("white")<<endl;
  ss << "=============================================================="<<endl;
  ss << "Number of stdin-estates        : "<<color("cyan")<<numOfStdinEStates<<color("white")<<endl;
  ss << "Number of stdoutvar-estates    : "<<color("cyan")<<numOfStdoutVarEStates<<color("white")<<endl;
  ss << "Number of stdoutconst-estates  : "<<color("cyan")<<numOfStdoutConstEStates<<color("white")<<endl;
  ss << "Number of stderr-estates       : "<<color("cyan")<<numOfStderrEStates<<color("white")<<endl;
  ss << "Number of failed-assert-estates: "<<color("cyan")<<numOfFailedAssertEStates<<color("white")<<endl;
  ss << "Number of const estates        : "<<color("cyan")<<numOfConstEStates<<color("white")<<endl;
  ss << "=============================================================="<<endl;
  ss << "Number of pstates              : "<<color("magenta")<<pstateSetSize<<color("white")<<" (memory: "<<color("magenta")<<pstateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<pstateSetLoadFactor<<  "/"<<pstateSetMaxCollisions<<")"<<endl;
  ss << "Number of estates              : "<<color("cyan")<<eStateSetSize<<color("white")<<" (memory: "<<color("cyan")<<eStateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<eStateSetLoadFactor<<  "/"<<eStateSetMaxCollisions<<")"<<endl;
  ss << "Number of transitions          : "<<color("blue")<<transitionGraphSize<<color("white")<<" (memory: "<<color("blue")<<transitionGraphBytes<<color("white")<<" bytes)"<<endl;
  ss << "Number of constraint sets      : "<<color("yellow")<<numOfconstraintSets<<color("white")<<" (memory: "<<color("yellow")<<constraintSetsBytes<<color("white")<<" bytes)"<<" ("<<""<<constraintSetsLoadFactor<<  "/"<<constraintSetsMaxCollisions<<")"<<endl;
  if(analyzer->getNumberOfThreadsToUse()==1 && analyzer->getSolver()->getId()==5 && analyzer->getExplorationMode()==EXPL_LOOP_AWARE) {
    ss << "Number of iterations           : "<<analyzer->getIterations()<<"-"<<analyzer->getApproximatedIterations()<<endl;
  }
  ss << "=============================================================="<<endl;
  ss << "Memory total                   : "<<color("green")<<totalMemory<<" bytes"<<color("white")<<endl;
  ss << "TimeMeasurement total          : "<<color("green")<<CodeThorn::readableruntime(totalRunTime)<<color("white")<<endl;
  ss << "=============================================================="<<endl;
  ss <<color("normal");
  analyzer->printStatusMessage(ss.str());
}
