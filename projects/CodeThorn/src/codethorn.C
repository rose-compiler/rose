/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "rose.h"

#include "codethorn.h"
#include "SgNodeHelper.h"
#include "Labeler.h"
#include "VariableIdMapping.h"
#include "StateRepresentations.h"
#include "Timer.h"
#include <cstdio>
#include <cstring>
#include <map>
#include <boost/program_options.hpp>
#include "InternalChecks.h"
#include "AstAnnotator.h"
#include "AstTerm.h"
#include "SgNodeHelper.h"
#include "AType.h"
#include "AstMatching.h"
#include "RewriteSystem.h"
#include "SpotConnection.h"
#include "CounterexampleAnalyzer.h"
#include "RefinementConstraints.h"
#include "AnalysisAbstractionLayer.h"
#include "ArrayElementAccessData.h"
#include "PragmaHandler.h"
#include "Miscellaneous2.h"
#include "FIConstAnalysis.h"
#include "ReachabilityAnalysis.h"
#include "EquivalenceChecking.h"
// test
#include "Evaluator.h"
#include "DotGraphCfgFrontend.h"
#include "ParProAnalyzer.h"
#include "PromelaCodeGenerator.h"

//BOOST includes
#include "boost/lexical_cast.hpp"

using namespace std;

namespace po = boost::program_options;
using namespace CodeThorn;
using namespace SPRAY;

// experimental
#include "IOSequenceGenerator.C"

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

class TermRepresentation : public DFAstAttribute {
public:
  TermRepresentation(SgNode* node) : _node(node) {}
  string toString() { return "AstTerm: "+SPRAY::AstTerm::astTermWithNullValuesToString(_node); }
private:
  SgNode* _node;
};

void attachTermRepresentation(SgNode* node) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin(); i!=ast.end();++i) {
    if(SgStatement* stmt=dynamic_cast<SgStatement*>(*i)) {
      AstAttribute* ara=new TermRepresentation(stmt);
      stmt->setAttribute("codethorn-term-representation",ara);
    }
  }
}

static Analyzer* global_analyzer=0;

set<VariableId> determineSetOfCompoundIncVars(VariableIdMapping* vim, SgNode* root) {
  ROSE_ASSERT(vim);
  ROSE_ASSERT(root);
  RoseAst ast(root) ;
  set<VariableId> compoundIncVarsSet;
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

VariableIdSet determineVarsInAssertConditions(SgNode* node, VariableIdMapping* variableIdMapping) {
  VariableIdSet usedVarsInAssertConditions;
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
            usedVarsInAssertConditions.insert(varId);
          }
        }
      }
    }
  }
  return usedVarsInAssertConditions;
}

