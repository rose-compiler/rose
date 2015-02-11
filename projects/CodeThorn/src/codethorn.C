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
#include <boost/program_options.hpp>
#include <map>
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
#include "Specialization.h"
#include <map>

// test
#include "Evaluator.h"

using namespace std;

namespace po = boost::program_options;
using namespace CodeThorn;

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

typedef map<SgForStatement*,SgPragmaDeclaration*> ForStmtToOmpPragmaMap;

// finds the list of pragmas (in traversal order) with the prefix 'prefix' (e.g. '#pragma omp parallel' is found for prefix 'omp')
ForStmtToOmpPragmaMap createOmpPragmaForStmtMap(SgNode* root) {
  //cout<<"PROGRAM:"<<root->unparseToString()<<endl;
  ForStmtToOmpPragmaMap map;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin(); i!=ast.end();++i) {
    if(SgPragmaDeclaration* pragmaDecl=isSgPragmaDeclaration(*i)) {
      string foundPragmaKeyWord=SageInterface::extractPragmaKeyword(pragmaDecl);
      //cout<<"DEBUG: PRAGMAKEYWORD:"<<foundPragmaKeyWord<<endl;
      if(foundPragmaKeyWord=="omp"||foundPragmaKeyWord=="simd") {
        RoseAst::iterator j=i;
        j.skipChildrenOnForward();
        ++j;
        if(SgForStatement* forStmt=isSgForStatement(*j)) {
          map[forStmt]=pragmaDecl;
        } else {
          cout<<"DEBUG: NOT a forstmt: "<<(*i)->unparseToString()<<endl;
        }
      }
    }
  }
  return map;
}

// finds the list of pragmas (in traversal order) with the prefix 'prefix' (e.g. '#pragma omp parallel' is found for prefix 'omp')
list<SgPragmaDeclaration*> findPragmaDeclarations(SgNode* root, string pragmaKeyWord) {
  list<SgPragmaDeclaration*> pragmaList;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin(); i!=ast.end();++i) {
    if(SgPragmaDeclaration* pragmaDecl=isSgPragmaDeclaration(*i)) {
      string foundPragmaKeyWord=SageInterface::extractPragmaKeyword(pragmaDecl);
      //cout<<"DEBUG: PRAGMAKEYWORD:"<<foundPragmaKeyWord<<endl;
      if(pragmaKeyWord==foundPragmaKeyWord || "end"+pragmaKeyWord==foundPragmaKeyWord) {
        pragmaList.push_back(pragmaDecl);
      }
    }
  }
  return pragmaList;
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
  // Polyhedral test codes
  setAstNodeVariant(V_SgPlusAssignOp, true);
  setAstNodeVariant(V_SgMinusAssignOp, true);
  setAstNodeVariant(V_SgMultAssignOp, true);
  setAstNodeVariant(V_SgDivAssignOp, true);
  setAstNodeVariant(V_SgPntrArrRefExp, true);
  setAstNodeVariant(V_SgPragmaDeclaration, true);
  setAstNodeVariant(V_SgPragma, true);
  setAstNodeVariant(V_SgDoubleVal, true);
  setAstNodeVariant(V_SgFloatVal, true);
  //SgIntegerDivideAssignOp

  //more general test codes
  setAstNodeVariant(V_SgPointerDerefExp, true);
  setAstNodeVariant(V_SgNullExpression, true);
  setAstNodeVariant(V_SgSizeOfOp,true);

}

bool isInsideOmpParallelFor(SgNode* node, ForStmtToOmpPragmaMap& forStmtToPragmaMap) {
  while(!isSgForStatement(node)||isSgProject(node))
    node=node->get_parent();
  ROSE_ASSERT(!isSgProject(node));
  // assuming only omp parallel for exist
  return forStmtToPragmaMap.find(isSgForStatement(node))!=forStmtToPragmaMap.end();
}

LoopInfoSet determineLoopInfoSet(SgNode* root, VariableIdMapping* variableIdMapping, Labeler* labeler) {
  cout<<"DEBUG: loop info set and determine iteration vars."<<endl;
  ForStmtToOmpPragmaMap forStmtToPragmaMap=createOmpPragmaForStmtMap(root);
  cout<<"DEBUG: found "<<forStmtToPragmaMap.size()<<" omp loops."<<endl;
  RoseAst ast(root);
  AstMatching m;
  string matchexpression="$FORSTMT=SgForStatement(_,_,SgPlusPlusOp($ITERVAR=SgVarRefExp)|$FORSTMT=SgMinusMinusOp($ITERVAR=SgVarRefExp),..)";
  MatchResult r=m.performMatching(matchexpression,root);
  LoopInfoSet loopInfoSet;
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    SgVarRefExp* node=isSgVarRefExp((*i)["$ITERVAR"]);
    ROSE_ASSERT(node);
    //cout<<"DEBUG: MATCH: "<<node->unparseToString()<<astTermWithNullValuesToString(node)<<endl;
    LoopInfo loopInfo;
    loopInfo.iterationVarId=variableIdMapping->variableId(node);
    loopInfo.iterationVarType=isInsideOmpParallelFor(node,forStmtToPragmaMap)?ITERVAR_PAR:ITERVAR_SEQ;
    SgNode* forNode=(*i)["$FORSTMT"];
    // WORKAROUND 1
    // TODO: investigate why the for pointer is not stored in the same match-result
    if(forNode==0) {
      forNode=node; // init
      while(!isSgForStatement(forNode)||isSgProject(forNode))
        forNode=forNode->get_parent();
    }
    ROSE_ASSERT(!isSgProject(forNode));

    loopInfo.forStmt=isSgForStatement(forNode);
    if(loopInfo.forStmt) {
      const SgStatementPtrList& stmtList=loopInfo.forStmt->get_init_stmt();
      ROSE_ASSERT(stmtList.size()==1);
      loopInfo.initStmt=stmtList[0];
      loopInfo.condExpr=loopInfo.forStmt->get_test_expr();
      loopInfo.computeLoopLabelSet(labeler);
      loopInfo.computeOuterLoopsVarIds(variableIdMapping);
    } else {
      cerr<<"WARNING: no for statement found."<<endl;
      if(forNode) {
        cerr<<"for-loop:"<<forNode->unparseToString()<<endl;
      } else {
        cerr<<"for-loop: 0"<<endl;
      }
    }
    loopInfoSet.push_back(loopInfo);
  }
  return loopInfoSet;
}

