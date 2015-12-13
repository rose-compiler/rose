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
#include "PragmaHandler.h"
#include "Miscellaneous2.h"
#include "FIConstAnalysis.h"
#include "ReachabilityAnalysis.h"
// test
#include "Evaluator.h"

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
          cout<<"DEBUG: NOT a for-stmt: "<<(*i)->unparseToString()<<endl;
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

bool isInsideOmpParallelFor(SgNode* node, ForStmtToOmpPragmaMap& forStmtToPragmaMap) {
  while(!isSgForStatement(node)||isSgProject(node))
    node=node->get_parent();
  ROSE_ASSERT(!isSgProject(node));
  // assuming only omp parallel for exist
  return forStmtToPragmaMap.find(isSgForStatement(node))!=forStmtToPragmaMap.end();
}

LoopInfoSet determineLoopInfoSet(SgNode* root, VariableIdMapping* variableIdMapping, Labeler* labeler) {
  cout<<"INFO: loop info set and determine iteration vars."<<endl;
  ForStmtToOmpPragmaMap forStmtToPragmaMap=createOmpPragmaForStmtMap(root);
  cout<<"INFO: found "<<forStmtToPragmaMap.size()<<" omp/simd loops."<<endl;
  LoopInfoSet loopInfoSet;
  RoseAst ast(root);
  AstMatching m;
  string matchexpression="SgForStatement(_,_,SgPlusPlusOp($ITERVAR=SgVarRefExp),..)";
  MatchResult r=m.performMatching(matchexpression,root);
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    SgVarRefExp* node=isSgVarRefExp((*i)["$ITERVAR"]);
    ROSE_ASSERT(node);
    //cout<<"DEBUG: MATCH: "<<node->unparseToString()<<astTermWithNullValuesToString(node)<<endl;
    LoopInfo loopInfo;
    loopInfo.iterationVarId=variableIdMapping->variableId(node);
    loopInfo.iterationVarType=isInsideOmpParallelFor(node,forStmtToPragmaMap)?ITERVAR_PAR:ITERVAR_SEQ;
    SgNode* forNode=0; //(*i)["$FORSTMT"];
    // WORKAROUND 1
    // TODO: investigate why the for pointer is not stored in the same match-result
    if(forNode==0) {
      forNode=node; // init
      while(!isSgForStatement(forNode)||isSgProject(forNode))
        forNode=forNode->get_parent();
    }
    ROSE_ASSERT(!isSgProject(forNode));
#if 0
    cout<<"DEBUG: FOR-ITER-VAR:"<<variableIdMapping->variableName(loopInfo.iterationVarId)<<":";
    cout<<"TYPE:"<<loopInfo.iterationVarType<<":";
    cout<<forNode->unparseToString()<<endl<<"---------------------------------"<<endl;
#endif
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
  cout<<"INFO: found "<<forStmtToPragmaMap.size()<<" omp/simd loops."<<endl;
  cout<<"INFO: found "<<Specialization::numParLoops(loopInfoSet,variableIdMapping)<<" parallel loops."<<endl;
  return loopInfoSet;
}

class TermRepresentation : public DFAstAttribute {
public:
  TermRepresentation(SgNode* node) : _node(node) {}
  string toString() { return "AstTerm: "+SPRAY::AstTerm::astTermWithNullValuesToString(_node); }
private:
  SgNode* _node;
};