int main( int argc, char * argv[] ) {
  try {
    Timer timer;
    timer.start();

  // Command line option handling.
    po::options_description visibleOptions("Supported options");
    po::options_description hiddenOptions("Hidden options");
    po::options_description cegpraOptions("CEGPRA options");
    po::options_description ltlOptions("LTL options");
    po::options_description svcompOptions("SV-Comp options");
    po::options_description rersOptions("RERS options");
    po::options_description patternSearchOptions("RERS options");
    po::options_description equivalenceCheckingOptions("Equivalence checking options");
    po::options_description experimentalOptions("Experimental options");
    po::options_description visualizationOptions("Visualization options");
    
    ltlOptions.add_options()
      ("csv-ltl", po::value< string >(), "output LTL verification results into a CSV file [arg]")
      ("csv-spot-ltl", po::value< string >(), "output SPOT's LTL verification results into a CSV file [arg]")
      ("csv-stats-size-and-ltl",po::value< string >(),"output statistics regarding the final model size and results for LTL properties into a CSV file [arg]")
      ("check-ltl", po::value< string >(), "take a text file of LTL I/O formulae [arg] and check whether or not the analyzed program satisfies these formulae. Formulae should start with '('. Use \"csv-spot-ltl\" option to specify an output csv file for the results.")
      ("check-ltl-counterexamples", po::value< string >(), "report ltl counterexamples if and only if they are not spurious [=yes|no]")
      ("check-ltl-sol", po::value< string >(), "take a source code file and an LTL formulae+solutions file ([arg], see RERS downloads for examples). Display if the formulae are satisfied and if the expected solutions are correct.")
      ("counterexamples-with-output", po::value< string >(), "reported counterexamples for LTL or reachability properties also include output values [=yes|no]")
      ("determine-prefix-depth", po::value< string >(), "if possible, display a guarantee about the length of the discovered prefix of possible program traces. [=yes|no]")
      ("inf-paths-only", po::value< string >(), "recursively prune the graph so that no leaves exist [=yes|no]")
      ("io-reduction", po::value< int >(), "(work in progress) reduce the transition system to only input/output/worklist states after every <arg> computed EStates.")
      ("keep-error-states",  po::value< string >(), "Do not reduce error states for the LTL analysis. [=yes|no]")      ("ltl-in-alphabet",po::value< string >(),"specify an input alphabet used by the LTL formulae (e.g. \"{1,2,3}\")")
      ("ltl-out-alphabet",po::value< string >(),"specify an output alphabet used by the LTL formulae (e.g. \"{19,20,21,22,23,24,25,26}\")")
      ("ltl-driven","select mode to verify LTLs driven by spot's access to the state transitions")
      ("no-input-input",  po::value< string >(), "remove transitions where one input states follows another without any output in between. Removal occurs before the LTL check. [=yes|no]")
      ("reconstruct-assert-paths", po::value< string >(), "takes a result file containing paths to reachable assertions and tries to reproduce them on the analyzed program. [=file-path]")
      ("reconstruct-max-length", po::value< int >(), "parameter of option \"reconstruct-input-paths\". Sets the maximum length of cyclic I/O patterns found by the analysis. [=pattern_length]")
      ("reconstruct-max-repetitions", po::value< int >(), "parameter of option \"reconstruct-input-paths\". Sets the maximum number of pattern repetitions that the search is following. [=#pattern_repetitions]")
      ("refinement-constraints-demo", po::value< string >(), "display constraints that are collected in order to later on help a refined analysis avoid spurious counterexamples. [=yes|no]")
      ("spot-stg",po::value< string >(), " generate STG in SPOT-format in file [arg]")
      ("std-io-only", po::value< string >(), "bypass and remove all states that are not standard I/O [=yes|no]")
      ("std-in-only", po::value< string >(), "bypass and remove all states that are not input-states [=yes|no]")
      ("std-out-only", po::value< string >(), "bypass and remove all states that are not output-states [=yes|no]")
      ("tg-ltl-reduced",po::value< string >(),"(experimental) compute LTL-reduced transition graph based on a subset of computed estates [=yes|no]")
      ("with-counterexamples", po::value< string >(), "adds counterexample traces to the analysis results. Applies to reachable assertions (work in progress) and falsified LTL properties. [=yes|no]")
      ("with-assert-counterexamples", po::value< string >(), "report counterexamples leading to failing assertion states (work in progress) [=yes|no]")
      ("with-ltl-counterexamples", po::value< string >(), "report counterexamples that violate LTL properties [=yes|no]")
      ;
    
    hiddenOptions.add_options()
      ("max-transitions-forced-top1",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output) (default: no limit).")
      ("max-transitions-forced-top2",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output,df) (default: no limit).")
      ("max-transitions-forced-top3",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output,df,ptr-vars) (default: no limit).")
      ("max-transitions-forced-top4",po::value< int >(),"Performs approximation after <arg> transitions (exact for all but inc-vars) (default: no limit).")
      ("max-transitions-forced-top5",po::value< int >(),"Performs approximation after <arg> transitions (exact for input,output,df and vars with 0 to 2 assigned values)) (default: no limit).")
      ("normalize",po::value< string >(),"normalize AST before analysis.")
      ("solver",po::value< int >(),"Set solver <arg> to use (one of 1,2,3,...).")
      ;

    cegpraOptions.add_options()
      ("csv-stats-cegpra",po::value< string >(),"output statistics regarding the counterexample-guided prefix refinement analysis (cegpra) into a CSV file [arg]")
      ("cegpra-ltl",po::value< int >(),"Select the ID of an LTL property that should be checked using cegpra (between 0 and 99).")
      ("cegpra-ltl-all",po::value< string >(),"Check all specified LTL properties using cegpra [=yes|no]")
      ("cegpra-max-iterations",po::value< int >(),"Select a maximum number of counterexamples anaylzed by cegpra (default: no limit).")
      ("viz-cegpra-detailed",po::value< string >(),"generate visualization (dot) output files with prefix <arg> for different stages within each loop of cegpra.")
      ;

    visualizationOptions.add_options()
      ("dot-io-stg", po::value< string >(), "output STG with explicit I/O node information in dot file [arg]")
      ("dot-io-stg-forced-top", po::value< string >(), "output STG with explicit I/O node information in dot file. Groups abstract states together. [arg]")
      ("tg1-estate-address", po::value< string >(), "transition graph 1: visualize address [=yes|no]")
      ("tg1-estate-id", po::value< string >(), "transition graph 1: visualize estate-id [=yes|no]")
      ("tg1-estate-properties", po::value< string >(), "transition graph 1: visualize all estate-properties [=yes|no]") 
      ("tg1-estate-predicate", po::value< string >(), "transition graph 1: show estate as predicate [=yes|no]")
      ("tg2-estate-address", po::value< string >(), "transition graph 2: visualize address [=yes|no]")
      ("tg2-estate-id", po::value< string >(), "transition graph 2: visualize estate-id [=yes|no]")
      ("tg2-estate-properties", po::value< string >(),"transition graph 2: visualize all estate-properties [=yes|no]")
      ("tg2-estate-predicate", po::value< string >(), "transition graph 2: show estate as predicate [=yes|no]")
      ("visualize-read-write-sets",po::value< string >(), "generate one graph for each parallel loop that illustrates the read and write accesses of the involved threads.")
      ("viz",po::value< string >(),"generate visualizations (dot) outputs [=yes|no]")
      ;

    experimentalOptions.add_options()
      ("annotate-terms",po::value< string >(),"annotate term representation of expressions in unparsed program.")
      ("arith-top",po::value< string >(),"Arithmetic operations +,-,*,/,% always evaluate to top [=yes|no]")
      ("cfg-dot-input",po::value< string >(),"reads a CFG from a given .dot file.")
      ("eliminate-stg-back-edges",po::value< string >(), " eliminate STG back-edges (STG becomes a tree).")
      ("generate-assertions",po::value< string >(),"generate assertions (pre-conditions) in program and output program (using ROSE unparser).")
      ("precision-exact-constraints",po::value< string >(),"(experimental) use precise constraint extraction [=yes|no]")
      ("reduce-cfg",po::value< string >(),"Reduce CFG nodes which are not relevant for the analysis. [=yes|no]")
      ("report-semantic-fold",po::value< string >(),"report each folding operation with the respective number of estates. [=yes|no]")
      ("semantic-fold",po::value< string >(),"compute semantically folded state transition graph [=yes|no]")
      ("semantic-fold-threshold",po::value< int >(),"Set threshold with <arg> for semantic fold operation (experimental)")
      ("post-semantic-fold",po::value< string >(),"compute semantically folded state transition graph only after the complete transition graph has been computed. [=yes|no]")
      ("set-stg-incomplete", po::value< string >(), "set to true if the generated STG will not contain all possible execution paths (e.g. if only a subset of the input values is used). [=yes|no]")
      ("tg-trace", po::value< string >(), "generate STG computation trace [=filename]")
      ("variable-value-threshold",po::value< int >(),"sets a threshold for the maximum number of different values are stored for each variable.")
      ;

    rersOptions.add_options()
      ("csv-assert", po::value< string >(), "output assert reachability results into a CSV file [arg]")
      ("eliminate-arrays",po::value< string >(), "transform all arrays into single variables.")
      ("iseq-file", po::value< string >(), "compute input sequence and generate file [arg]")
      ("iseq-length", po::value< int >(), "set length [arg] of input sequence to be computed.")
      ("iseq-random-num", po::value< int >(), "select random search and number of paths.")
      ("rers-binary",po::value< string >(),"Call rers binary functions in analysis. Use [=yes|no]")
      ("rers-numeric", po::value< string >(), "print rers I/O values as raw numeric numbers.")
      ("rersmode", po::value< string >(), "sets several options such that RERS-specifics are utilized and observed.")
      ("stderr-like-failed-assert", po::value< string >(), "treat output on stderr similar to a failed assert [arg] (default:no)")
      ;

    svcompOptions.add_options()
      ("error-function", po::value< string >(), "detect a verifier error function with name [arg] (terminates verification)")
      ("enable-external-function-semantics",  "assumes specific semantics for the external functions: __VERIFIER_error,__VERIFIER_nondet_int,exit functions.")
      ;

    equivalenceCheckingOptions.add_options()
      ("dump-sorted",po::value< string >(), " [experimental] generates sorted array updates in file <file>")
      ("dump-non-sorted",po::value< string >(), " [experimental] generates non-sorted array updates in file <file>")
      ("limit-to-fragment",po::value< string >(), "the argument is used to find fragments marked by two prgagmas of that '<name>' and 'end<name>'")
      ("print-update-infos",po::value< string >(), "[experimental] print information about array updates on stdout")
      ("rule-const-subst",po::value< string >(), " [experimental] use const-expr substitution rule <arg>")
      ("specialize-fun-name", po::value< string >(), "function of name [arg] to be specialized")
      ("specialize-fun-param", po::value< vector<int> >(), "function parameter number to be specialized (starting at 1)")
      ("specialize-fun-const", po::value< vector<int> >(), "constant [arg], the param is to be specialized to.")
      ("specialize-fun-varinit", po::value< vector<string> >(), "variable name of which the initialization is to be specialized (overrides any initializer expression)")
      ("specialize-fun-varinit-const", po::value< vector<int> >(), "constant [arg], the variable initialization is to be specialized to.")
      ("verify-update-sequence-race-conditions",po::value< string >(), "[experimental] check race conditions of update sequence")
      ;

    patternSearchOptions.add_options()
      ("pattern-search-max-depth", po::value< int >(), "parameter of the pattern search mode. Sets the maximum input depth that is searched for cyclic I/O patterns (default: 10).")
      ("pattern-search-repetitions", po::value< int >(), "parameter of the pattern search mode. Sets the number of unrolled iterations of cyclic I/O patterns (default: 100).")
      ("pattern-search-max-suffix", po::value< int >(), "parameter of the pattern search mode. Sets the maximum input depth of the suffix that is searched for failing assertions after following an I/O-pattern (default: 5).")
      ("pattern-search-asserts", po::value< string >(), "reads a .csv-file (one line per assertion, e.g. \"1,yes\"). The pattern search terminates early if traces to all errors with \"yes\" entries have been found. [=file-path]")
      ("pattern-search-exploration", po::value< string >(), "exploration mode for the pattern search. Note: all suffixes will always be checked using depth-first search. [=depth-first|breadth-first]")
      ;

    visibleOptions.add_options()
      ("csv-stats",po::value< string >(),"output statistics into a CSV file [arg]")
      ("colors",po::value< string >(),"use colors in output [=yes|no]")
      ("display-diff",po::value< int >(),"Print statistics every <arg> computed estates.")
      ("exploration-mode",po::value< string >(), " set mode in which state space is explored ([breadth-first], depth-first, loop-aware, loop-aware-sync)")
      ("help,h", "produce this help message")
      ("help-cegpra", "show options for CEGRPA")
      ("help-eq", "show options for program equivalence checking")
      ("help-exp", "show options for experimental features")
      ("help-pat", "show options for pattern search mode")
      ("help-svcomp", "show options for SV-Comp specific features")
      ("help-rers", "show options for RERS specific features")
      ("help-ltl", "show options for LTL verification")
      ("help-vis", "show options for visualization output files")
      ("internal-checks", "run internal consistency checks (without input program)")
      ("input-values",po::value< string >(),"specify a set of input values (e.g. \"{1,2,3}\")")
      ("input-values-as-constraints",po::value<string >(),"represent input var values as constraints (otherwise as constants in PState)")
      ("input-sequence",po::value< string >(),"specify a sequence of input values (e.g. \"[1,2,3]\")")
      ("max-transitions",po::value< int >(),"Passes (possibly) incomplete STG to verifier after max transitions (default: no limit).")
      ("max-iterations",po::value< int >(),"Passes (possibly) incomplete STG to verifier after max loop iterations (default: no limit). Currently requires --exploration-mode=loop-aware[-sync].")
      ("max-transitions-forced-top",po::value< int >(),"same as max-transitions-forced-top1 (default).")
      ("max-iterations-forced-top",po::value< int >(),"Performs approximation after <arg> loop iterations (default: no limit). Currently requires --exploration-mode=loop-aware[-sync].")
      ("max-memory-stg",po::value< int >(),"Stop computing the STG after it amounts to approximately <arg> Bytes of memory. (default: no limit). Currently requires --solver=12.")
      ("resource-limit-diff",po::value< int >(),"Check if the resource limit is reached every <arg> computed estates.")
      ("print-all-options",po::value< string >(),"print the default values for all yes/no command line options.")
      ("rewrite","rewrite AST applying all rewrite system rules.")
      ("run-rose-tests",po::value< string >(),"Run ROSE AST tests. [=yes|no]")
      ("threads",po::value< int >(),"Run analyzer in parallel using <arg> threads (experimental)")
      ("version,v", "display the version")
      ;

    po::options_description all("All supported options");
    all.add(visibleOptions)
      .add(hiddenOptions)
      .add(cegpraOptions)
      .add(equivalenceCheckingOptions)
      .add(experimentalOptions)
      .add(ltlOptions)
      .add(patternSearchOptions)
      .add(rersOptions)
      .add(svcompOptions)
      .add(visualizationOptions)
      ;
    po::store(po::command_line_parser(argc, argv).options(all).allow_unregistered().run(), args);
    po::notify(args);

    if (args.count("help")) {
      cout << visibleOptions << "\n";return 0;
    } else if(args.count("help-cegpra")) {
      cout << cegpraOptions << "\n";return 0;
    } else if(args.count("help-eq")) {
      cout << equivalenceCheckingOptions << "\n";return 0;
    } else if(args.count("help-exp")) {
      cout << experimentalOptions << "\n";return 0;
    } else if(args.count("help-ltl")) {
      cout << ltlOptions << "\n";return 0;
    } else if(args.count("help-pat")) {
      cout << patternSearchOptions << "\n";return 0;
    } else if(args.count("help-rers")) {
      cout << rersOptions << "\n";return 0;
    } else if(args.count("help-svcomp")) {
      cout << svcompOptions << "\n";return 0;
    } else if(args.count("help-vis")) {
      cout << visualizationOptions << "\n";return 0;
    }

    if (args.count("version")) {
      cout << "CodeThorn version 1.7.0\n";
      cout << "Written by Markus Schordan, Adrian Prantl, and Marc Jasper\n";
      return 0;
    }

  boolOptions.init(argc,argv);
  boolOptions.registerOption("tg1-estate-address",false);
  boolOptions.registerOption("tg1-estate-id",false);
  boolOptions.registerOption("tg1-estate-properties",true);
  boolOptions.registerOption("tg1-estate-predicate",false);
  boolOptions.registerOption("tg2-estate-address",false);
  boolOptions.registerOption("tg2-estate-id",true);
  boolOptions.registerOption("tg2-estate-properties",false);
  boolOptions.registerOption("tg2-estate-predicate",false);
  boolOptions.registerOption("colors",true);
  boolOptions.registerOption("precision-exact-constraints",false);
  boolOptions.registerOption("tg-ltl-reduced",false);
  boolOptions.registerOption("semantic-fold",false);
  boolOptions.registerOption("post-semantic-fold",false);
  boolOptions.registerOption("report-semantic-fold",false);
  boolOptions.registerOption("eliminate-arrays",false);

  boolOptions.registerOption("viz",false);
  boolOptions.registerOption("visualize-read-write-sets",false);
  boolOptions.registerOption("run-rose-tests",false);
  boolOptions.registerOption("reduce-cfg",false);
  boolOptions.registerOption("print-all-options",false);
  boolOptions.registerOption("annotate-terms",false);
  boolOptions.registerOption("generate-assertions",false);

  boolOptions.registerOption("input-values-as-constraints",false);

  boolOptions.registerOption("arith-top",false);
  boolOptions.registerOption("rers-binary",false);
  boolOptions.registerOption("relop-constraints",false); // not accessible on command line
  boolOptions.registerOption("stderr-like-failed-assert",false);
  boolOptions.registerOption("rersmode",false);
  boolOptions.registerOption("rers-numeric",false);
  boolOptions.registerOption("eliminate-stg-back-edges",false);
  boolOptions.registerOption("rule-const-subst",true);

  boolOptions.registerOption("inf-paths-only",false);
  boolOptions.registerOption("std-io-only",false);
  boolOptions.registerOption("std-in-only",false);
  boolOptions.registerOption("std-out-only",false);
  boolOptions.registerOption("keep-error-states",false);
  boolOptions.registerOption("no-input-input",false);

  boolOptions.registerOption("with-counterexamples",false);
  boolOptions.registerOption("with-assert-counterexamples",false);
  boolOptions.registerOption("with-ltl-counterexamples",false);
  boolOptions.registerOption("counterexamples-with-output",false);
  boolOptions.registerOption("check-ltl-counterexamples",false); 
  boolOptions.registerOption("cegpra-ltl-all",false); 
  boolOptions.registerOption("refinement-constraints-demo",false);
  boolOptions.registerOption("determine-prefix-depth",false);
  boolOptions.registerOption("set-stg-incomplete",false);

  boolOptions.registerOption("print-update-infos",false);
  boolOptions.registerOption("verify-update-sequence-race-conditions",true);

  boolOptions.registerOption("normalize",true);

  boolOptions.processOptions();

  //TODO: remove this temporary test
  if (args.count("cfg-dot-input")) {
    DotGraphCfgFrontend dotGraphCfgFrontend;
    string filename = args["cfg-dot-input"].as<string>();
    CfgsAndAnnotationMap cfgsAndMap = dotGraphCfgFrontend.parseDotCfgs(filename);
    cout << "DEBUG: generating PROMELA code..." << endl;
    PromelaCodeGenerator codeGenerator;
    string promelaCode = codeGenerator.generateCode(cfgsAndMap);
    cout << "DEBUG: done." << endl;
    string promelaOutputFilename = "promelaCode.pml";
    write_file(promelaOutputFilename, promelaCode);
    cout << "generated " << promelaOutputFilename <<"."<<endl;
    list<Flow> cfgs = cfgsAndMap.first;
    EdgeAnnotationMap edgeAnnotationMap = cfgsAndMap.second;
    int counter = 0;
    for(list<Flow>::iterator i=cfgs.begin(); i!=cfgs.end(); i++) {
      Flow cfg = *i;
      cout << "DEBUG: current cfg's start state id is: " << cfg.getStartLabel().getId() << endl;
      cfg.setDotOptionDisplayLabel(false);
      cfg.setDotOptionDisplayStmt(false);
      cfg.setDotOptionEdgeAnnotationsOnly(true);
      string outputFilename = "cfg" + boost::lexical_cast<string>(counter) + ".dot";
      write_file(outputFilename, cfg.toDot(NULL));
      cout << "generated " << outputFilename <<"."<<endl;
      counter++;
    }
    
    vector<Flow> cfgsAsVector;
    cfgsAsVector.reserve(cfgs.size());
    copy(begin(cfgs), end(cfgs), back_inserter(cfgsAsVector));
    ParProAnalyzer parProAnalyzer(cfgsAsVector);
    parProAnalyzer.setAnnotationMap(edgeAnnotationMap);
    parProAnalyzer.initializeSolver();
    parProAnalyzer.runSolver();
    ParProTransitionGraph* transitionGraph = parProAnalyzer.getTransitionGraph();
    cout << "DEBUG: transitionGraph->size() = " << transitionGraph->size() << endl;
    Visualizer visualizer;
    string dotStg = visualizer.parProTransitionGraphToDot(transitionGraph);
    string outputFilename = "stgParallelProgram.dot";
    write_file(outputFilename, dotStg);
    cout << "generated " << outputFilename <<"."<<endl;

    if (args.count("check-ltl")) {
      string ltl_filename = args["check-ltl"].as<string>();
      bool withCounterexample = false;
      if(boolOptions["with-counterexamples"] || boolOptions["with-ltl-counterexamples"]) {  //output a counter-example input sequence for falsified formulae
	withCounterexample = true;
      }
      timer.start();
      PropertyValueTable* ltlResults;
      SpotConnection spotConnection(ltl_filename);
      cout << "STATUS: generating LTL results"<<endl;
      bool spuriousNoAnswers = false;
      spotConnection.checkLtlPropertiesParPro( *transitionGraph, withCounterexample, spuriousNoAnswers);
      ltlResults = spotConnection.getLtlResults();
      ltlResults-> printResults("YES (verified)", "NO (falsified)", "ltl_property_", withCounterexample);
      cout << "=============================================================="<<endl;
      ltlResults->printResultsStatistics();
      cout << "=============================================================="<<endl;
      if (args.count("csv-spot-ltl")) {  //write results to a file instead of displaying them directly
	std::string csv_filename = args["csv-spot-ltl"].as<string>();
	cout << "STATUS: writing ltl results to file: " << csv_filename << endl;
	ltlResults->writeFile(csv_filename.c_str(), false, 0, withCounterexample);
      }
      delete ltlResults;
      ltlResults = NULL;
    }
    cout << "DEBUG: parseDotCfg test complete." << endl;
    exit(0);
  }

  Analyzer analyzer;
  global_analyzer=&analyzer;

  // this must be set early, as subsequent initialization depends on this flag
  if (args.count("ltl-driven")) {
    analyzer.setModeLTLDriven(true);
  }

  if (args.count("cegpra-ltl") || boolOptions["cegpra-ltl-all"]) {
    analyzer.setMaxTransitionsForcedTop(1); //initial over-approximated model
    boolOptions.registerOption("no-input-input",true);
    boolOptions.registerOption("with-ltl-counterexamples",true);
    boolOptions.registerOption("counterexamples-with-output",true);
    cout << "STATUS: CEGPRA activated (with it LTL counterexamples that include output states)." << endl;
    cout << "STATUS: CEGPRA mode: will remove input state --> input state transitions in the approximated STG. " << endl;
  }

  if (boolOptions["counterexamples-with-output"]) {
    boolOptions.registerOption("with-ltl-counterexamples",true);
  }

  if (boolOptions["check-ltl-counterexamples"]) {
    boolOptions.registerOption("no-input-input",true);
    boolOptions.registerOption("with-ltl-counterexamples",true);
    boolOptions.registerOption("counterexamples-with-output",true);
    cout << "STATUS: option check-ltl-counterexamples activated (analyzing counterexamples, returning those that are not spurious.)" << endl;
    cout << "STATUS: option check-ltl-counterexamples: activates LTL counterexamples with output. Removes input state --> input state transitions in the approximated STG. " << endl;
  }

  if(boolOptions["print-all-options"]) {
    cout<<boolOptions.toString(); // prints all bool options
    exit(1);
  }
  
  if(boolOptions["arith-top"]) {
    CodeThorn::AType::ConstIntLattice cil;
    cil.arithTop=true;
  }

  if (args.count("internal-checks")) {
    if(CodeThorn::internalChecks(argc,argv)==false)
      return 1;
    else
      return 0;
  }
  string option_pragma_name;
  if (args.count("limit-to-fragment")) {
    option_pragma_name = args["limit-to-fragment"].as<string>();
  }

  if(args.count("tg-trace")) {
    analyzer.setStgTraceFileName(args["tg-trace"].as<string>());
  }

  if(args.count("input-values")) {
    string setstring=args["input-values"].as<string>();
    cout << "STATUS: input-values="<<setstring<<endl;

    set<int> intSet=Parse::integerSet(setstring);
    for(set<int>::iterator i=intSet.begin();i!=intSet.end();++i) {
      analyzer.insertInputVarValue(*i);
    }
  }

  if(args.count("input-sequence")) {
    string liststring=args["input-sequence"].as<string>();
    cout << "STATUS: input-sequence="<<liststring<<endl;

    list<int> intList=Parse::integerList(liststring);
    for(list<int>::iterator i=intList.begin();i!=intList.end();++i) {
      analyzer.addInputSequenceValue(*i);
    }
  }

  if(args.count("exploration-mode")) {
    string explorationMode=args["exploration-mode"].as<string>();
    if(explorationMode=="depth-first") {
      analyzer.setExplorationMode(Analyzer::EXPL_DEPTH_FIRST);
    } else if(explorationMode=="breadth-first") {
      analyzer.setExplorationMode(Analyzer::EXPL_BREADTH_FIRST);
    } else if(explorationMode=="loop-aware") {
      analyzer.setExplorationMode(Analyzer::EXPL_LOOP_AWARE);
    } else if(explorationMode=="loop-aware-sync") {
      analyzer.setExplorationMode(Analyzer::EXPL_LOOP_AWARE_SYNC);
    } else if(explorationMode=="random-mode1") {
      analyzer.setExplorationMode(Analyzer::EXPL_RANDOM_MODE1);
    } else {
      cerr<<"Error: unknown state space exploration mode specified with option --exploration-mode."<<endl;
      exit(1);
    }
  } else {
    // default value
    analyzer.setExplorationMode(Analyzer::EXPL_BREADTH_FIRST);
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

  if(args.count("max-transitions")) {
    analyzer.setMaxTransitions(args["max-transitions"].as<int>());
  }

  if(args.count("max-iterations")) {
    analyzer.setMaxIterations(args["max-iterations"].as<int>());
  }

  if(args.count("max-iterations-forced-top")) {
    analyzer.setMaxIterationsForcedTop(args["max-iterations-forced-top"].as<int>());
    analyzer.setGlobalTopifyMode(Analyzer::GTM_IO);
  }

  if(args.count("max-transitions-forced-top")) {
    analyzer.setMaxTransitionsForcedTop(args["max-transitions-forced-top"].as<int>());
    analyzer.setGlobalTopifyMode(Analyzer::GTM_IO);
  } else if(args.count("max-transitions-forced-top1")) {
    analyzer.setMaxTransitionsForcedTop(args["max-transitions-forced-top1"].as<int>());
    analyzer.setGlobalTopifyMode(Analyzer::GTM_IO);
  } else if(args.count("max-transitions-forced-top2")) {
    analyzer.setMaxTransitionsForcedTop(args["max-transitions-forced-top2"].as<int>());
    analyzer.setGlobalTopifyMode(Analyzer::GTM_IOCF);
  } else if(args.count("max-transitions-forced-top3")) {
    analyzer.setMaxTransitionsForcedTop(args["max-transitions-forced-top3"].as<int>());
    analyzer.setGlobalTopifyMode(Analyzer::GTM_IOCFPTR);
  } else if(args.count("max-transitions-forced-top4")) {
    analyzer.setMaxTransitionsForcedTop(args["max-transitions-forced-top4"].as<int>());
    analyzer.setGlobalTopifyMode(Analyzer::GTM_COMPOUNDASSIGN);
  } else if(args.count("max-transitions-forced-top5")) {
    analyzer.setMaxTransitionsForcedTop(args["max-transitions-forced-top5"].as<int>());
    analyzer.setGlobalTopifyMode(Analyzer::GTM_FLAGS);
  }

  if (args.count("max-memory-stg")) {
    bool notSupported=false;
    if(args.count("solver")) {
      int solver=args["solver"].as<int>();
      if (solver != 12) {
	notSupported = true;
      }
    } else {
      cout<<"ERROR: solver 12 is currently not the default solver."<<endl;
      notSupported=true; 
    }
    if(notSupported) {
      cout << "Error: option \"--max-memory-stg\" currently requires \"--solver=12\"." << endl;
      exit(1);
    }
    long int maxBytes = args["max-memory-stg"].as<int>();
    analyzer.setMaxBytesStg(maxBytes);
  }

  if(args.count("reconstruct-assert-paths")) {
    string previousAssertFilePath=args["reconstruct-assert-paths"].as<string>();
    PropertyValueTable* previousAssertResults=analyzer.loadAssertionsToReconstruct(previousAssertFilePath);
    analyzer.setReconstructPreviousResults(previousAssertResults);
  }

  if(args.count("reconstruct-max-length")) {
    // a cycilc pattern needs to be discovered two times, therefore multiply the length by 2
    analyzer.setReconstructMaxInputDepth(args["reconstruct-max-length"].as<int>() * 2);
  }

  if(args.count("reconstruct-max-repetitions")) {
    analyzer.setReconstructMaxRepetitions(args["reconstruct-max-repetitions"].as<int>());
  }

  if(args.count("pattern-search-max-depth")) {
    analyzer.setPatternSearchMaxDepth(args["pattern-search-max-depth"].as<int>());
  } else {
    analyzer.setPatternSearchMaxDepth(10);
  }

  if(args.count("pattern-search-repetitions")) {
    analyzer.setPatternSearchRepetitions(args["pattern-search-repetitions"].as<int>());
  } else {
    analyzer.setPatternSearchRepetitions(100);
  }

  if(args.count("pattern-search-max-suffix")) {
    analyzer.setPatternSearchMaxSuffixDepth(args["pattern-search-max-suffix"].as<int>());
  } else {
    analyzer.setPatternSearchMaxSuffixDepth(5);
  }

  if(args.count("pattern-search-asserts")) {
    string patternSearchAssertsPath=args["pattern-search-asserts"].as<string>();
    PropertyValueTable* patternSearchAsserts=analyzer.loadAssertionsToReconstruct(patternSearchAssertsPath);
    analyzer.setPatternSearchAssertTable(patternSearchAsserts);
  } else {
    PropertyValueTable* patternSearchAsserts = new PropertyValueTable(100);
    patternSearchAsserts->convertValue(PROPERTY_VALUE_UNKNOWN, PROPERTY_VALUE_YES);
    analyzer.setPatternSearchAssertTable(patternSearchAsserts);
  }

  if(args.count("pattern-search-exploration")) {
    string patternSearchExpMode=args["pattern-search-exploration"].as<string>();
    if (patternSearchExpMode == "breadth-first") {
      analyzer.setPatternSearchExploration(Analyzer::EXPL_BREADTH_FIRST);
    } else if (patternSearchExpMode == "depth-first") {
      analyzer.setPatternSearchExploration(Analyzer::EXPL_DEPTH_FIRST);
    } else {
      cout << "ERROR: pattern search exploration mode \"" << patternSearchExpMode << "\" currently not supported." << endl;
      ROSE_ASSERT(0);
    }
  } else {
    analyzer.setPatternSearchExploration(Analyzer::EXPL_DEPTH_FIRST);
  }

  int numberOfThreadsToUse=1;
  if(args.count("threads")) {
    numberOfThreadsToUse=args["threads"].as<int>();
  }
  analyzer.setNumberOfThreadsToUse(numberOfThreadsToUse);

  if(args.count("semantic-fold-threshold")) {
    int semanticFoldThreshold=args["semantic-fold-threshold"].as<int>();
    analyzer.setSemanticFoldThreshold(semanticFoldThreshold);
  }
  if(args.count("display-diff")) {
    int displayDiff=args["display-diff"].as<int>();
    analyzer.setDisplayDiff(displayDiff);
  }
  if(args.count("resource-limit-diff")) {
    int resourceLimitDiff=args["resource-limit-diff"].as<int>();
    analyzer.setResourceLimitDiff(resourceLimitDiff);
  }
  int ltlSolverNr=11;
  int loopAwareSyncSolverNr=12;
  if(args.count("solver")) {
    int solver=args["solver"].as<int>();
    if(analyzer.getModeLTLDriven()) {
      if(solver!=ltlSolverNr) {
        cerr<<"Error: ltl-driven mode requires solver "<<ltlSolverNr<<", but solver "<<solver<<" was selected."<<endl;
        exit(1);
      }
    }
    if(analyzer.getExplorationMode() == Analyzer::EXPL_LOOP_AWARE_SYNC) {
      if(solver!=loopAwareSyncSolverNr) {
        cerr<<"Error: exploration mode loop-aware-sync requires solver "<<loopAwareSyncSolverNr<<", but solver "<<solver<<" was selected."<<endl;
        exit(1);
      }
    }
    analyzer.setSolver(solver);
  }
  if(analyzer.getModeLTLDriven()) {
    analyzer.setSolver(ltlSolverNr);
  }
  if(args.count("variable-value-threshold")) {
    analyzer.setVariableValueThreshold(args["variable-value-threshold"].as<int>());
  }

  string option_specialize_fun_name="";
  vector<int> option_specialize_fun_param_list;
  vector<int> option_specialize_fun_const_list;
  vector<string> option_specialize_fun_varinit_list;
  vector<int> option_specialize_fun_varinit_const_list;
  if(args.count("specialize-fun-name")) {
    option_specialize_fun_name = args["specialize-fun-name"].as<string>();
    //cout << "DEBUG: option_specialize_fun_name: "<< option_specialize_fun_name<<endl;
  } else {
    //cout << "DEBUG: option_specialize_fun_name: NONE"<< option_specialize_fun_name<<endl;
  }

  if(args.count("specialize-fun-param")) {
    option_specialize_fun_param_list=args["specialize-fun-param"].as< vector<int> >();
    option_specialize_fun_const_list=args["specialize-fun-const"].as< vector<int> >();
  }

  if(args.count("specialize-fun-varinit")) {
    option_specialize_fun_varinit_list=args["specialize-fun-varinit"].as< vector<string> >();
    option_specialize_fun_varinit_const_list=args["specialize-fun-varinit-const"].as< vector<int> >();
  }

  //cout<<"DEBUG: "<<"specialize-params:"<<option_specialize_fun_const_list.size()<<endl;

  if(args.count("specialize-fun-name")) {
    if((args.count("specialize-fun-param")||args.count("specialize-fun-const"))
       && !(args.count("specialize-fun-name")&&args.count("specialize-fun-param")&&args.count("specialize-fun-param"))) {
      cout<<"Error: options --specialize-fun-name=NAME --specialize-fun-param=NUM --specialize-fun-const=NUM must be used together."<<endl;
      exit(1);
    }
    if((args.count("specialize-fun-varinit")||args.count("specialize-fun-varinit-const"))
       && !(args.count("specialize-fun-varinit")&&args.count("specialize-fun-varinit-const"))) {
      cout<<"Error: options --specialize-fun-name=NAME --specialize-fun-varinit=NAME --specialize-fun-const=NUM must be used together."<<endl;
      exit(1);
    }
  }
  // clean up string-options in argv
  for (int i=1; i<argc; ++i) {
    if (string(argv[i]).find("--cfg-dot-input")==0
        || string(argv[i]).find("--csv-assert")==0
        || string(argv[i]).find("--csv-stats")==0
        || string(argv[i]).find("--csv-stats-cegpra")==0
        || string(argv[i]).find("--csv-stats-size-and-ltl")==0
        || string(argv[i]).find("--threads" )==0
        || string(argv[i]).find("--display-diff")==0
        || string(argv[i]).find("--input-values")==0
        || string(argv[i]).find("--csv-ltl")==0
        || string(argv[i]).find("--spot-stg")==0
        || string(argv[i]).find("--dump-sorted")==0
        || string(argv[i]).find("--dump-non-sorted")==0
        || string(argv[i]).find("--limit-to-fragment")==0
        || string(argv[i]).find("--check-ltl")==0
        || string(argv[i]).find("--csv-spot-ltl")==0
        || string(argv[i]).find("--check-ltl-sol")==0
        || string(argv[i]).find("--ltl-in-alphabet")==0
        || string(argv[i]).find("--ltl-out-alphabet")==0
        || string(argv[i]).find("--ltl-driven")==0
        || string(argv[i]).find("--reconstruct-assert-paths")==0
        || string(argv[i]).find("--pattern-search-asserts")==0
        || string(argv[i]).find("--pattern-search-exploration")==0
        || string(argv[i]).find("--specialize-fun-name")==0
        || string(argv[i]).find("--specialize-fun-param")==0
        ) {
      // do not confuse ROSE frontend
      argv[i] = strdup("");
    }
  }

  if((args.count("print-update-infos")||args.count("verify-update-sequence-race-conditions"))&&(args.count("dump-sorted")==0 && args.count("dump-non-sorted")==0)) {
    cerr<<"Error: option print-update-infos/verify-update-sequence-race-conditions must be used together with option --dump-non-sorted or --dump-sorted."<<endl;
    exit(1);
  }
  RewriteSystem rewriteSystem;
  if(args.count("dump-sorted")>0 || args.count("dump-non-sorted")>0) {
    analyzer.setSkipSelectedFunctionCalls(true);
    analyzer.setSkipArrayAccesses(true);
    if(numberOfThreadsToUse>1) {
      cerr<<"Error: multi threaded rewrite not supported yet."<<endl;
      exit(1);
    }
  }
  
  // DEBUG
  //cout<<"DEBUG: ignoring lhs-array accesses"<<endl;
  //analyzer.setSkipArrayAccesses(true);

  // handle RERS mode: reconfigure options
  if(boolOptions["rersmode"]) {
    cout<<"INFO: RERS MODE activated [stderr output is treated like a failed assert]"<<endl;
    boolOptions.registerOption("stderr-like-failed-assert",true);
  }

  if(args.count("enable-external-function-semantics")) {
    analyzer.enableExternalFunctionSemantics();
  }
  if(args.count("error-function")) {
    string errorFunctionName=args["error-function"].as<string>();
    analyzer.setExternalErrorFunctionName(errorFunctionName);
  }

  analyzer.setTreatStdErrLikeFailedAssert(boolOptions["stderr-like-failed-assert"]);

  // Build the AST used by ROSE
  cout << "INIT: Parsing and creating AST: started."<<endl;
  timer.stop();
  timer.start();
  SgProject* sageProject = frontend(argc,argv);
  double frontEndRunTime=timer.getElapsedTimeInMilliSec();
  cout << "INIT: Parsing and creating AST: finished."<<endl;
  
  analyzer.getVariableIdMapping()->computeVariableSymbolMapping(sageProject);

  if(boolOptions["run-rose-tests"]) {
    cout << "INIT: Running ROSE AST tests."<<endl;
    // Run internal consistency tests on AST
    AstTests::runAllTests(sageProject);

    // test: constant expressions
    {
      cout<<"STATUS: testing constant expressions."<<endl;
      CppConstExprEvaluator* evaluator=new CppConstExprEvaluator();
      list<SgExpression*> exprList=exprRootList(sageProject);
      cout<<"INFO: found "<<exprList.size()<<" expressions."<<endl;
      for(list<SgExpression*>::iterator i=exprList.begin();i!=exprList.end();++i) {
        EvalResult r=evaluator->traverse(*i);
        if(r.isConst()) {
          cout<<"Found constant expression: "<<(*i)->unparseToString()<<" eq "<<r.constValue()<<endl;
        }
      }
      delete evaluator;
    }
    return 0;
  }

  SgNode* root=sageProject;
  ROSE_ASSERT(root);

  // only handle pragmas if fun_name is not set on the command line
  if(option_specialize_fun_name=="") {
    cout <<"STATUS: handling pragmas started."<<endl;
    PragmaHandler pragmaHandler;
    pragmaHandler.handlePragmas(sageProject,&analyzer);
    // TODO: requires more refactoring
    option_specialize_fun_name=pragmaHandler.option_specialize_fun_name;
    boolOptions.registerOption("verify-update-sequence-race-conditions",true);
    // unparse specialized code
    //sageProject->unparse(0,0);
    cout <<"STATUS: handling pragmas finished."<<endl;
  } else {
    // do specialization and setup data structures
    analyzer.setSkipSelectedFunctionCalls(true);
    analyzer.setSkipArrayAccesses(true);
    boolOptions.registerOption("verify-update-sequence-race-conditions",true);

    //TODO1: refactor into separate function
    int numSubst=0;
    if(option_specialize_fun_name!="") {
      Specialization speci;
      cout<<"STATUS: specializing function: "<<option_specialize_fun_name<<endl;
      
      string funNameToFind=option_specialize_fun_name;
      
      for(size_t i=0;i<option_specialize_fun_param_list.size();i++) {
        int param=option_specialize_fun_param_list[i];
        int constInt=option_specialize_fun_const_list[i];
        numSubst+=speci.specializeFunction(sageProject,funNameToFind, param, constInt, analyzer.getVariableIdMapping());
      }
      cout<<"STATUS: specialization: number of variable-uses replaced with constant: "<<numSubst<<endl;
      int numInit=0;
      //cout<<"DEBUG: var init spec: "<<endl;
      for(size_t i=0;i<option_specialize_fun_varinit_list.size();i++) {
        string varInit=option_specialize_fun_varinit_list[i];
        int varInitConstInt=option_specialize_fun_varinit_const_list[i];
        //cout<<"DEBUG: checking for varInitName nr "<<i<<" var:"<<varInit<<" Const:"<<varInitConstInt<<endl;
        numInit+=speci.specializeFunction(sageProject,funNameToFind, -1, 0, varInit, varInitConstInt,analyzer.getVariableIdMapping());
      }
      cout<<"STATUS: specialization: number of variable-inits replaced with constant: "<<numInit<<endl;
    }
  }

  if(args.count("rewrite")) {
    cout <<"STATUS: rewrite started."<<endl;
    rewriteSystem.resetStatistics();
    rewriteSystem.rewriteAst(root,analyzer.getVariableIdMapping() ,true,false,true);
    cout<<"Rewrite statistics:"<<endl<<rewriteSystem.getStatistics().toString()<<endl;
    sageProject->unparse(0,0);
    cout<<"STATUS: generated rewritten program."<<endl;
    exit(0);
  }

  {
    // TODO: refactor this into class Analyzer after normalization has been moved to class Analyzer.
    set<VariableId> compoundIncVarsSet=determineSetOfCompoundIncVars(analyzer.getVariableIdMapping(),root);
    analyzer.setCompoundIncVarsSet(compoundIncVarsSet);
    cout<<"STATUS: determined "<<compoundIncVarsSet.size()<<" compound inc/dec variables before normalization."<<endl;
  }
  {
    VariableIdSet varsInAssertConditions=determineVarsInAssertConditions(root,analyzer.getVariableIdMapping());
    cout<<"STATUS: determined "<<varsInAssertConditions.size()<< " variables in (guarding) assert conditions."<<endl;
    analyzer.setAssertCondVarsSet(varsInAssertConditions);
  }
  // problematic? TODO: debug
#if 0
  {
    cout<<"STATUS: performing flow-insensitive const analysis."<<endl;
    VarConstSetMap varConstSetMap;
    VariableIdSet variablesOfInterest1,variablesOfInterest2;
    FIConstAnalysis fiConstAnalysis(analyzer.getVariableIdMapping());
    fiConstAnalysis.runAnalysis(sageProject);
    VariableConstInfo* variableConstInfo=fiConstAnalysis.getVariableConstInfo();
    variablesOfInterest1=fiConstAnalysis.determinedConstantVariables();
    for(VariableIdSet::iterator i=variablesOfInterest1.begin();i!=variablesOfInterest1.end();++i) {
      if(!variableConstInfo->isAny(*i) && variableConstInfo->width(*i)<=2) {
        variablesOfInterest2.insert(*i);
      }
    }
    analyzer.setSmallActivityVarsSet(variablesOfInterest2);
    cout<<"INFO: variables with number of values <=2:"<<variablesOfInterest2.size()<<endl;
  }
#endif

  if(boolOptions["normalize"]) {
    cout <<"STATUS: Normalization started."<<endl;
    rewriteSystem.resetStatistics();
    rewriteSystem.rewriteCompoundAssignmentsInAst(root,analyzer.getVariableIdMapping());
    cout <<"STATUS: Normalization finished."<<endl;
  }
  cout << "INIT: Checking input program."<<endl;
  CodeThornLanguageRestrictor lr;
  //lr.checkProgram(root);
  timer.start();

  //cout << "INIT: Running variable<->symbol mapping check."<<endl;
  //VariableIdMapping varIdMap;
  //analyzer.getVariableIdMapping()->setModeVariableIdForEachArrayElement(true);
  //analyzer.getVariableIdMapping()->computeVariableSymbolMapping(sageProject);
  //cout << "STATUS: Variable<->Symbol mapping created."<<endl;
#if 0
  if(!analyzer.getVariableIdMapping()->isUniqueVariableSymbolMapping()) {
    cerr << "WARNING: Variable<->Symbol mapping not bijective."<<endl;
    //varIdMap.reportUniqueVariableSymbolMappingViolations();
  }
#endif
#if 0
  analyzer.getVariableIdMapping()->toStream(cout);
#endif

  // currently not used, but may be revived to properly handle new annotation
  SgNode* fragmentStartNode=0;
  if(option_pragma_name!="") {
    list<SgPragmaDeclaration*> pragmaDeclList=EquivalenceChecking::findPragmaDeclarations(root, option_pragma_name);
    if(pragmaDeclList.size()==0) {
      cerr<<"Error: pragma "<<option_pragma_name<<" marking the fragment not found."<<endl;
      exit(1);
    }
    if(pragmaDeclList.size()>2) {
      cerr<<"Error: pragma "<<option_pragma_name<<" : too many markers found ("<<pragmaDeclList.size()<<")"<<endl;
      exit(1);
    }
    cout<<"STATUS: Fragment marked by "<<option_pragma_name<<": correctly identified."<<endl;

    ROSE_ASSERT(pragmaDeclList.size()==1);
    list<SgPragmaDeclaration*>::iterator i=pragmaDeclList.begin();
    fragmentStartNode=*i;
  }

  if(boolOptions["eliminate-arrays"]) {
    //analyzer.initializeVariableIdMapping(sageProject);
    Specialization speci;
    speci.transformArrayProgram(sageProject, &analyzer);
    sageProject->unparse(0,0);
    exit(0);
  }

  cout << "INIT: creating solver."<<endl;
  if(option_specialize_fun_name!="") {
    analyzer.initializeSolver1(option_specialize_fun_name,root,true);
  } else {
    analyzer.initializeSolver1("main",root,false);
  }
  analyzer.initLabeledAssertNodes(sageProject);

  if(args.count("reconstruct-assert-paths")) {
    analyzer.setSolver(9);
    analyzer.setStartPState(*analyzer.popWorkList()->pstate());
  }

  if(args.count("pattern-search-max-depth") || args.count("pattern-search-max-suffix") 
      || args.count("pattern-search-asserts") || args.count("pattern-search-max-exploration")) {
    cout << "INFO: at least one of the parameters of mode \"pattern search\" was set. Choosing solver 10." << endl;
    analyzer.setSolver(10);
    analyzer.setStartPState(*analyzer.popWorkList()->pstate());
  }

  double initRunTime=timer.getElapsedTimeInMilliSec();

  timer.start();
  cout << "=============================================================="<<endl;
  if(boolOptions["semantic-fold"]) {
        analyzer.setSolver(4);
  }
  if(!analyzer.getModeLTLDriven()) {
    analyzer.runSolver();
  }

  if(boolOptions["post-semantic-fold"]) {
    cout << "Performing post semantic folding (this may take some time):"<<endl;
    analyzer.semanticFoldingOfTransitionGraph();
  }

  double analysisRunTime=timer.getElapsedTimeInMilliSec();
  cout << "=============================================================="<<endl;
  double extractAssertionTracesTime= 0;
  int maxOfShortestAssertInput = -1;
  if ( boolOptions["with-counterexamples"] || boolOptions["with-assert-counterexamples"]) {
    cout << "STATUS: extracting assertion traces (this may take some time)"<<endl;
    timer.start();
    maxOfShortestAssertInput = analyzer.extractAssertionTraces();
    extractAssertionTracesTime = timer.getElapsedTimeInMilliSec();
    if (maxOfShortestAssertInput > -1) {
      cout << "STATUS: maximum input sequence length of first assert occurences: " << maxOfShortestAssertInput << endl;
    } else {
      cout << "STATUS: determining maximum of shortest assert counterexamples not possible. " << endl;
    }
  }

  double determinePrefixDepthTime= 0;
  int inputSeqLengthCovered = -1;
  if ( boolOptions["determine-prefix-depth"]) {
    cout << "ERROR: option \"determine-prefix-depth\" currenlty deactivated." << endl;
    return 1;
  }
  double totalInputTracesTime = extractAssertionTracesTime + determinePrefixDepthTime;

  cout << "=============================================================="<<endl;
  bool withCe = boolOptions["with-counterexamples"] || boolOptions["with-assert-counterexamples"];
  analyzer.reachabilityResults.printResults("YES (REACHABLE)", "NO (UNREACHABLE)", "error_", withCe);

  if (args.count("csv-assert")) {
    string filename=args["csv-assert"].as<string>().c_str();
    analyzer.reachabilityResults.writeFile(filename.c_str(), false, 0, withCe);
    cout << "Reachability results written to file \""<<filename<<"\"." <<endl;
    cout << "=============================================================="<<endl;
  }
  if(boolOptions["tg-ltl-reduced"]) {
    analyzer.stdIOFoldingOfTransitionGraph();
    cout << "Size of transition graph after reduction : "<<analyzer.getTransitionGraph()->size()<<endl;
  }
  if(boolOptions["eliminate-stg-back-edges"]) {
    int numElim=analyzer.getTransitionGraph()->eliminateBackEdges();
    cout<<"STATUS: eliminated "<<numElim<<" STG back edges."<<endl;
  }

  analyzer.reachabilityResults.printResultsStatistics();
  cout << "=============================================================="<<endl;

  long pstateSetSize=analyzer.getPStateSet()->size();
  long pstateSetBytes=analyzer.getPStateSet()->memorySize();
  long pstateSetMaxCollisions=analyzer.getPStateSet()->maxCollisions();
  long pstateSetLoadFactor=analyzer.getPStateSet()->loadFactor();
  long eStateSetSize=analyzer.getEStateSet()->size();
  long eStateSetBytes=analyzer.getEStateSet()->memorySize();
  long eStateSetMaxCollisions=analyzer.getEStateSet()->maxCollisions();
  double eStateSetLoadFactor=analyzer.getEStateSet()->loadFactor();
  long transitionGraphSize=analyzer.getTransitionGraph()->size();
  long transitionGraphBytes=transitionGraphSize*sizeof(Transition);
  long numOfconstraintSets=analyzer.getConstraintSetMaintainer()->numberOf();
  long constraintSetsBytes=analyzer.getConstraintSetMaintainer()->memorySize();
  long constraintSetsMaxCollisions=analyzer.getConstraintSetMaintainer()->maxCollisions();
  double constraintSetsLoadFactor=analyzer.getConstraintSetMaintainer()->loadFactor();
  long numOfStdinEStates=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDIN_VAR));
  long numOfStdoutVarEStates=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_VAR));
  long numOfStdoutConstEStates=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_CONST));
  long numOfStderrEStates=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDERR_VAR));
  long numOfFailedAssertEStates=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::FAILED_ASSERT));
  long numOfConstEStates=0;//(analyzer.getEStateSet()->numberOfConstEStates(analyzer.getVariableIdMapping()));
  long numOfStdoutEStates=numOfStdoutVarEStates+numOfStdoutConstEStates;

  long totalMemory=pstateSetBytes+eStateSetBytes+transitionGraphBytes+constraintSetsBytes;

  double totalRunTime=frontEndRunTime+initRunTime+analysisRunTime;

  long pstateSetSizeInf = 0;
  long eStateSetSizeInf = 0;
  long transitionGraphSizeInf = 0;
  long eStateSetSizeStgInf = 0;
  double infPathsOnlyTime = 0;
  double stdIoOnlyTime = 0;

  if(boolOptions["inf-paths-only"]) {
    assert (!boolOptions["keep-error-states"]);
    cout << "STATUS: recursively removing all leaves."<<endl;
    timer.start();
    analyzer.pruneLeavesRec();
    infPathsOnlyTime = timer.getElapsedTimeInMilliSec();

    pstateSetSizeInf=analyzer.getPStateSet()->size();
    eStateSetSizeInf = analyzer.getEStateSet()->size();
    transitionGraphSizeInf = analyzer.getTransitionGraph()->size();
    eStateSetSizeStgInf = (analyzer.getTransitionGraph())->estateSet().size();
  }
  
  if(boolOptions["std-in-only"]) {
    cout << "STATUS: reducing STG to Input-states."<<endl;
    analyzer.reduceGraphInOutWorklistOnly(true,false,boolOptions["keep-error-states"]);
  }

  if(boolOptions["std-out-only"]) {
    cout << "STATUS: reducing STG to output-states."<<endl;
    analyzer.reduceGraphInOutWorklistOnly(false,true,boolOptions["keep-error-states"]);
  }

  if(boolOptions["std-io-only"]) {
    cout << "STATUS: bypassing all non standard I/O states. (P2)"<<endl;
    timer.start();
    //analyzer.removeNonIOStates();  //old version, works correclty but has a long execution time
    analyzer.reduceGraphInOutWorklistOnly(true,true,boolOptions["keep-error-states"]);
    stdIoOnlyTime = timer.getElapsedTimeInMilliSec();
  }

  long eStateSetSizeIoOnly = 0;
  long transitionGraphSizeIoOnly = 0;
  double spotLtlAnalysisTime = 0;

  stringstream statisticsSizeAndLtl;
  stringstream statisticsCegpra;

  if (args.count("check-ltl")) {
    cout<<"INFO: STG size: "<<analyzer.getTransitionGraph()->size()<<endl;
    string ltl_filename = args["check-ltl"].as<string>();
    if(boolOptions["rersmode"]) {  //reduce the graph accordingly, if not already done
      if (!boolOptions["inf-paths-only"] && !boolOptions["keep-error-states"] &&!analyzer.getModeLTLDriven()) {
        cout << "STATUS: recursively removing all leaves (due to RERS-mode)."<<endl;
        timer.start();
        analyzer.pruneLeavesRec();
        infPathsOnlyTime = timer.getElapsedTimeInMilliSec();

        pstateSetSizeInf=analyzer.getPStateSet()->size();
        eStateSetSizeInf = analyzer.getEStateSet()->size();
        transitionGraphSizeInf = analyzer.getTransitionGraph()->size();
        eStateSetSizeStgInf = (analyzer.getTransitionGraph())->estateSet().size();
      }
      if (!boolOptions["std-io-only"] &&!analyzer.getModeLTLDriven()) {
        cout << "STATUS: bypassing all non standard I/O states (due to RERS-mode) (P1)."<<endl;
        timer.start();
        analyzer.reduceGraphInOutWorklistOnly(true, true, boolOptions["keep-error-states"]);
        stdIoOnlyTime = timer.getElapsedTimeInMilliSec();
        printStgSize(analyzer.getTransitionGraph(), "after reducing non-I/O states");
      }
    }
    if(boolOptions["no-input-input"]) {  //delete transitions that indicate two input states without an output in between
      analyzer.removeInputInputTransitions();
      printStgSize(analyzer.getTransitionGraph(), "after reducing input->input transitions");
    }
    bool withCounterexample = false;
    if(boolOptions["with-counterexamples"] || boolOptions["with-ltl-counterexamples"]) {  //output a counter-example input sequence for falsified formulae
      withCounterexample = true;
    }

    timer.start();
    std::set<int> ltlInAlphabet = analyzer.getInputVarValues();
    //take fixed ltl input alphabet if specified, instead of the input values used for stg computation
    if (args.count("ltl-in-alphabet")) {
      string setstring=args["ltl-in-alphabet"].as<string>();
      ltlInAlphabet=Parse::integerSet(setstring);
      cout << "STATUS: LTL input alphabet explicitly selected: "<< setstring << endl;
    }
    //take ltl output alphabet if specifically described, otherwise take the old RERS specific 21...26 (a.k.a. oU...oZ)
    std::set<int> ltlOutAlphabet = Parse::integerSet("{21,22,23,24,25,26}");
    if (args.count("ltl-out-alphabet")) {
      string setstring=args["ltl-out-alphabet"].as<string>();
      ltlOutAlphabet=Parse::integerSet(setstring);
      cout << "STATUS: LTL output alphabet explicitly selected: "<< setstring << endl;
    }
    PropertyValueTable* ltlResults;
    SpotConnection spotConnection(ltl_filename);
    spotConnection.setModeLTLDriven(analyzer.getModeLTLDriven());

    cout << "STATUS: generating LTL results"<<endl;
    bool spuriousNoAnswers = false;
    if (boolOptions["check-ltl-counterexamples"]) {
      spuriousNoAnswers = true;
    }
    spotConnection.checkLtlProperties( *(analyzer.getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample, spuriousNoAnswers);
    spotLtlAnalysisTime=timer.getElapsedTimeInMilliSec();
    ltlResults = spotConnection.getLtlResults();
    if (args.count("cegpra-ltl") || boolOptions["cegpra-ltl-all"]) {
      if (args.count("csv-stats-cegpra")) {
        statisticsCegpra << "init,";
        printStgSize(analyzer.getTransitionGraph(), "initial abstract model", &statisticsCegpra);
        statisticsCegpra << ",na,na";
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_YES);
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_NO);
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_UNKNOWN);
      }
      CounterexampleAnalyzer ceAnalyzer(&analyzer, &statisticsCegpra);
      if (args.count("cegpra-max-iterations")) {
        ceAnalyzer.setMaxCounterexamples(args["cegpra-max-iterations"].as<int>());
      }
      if (boolOptions["cegpra-ltl-all"]) {
        ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(spotConnection, ltlInAlphabet, ltlOutAlphabet);
      } else {  // cegpra for single LTL property
        int property = args["cegpra-ltl"].as<int>();
        ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(property, spotConnection, ltlInAlphabet, ltlOutAlphabet);
      }
    }

    if (boolOptions["check-ltl-counterexamples"]) {
      cout << "STATUS: checking for spurious counterexamples..."<<endl;
      CounterexampleAnalyzer ceAnalyzer(&analyzer);
      RefinementConstraints constraintManager(analyzer.getFlow(), analyzer.getLabeler(), 
                analyzer.getExprAnalyzer(), analyzer.getCFAnalyzer(), analyzer.getVariableIdMapping());
      for (unsigned int i = 0; i < ltlResults->size(); i++) {
        //only check counterexamples
        if (ltlResults->getPropertyValue(i) == PROPERTY_VALUE_NO) {
          std::string counterexample = ltlResults->getCounterexample(i);
          CEAnalysisResult ceAnalysisResult = ceAnalyzer.analyzeCounterexample(counterexample, NULL, true, true);
          if (ceAnalysisResult.analysisResult == CE_TYPE_SPURIOUS) {
            //reset property to unknown
            ltlResults->setCounterexample(i, "");
            ltlResults->setPropertyValue(i, PROPERTY_VALUE_UNKNOWN);
            cout << "INFO: property " << i << " was reset to unknown (spurious counterexample)." << endl;
            if (boolOptions["refinement-constraints-demo"]) {
              constraintManager.addConstraintsByLabel(ceAnalysisResult.spuriousTargetLabel);
            }
          } else if (ceAnalysisResult.analysisResult == CE_TYPE_REAL) {
            //cout << "DEBUG: counterexample is a real counterexample! success" << endl;
          }
        }
      }
      cout << "STATUS: counterexample check done."<<endl;
      if (boolOptions["refinement-constraints-demo"]) {
        cout << "=============================================================="<<endl;
        cout << "STATUS: refinement constraints collected from all LTL counterexamples: "<< endl;
        VariableIdSet varIds = (analyzer.getVariableIdMapping())->getVariableIdSet();
        for (VariableIdSet::iterator i = varIds.begin(); i != varIds.end(); i++) {
          set<int> constraints = constraintManager.getConstraintsForVariable(*i);
          if (constraints.size() > 0) {
            //print the constraints collected for this VariableId
            cout << (analyzer.getVariableIdMapping())->variableName(*i) << ":";
            for (set<int>::iterator k = constraints.begin(); k!= constraints.end(); ++k) {
              if (k != constraints.begin()) {
                cout << ",";
              }
              cout << *k;
            }
            cout << endl;
          }
        }
        cout << "=============================================================="<<endl;
      }
    }

    ltlResults-> printResults("YES (verified)", "NO (falsified)", "ltl_property_", withCounterexample);
    cout << "=============================================================="<<endl;
    ltlResults->printResultsStatistics();
    cout << "=============================================================="<<endl;
    if (args.count("csv-spot-ltl")) {  //write results to a file instead of displaying them directly
      std::string csv_filename = args["csv-spot-ltl"].as<string>();
      cout << "STATUS: writing ltl results to file: " << csv_filename << endl;
      ltlResults->writeFile(csv_filename.c_str(), false, 0, withCounterexample);
    }
    if (args.count("csv-stats-size-and-ltl")) {
      printStgSize(analyzer.getTransitionGraph(), "final model", &statisticsSizeAndLtl);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_YES);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_NO);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_UNKNOWN);
    }
    delete ltlResults;
    ltlResults = NULL;

    //temporaryTotalRunTime = totalRunTime + infPathsOnlyTime + stdIoOnlyTime + spotLtlAnalysisTime;
    //printAnalyzerStatistics(analyzer, temporaryTotalRunTime, "LTL check complete. Reduced transition system:");
  }
  double totalLtlRunTime =  infPathsOnlyTime + stdIoOnlyTime + spotLtlAnalysisTime;

  // TEST
  if (boolOptions["generate-assertions"]) {
    AssertionExtractor assertionExtractor(&analyzer);
    assertionExtractor.computeLabelVectorOfEStates();
    assertionExtractor.annotateAst();
    AstAnnotator ara(analyzer.getLabeler());
    ara.annotateAstAttributesAsCommentsBeforeStatements(sageProject,"ctgen-pre-condition");
    cout << "STATUS: Generated assertions."<<endl;
  }
  double arrayUpdateExtractionRunTime=0.0;
  double arrayUpdateSsaNumberingRunTime=0.0;
  double sortingAndIORunTime=0.0;
  double verifyUpdateSequenceRaceConditionRunTime=0.0;
  
  int verifyUpdateSequenceRaceConditionsResult=-1;
  int verifyUpdateSequenceRaceConditionsTotalLoopNum=-1;
  int verifyUpdateSequenceRaceConditionsParLoopNum=-1;

  if(args.count("dump-sorted")>0 || args.count("dump-non-sorted")>0) {
    Specialization speci;
    if (boolOptions["visualize-read-write-sets"]) {
      speci.setVisualizeReadWriteAccesses(true);
    }
    ArrayUpdatesSequence arrayUpdates;
    cout<<"STATUS: performing array analysis on STG."<<endl;
    cout<<"STATUS: identifying array-update operations in STG and transforming them."<<endl;

    Label fragmentStartLabel=Labeler::NO_LABEL;
    if(fragmentStartNode!=0) {
      fragmentStartLabel=analyzer.getLabeler()->getLabel(fragmentStartNode);
      cout<<"INFO: Fragment: start-node: "<<fragmentStartNode<<"  start-label: "<<fragmentStartLabel<<endl;
      cout<<"INFO: Fragment: start-node: currently not supported."<<endl;
    }
    
    bool useConstSubstitutionRule=boolOptions["rule-const-subst"];

    timer.start();
#if 1
    speci.extractArrayUpdateOperations(&analyzer,
                                       arrayUpdates,
                                       rewriteSystem,
                                       useConstSubstitutionRule
                                       );
#endif
    arrayUpdateExtractionRunTime=timer.getElapsedTimeInMilliSec();

    if(boolOptions["verify-update-sequence-race-conditions"]) {
      SgNode* root=analyzer.startFunRoot;
      VariableId parallelIterationVar;
      LoopInfoSet loopInfoSet=EquivalenceChecking::determineLoopInfoSet(root,analyzer.getVariableIdMapping(), analyzer.getLabeler());
      cout<<"INFO: number of iteration vars: "<<loopInfoSet.size()<<endl;
      verifyUpdateSequenceRaceConditionsTotalLoopNum=loopInfoSet.size();
      verifyUpdateSequenceRaceConditionsParLoopNum=Specialization::numParLoops(loopInfoSet, analyzer.getVariableIdMapping());
      timer.start();
      verifyUpdateSequenceRaceConditionsResult=speci.verifyUpdateSequenceRaceConditions(loopInfoSet,arrayUpdates,analyzer.getVariableIdMapping());
      verifyUpdateSequenceRaceConditionRunTime=timer.getElapsedTimeInMilliSec();
    }

    if(boolOptions["print-update-infos"]) {
      speci.printUpdateInfos(arrayUpdates,analyzer.getVariableIdMapping());
    }
    cout<<"STATUS: establishing array-element SSA numbering."<<endl;
    timer.start();
    speci.createSsaNumbering(arrayUpdates, analyzer.getVariableIdMapping());
    arrayUpdateSsaNumberingRunTime=timer.getElapsedTimeInMilliSec();
    
    if(args.count("dump-non-sorted")) {
      string filename=args["dump-non-sorted"].as<string>();
      speci.writeArrayUpdatesToFile(arrayUpdates, filename, SAR_SSA, false);
    }
    if(args.count("dump-sorted")) {
      timer.start();
      string filename=args["dump-sorted"].as<string>();
      speci.writeArrayUpdatesToFile(arrayUpdates, filename, SAR_SSA, true);
      sortingAndIORunTime=timer.getElapsedTimeInMilliSec();
    }
    totalRunTime+=arrayUpdateExtractionRunTime+verifyUpdateSequenceRaceConditionRunTime+arrayUpdateSsaNumberingRunTime+sortingAndIORunTime;
  }

  double overallTime =totalRunTime + totalInputTracesTime + totalLtlRunTime;

  // MS: all measurements are available here. We can print any information also on screen.
  cout <<color("white");
  cout << "=============================================================="<<endl;
  cout <<color("normal")<<"STG generation and assertion analysis complete"<<color("white")<<endl;
  cout << "=============================================================="<<endl;
  cout << "Number of stdin-estates        : "<<color("cyan")<<numOfStdinEStates<<color("white")<<endl;
  cout << "Number of stdoutvar-estates    : "<<color("cyan")<<numOfStdoutVarEStates<<color("white")<<endl;
  cout << "Number of stdoutconst-estates  : "<<color("cyan")<<numOfStdoutConstEStates<<color("white")<<endl;
  cout << "Number of stderr-estates       : "<<color("cyan")<<numOfStderrEStates<<color("white")<<endl;
  cout << "Number of failed-assert-estates: "<<color("cyan")<<numOfFailedAssertEStates<<color("white")<<endl;
  cout << "Number of const estates        : "<<color("cyan")<<numOfConstEStates<<color("white")<<endl;
  cout << "=============================================================="<<endl;
  cout << "Number of pstates              : "<<color("magenta")<<pstateSetSize<<color("white")<<" (memory: "<<color("magenta")<<pstateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<pstateSetLoadFactor<<  "/"<<pstateSetMaxCollisions<<")"<<endl;
  cout << "Number of estates              : "<<color("cyan")<<eStateSetSize<<color("white")<<" (memory: "<<color("cyan")<<eStateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<eStateSetLoadFactor<<  "/"<<eStateSetMaxCollisions<<")"<<endl;
  cout << "Number of transitions          : "<<color("blue")<<transitionGraphSize<<color("white")<<" (memory: "<<color("blue")<<transitionGraphBytes<<color("white")<<" bytes)"<<endl;
  cout << "Number of constraint sets      : "<<color("yellow")<<numOfconstraintSets<<color("white")<<" (memory: "<<color("yellow")<<constraintSetsBytes<<color("white")<<" bytes)"<<" ("<<""<<constraintSetsLoadFactor<<  "/"<<constraintSetsMaxCollisions<<")"<<endl;
  if(analyzer.getNumberOfThreadsToUse()==1 && analyzer.getSolver()==5 && analyzer.getExplorationMode()==Analyzer::EXPL_LOOP_AWARE) {
    cout << "Number of iterations           : "<<analyzer.getIterations()<<"-"<<analyzer.getApproximatedIterations()<<endl;
  }
  cout << "=============================================================="<<endl;
  cout << "Memory total         : "<<color("green")<<totalMemory<<" bytes"<<color("white")<<endl;
  cout << "Time total           : "<<color("green")<<CodeThorn::readableruntime(totalRunTime)<<color("white")<<endl;
  cout << "=============================================================="<<endl;
  cout <<color("normal");
  //printAnalyzerStatistics(analyzer, totalRunTime, "STG generation and assertion analysis complete");

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
    text<<"threads,"<<numberOfThreadsToUse<<endl;
    //    text<<"abstract-and-const-states,"
    //    <<"";

    // iterations (currently only supported for sequential analysis)
    text<<"iterations,";
    if(analyzer.getNumberOfThreadsToUse()==1 && analyzer.getSolver()==5 && analyzer.getExplorationMode()==Analyzer::EXPL_LOOP_AWARE)
      text<<analyzer.getIterations()<<","<<analyzer.getApproximatedIterations();
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
    text<<"input length coverage & longest minimal assert input,"
        <<inputSeqLengthCovered<<", "
        <<maxOfShortestAssertInput<<endl;
    
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

  if (args.count("check-ltl-sol")) {
    string ltl_filename = args["check-ltl-sol"].as<string>();
    // rers mode reduces the STG. In case of ltl-driven mode there is nothing to reduce.
    if(boolOptions["rersmode"] && !analyzer.getModeLTLDriven()) {  //reduce the graph accordingly, if not already done
      if (!boolOptions["inf-paths-only"]) {
        cout << "STATUS: recursively removing all leaves (due to RERS-mode)."<<endl;
        analyzer.pruneLeavesRec();
      }
      if (!boolOptions["std-io-only"]) {
        cout << "STATUS: bypassing all non standard I/O states (due to RERS-mode). (P3)"<<endl;
        analyzer.removeNonIOStates();
      }
    }
    std::set<int> ltlInAlphabet = analyzer.getInputVarValues();
    //take fixed ltl input alphabet if specified, instead of the input values used for stg computation
    if (args.count("ltl-in-alphabet")) {
      string setstring=args["ltl-in-alphabet"].as<string>();
      ltlInAlphabet=Parse::integerSet(setstring);
      cout << "STATUS: LTL input alphabet explicitly selected: "<< setstring << endl;
    }
    //take ltl output alphabet if specifically described, otherwise the usual 21...26 (a.k.a. oU...oZ)
    std::set<int> ltlOutAlphabet = Parse::integerSet("{21,22,23,24,25,26}");
    if (args.count("ltl-out-alphabet")) {
      string setstring=args["ltl-out-alphabet"].as<string>();
      ltlOutAlphabet=Parse::integerSet(setstring);
      cout << "STATUS: LTL output alphabet explicitly selected: "<< setstring << endl;
    }
    SpotConnection* spotConnection = new SpotConnection();
    spotConnection->setModeLTLDriven(analyzer.getModeLTLDriven());
    spotConnection->compareResults( *(analyzer.getTransitionGraph()) , ltl_filename, ltlInAlphabet, ltlOutAlphabet);
    cout << "=============================================================="<<endl;
  }

  Visualizer visualizer(analyzer.getLabeler(),analyzer.getVariableIdMapping(),analyzer.getFlow(),analyzer.getPStateSet(),analyzer.getEStateSet(),analyzer.getTransitionGraph());
  if(boolOptions["viz"]) {
    cout << "generating graphviz files:"<<endl;
    string dotFile="digraph G {\n";
    dotFile+=visualizer.transitionGraphToDot();
    dotFile+="}\n";
    write_file("transitiongraph1.dot", dotFile);
    cout << "generated transitiongraph1.dot."<<endl;
    string dotFile3=visualizer.foldedTransitionGraphToDot();
    write_file("transitiongraph2.dot", dotFile3);
    cout << "generated transitiongraph2.dot."<<endl;

    string datFile1=(analyzer.getTransitionGraph())->toString();
    write_file("transitiongraph1.dat", datFile1);
    cout << "generated transitiongraph1.dat."<<endl;
    
    assert(analyzer.startFunRoot);
    //analyzer.generateAstNodeInfo(analyzer.startFunRoot);
    //dotFile=astTermWithNullValuesToDot(analyzer.startFunRoot);
    analyzer.generateAstNodeInfo(sageProject);
    cout << "generated node info."<<endl;
    dotFile=SPRAY::AstTerm::functionAstTermsWithNullValuesToDot(sageProject);
    write_file("ast.dot", dotFile);
    cout << "generated ast.dot."<<endl;
    
    write_file("cfg.dot", analyzer.flow.toDot(analyzer.cfanalyzer->getLabeler()));
    cout << "generated cfg.dot."<<endl;
    cout << "=============================================================="<<endl;
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

  if (args.count("spot-stg")) {
    string filename=args["spot-stg"].as<string>();
    cout << "generating spot IO STG file:"<<filename<<endl;
    if(boolOptions["rersmode"]) {  //reduce the graph accordingly, if not already done
      if (!boolOptions["inf-paths-only"]) {
        cout << "STATUS: recursively removing all leaves (due to RERS-mode)."<<endl;
        analyzer.pruneLeavesRec();
      }
      if (!boolOptions["std-io-only"]) {
        cout << "STATUS: bypassing all non standard I/O states (due to RERS-mode). (P4)"<<endl;
        analyzer.removeNonIOStates();
      }
    }
    string spotSTG=analyzer.generateSpotSTG();
    write_file(filename, spotSTG);
    cout << "=============================================================="<<endl;
  }

  // InputPathGenerator
#if 1
  {
    if(args.count("iseq-file")) {
      int iseqLen=0;
      if(args.count("iseq-length")) {
        iseqLen=args["iseq-length"].as<int>();
      } else {
        cerr<<"Error: input-sequence file specified, but no sequence length."<<endl;
        exit(1);
      }
      string fileName=args["iseq-file"].as<string>();
      cout<<"STATUS: computing input sequences of length "<<iseqLen<<endl;
      IOSequenceGenerator iosgen;
      if(args.count("iseq-random-num")) {
        int randomNum=args["iseq-random-num"].as<int>();
        cout<<"STATUS: reducing input sequence set to "<<randomNum<<" random elements."<<endl;
        iosgen.computeRandomInputPathSet(iseqLen,*analyzer.getTransitionGraph(),randomNum);
      } else {
        iosgen.computeInputPathSet(iseqLen,*analyzer.getTransitionGraph());
      }
      cout<<"STATUS: generating input sequence file "<<fileName<<endl;
      iosgen.generateFile(fileName);
    } else {
      if(args.count("iseq-length")) {
        cerr<<"Error: input sequence length specified without also providing a file name (use option --iseq-file)."<<endl;
        exit(1);
      }
    }
  }
#endif

#if 0
  {
    cout << "EStateSet:\n"<<analyzer.getEStateSet()->toString(analyzer.getVariableIdMapping())<<endl;
    cout << "ConstraintSet:\n"<<analyzer.getConstraintSetMaintainer()->toString()<<endl;
    if(analyzer.variableValueMonitor.isActive())
      cout << "VariableValueMonitor:\n"<<analyzer.variableValueMonitor.toString(analyzer.getVariableIdMapping())<<endl;
    cout << "MAP:"<<endl;
    cout << analyzer.getLabeler()->toString();
  }
#endif

  if (boolOptions["annotate-terms"]) {
    // TODO: it might be useful to be able to select certain analysis results to be only annotated
    cout << "INFO: Annotating term representations."<<endl;
    attachTermRepresentation(sageProject);
    AstAnnotator ara(analyzer.getLabeler());
    ara.annotateAstAttributesAsCommentsBeforeStatements(sageProject,"codethorn-term-representation");
  }

  if (boolOptions["annotate-terms"]||boolOptions["generate-assertions"]) {
    cout << "INFO: Generating annotated program."<<endl;
    //backend(sageProject);
    sageProject->unparse(0,0);
  }
  // reset terminal
  cout<<color("normal")<<"done."<<endl;
  
  } catch(char const* str) {
    cerr << "*Exception raised: " << str << endl;
    return 1;
  } catch(string str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
  } catch(...) {
    cerr << "Unknown exception raised." << endl;
    return 1;
 }
  return 0;
}