class TermRepresentation : public DFAstAttribute {
public:
  TermRepresentation(SgNode* node) : _node(node) {}
  string toString() { return "AstTerm: "+astTermWithNullValuesToString(_node); }
private:
  SgNode* _node;
};

class PointerExprListAnnotation : public DFAstAttribute {
public:
  PointerExprListAnnotation(SgNode* node) : _node(node) {
    //std::cout<<"DEBUG:generated: "+pointerExprToString(node)+"\n";
  }
  string toString() { 
    return "// POINTEREXPR: "+pointerExprToString(_node);
  }
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

void attachPointerExprLists(SgNode* node) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin(); i!=ast.end();++i) {
    SgStatement* stmt=0;
    // SgVariableDeclaration is necessary to allow for pointer initialization
    if((stmt=dynamic_cast<SgExprStatement*>(*i))
       ||(stmt=dynamic_cast<SgVariableDeclaration*>(*i))
       ||(stmt=dynamic_cast<SgReturnStmt*>(*i))
       ) {
      AstAttribute* ara=new PointerExprListAnnotation(stmt);
      stmt->setAttribute("codethorn-pointer-expr-lists",ara);
    }
  }
}

void printAssertStatistics(Analyzer& analyzer, SgProject* sageProject) {
  LabelSet lset=analyzer.getTransitionGraph()->labelSetOfIoOperations(InputOutput::FAILED_ASSERT);
  list<pair<SgLabelStatement*,SgNode*> > assertNodes=analyzer.listOfLabeledAssertNodes(sageProject);
  int reachable=0;
  int unreachable=0;
  for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=assertNodes.begin();i!=assertNodes.end();++i) {
    Label lab=analyzer.getLabeler()->getLabel((*i).second);
    if(lset.find(lab)!=lset.end())
      reachable++;
    else
      unreachable++;
  }
  int n=assertNodes.size();
  assert(reachable+unreachable == n);

  if(boolOptions["rers-binary"]) {
    reachable=0;
    unreachable=0;
    for(int i=0;i<62;i++) {
      if(analyzer.binaryBindingAssert[i])
        reachable++;
      else
        unreachable++;
    }
  }
  cout<<color("white")<<"Assert reachability statistics: "
      <<color("white")<<"YES: "<<color("green")<<reachable
      <<color("white")<<", NO: " <<color("cyan")<<unreachable
      <<color("white")<<", TOTAL: " <<n
      <<endl<<color("normal")

    ;
}

//
string readableruntime(double timeInMilliSeconds) {
  stringstream s;
  double time=timeInMilliSeconds;
  s << std::fixed << std::setprecision(2); // 2 digits past decimal point.
  if(time<1000.0) {
    s<<time<<" ms";
    return s.str();
  } else {
    time=time/1000;
  }
  if(time<60) {
    s<<time<<" secs"; 
    return s.str();
  } else {
    time=time/60;
  }
  if(time<60) {
    s<<time<<" mins"; 
    return s.str();
  } else {
    time=time/60;
  }
  if(time<24) {
    s<<time<<" hours"; 
    return s.str();
  } else {
    time=time/24;
  }
  if(time<31) {
    s<<time<<" days"; 
    return s.str();
  } else {
    time=time/(((double)(365*3+366))/12*4);
  }
  s<<time<<" months"; 
  return s.str();
}

static Analyzer* global_analyzer=0;