class PointerExprListAnnotation : public DFAstAttribute {
public:
  PointerExprListAnnotation(SgNode* node) : _node(node) {
    //std::cout<<"DEBUG:generated: "+pointerExprToString(node)+"\n";
  }
  string toString() { 
    return "// POINTEREXPR: "+SPRAY::AstTerm::pointerExprToString(_node);
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
    ("csv-stats-size-and-ltl",po::value< string >(),"output statistics regarding the final model size and results for LTL properties into a CSV file [arg]")
    ("csv-stats-cegpra",po::value< string >(),"output statistics regarding the counterexample-guided prefix refinement analysis (cegpra) into a CSV file [arg]")
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
    ("viz-cegpra-detailed",po::value< string >(),"generate visualization (dot) output files with prefix <arg> for different stages within each loop of cegpra.")
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
    ("max-iterations",po::value< int >(),"Passes (possibly) incomplete STG to verifier after max loop iterations (default: no limit). Currently requires --exploration-mode=loop-aware.")
    ("max-transitions-forced-top",po::value< int >(),"same as max-transitions-forced-top1 (default).")
    ("max-transitions-forced-top1",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output) (default: no limit).")
    ("max-transitions-forced-top2",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output,df) (default: no limit).")
    ("max-transitions-forced-top3",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output,df,ptr-vars) (default: no limit).")
    ("max-transitions-forced-top4",po::value< int >(),"Performs approximation after <arg> transitions (exact for all but inc-vars) (default: no limit).")
    ("max-transitions-forced-top5",po::value< int >(),"Performs approximation after <arg> transitions (exact for input,output,df and vars with 0 to 2 assigned values)) (default: no limit).")
    ("max-iterations-forced-top",po::value< int >(),"Performs approximation after <arg> loop iterations (default: no limit). Currently requires --exploration-mode=loop-aware.")
    ("variable-value-threshold",po::value< int >(),"sets a threshold for the maximum number of different values are stored for each variable.")
    ("dot-io-stg", po::value< string >(), "output STG with explicit I/O node information in dot file [arg]")
    ("dot-io-stg-forced-top", po::value< string >(), "output STG with explicit I/O node information in dot file. Groups abstract states together. [arg]")
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
    ("normalize",po::value< string >(),"normalize AST before analysis.")
    ("specialize-fun-name", po::value< string >(), "function of name [arg] to be specialized")
    ("specialize-fun-param", po::value< vector<int> >(), "function parameter number to be specialized (starting at 1)")
    ("specialize-fun-const", po::value< vector<int> >(), "constant [arg], the param is to be specialized to.")
    ("specialize-fun-varinit", po::value< vector<string> >(), "variable name of which the initialization is to be specialized (overrides any initializer expression)")
    ("specialize-fun-varinit-const", po::value< vector<int> >(), "constant [arg], the variable initialization is to be specialized to.")
    ("iseq-file", po::value< string >(), "compute input sequence and generate file [arg]")
    ("iseq-length", po::value< int >(), "set length [arg] of input sequence to be computed.")
    ("iseq-random-num", po::value< int >(), "select random search and number of paths.")
    ("error-function", po::value< string >(), "detect a verifier error function with name [arg] (terminates verification)")
    ("enable-external-function-semantics",  "assumes specific semantics for the external functions: __VERIFIER_error,__VERIFIER_nondet_int,exit functions.")
    ("inf-paths-only", po::value< string >(), "recursively prune the graph so that no leaves exist [=yes|no]")
    ("std-io-only", po::value< string >(), "bypass and remove all states that are not standard I/O [=yes|no]")
    ("std-in-only", po::value< string >(), "bypass and remove all states that are not input-states [=yes|no]")
    ("std-out-only", po::value< string >(), "bypass and remove all states that are not output-states [=yes|no]")
    ("check-ltl", po::value< string >(), "take a text file of LTL I/O formulae [arg] and check whether or not the analyzed program satisfies these formulae. Formulae should start with '('. Use \"csv-spot-ltl\" option to specify an output csv file for the results.")
    ("check-ltl-sol", po::value< string >(), "take a source code file and an LTL formulae+solutions file ([arg], see RERS downloads for examples). Display if the formulae are satisfied and if the expected solutions are correct.")
    ("ltl-in-alphabet",po::value< string >(),"specify an input alphabet used by the LTL formulae (e.g. \"{1,2,3}\")")
    ("ltl-out-alphabet",po::value< string >(),"specify an output alphabet used by the LTL formulae (e.g. \"{19,20,21,22,23,24,25,26}\")")
    ("io-reduction", po::value< int >(), "(work in progress) reduce the transition system to only input/output/worklist states after every <arg> computed EStates.")
    ("keep-error-states",  po::value< string >(), "Do not reduce error states for the LTL analysis. [=yes|no]")
    ("no-input-input",  po::value< string >(), "remove transitions where one input states follows another without any output in between. Removal occurs before the LTL check. [=yes|no]")
    ("with-counterexamples", po::value< string >(), "adds counterexample traces to the analysis results. Applies to reachable assertions (work in progress) and falsified LTL properties. [=yes|no]")
    ("with-assert-counterexamples", po::value< string >(), "report counterexamples leading to failing assertion states (work in progress) [=yes|no]")
    ("with-ltl-counterexamples", po::value< string >(), "report counterexamples that violate LTL properties [=yes|no]")
    ("counterexamples-with-output", po::value< string >(), "reported counterexamples for LTL or reachability properties also include output values [=yes|no]")
    ("check-ltl-counterexamples", po::value< string >(), "report ltl counterexamples if and only if they are not spurious [=yes|no]")
    ("reconstruct-assert-paths", po::value< string >(), "takes a result file containing paths to reachable assertions and tries to reproduce them on the analyzed program. [=file-path]")
    ("reconstruct-max-length", po::value< int >(), "parameter of option \"reconstruct-input-paths\". Sets the maximum length of cyclic I/O patterns found by the analysis. [=pattern_length]")
    ("reconstruct-max-repetitions", po::value< int >(), "parameter of option \"reconstruct-input-paths\". Sets the maximum number of pattern repetitions that the search is following. [=#pattern_repetitions]")
    ("pattern-search-max-depth", po::value< int >(), "parameter of the pattern search mode. Sets the maximum input depth that is searched for cyclic I/O patterns (default: 10).")
    ("pattern-search-repetitions", po::value< int >(), "parameter of the pattern search mode. Sets the number of unrolled iterations of cyclic I/O patterns (default: 100).")
    ("pattern-search-max-suffix", po::value< int >(), "parameter of the pattern search mode. Sets the maximum input depth of the suffix that is searched for failing assertions after following an I/O-pattern (default: 5).")
    ("pattern-search-asserts", po::value< string >(), "reads a .csv-file (one line per assertion, e.g. \"1,yes\"). The pattern search terminates early if traces to all errors with \"yes\" entries have been found. [=file-path]")
    ("pattern-search-exploration", po::value< string >(), "exploration mode for the pattern search. Note: all suffixes will always be checked using depth-first search. [=depth-first|breadth-first]")
    ("refinement-constraints-demo", po::value< string >(), "display constraints that are collected in order to later on help a refined analysis avoid spurious counterexamples. [=yes|no]")
    ("cegpra-ltl",po::value< int >(),"Select the ID of an LTL property that should be checked using cegpra (between 0 and 99).")
    ("cegpra-ltl-all",po::value< string >(),"Check all specified LTL properties using cegpra [=yes|no]")
    ("cegpra-max-iterations",po::value< int >(),"Select a maximum number of counterexamples anaylzed by cegpra (default: no limit).")
    ("set-stg-incomplete", po::value< string >(), "set to true if the generated STG will not contain all possible execution paths (e.g. if only a subset of the input values is used). [=yes|no]")
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
    cout << "CodeThorn version 1.6.0\n";
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

  boolOptions.registerOption("minimize-states",false);
  boolOptions.registerOption("normalize",true);

  boolOptions.processOptions();

  Analyzer analyzer;
  global_analyzer=&analyzer;

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
      if(explorationMode!="loop-aware") {
        notSupported=true;
      }
    }
    if(notSupported) {
      cout << "Error: \"max-iterations[-forced-top]\" modes currently require \"--exploration-mode=loop-aware\"." << endl;
      exit(1);
    }
  }

  if(args.count("max-transitions")) {
    analyzer.setMaxTransitions(args["max-transitions"].as<int>());
  }

  if(args.count("max-iterations")) {
    analyzer.setMaxIterations(args["max-iterations"].as<int>());
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
  vector<string> option_specialize_fun_varinit_list;
  vector<int> option_specialize_fun_varinit_const_list;
  if(args.count("specialize-fun-name")) {
    option_specialize_fun_name = args["specialize-fun-name"].as<string>();
    cout << "DEBUG: option_specialize_fun_name: "<< option_specialize_fun_name<<endl;
  } else {
    cout << "DEBUG: option_specialize_fun_name: NONE"<< option_specialize_fun_name<<endl;
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
    if (string(argv[i]).find("--csv-assert")==0
        || string(argv[i]).find("--csv-stats")==0
        || string(argv[i]).find("--csv-stats-cegpra")==0
        || string(argv[i]).find("--csv-stats-size-and-ltl")==0
        || string(argv[i]).find("--csv-assert-live")==0
        || string(argv[i]).find("--threads" )==0
        || string(argv[i]).find("--display-diff")==0
        || string(argv[i]).find("--input-values")==0
        || string(argv[i]).find("--ltl-verifier")==0
        || string(argv[i]).find("--dot-io-stg")==0
        || string(argv[i]).find("--dot-io-stg-forced-top")==0
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

#if 0
  SgNodeHelper::PragmaList pragmaList=SgNodeHelper::collectPragmaLines("verify",root);
  if(size_t numPragmas=pragmaList.size()>0) {
    cout<<"STATUS: found "<<numPragmas<<" provesa pragmas."<<endl;
    ROSE_ASSERT(numPragmas==1);
    SgNodeHelper::PragmaList::iterator i=pragmaList.begin();
    std::pair<std::string, SgNode*> p=*i;
    option_specialize_fun_name="kernel_jacobi_2d_imper";
    option_specialize_fun_param_list.push_back(0);
    option_specialize_fun_const_list.push_back(2);
    option_specialize_fun_param_list.push_back(1);
    option_specialize_fun_const_list.push_back(16);
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
      
      //root=speci.getSpecializedFunctionRootNode();
      sageProject->unparse(0,0);
      //exit(0);
    }
  }
#else
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
#endif

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
  lr.checkProgram(root);
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
  analyzer.runSolver();

  if(boolOptions["post-semantic-fold"]) {
    cout << "Performing post semantic folding (this may take some time):"<<endl;
    analyzer.semanticFoldingOfTransitionGraph();
  }
  if(boolOptions["semantic-elimination"]) {
    cout << "Performing post semantic elimination of transitions (this may take some time):"<<endl;
    analyzer.semanticEliminationOfTransitions();
  }

  if(boolOptions["semantic-explosion"]) {
    cout << "Performing post semantic 'explosion' of output node constraints (this may take some time):"<<endl;
    analyzer.semanticExplosionOfInputNodesFromOutputNodeConstraints();
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
  long numOfConstEStates=0;//(analyzer.getEStateSet()->numberOfConstEStates(analyzer.getVariableIdMapping()));
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
    assert (!boolOptions["keep-error-states"]);
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
    analyzer.reduceGraphInOutWorklistOnly(true,false,boolOptions["keep-error-states"]);
  }

  if(boolOptions["std-out-only"]) {
    cout << "STATUS: reducing STG to output-states."<<endl;
    analyzer.reduceGraphInOutWorklistOnly(false,true,boolOptions["keep-error-states"]);
  }

  if(boolOptions["std-io-only"]) {
    cout << "STATUS: bypassing all non standard I/O states."<<endl;
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
    string ltl_filename = args["check-ltl"].as<string>();
    if(boolOptions["rersmode"]) {  //reduce the graph accordingly, if not already done
      if (!boolOptions["inf-paths-only"] && !boolOptions["keep-error-states"]) {
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
      LoopInfoSet loopInfoSet=determineLoopInfoSet(root,analyzer.getVariableIdMapping(), analyzer.getLabeler());
      cout<<"INFO: number of iteration vars: "<<loopInfoSet.size()<<endl;
      Specialization::numParLoops(loopInfoSet, analyzer.getVariableIdMapping());
      timer.start();
      verifyUpdateSequenceRaceConditionsResult=speci.verifyUpdateSequenceRaceConditions(loopInfoSet,arrayUpdates,analyzer.getVariableIdMapping());
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
  
    sageProject->unparse(0,0);

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
  cout << "Time total           : "<<color("green")<<readableruntime(totalRunTime)<<color("white")<<endl;
  cout << "=============================================================="<<endl;
  cout <<color("normal");
}