void CodeThorn::printStgSize(TransitionGraph* model, string optionalComment, stringstream* csvOutput) {
  long inStates = model->numberOfObservableStates(true, false, false);
  long outStates = model->numberOfObservableStates(false, true, false);
  long errStates = model->numberOfObservableStates(false, false, true);
  cout << "STATUS: STG size";
  if (optionalComment != "") {
    cout << " (" << optionalComment << ")"; 
  }
  cout << ". #transitions: " << model->size();
  cout << ", #states: " << model->estateSet().size() 
       << " (" << inStates << " in / " << outStates << " out / " << errStates << " err)" << endl;
  if (csvOutput) {
    (*csvOutput) << model->size() <<","<< model->estateSet().size() <<","<< inStates <<","<< outStates <<","<< errStates; 
  }
}

//currently not used. conceived due to different statistics after LTL evaluation that could be printed in the same way as above.
void CodeThorn::printAnalyzerStatistics(Analyzer& analyzer, double totalRunTime, string title) {
  long pstateSetSize=analyzer.getPStateSet()->size();
  long pstateSetBytes=analyzer.getPStateSet()->memorySize();
  long pstateSetMaxCollisions=analyzer.getPStateSet()->maxCollisions();
  long pstateSetLoadFactor=analyzer.getPStateSet()->loadFactor();
  long eStateSetSize=analyzer.getEStateSet()->size();
  long eStateSetBytes=analyzer.getEStateSet()->memorySize();
  long eStateSetMaxCollisions=analyzer.getEStateSet()->maxCollisions();
  double eStateSetLoadFactor=analyzer.getEStateSet()->loadFactor();
  long transitionGraphSize=analyzer.getTransitionGraph()->size();
  long transitionGraphBytes=transitionGraphSize*sizeof(Transition);
  long numOfconstraintSets=analyzer.getConstraintSetMaintainer()->numberOf();
  long constraintSetsBytes=analyzer.getConstraintSetMaintainer()->memorySize();
  long constraintSetsMaxCollisions=analyzer.getConstraintSetMaintainer()->maxCollisions();
  double constraintSetsLoadFactor=analyzer.getConstraintSetMaintainer()->loadFactor();

  long numOfStdinEStates=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDIN_VAR));
  long numOfStdoutVarEStates=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_VAR));
  long numOfStdoutConstEStates=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_CONST));
  long numOfStderrEStates=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDERR_VAR));
  long numOfFailedAssertEStates=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::FAILED_ASSERT));
  long numOfConstEStates=(analyzer.getEStateSet()->numberOfConstEStates(analyzer.getVariableIdMapping()));
  //long numOfStdoutEStates=numOfStdoutVarEStates+numOfStdoutConstEStates;
  
  long totalMemory=pstateSetBytes+eStateSetBytes+transitionGraphBytes+constraintSetsBytes;

  cout <<color("white");
  cout << "=============================================================="<<endl;
  cout <<color("normal")<<title<<color("white")<<endl;
  cout << "=============================================================="<<endl;
  cout << "Number of stdin-estates        : "<<color("cyan")<<numOfStdinEStates<<color("white")<<endl;
  cout << "Number of stdoutvar-estates    : "<<color("cyan")<<numOfStdoutVarEStates<<color("white")<<endl;
  cout << "Number of stdoutconst-estates  : "<<color("cyan")<<numOfStdoutConstEStates<<color("white")<<endl;
  cout << "Number of stderr-estates       : "<<color("cyan")<<numOfStderrEStates<<color("white")<<endl;
  cout << "Number of failed-assert-estates: "<<color("cyan")<<numOfFailedAssertEStates<<color("white")<<endl;
  cout << "Number of const estates        : "<<color("cyan")<<numOfConstEStates<<color("white")<<endl;
  cout << "=============================================================="<<endl;
  cout << "Number of pstates              : "<<color("magenta")<<pstateSetSize<<color("white")<<" (memory: "<<color("magenta")<<pstateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<pstateSetLoadFactor<<  "/"<<pstateSetMaxCollisions<<")"<<endl;
  cout << "Number of estates              : "<<color("cyan")<<eStateSetSize<<color("white")<<" (memory: "<<color("cyan")<<eStateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<eStateSetLoadFactor<<  "/"<<eStateSetMaxCollisions<<")"<<endl;
  cout << "Number of transitions          : "<<color("blue")<<transitionGraphSize<<color("white")<<" (memory: "<<color("blue")<<transitionGraphBytes<<color("white")<<" bytes)"<<endl;
  cout << "Number of constraint sets      : "<<color("yellow")<<numOfconstraintSets<<color("white")<<" (memory: "<<color("yellow")<<constraintSetsBytes<<color("white")<<" bytes)"<<" ("<<""<<constraintSetsLoadFactor<<  "/"<<constraintSetsMaxCollisions<<")"<<endl;
  cout << "=============================================================="<<endl;
  cout << "Memory total         : "<<color("green")<<totalMemory<<" bytes"<<color("white")<<endl;
  cout << "Time total           : "<<color("green")<<CodeThorn::readableruntime(totalRunTime)<<color("white")<<endl;
  cout << "=============================================================="<<endl;
  cout <<color("normal");
}