int main( int argc, char * argv[] ) {
  string ltl_file;
  try {
    Timer timer;
    timer.start();

  // Command line option handling.
  po::options_description desc
    ("CodeThorn\n"
     "Written by Markus Schordan, Adrian Prantl, and Marc Jasper\n"
     "Supported options");

  desc.add_options()
    ("help,h", "produce this help message")
    ("rose-help", "show help for compiler frontend options")
    ("version,v", "display the version")
    ("internal-checks", "run internal consistency checks (without input program)")
    ("verify", po::value< string >(), "verify all LTL formulae in the file [arg]")
    ("ltl-verifier",po::value< int >(),"specify which ltl-verifier to use [=1|2]")
    ("csv-ltl", po::value< string >(), "output LTL verification results into a CSV file [arg]")
    ("debug-mode",po::value< int >(),"set debug mode [arg]")
    ("csv-spot-ltl", po::value< string >(), "output SPOT's LTL verification results into a CSV file [arg]")
    ("csv-assert", po::value< string >(), "output assert reachability results into a CSV file [arg]")
    ("csv-assert-live", po::value< string >(), "output assert reachability results during analysis into a CSV file [arg]")
    ("csv-stats",po::value< string >(),"output statistics into a CSV file [arg]")
    ("tg1-estate-address", po::value< string >(), "transition graph 1: visualize address [=yes|no]")
    ("tg1-estate-id", po::value< string >(), "transition graph 1: visualize estate-id [=yes|no]")
    ("tg1-estate-properties", po::value< string >(), "transition graph 1: visualize all estate-properties [=yes|no]") 
    ("tg1-estate-predicate", po::value< string >(), "transition graph 1: show estate as predicate [=yes|no]")
    ("tg2-estate-address", po::value< string >(), "transition graph 2: visualize address [=yes|no]")
    ("tg2-estate-id", po::value< string >(), "transition graph 2: visualize estate-id [=yes|no]")
    ("tg2-estate-properties", po::value< string >(),"transition graph 2: visualize all estate-properties [=yes|no]")
    ("tg2-estate-predicate", po::value< string >(), "transition graph 2: show estate as predicate [=yes|no]")
    ("tg-trace", po::value< string >(), "generate STG computation trace [=filename]")
    ("colors",po::value< string >(),"use colors in output [=yes|no]")
    ("report-stdout",po::value< string >(),"report stdout estates during analysis [=yes|no]")
    ("report-stderr",po::value< string >(),"report stderr estates during analysis [=yes|no]")
    ("report-failed-assert",po::value< string >(),
     "report failed assert estates during analysis [=yes|no]")
    ("precision-exact-constraints",po::value< string >(),
     "(experimental) use precise constraint extraction [=yes|no]")
    ("tg-ltl-reduced",po::value< string >(),"(experimental) compute LTL-reduced transition graph based on a subset of computed estates [=yes|no]")
    ("semantic-fold",po::value< string >(),"compute semantically folded state transition graph [=yes|no]")
    ("semantic-elimination",po::value< string >(),"eliminate input-input transitions in STG [=yes|no]")
    ("semantic-explosion",po::value< string >(),"semantic explosion of input states (requires folding and elimination) [=yes|no]")
    ("post-semantic-fold",po::value< string >(),"compute semantically folded state transition graph only after the complete transition graph has been computed. [=yes|no]")
    ("report-semantic-fold",po::value< string >(),"report each folding operation with the respective number of estates. [=yes|no]")
    ("semantic-fold-threshold",po::value< int >(),"Set threshold with <arg> for semantic fold operation (experimental)")
    ("post-collapse-stg",po::value< string >(),"compute collapsed state transition graph after the complete transition graph has been computed. [=yes|no]")
    ("viz",po::value< string >(),"generate visualizations (dot) outputs [=yes|no]")
    ("update-input-var",po::value< string >(),"For testing purposes only. Default is Yes. [=yes|no]")
    ("run-rose-tests",po::value< string >(),"Run ROSE AST tests. [=yes|no]")
    ("reduce-cfg",po::value< string >(),"Reduce CFG nodes which are not relevant for the analysis. [=yes|no]")
    ("threads",po::value< int >(),"Run analyzer in parallel using <arg> threads (experimental)")
    ("display-diff",po::value< int >(),"Print statistics every <arg> computed estates.")
    ("solver",po::value< int >(),"Set solver <arg> to use (one of 1,2,3).")
    ("ltl-verbose",po::value< string >(),"LTL verifier: print log of all derivations.")
    ("ltl-output-dot",po::value< string >(),"LTL visualization: generate dot output.")
    ("ltl-show-derivation",po::value< string >(),"LTL visualization: show derivation in dot output.")
    ("ltl-show-node-detail",po::value< string >(),"LTL visualization: show node detail in dot output.")
    ("ltl-collapsed-graph",po::value< string >(),"LTL visualization: show collapsed graph in dot output.")
    ("input-values",po::value< string >(),"specify a set of input values (e.g. \"{1,2,3}\")")
    ("input-values-as-constraints",po::value<string >(),"represent input var values as constraints (otherwise as constants in PState)")
    ("input-sequence",po::value< string >(),"specify a sequence of input values (e.g. \"[1,2,3]\")")
    ("arith-top",po::value< string >(),"Arithmetic operations +,-,*,/,% always evaluate to top [=yes|no]")
    ("abstract-interpreter",po::value< string >(),"Run analyzer in abstract interpreter mode. Use [=yes|no]")
    ("rers-binary",po::value< string >(),"Call rers binary functions in analysis. Use [=yes|no]")
    ("print-all-options",po::value< string >(),"print all yes/no command line options.")
    ("eliminate-arrays",po::value< string >(), "transform all arrays into single variables.")
    ("annotate-results",po::value< string >(),"annotate results in program and output program (using ROSE unparser).")
    ("generate-assertions",po::value< string >(),"generate assertions (pre-conditions) in program and output program (using ROSE unparser).")
    ("rersformat",po::value< int >(),"Set year of rers format (2012, 2013).")
    ("max-transitions",po::value< int >(),"Passes (possibly) incomplete STG to verifier after max transitions (default: no limit).")
    ("max-transitions-forced-top",po::value< int >(),"Performs approximation after <arg> transitions (default: no limit).")
    ("variable-value-threshold",po::value< int >(),"sets a threshold for the maximum number of different values are stored for each variable.")
    ("dot-io-stg", po::value< string >(), "output STG with explicit I/O node information in dot file [arg]")
    ("stderr-like-failed-assert", po::value< string >(), "treat output on stderr similar to a failed assert [arg] (default:no)")
    ("rersmode", po::value< string >(), "sets several options such that RERS-specifics are utilized and observed.")
    ("rers-numeric", po::value< string >(), "print rers I/O values as raw numeric numbers.")
    ("exploration-mode",po::value< string >(), " set mode in which state space is explored ([breadth-first], depth-first, loop-aware)")
    ("eliminate-stg-back-edges",po::value< string >(), " eliminate STG back-edges (STG becomes a tree).")
    ("spot-stg",po::value< string >(), " generate STG in SPOT-format in file [arg]")
    ("dump-sorted",po::value< string >(), " [experimental] generates sorted array updates in file <file>")
    ("dump-non-sorted",po::value< string >(), " [experimental] generates non-sorted array updates in file <file>")
    ("print-update-infos",po::value< string >(), "[experimental] print information about array updates on stdout")
    ("verify-update-sequence-race-conditions",po::value< string >(), "[experimental] check race conditions of update sequence")
    ("rule-const-subst",po::value< string >(), " [experimental] use const-expr substitution rule <arg>")
    ("limit-to-fragment",po::value< string >(), "the argument is used to find fragments marked by two prgagmas of that '<name>' and 'end<name>'")
    ("rewrite","rewrite AST applying all rewrite system rules.")
    ("specialize-fun-name", po::value< string >(), "function of name [arg] to be specialized")
    ("specialize-fun-param", po::value< vector<int> >(), "function parameter number to be specialized (starting at 1)")
    ("specialize-fun-const", po::value< vector<int> >(), "constant [arg], the param is to be specialized to.")
    ("iseq-file", po::value< string >(), "compute input sequence and generate file [arg]")
    ("iseq-length", po::value< int >(), "set length [arg] of input sequence to be computed.")
    ("iseq-random-num", po::value< int >(), "select random search and number of paths.")
    ("inf-paths-only", po::value< string >(), "recursively prune the graph so that no leaves exist [=yes|no]")
    ("std-io-only", po::value< string >(), "bypass and remove all states that are not standard I/O [=yes|no]")
    ("std-in-only", po::value< string >(), "bypass and remove all states that are not input-states [=yes|no]")
    ("std-out-only", po::value< string >(), "bypass and remove all states that are not output-states [=yes|no]")
    ("check-ltl", po::value< string >(), "take a text file of LTL I/O formulae [arg] and check whether or not the analyzed program satisfies these formulae. Formulae should start with '('. Use \"csv-spot-ltl\" option to specify an output csv file for the results.")
    ("check-ltl-sol", po::value< string >(), "take a source code file and an LTL formulae+solutions file ([arg], see RERS downloads for examples). Display if the formulae are satisfied and if the expected solutions are correct.")
    ("ltl-in-alphabet",po::value< string >(),"specify an input alphabet used by the LTL formulae (e.g. \"{1,2,3}\")")
    ("ltl-out-alphabet",po::value< string >(),"specify an output alphabet used by the LTL formulae (e.g. \"{19,20,21,22,23,24,25,26}\")")
    ("io-reduction", po::value< int >(), "(work in progress) reduce the transition system to only input/output/worklist states after every <arg> computed EStates.")
    ("no-input-input",  po::value< string >(), "remove transitions where one input states follows another without any output in between. Removal occurs before the LTL check. [=yes|no]")
    ("with-counterexamples", po::value< string >(), "adds counterexample traces to the analysis results. Applies to reachable assertions (work in progress) and falsified LTL properties. [=yes|no]")
    ("with-assert-counterexamples", po::value< string >(), "report counterexamples leading to failing assertion states (work in progress) [=yes|no]")
    ("with-ltl-counterexamples", po::value< string >(), "report counterexamples that violate LTL properties [=yes|no]")
    ("counterexamples-with-output", po::value< string >(), "reported counterexamples for LTL or reachability properties also include output values [=yes|no]")
    ("check-ltl-counterexamples", po::value< string >(), "report ltl counterexamples if and only if they are not spurious [=yes|no]")
    ("refinement-constraints-demo", po::value< string >(), "display constraints that are collected in order to later on help a refined analysis avoid spurious counterexamples. [=yes|no]")
    ("incomplete-stg", po::value< string >(), "set to true if the generated STG will not contain all possible execution paths (e.g. if only a subset of the input values is used). [=yes|no]")
    ("determine-prefix-depth", po::value< string >(), "if possible, display a guarantee about the length of the discovered prefix of possible program traces. [=yes|no]")
    ("minimize-states", po::value< string >(), "does not store single successor states (minimizes number of states).")
    ;

  po::store(po::command_line_parser(argc, argv).
        options(desc).allow_unregistered().run(), args);
  po::notify(args);

  if (args.count("help")) {
    cout << desc << "\n";
    return 0;
  }

  if (args.count("rose-help")) {
    argv[1] = strdup("--help");
  }

  if (args.count("version")) {
    cout << "CodeThorn version 1.5.0\n";
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
  boolOptions.registerOption("report-stdout",false);
  boolOptions.registerOption("report-stderr",false);
  boolOptions.registerOption("report-failed-assert",false);
  boolOptions.registerOption("precision-exact-constraints",false);
  boolOptions.registerOption("tg-ltl-reduced",false);
  boolOptions.registerOption("semantic-fold",false);
  boolOptions.registerOption("semantic-elimination",false);
  boolOptions.registerOption("semantic-explosion",false);
  boolOptions.registerOption("post-semantic-fold",false);
  boolOptions.registerOption("report-semantic-fold",false);
  boolOptions.registerOption("post-collapse-stg",true);
  boolOptions.registerOption("eliminate-arrays",false);

  boolOptions.registerOption("viz",false);
  boolOptions.registerOption("update-input-var",true);
  boolOptions.registerOption("run-rose-tests",false);
  boolOptions.registerOption("reduce-cfg",false);
  boolOptions.registerOption("print-all-options",false);
  boolOptions.registerOption("annotate-results",false);
  boolOptions.registerOption("generate-assertions",false);

  boolOptions.registerOption("ltl-output-dot",false);
  boolOptions.registerOption("ltl-verbose",false);
  boolOptions.registerOption("ltl-show-derivation",true);
  boolOptions.registerOption("ltl-show-node-detail",true);
  boolOptions.registerOption("ltl-collapsed-graph",false);
  boolOptions.registerOption("input-values-as-constraints",false);

  boolOptions.registerOption("arith-top",false);
  boolOptions.registerOption("abstract-interpreter",false);
  boolOptions.registerOption("rers-binary",false);
  boolOptions.registerOption("relop-constraints",false); // not accessible on command line yet
  boolOptions.registerOption("stderr-like-failed-assert",false);
  boolOptions.registerOption("rersmode",false);
  boolOptions.registerOption("rers-numeric",false);
  boolOptions.registerOption("eliminate-stg-back-edges",false);
  boolOptions.registerOption("rule-const-subst",true);

  boolOptions.registerOption("inf-paths-only",false);
  boolOptions.registerOption("std-io-only",false);
  boolOptions.registerOption("std-in-only",false);
  boolOptions.registerOption("std-out-only",false);
  boolOptions.registerOption("no-input-input",false);

  boolOptions.registerOption("with-counterexamples",false);
  boolOptions.registerOption("with-assert-counterexamples",false);
  boolOptions.registerOption("with-ltl-counterexamples",false);
  boolOptions.registerOption("counterexamples-with-output",false);
  boolOptions.registerOption("check-ltl-counterexamples",false); 
  boolOptions.registerOption("refinement-constraints-demo",false);
  boolOptions.registerOption("determine-prefix-depth",false);
  boolOptions.registerOption("incomplete-stg",false);

  boolOptions.registerOption("print-update-infos",false);
  boolOptions.registerOption("verify-update-sequence-race-conditions",true);

  boolOptions.registerOption("minimize-states",false);

  boolOptions.processOptions();

  if (boolOptions["counterexamples-with-output"]) {
    boolOptions.registerOption("with-ltl-counterexamples",true);
  }

  if (boolOptions["check-ltl-counterexamples"]) {
    boolOptions.registerOption("with-ltl-counterexamples",true);
    boolOptions.registerOption("counterexamples-with-output",true);
  }

  if(boolOptions["print-all-options"]) {
    cout<<boolOptions.toString(); // prints all bool options
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

  Analyzer analyzer;
  global_analyzer=&analyzer;
  
  string option_pragma_name;
  if (args.count("limit-to-fragment")) {
    option_pragma_name = args["limit-to-fragment"].as<string>();
  }

  // clean up verify and csv-ltl option in argv
  if (args.count("verify")) {
    cerr<<"Option --verify is deprecated."<<endl;
    exit(1);
    //ltl_file = args["verify"].as<string>();
  }
  if(args.count("csv-assert-live")) {
    analyzer.setCsvAssertLiveFileName(args["csv-assert-live"].as<string>());
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

  if(args.count("rersformat")) {
    int year=args["rersformat"].as<int>();
    if(year==2012)
      resultsFormat=RF_RERS2012;
    if(year==2013)
      resultsFormat=RF_RERS2013;
    // otherwise it remains RF_UNKNOWN
  }

  if(args.count("exploration-mode")) {
    string explorationMode=args["exploration-mode"].as<string>();
    if(explorationMode=="depth-first") {
      analyzer.setExplorationMode(Analyzer::EXPL_DEPTH_FIRST);
    } else if(explorationMode=="breadth-first") {
      analyzer.setExplorationMode(Analyzer::EXPL_BREADTH_FIRST);
    } else if(explorationMode=="loop-aware") {
      analyzer.setExplorationMode(Analyzer::EXPL_LOOP_AWARE);
    } else {
      cerr<<"Error: unknown state space exploration mode specified with option --exploration-mode."<<endl;
      exit(1);
    }
  } else {
    // default value
    analyzer.setExplorationMode(Analyzer::EXPL_BREADTH_FIRST);
  }
  if(args.count("max-transitions")) {
    analyzer.setMaxTransitions(args["max-transitions"].as<int>());
  }

  if(args.count("max-transitions-forced-top")) {
    analyzer.setMaxTransitionsForcedTop(args["max-transitions-forced-top"].as<int>());
  }
  if(boolOptions["minimize-states"]) {
    analyzer.setMinimizeStates(true);
  }

  int numberOfThreadsToUse=1;
  if(args.count("threads")) {
    numberOfThreadsToUse=args["threads"].as<int>();
  }
  analyzer.setNumberOfThreadsToUse(numberOfThreadsToUse);

  // check threads == 1
#if 0
  if(args.count("rers-binary") && numberOfThreadsToUse>1) {
	cerr<<"Error: binary mode is only supported for 1 thread."<<endl;
	exit(1);
  }
#endif
  if(args.count("semantic-fold-threshold")) {
    int semanticFoldThreshold=args["semantic-fold-threshold"].as<int>();
    analyzer.setSemanticFoldThreshold(semanticFoldThreshold);
  }
  if(args.count("display-diff")) {
    int displayDiff=args["display-diff"].as<int>();
    analyzer.setDisplayDiff(displayDiff);
  }
  if(args.count("solver")) {
    int solver=args["solver"].as<int>();
    analyzer.setSolver(solver);
  }
  if(args.count("ltl-verifier")) {
    int ltlVerifier=args["ltl-verifier"].as<int>();
    analyzer.setLTLVerifier(ltlVerifier);
  }
  if(args.count("debug-mode")) {
    option_debug_mode=args["debug-mode"].as<int>();
  }
  if(args.count("variable-value-threshold")) {
    analyzer.setVariableValueThreshold(args["variable-value-threshold"].as<int>());
  }

  string option_specialize_fun_name="";
  vector<int> option_specialize_fun_param_list;
  vector<int> option_specialize_fun_const_list;
  if(args.count("specialize-fun-name")) {
    option_specialize_fun_name = args["specialize-fun-name"].as<string>();
  }
  if(args.count("specialize-fun-param")) {
    option_specialize_fun_param_list=args["specialize-fun-param"].as< vector<int> >();
    option_specialize_fun_const_list=args["specialize-fun-const"].as< vector<int> >();
  }
  //cout<<"DEBUG: "<<"specialize-params:"<<option_specialize_fun_const_list.size()<<endl;

  if((args.count("specialize-fun-name")||args.count("specialize-fun-param")||args.count("specialize-fun-const"))
     && !(args.count("specialize-fun-name")&&args.count("specialize-fun-param")&&args.count("specialize-fun-param"))) {
    cout<<"Error: options --specialize-fun-name=NAME --specialize-fun-param=NUM --specialize-fun-const=NUM must be used together."<<endl;
    exit(1);
  }

  // clean up string-options in argv
  for (int i=1; i<argc; ++i) {
    if (string(argv[i]).find("--csv-assert")==0
        || string(argv[i]).find("--csv-stats")==0
        || string(argv[i]).find("--csv-assert-live")==0
        || string(argv[i]).find("--threads" )==0
        || string(argv[i]).find("--display-diff")==0
        || string(argv[i]).find("--input-values")==0
        || string(argv[i]).find("--ltl-verifier")==0
        || string(argv[i]).find("--dot-io-stg")==0
        || string(argv[i]).find("--verify")==0
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

  // handle RERS mode: reconfigure options
  if(boolOptions["rersmode"]) {
    cout<<"INFO: RERS MODE activated [stderr output is treated like a failed assert]"<<endl;
    boolOptions.registerOption("stderr-like-failed-assert",true);
  }

  if(boolOptions["semantic-elimination"]) {
    boolOptions.registerOption("semantic-fold",true);
  }

  if(boolOptions["semantic-explosion"]) {
    boolOptions.registerOption("semantic-fold",true);
    //boolOptions.registerOption("semantic-elimination",true);
  }

  analyzer.setTreatStdErrLikeFailedAssert(boolOptions["stderr-like-failed-assert"]);
  

  // Build the AST used by ROSE
  cout << "INIT: Parsing and creating AST: started."<<endl;
  timer.stop();
  timer.start();
  SgProject* sageProject = frontend(argc,argv);
  double frontEndRunTime=timer.getElapsedTimeInMilliSec();
  cout << "INIT: Parsing and creating AST: finished."<<endl;
  
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
  VariableIdMapping variableIdMapping;
  variableIdMapping.computeVariableSymbolMapping(sageProject);

  int numSubst=0;
  if(option_specialize_fun_name!="")
  {
    Specialization speci;
    cout<<"STATUS: specializing function: "<<option_specialize_fun_name<<endl;

    string funNameToFind=option_specialize_fun_name;

    for(size_t i=0;i<option_specialize_fun_param_list.size();i++) {
      int param=option_specialize_fun_param_list[i];
      int constInt=option_specialize_fun_const_list[i];
      numSubst+=speci.specializeFunction(sageProject,funNameToFind, param, constInt, &variableIdMapping);
    }

    cout<<"STATUS: specialization: number of variable-uses replaced with constant: "<<numSubst<<endl;
    //root=speci.getSpecializedFunctionRootNode();
    sageProject->unparse(0,0);
    //exit(0);
  }


  if(args.count("rewrite")) {
    rewriteSystem.resetStatistics();
    rewriteSystem.rewriteAst(root, &variableIdMapping,true,false,true);
    cout<<"Rewrite statistics:"<<endl<<rewriteSystem.getStatistics().toString()<<endl;
    sageProject->unparse(0,0);
    cout<<"STATUS: generated rewritten program."<<endl;
    exit(0);
  }

  cout << "INIT: Checking input program."<<endl;
  CodeThornLanguageRestrictor lr;
  lr.checkProgram(root);
  timer.start();

  cout << "INIT: Running variable<->symbol mapping check."<<endl;
  //VariableIdMapping varIdMap;
  analyzer.getVariableIdMapping()->setModeVariableIdForEachArrayElement(true);
  analyzer.getVariableIdMapping()->computeVariableSymbolMapping(sageProject);
  cout << "STATUS: Variable<->Symbol mapping created."<<endl;
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
    list<SgPragmaDeclaration*> pragmaDeclList=findPragmaDeclarations(root, option_pragma_name);
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

  double initRunTime=timer.getElapsedTimeInMilliSec();

  timer.start();
  cout << "=============================================================="<<endl;
  if(boolOptions["semantic-fold"]) {
        analyzer.setSolver(4);
  }
  analyzer.runSolver();

  if(boolOptions["post-semantic-fold"]) {
    cout << "Performing post semantic folding (this may take some time):"<<endl;
    analyzer.semanticFoldingOfTransitionGraph();
  }
  if(boolOptions["semantic-elimination"]) {
    analyzer.semanticEliminationOfTransitions();
  }

  if(boolOptions["semantic-explosion"]) {
    analyzer.semanticExplosionOfInputNodesFromOutputNodeConstraints();
  }

  double analysisRunTime=timer.getElapsedTimeInMilliSec();
  cout << "=============================================================="<<endl;

  double extractAssertionTracesTime= 0;
  int maxOfShortestAssertInput = -1;
  if ( boolOptions["with-counterexamples"] || boolOptions["with-assert-counterexamples"]) {
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
#if 1
    analyzer.stdIOFoldingOfTransitionGraph();
#else
    cout << "(Experimental) Reducing transition graph ..."<<endl;
    set<const EState*> xestates=analyzer.nonLTLRelevantEStates();
    cout << "Size of transition graph before reduction: "<<analyzer.getTransitionGraph()->size()<<endl;
    cout << "Number of EStates to be reduced: "<<xestates.size()<<endl;
    analyzer.getTransitionGraph()->reduceEStates(xestates);
#endif
    cout << "Size of transition graph after reduction : "<<analyzer.getTransitionGraph()->size()<<endl;
    cout << "=============================================================="<<endl;
  }
  if(boolOptions["eliminate-stg-back-edges"]) {
    int numElim=analyzer.getTransitionGraph()->eliminateBackEdges();
    cout<<"STATUS: eliminated "<<numElim<<" STG back edges."<<endl;
  }

  // TODO: reachability in presence of semantic folding
  //  if(boolOptions["semantic-fold"] || boolOptions["post-semantic-fold"]) {
    analyzer.reachabilityResults.printResultsStatistics();
    //  } else {
    //printAssertStatistics(analyzer,sageProject);
    //}
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
  long numOfConstEStates=(analyzer.getEStateSet()->numberOfConstEStates(analyzer.getVariableIdMapping()));
  long numOfStdoutEStates=numOfStdoutVarEStates+numOfStdoutConstEStates;

  long totalMemory=pstateSetBytes+eStateSetBytes+transitionGraphBytes+constraintSetsBytes;

  double totalRunTime=frontEndRunTime+initRunTime+analysisRunTime;

  long pstateSetSizeInf = 0;
  long eStateSetSizeInf = 0;
  long transitionGraphSizeInf = 0;
  long eStateSetSizeStgInf = 0;
  //long numOfconstraintSetsInf = 0;
  //long numOfStdinEStatesInf = 0;
  //long numOfStdoutVarEStatesInf = 0;
  //long numOfStdoutConstEStatesInf = 0;
  //long numOfStdoutEStatesInf = 0;
  //long numOfStderrEStatesInf = 0;
  //long numOfFailedAssertEStatesInf = 0;
  //long numOfConstEStatesInf = 0;
  double infPathsOnlyTime = 0;
  double stdIoOnlyTime = 0;

  if(boolOptions["inf-paths-only"]) {
    cout << "STATUS: recursively removing all leaves."<<endl;
    timer.start();
    analyzer.pruneLeavesRec();
    infPathsOnlyTime = timer.getElapsedTimeInMilliSec();

    pstateSetSizeInf=analyzer.getPStateSet()->size();
    eStateSetSizeInf = analyzer.getEStateSet()->size();
    transitionGraphSizeInf = analyzer.getTransitionGraph()->size();
    eStateSetSizeStgInf = (analyzer.getTransitionGraph())->estateSet().size();
    //numOfconstraintSetsInf=analyzer.getConstraintSetMaintainer()->numberOf();
    //numOfStdinEStatesInf=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDIN_VAR));
    //numOfStdoutVarEStatesInf=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_VAR));
    //numOfStdoutConstEStatesInf=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_CONST));
    //numOfStderrEStatesInf=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDERR_VAR));
    //numOfFailedAssertEStatesInf=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::FAILED_ASSERT));
    //numOfConstEStatesInf=(analyzer.getEStateSet()->numberOfConstEStates(analyzer.getVariableIdMapping()));
    //numOfStdoutEStatesInf=numOfStdoutVarEStatesInf+numOfStdoutConstEStatesInf;
  }
  
  if(boolOptions["std-in-only"]) {
    cout << "STATUS: reducing STG to Input-states."<<endl;
    analyzer.reduceGraphInOutWorklistOnly(true,false);
  }

  if(boolOptions["std-out-only"]) {
    cout << "STATUS: reducing STG to output-states."<<endl;
    analyzer.reduceGraphInOutWorklistOnly(false,true);
  }

  if(boolOptions["std-io-only"]) {
    cout << "STATUS: bypassing all non standard I/O states."<<endl;
    timer.start();
    //analyzer.removeNonIOStates();  //old version, works correclty but has a long execution time
    analyzer.reduceGraphInOutWorklistOnly(true,true);
    stdIoOnlyTime = timer.getElapsedTimeInMilliSec();
  }

  long eStateSetSizeIoOnly = 0;
  long transitionGraphSizeIoOnly = 0;
  double spotLtlAnalysisTime = 0;

  if (args.count("check-ltl")) {
    string ltl_filename = args["check-ltl"].as<string>();
    if(boolOptions["rersmode"]) {  //reduce the graph accordingly, if not already done
      if (!boolOptions["inf-paths-only"]) {
        cout << "STATUS: recursively removing all leaves (due to RERS-mode)."<<endl;
        timer.start();
        analyzer.pruneLeavesRec();
        infPathsOnlyTime = timer.getElapsedTimeInMilliSec();

        pstateSetSizeInf=analyzer.getPStateSet()->size();
        eStateSetSizeInf = analyzer.getEStateSet()->size();
        transitionGraphSizeInf = analyzer.getTransitionGraph()->size();
        eStateSetSizeStgInf = (analyzer.getTransitionGraph())->estateSet().size();
        //numOfconstraintSetsInf=analyzer.getConstraintSetMaintainer()->numberOf();
        //numOfStdinEStatesInf=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDIN_VAR));
        //numOfStdoutVarEStatesInf=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_VAR));
        //numOfStdoutConstEStatesInf=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_CONST));
        //numOfStderrEStatesInf=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDERR_VAR));
        //numOfFailedAssertEStatesInf=(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::FAILED_ASSERT));
        //numOfConstEStatesInf=(analyzer.getEStateSet()->numberOfConstEStates(analyzer.getVariableIdMapping()));
        //numOfStdoutEStatesInf=numOfStdoutVarEStatesInf+numOfStdoutConstEStatesInf;
      }
      if (!boolOptions["std-io-only"]) {
        cout << "STATUS: bypassing all non standard I/O states (due to RERS-mode)."<<endl;
        timer.start();
        //analyzer.removeNonIOStates();  //old version, works correclty but has a long execution time
        analyzer.reduceGraphInOutWorklistOnly();
        stdIoOnlyTime = timer.getElapsedTimeInMilliSec();

        eStateSetSizeIoOnly = (analyzer.getTransitionGraph())->estateSet().size();
        transitionGraphSizeIoOnly = (analyzer.getTransitionGraph())->size();
        cout << "STATUS: number of transitions remaining after reduction to I/O/(worklist) states only: " << transitionGraphSizeIoOnly << endl;
        cout << "STATUS: number of states remaining after reduction to I/O/(worklist) states only: " <<eStateSetSizeIoOnly << endl;
      }
    }
    if(boolOptions["no-input-input"]) {  //delete transitions that indicate two input states without an output in between
      analyzer.removeInputInputTransitions();
      transitionGraphSizeIoOnly = (analyzer.getTransitionGraph())->size();
      cout << "STATUS: number of transitions remaining after removing input->input transitions: " << transitionGraphSizeIoOnly << endl;
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
    cout << "STATUS: generating LTL results"<<endl;
    SpotConnection spotConnection(ltl_filename);
    spotConnection.checkLtlProperties( *(analyzer.getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample);
    spotLtlAnalysisTime=timer.getElapsedTimeInMilliSec();
    PropertyValueTable* ltlResults = spotConnection.getLtlResults();

    if (boolOptions["check-ltl-counterexamples"]) {
      cout << "STATUS: checking for spurious counterexamples..."<<endl;
      CounterexampleAnalyzer ceAnalyzer(analyzer);
      RefinementConstraints constraintManager(analyzer.getFlow(), analyzer.getLabeler(), 
                analyzer.getExprAnalyzer(), analyzer.getCFAnalyzer(), analyzer.getVariableIdMapping());
      for (unsigned int i = 0; i < ltlResults->size(); i++) {
        //only check counterexamples
        if (ltlResults->getPropertyValue(i) == PROPERTY_VALUE_NO) {
          std::string counterexample = ltlResults->getCounterexample(i);
          CEAnalysisResult ceAnalysisResult = ceAnalyzer.analyzeCounterexample(counterexample);
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
  if(args.count("dump-sorted")>0 || args.count("dump-non-sorted")>0) {
    Specialization speci;
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
    speci.extractArrayUpdateOperations(&analyzer,
                                       arrayUpdates,
                                       rewriteSystem,
                                       useConstSubstitutionRule
                                       );
    arrayUpdateExtractionRunTime=timer.getElapsedTimeInMilliSec();

    if(boolOptions["verify-update-sequence-race-conditions"]) {
      SgNode* root=analyzer.startFunRoot;
      VariableId parallelIterationVar;
      LoopInfoSet iterationVars=determineLoopInfoSet(root,&variableIdMapping, analyzer.getLabeler());
      cout<<"DEBUG: number of iteration vars: "<<iterationVars.size()<<endl;

      timer.start();
      verifyUpdateSequenceRaceConditionsResult=speci.verifyUpdateSequenceRaceConditions(iterationVars,arrayUpdates,analyzer.getVariableIdMapping());
      verifyUpdateSequenceRaceConditionRunTime=timer.getElapsedTimeInMilliSec();
    }

    if(boolOptions["print-update-infos"]) {
      speci.printUpdateInfos(arrayUpdates,analyzer.getVariableIdMapping());
    }
    cout<<"STATUS: establishing array-element SSA numbering."<<endl;
    timer.start();
#if 0
    attachSsaNumberingtoDefs(arrayUpdates, analyzer.getVariableIdMapping());
    substituteArrayRefs(arrayUpdates, analyzer.getVariableIdMapping(),SAR_SSA);
#else
    speci.createSsaNumbering(arrayUpdates, analyzer.getVariableIdMapping());
#endif
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
  cout << "Time total           : "<<color("green")<<readableruntime(totalRunTime)<<color("white")<<endl;
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
        <<readableruntime(frontEndRunTime)<<", "
        <<readableruntime(initRunTime)<<", "
        <<readableruntime(analysisRunTime)<<", "
        <<readableruntime(verifyUpdateSequenceRaceConditionRunTime)<<", "
        <<readableruntime(arrayUpdateExtractionRunTime)<<", "
        <<readableruntime(arrayUpdateSsaNumberingRunTime)<<", "
        <<readableruntime(sortingAndIORunTime)<<", "
        <<readableruntime(totalRunTime)<<", "
        <<readableruntime(extractAssertionTracesTime)<<", "
        <<readableruntime(determinePrefixDepthTime)<<", "
        <<readableruntime(totalInputTracesTime)<<", "
        <<readableruntime(infPathsOnlyTime)<<", "
        <<readableruntime(stdIoOnlyTime)<<", "
        <<readableruntime(spotLtlAnalysisTime)<<", "
        <<readableruntime(totalLtlRunTime)<<", "
        <<readableruntime(overallTime)<<endl;
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

  if (args.count("check-ltl-sol")) {
    string ltl_filename = args["check-ltl-sol"].as<string>();
    if(boolOptions["rersmode"]) {  //reduce the graph accordingly, if not already done
      if (!boolOptions["inf-paths-only"]) {
        cout << "STATUS: recursively removing all leaves (due to RERS-mode)."<<endl;
        analyzer.pruneLeavesRec();
      }
      if (!boolOptions["std-io-only"]) {
        cout << "STATUS: bypassing all non standard I/O states (due to RERS-mode)."<<endl;
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
    dotFile=functionAstTermsWithNullValuesToDot(sageProject);
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

  if (args.count("spot-stg")) {
    string filename=args["spot-stg"].as<string>();
    cout << "generating spot IO STG file:"<<filename<<endl;
    if(boolOptions["rersmode"]) {  //reduce the graph accordingly, if not already done
      if (!boolOptions["inf-paths-only"]) {
        cout << "STATUS: recursively removing all leaves (due to RERS-mode)."<<endl;
        analyzer.pruneLeavesRec();
      }
      if (!boolOptions["std-io-only"]) {
        cout << "STATUS: bypassing all non standard I/O states (due to RERS-mode)."<<endl;
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

#if 0
  {
  // check output var to be constant in transition graph
  TransitionGraph* tg=analyzer.getTransitionGraph();
  for(TransitionGraph::iterator i=tg->begin();i!=tg->end();++i) {
    const EState* es1=(*i).source;
    InputOutput myio=es1->io;
    assert(myio.op==InputOutput::STDOUT_VAR 
           && 
           es1->pstate->varIsConst(es1->io.var)
           );
  }
#endif
  
  if (boolOptions["annotate-results"]) {
    // TODO: it might be useful to be able to select certain analysis results to be only annotated
    cout << "INFO: Annotating analysis results."<<endl;
    attachTermRepresentation(sageProject);
    attachPointerExprLists(sageProject);
    AstAnnotator ara(analyzer.getLabeler());
    ara.annotateAstAttributesAsCommentsBeforeStatements(sageProject,"codethorn-term-representation");
    ara.annotateAstAttributesAsCommentsBeforeStatements(sageProject,"codethorn-pointer-expr-lists");
  }

  if (boolOptions["annotate-results"]||boolOptions["generate-assertions"]) {
    cout << "INFO: Generating annotated program."<<endl;
    //backend(sageProject);
    sageProject->unparse(0,0);
  }
  // reset terminal
  cout<<color("normal")<<"done."<<endl;
  
  } catch(char* str) {
    cerr << "*Exception raised: " << str << endl;
    return 1;
  } catch(const char* str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
  } catch(string str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
 }
  return 0;
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
  cout << "Time total           : "<<color("green")<<readableruntime(totalRunTime)<<color("white")<<endl;
  cout << "=============================================================="<<endl;
  cout <<color("normal");
}

