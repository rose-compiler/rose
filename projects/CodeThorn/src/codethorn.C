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
#include "LTLCheckerFixpoint.h"
#include "LTLCheckerUnified.h"
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

namespace po = boost::program_options;
using namespace CodeThorn;

// finds the list of pragmas (in traversal order) with the prefix 'prefix' (e.g. '#pragma omp parallel' is found for prefix 'omp')
list<SgPragmaDeclaration*> findPragmaDeclarations(SgNode* root, string prefix) {
  list<SgPragmaDeclaration*> pragmaList;
  RoseAst ast(root);
  cout<<"STATUS: searching for fragment markers."<<endl;
  for(RoseAst::iterator i=ast.begin(); i!=ast.end();++i) {
    if(SgPragmaDeclaration* pragmaDecl=isSgPragmaDeclaration(*i)) {
      string foundPrefix=SageInterface::extractPragmaKeyword(pragmaDecl);
      //cout<<"DEBUG: PREFIX:"<<foundPrefix<<endl;
      if(prefix==foundPrefix || "end"+prefix==foundPrefix) {
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

void generateAssertsCsvFile(Analyzer& analyzer, SgProject* sageProject, string filename) {
  ofstream* csv = NULL;
  csv = new ofstream();
  // use binary and \r\n tp enforce DOS line endings
  // http://tools.ietf.org/html/rfc4180
  csv->open(filename.c_str(), ios::trunc|ios::binary);
  //*csv << "Index;\"Assert Error Label\";ReachabilityResult;Confidence\r\n";
  
  LabelSet lset=analyzer.getTransitionGraph()->labelSetOfIoOperations(InputOutput::FAILED_ASSERT);
  list<pair<SgLabelStatement*,SgNode*> > assertNodes=analyzer.listOfLabeledAssertNodes(sageProject);
  if(boolOptions["rers-binary"]) {
    int assertStart=-1;
    int assertEnd=-1;
    switch(resultsFormat) {
    case RF_RERS2012: assertStart=0; assertEnd=60;break;
    case RF_RERS2013: assertStart=100; assertEnd=159;break;
    default: assert(0);
    }
    for(int i=assertStart;i<=assertEnd;i++) {
      *csv << i<<",";
      if(analyzer.binaryBindingAssert[i]) {
        *csv << "yes,9";
      } else {
        *csv << "no,9";
      }
      *csv << "\n";
    }
  } else {
    for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=assertNodes.begin();i!=assertNodes.end();++i) {
      string name=SgNodeHelper::getLabelName((*i).first);
      if(name=="globalError")
        name="error_60";
      int num;
      stringstream(name.substr(6,name.size()-6))>>num;
      switch(resultsFormat) {
      case RF_RERS2012: *csv<<(num);break;
      case RF_RERS2013: *csv<<(num+100);break;
      default: assert(0);
      } 
      *csv <<",";
      Label lab=analyzer.getLabeler()->getLabel((*i).second);
      if(lset.find(lab)!=lset.end()) {
        *csv << "yes,9";
      } else {
        *csv << "no,9";
      }
      *csv << "\n";
    }
  }
  if (csv) delete csv;
}

void printAsserts(Analyzer& analyzer, SgProject* sageProject) {
  if(boolOptions["rers-binary"]) {
    int assertStart=-1;
    int assertEnd=-1;
    switch(resultsFormat) {
    case RF_RERS2012: assertStart=0; assertEnd=60;break;
    case RF_RERS2013: assertStart=100; assertEnd=159;break;
    default: assert(0);
    }
    for(int i=assertStart;i<=assertEnd;i++) {
      cout <<color("white")<<"assert: error_"<<i<<": ";
      if(analyzer.binaryBindingAssert[i]) {
        cout << color("green")<<"YES (REACHABLE)";
      } else {
        cout << color("cyan")<<"NO (UNREACHABLE)";
      }
      cout << endl;
    }
    cout<<color("normal");
    return;
  }
  LabelSet lset=analyzer.getTransitionGraph()->labelSetOfIoOperations(InputOutput::FAILED_ASSERT);
  list<pair<SgLabelStatement*,SgNode*> > assertNodes=analyzer.listOfLabeledAssertNodes(sageProject);
  for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=assertNodes.begin();i!=assertNodes.end();++i) {
    cout << "assert: "
         << SgNodeHelper::getLabelName((*i).first)
      //     << SgNodeHelper::nodeToString((*i).second)<< " : "
      ;
    cout << ": ";
    Label lab=analyzer.getLabeler()->getLabel((*i).second);
    if(lset.find(lab)!=lset.end()) {
      cout << color("green")<<"YES (REACHABLE)"<<color("normal");
    }
    else {
      cout << color("cyan")<<"NO (UNREACHABLE)"<<color("normal");
    }
    cout << endl;
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

void generateLTLOutput(Analyzer& analyzer, string ltl_file) {
  extern CodeThorn::LTL::Formula* ltl_val;
  //
  // Verification
  //
  int n = 0;
  int n_yes = 0;
  int n_no = 0;
  int n_undecided = 0;
  int n_failed = 0;

  assert(analyzer.getEStateSet());
  assert(analyzer.getTransitionGraph());
  if (ltl_file.size()) {
    CodeThorn::FixpointLTL::Checker* checker1 = 0; 
    CodeThorn::UnifiedLTL::UChecker* checker2 = 0; 
    switch(analyzer.getLTLVerifier()) {
    case 1: 
      checker1 = new CodeThorn::FixpointLTL::Checker(*analyzer.getEStateSet(), 
                             *analyzer.getTransitionGraph());

      break;
    case 2:
#if 1
      checker2 = new CodeThorn::UnifiedLTL::UChecker(*analyzer.getEStateSet(),
                             *analyzer.getTransitionGraph());
#else
      {
        checker2 = new CodeThorn::UnifiedLTL::UChecker(*analyzer.getTransitionGraph()->estateSet());
                             *analyzer.getTransitionGraph());
      }
#endif
      break;
    default: 
      cerr << "Error: unknown ltl-verifier specified with ltl-verifier option."<<endl;
      exit(1);
    }
    ltl_input = fopen(ltl_file.c_str(), "r");
    if (ltl_input == NULL)
      cerr<<"Error: could not open file "<<ltl_file.c_str()<<endl;
    assert(ltl_input);

    ofstream* csv = NULL;
    if (args.count("csv-ltl")) {
      csv = new ofstream();
      // use binary and \r\n tp enforce DOS line endings
      // http://tools.ietf.org/html/rfc4180
      csv->open(args["csv-ltl"].as<string>().c_str(), ios::trunc|ios::binary);
      //*csv << "Index,\"LTL formula\",Result,Confidence\r\n";
    }

    while ( !ltl_eof) {
      try { 
    ltl_label = 0;
        if (ltl_parse()) {
          cerr<<color("red")<< "Syntax error" <<color("normal")<<endl;
      ++n;
      ++n_failed;
      continue;
        }
        if (ltl_val == NULL) {
      // empty line
      continue;
    }
      } catch(const char* s) {
        if (ltl_val) cout<<color("normal")<<string(*ltl_val)<<endl;
        cout<< s<<endl<<color("red")<< "Grammar Error" <<color("normal")<<endl;
    ++n;
    ++n_failed;
    continue;
      } catch(...) {
    cout<<color("red")<< "Parser exception" << endl;
    ++n;
    ++n_failed;
    continue;
      }  
      
      ++n;
      string formula = *ltl_val;
      cout<<endl<<"Verifying formula "<<color("white")<<formula<<color("normal")<<"."<<endl;
      //if (csv) *csv << n <<";\"" <<formula<<"\";";

      if(csv) {
        switch(resultsFormat) {
        case RF_RERS2012: *csv << (n-1)+61 <<",";break; // MS: n starts at 1
        case RF_RERS2013: *csv << (n-1) <<",";break;
        case RF_UNKNOWN: /* intentional fall-through */
        default: assert(0);
        }
      }
      try {
    AType::BoolLattice result;
    if (checker1) result = checker1->verify(*ltl_val);
    if (checker2) result = checker2->verify(*ltl_val);

    if (result.isTrue()) {
      ++n_yes;
      cout<<color("green")<<"YES"<<color("normal")<<endl;
      if (csv) *csv << "yes,9\r\n";
    } else if (result.isFalse()) {
      ++n_no;
      cout<<color("cyan")<<"NO"<<color("normal")<<endl;
      if (csv) *csv << "no,9\r\n";
    } else {
      ++n_undecided;
      cout<<color("magenta")<<"UNKNOWN"<<color("normal")<<endl;
      if (csv) *csv << "unknown,0\r\n";
    }
      } catch(const char* str) {
    ++n_failed;
    cerr << "Exception raised: " << str << endl;
    cout<<color("red")<<"ERROR"<<color("normal")<<endl;
    if (csv) *csv << "error,0\r\n";
      } catch(string str) {
    ++n_failed;
    cerr << "Exception raised: " << str << endl;
    cout<<color("red")<<"ERROR"<<color("normal")<<endl;
    if (csv) *csv << "error,0\r\n";
      } catch(...) {
    ++n_failed;
    cout<<color("red")<<"ERROR"<<color("normal")<<endl;
    if (csv) *csv << "error,0\r\n";
      }  
    }
    fclose(ltl_input);
    if (csv) delete csv;
    if (checker1) delete checker1;
    if (checker2) delete checker2;
    assert(n_yes+n_no+n_undecided+n_failed == n);
    cout<<"\nStatistics "<<endl
        <<"========== "<<endl
    <<n_yes      <<"/"<<n<<color("green")  <<" YES, "       <<color("normal")     
    <<n_no       <<"/"<<n<<color("cyan")   <<" NO, "        <<color("normal")     
    <<n_undecided<<"/"<<n<<color("magenta")<<" UNKNOWN, "   <<color("normal")     
    <<n_failed   <<"/"<<n<<color("red")    <<" ERROR"       <<color("normal")     
    <<endl;

  } 
}

string readableruntime(double time) {
  stringstream s;
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

struct RewriteStatistics {
  RewriteStatistics(){init();}
  void init() {
    numElimMinusOperator=0;
    numElimAssignOperator=0;
    numAddOpReordering=0;
    numConstantFolding=0;
    numVariableElim=0;
    numArrayUpdates=0;
    numConstExprElim=0;
  }
  int numElimMinusOperator;
  int numElimAssignOperator;
  int numAddOpReordering;
  int numConstantFolding;
  int numVariableElim;
  int numArrayUpdates; // number of array updates (i.e. assignments)
  int numConstExprElim; // number of const-expr found and substituted by constant (new rule, includes variables)
} dump1_stats;

void myReplaceExpression(SgExpression* e1, SgExpression* e2, bool mode=false) {
  SgExpression* p=isSgExpression(e1->get_parent());
  if(p && mode!=true) {
    p->replace_expression(e1,e2);
    e2->set_parent(p);
  } else {
    SageInterface::replaceExpression(e1,e2,mode); // this function is more general but very slow
  }
}

// not used yet
void threadsafeReplaceExpression(SgExpression* exp1, SgExpression* exp2, bool mode) {
#pragma omp critical
  {
    myReplaceExpression(exp1,exp2,mode);
  }
}

void rewriteCompoundAssignments(SgNode*& root, VariableIdMapping* variableIdMapping) {

  // Rewrite-rule 0: $Left OP= $Right => $Left = $Left OP $Right
  if(isSgCompoundAssignOp(root)) {
    dump1_stats.numElimAssignOperator++;
    SgExpression* lhsCopy=SageInterface::copyExpression(isSgExpression(SgNodeHelper::getLhs(root)));
    SgExpression* lhsCopy2=SageInterface::copyExpression(isSgExpression(SgNodeHelper::getLhs(root)));
    SgExpression* rhsCopy=SageInterface::copyExpression(isSgExpression(SgNodeHelper::getRhs(root)));
    SgExpression* newExp;
    //TODO: check whether build functions set parent pointers
    switch(root->variantT()) {
    case V_SgPlusAssignOp:
      newExp=SageBuilder::buildBinaryExpression<SgAddOp>(lhsCopy,rhsCopy);
      root=SageBuilder::buildBinaryExpression<SgAssignOp>(lhsCopy2,newExp);
      break;
    case V_SgDivAssignOp:
      newExp=SageBuilder::buildBinaryExpression<SgDivideOp>(lhsCopy,rhsCopy);
      root=SageBuilder::buildBinaryExpression<SgAssignOp>(lhsCopy2,newExp);
      break;
    case V_SgMinusAssignOp:
      newExp=SageBuilder::buildBinaryExpression<SgSubtractOp>(lhsCopy,rhsCopy);
      root=SageBuilder::buildBinaryExpression<SgAssignOp>(lhsCopy2,newExp);
      break;
    case V_SgMultAssignOp:
      newExp=SageBuilder::buildBinaryExpression<SgMultiplyOp>(lhsCopy,rhsCopy);
      root=SageBuilder::buildBinaryExpression<SgAssignOp>(lhsCopy2,newExp);
      break;
    default: /* ignore all other cases - all other expr remain unmodified */
      ;
    }
  }
}

 
 // rewrites an AST
 // requirements: all variables have been replaced by constants
 // uses AstMatching to match patterns.
void rewriteAst(SgNode*& root, VariableIdMapping* variableIdMapping, bool rewriteTrace=false, bool ruleAddReorder=false) {
   //  cout<<"Rewriting AST:"<<endl;
   bool someTransformationApplied=false;
   bool transformationApplied=false;
   AstMatching m;
   // outer loop (overall fixpoint on all transformations)
   /* Transformations:
      1) eliminate unary operator -(integer) in tree
      2) normalize expressions (reordering of inner nodes and leave nodes)
      3) constant folding (leave nodes)
   */

   {
     rewriteCompoundAssignments(root,variableIdMapping);
   }

   do{
     someTransformationApplied=false;


 do {
     // Rewrite-rule 1: $UnaryOpSg=MinusOp($IntVal1=SgIntVal) => SgIntVal.val=-$Intval.val
     transformationApplied=false;
     MatchResult res=m.performMatching(
                                       "$UnaryOp=SgMinusOp($IntVal=SgIntVal)\
                                       ",root);
     if(res.size()>0) {
       for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
         // match found
         SgExpression* op=isSgExpression((*i)["$UnaryOp"]);
         SgIntVal* val=isSgIntVal((*i)["$IntVal"]);
         //cout<<"FOUND UNARY CONST: "<<op->unparseToString()<<endl;
         int rawval=val->get_value();
         // replace with folded value (using integer semantics)
         switch(op->variantT()) {
         case V_SgMinusOp:
           myReplaceExpression(op,SageBuilder::buildIntVal(-rawval),false);
           break;
         default:
           cerr<<"Error: rewrite phase: unsopported operator in matched unary expression. Bailing out."<<endl;
           exit(1);
         }
         transformationApplied=true;
         someTransformationApplied=true;
         dump1_stats.numElimMinusOperator++;
       }
     }
  } while(transformationApplied); // a loop will eliminate -(-(5)) to 5

 if(ruleAddReorder) {
   do {
     // the following rules guarantee convergence
     
     // REWRITE: re-ordering (normalization) of expressions
     // Rewrite-rule 1: SgAddOp(SgAddOp($Remains,$Other),$IntVal=SgIntVal) => SgAddOp(SgAddOp($Remains,$IntVal),$Other) 
     //                 where $Other!=SgIntVal && $Other!=SgFloatVal && $Other!=SgDoubleVal; ($Other notin {SgIntVal,SgFloatVal,SgDoubleVal})
     transformationApplied=false;
     MatchResult res=m.performMatching("$BinaryOp1=SgAddOp(SgAddOp($Remains,$Other),$IntVal=SgIntVal)",root);
     if(res.size()>0) {
       for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
         // match found
         SgExpression* other=isSgExpression((*i)["$Other"]);
         if(other) {
           if(!isSgIntVal(other) && !isSgFloatVal(other) && !isSgDoubleVal(other)) {
             //SgNode* op1=(*i)["$BinaryOp1"];
             SgExpression* val=isSgExpression((*i)["$IntVal"]);
             //cout<<"FOUND: "<<op1->unparseToString()<<endl;
             if(rewriteTrace)
               cout<<"Rule AddOpReorder: "<<((*i)["$BinaryOp1"])->unparseToString()<<" => ";
             // replace op1-rhs with op2-rhs
             SgExpression* other_copy=SageInterface::copyExpression(other);
             SgExpression* val_copy=SageInterface::copyExpression(val);
             myReplaceExpression(other,val_copy,false);
             myReplaceExpression(val,other_copy,false);
             //cout<<"REPLACED: "<<op1->unparseToString()<<endl;
             transformationApplied=true;
             someTransformationApplied=true;
             if(rewriteTrace)
               cout<<((*i)["$BinaryOp1"])->unparseToString()<<endl;
             dump1_stats.numAddOpReordering++;
           }       
         }
       }
     }
   } while(transformationApplied);
 }

   // REWRITE: constant folding of constant integer (!) expressions
   // we intentionally avoid folding of float values
 do {
     // Rewrite-rule 2: SgAddOp($IntVal1=SgIntVal,$IntVal2=SgIntVal) => SgIntVal
     //                 where SgIntVal.val=$IntVal1.val+$IntVal2.val
     transformationApplied=false;
     MatchResult res=m.performMatching(
                                       "$BinaryOp1=SgAddOp($IntVal1=SgIntVal,$IntVal2=SgIntVal)\
                                       |$BinaryOp1=SgSubtractOp($IntVal1=SgIntVal,$IntVal2=SgIntVal)\
                                       |$BinaryOp1=SgMultiplyOp($IntVal1=SgIntVal,$IntVal2=SgIntVal)\
                                       |$BinaryOp1=SgDivideOp($IntVal1=SgIntVal,$IntVal2=SgIntVal)\
                                       ",root);
     if(res.size()>0) {
       for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
         // match found
         SgExpression* op1=isSgExpression((*i)["$BinaryOp1"]);
         SgIntVal* val1=isSgIntVal((*i)["$IntVal1"]);
         SgIntVal* val2=isSgIntVal((*i)["$IntVal2"]);
         //cout<<"FOUND CONST: "<<op1->unparseToString()<<endl;
         int rawval1=val1->get_value();
         int rawval2=val2->get_value();
         // replace with folded value (using integer semantics)
         switch(op1->variantT()) {
         case V_SgAddOp:
           myReplaceExpression(op1,SageBuilder::buildIntVal(rawval1+rawval2),false);
           break;
         case V_SgSubtractOp:
           myReplaceExpression(op1,SageBuilder::buildIntVal(rawval1-rawval2),false);
           break;
         case V_SgMultiplyOp:
           myReplaceExpression(op1,SageBuilder::buildIntVal(rawval1*rawval2),false);
           break;
         case V_SgDivideOp:
           myReplaceExpression(op1,SageBuilder::buildIntVal(rawval1/rawval2),false);
           break;
         default:
           cerr<<"Error: rewrite phase: unsopported operator in matched expression. Bailing out."<<endl;
           exit(1);
         }
         transformationApplied=true;
         someTransformationApplied=true;
         dump1_stats.numConstantFolding++;
       }
     }
  } while(transformationApplied);
 //if(someTransformationApplied) cout<<"DEBUG: transformed: "<<root->unparseToString()<<endl;
   } while(someTransformationApplied);
 }

 void substituteConstArrayIndexExprsWithConst(VariableIdMapping* variableIdMapping, ExprAnalyzer* exprAnalyzer, const EState* estate, SgNode* root) {
   typedef pair<SgExpression*,int> SubstitutionPair;
   typedef list<SubstitutionPair > SubstitutionList;
   SubstitutionList substitutionList;
   AstMatching m;
   MatchResult res;
 #pragma omp critical
   {
   res=m.performMatching("SgPntrArrRefExp(_,$ArrayIndexExpr)",root);
   }
   if(res.size()>0) {
     for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
         // match found
       SgExpression* arrayIndexExpr=isSgExpression((*i)["$ArrayIndexExpr"]);
       if(arrayIndexExpr) {
         // avoid substituting a constant by a constant
         if(!isSgIntVal(arrayIndexExpr)) {
           list<SingleEvalResultConstInt> evalResultList=exprAnalyzer->evalConstInt(arrayIndexExpr,*estate,true, true);
           // only when we get exactly one result it is considered for substitution
           // there can be multiple const-results which do not allow to replace it with a single const
           if(evalResultList.size()==1) {
             list<SingleEvalResultConstInt>::iterator i=evalResultList.begin();
             ROSE_ASSERT(evalResultList.size()==1);
             AValue varVal=(*i).value();
             if(varVal.isConstInt()) {
               int varIntValue=varVal.getIntValue();
               //cout<<"INFO: const: "<<varIntValue<<" substituting: "<<arrayIndexExpr->unparseToString()<<endl;
               myReplaceExpression(arrayIndexExpr,SageBuilder::buildIntVal(varIntValue));
               dump1_stats.numConstExprElim++;
             }
           }
         }
       }
     }
   }
 }

 void substituteVariablesWithConst(VariableIdMapping* variableIdMapping, const PState* pstate, SgNode *node) {
   typedef pair<SgExpression*,int> SubstitutionPair;
   typedef list<SubstitutionPair > SubstitutionList;
   SubstitutionList substitutionList;
   RoseAst ast(node);
   for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
     if(SgVarRefExp* varRef=isSgVarRefExp(*i)) {
       VariableId varRefId=variableIdMapping->variableId(varRef);
       if(pstate->varIsConst(varRefId)) {
         AValue varVal=pstate->varValue(varRefId);
         int varIntValue=varVal.getIntValue();
         SubstitutionPair p=make_pair(varRef,varIntValue);
         substitutionList.push_back(p);
       }
     }
   }
   for(SubstitutionList::iterator i=substitutionList.begin(); i!=substitutionList.end(); ++i) {
     // buildSignedIntType()
     // buildFloatType()
     // buildDoubleType()
     // SgIntVal* buildIntVal(int)
     // replaceExpression (SgExpression *oldExp, SgExpression *newExp, bool keepOldExp=false)
     //cout<<"subst:"<<(*i).first->unparseToString()<<" : "<<(*i).second<<endl;
#if 0
     SageInterface::replaceExpression((*i).first,SageBuilder::buildIntVal((*i).second));
#else
         myReplaceExpression((*i).first,SageBuilder::buildIntVal((*i).second));
         //isSgExpression((*i).first->get_parent())->replace_expression((*i).first,SageBuilder::buildIntVal((*i).second));
#endif
   }
   dump1_stats.numVariableElim+=substitutionList.size();
 }

 struct EStateExprInfo {
   const EState* first;
   SgExpression* second;
   bool mark;
   EStateExprInfo():first(0),second(0),mark(false){}
   EStateExprInfo(const EState* estate,SgExpression* exp):first(estate),second(exp),mark(false){}
 };

 //typedef pair<const EState*, SgExpression*> EStateExprPair;
 typedef vector<EStateExprInfo> ArrayUpdatesSequence;

 bool isAtMarker(Label lab, const EState* estate) {
   Label elab=estate->label();
   return elab==lab;
 }
 void extractArrayUpdateOperations(Analyzer* ana, ArrayUpdatesSequence& arrayUpdates, bool useConstExprSubstRule=true, Label startMarkerLabel=Labeler::NO_LABEL, Label endMarkerLabel=Labeler::NO_LABEL) {
   Labeler* labeler=ana->getLabeler();
   VariableIdMapping* variableIdMapping=ana->getVariableIdMapping();
   TransitionGraph* tg=ana->getTransitionGraph();
   const EState* estate=tg->getStartEState();
   EStatePtrSet succSet=tg->succ(estate);
   ExprAnalyzer* exprAnalyzer=ana->getExprAnalyzer();
   int numProcessedArrayUpdates=0;
   vector<pair<const EState*, SgExpression*> > stgArrayUpdateSequence;

   // initialize markers
   // if NO_LABEL is specified then assume start-label at first node and that the end-label is never found (on the path).
   bool foundStartMarker=false;
   bool foundEndMarker=false;
   if(startMarkerLabel==Labeler::NO_LABEL) {
     foundStartMarker=true;
   } else {
     foundStartMarker=isAtMarker(startMarkerLabel,estate);
   }
   if(endMarkerLabel==Labeler::NO_LABEL) {
     foundEndMarker=isAtMarker(endMarkerLabel,estate);
   } else {
     foundEndMarker=false;
   }

   while(succSet.size()>=1) {
     if(succSet.size()>1) {
       cerr<<estate->toString()<<endl;
       cerr<<"Error: STG-States with more than one successor not supported in term extraction yet."<<endl;
       exit(1);
     } else {
       EStatePtrSet::iterator i=succSet.begin();
       estate=*i;
     }  
     foundStartMarker=foundStartMarker||isAtMarker(startMarkerLabel,estate);
     foundEndMarker=foundEndMarker||isAtMarker(endMarkerLabel,estate);
     if(foundStartMarker && !foundEndMarker) {
       // investigate state
       Label lab=estate->label();
       SgNode* node=labeler->getNode(lab);
       // eliminate superfluous root nodes
       if(isSgExprStatement(node))
         node=SgNodeHelper::getExprStmtChild(node);
       if(isSgExpressionRoot(node))
         node=SgNodeHelper::getExprRootChild(node);
       if(SgExpression* exp=isSgExpression(node)) {
         if(SgNodeHelper::isArrayElementAssignment(exp)||SgNodeHelper::isFloatingPointAssignment(node)) {
           stgArrayUpdateSequence.push_back(make_pair(estate,exp));
         }
       }
     }

     // next successor set
     succSet=tg->succ(estate);
   }

   // stgArrayUpdateSequence is now a vector of all array update operations from the STG
   // prepare array for parallel assignments of rewritten ASTs
   arrayUpdates.resize(stgArrayUpdateSequence.size());
   int N=stgArrayUpdateSequence.size();

   // this loop is prepared for parallel execution (but rewriting the AST in parallel causes problems)
   //  #pragma omp parallel for
   for(int i=0;i<N;++i) {
     const EState* p_estate=stgArrayUpdateSequence[i].first;
     const PState* p_pstate=p_estate->pstate();
     SgExpression* p_exp=stgArrayUpdateSequence[i].second;
     SgNode* p_expCopy;
     p_expCopy=SageInterface::copyExpression(p_exp);
#if 1
     // p_expCopy is a pointer to an assignment expression (only rewriteAst changes this variable)
     if(useConstExprSubstRule) {
       substituteConstArrayIndexExprsWithConst(variableIdMapping, exprAnalyzer,p_estate,p_expCopy);
       rewriteCompoundAssignments(p_expCopy,variableIdMapping);
     } else {
       substituteVariablesWithConst(variableIdMapping,p_pstate,p_expCopy);
       rewriteAst(p_expCopy, variableIdMapping);
     }
#endif
     SgExpression* p_expCopy2=isSgExpression(p_expCopy);
     if(!p_expCopy2) {
       cerr<<"Error: wrong node type in array update extraction. Expected SgExpression* but found "<<p_expCopy->class_name()<<endl;
       exit(1);
    }
    numProcessedArrayUpdates++;
    if(numProcessedArrayUpdates%100==0) {
      cout<<"INFO: transformed arrayUpdates: "<<numProcessedArrayUpdates<<" / "<<stgArrayUpdateSequence.size() <<endl;
    }
    arrayUpdates[i]=EStateExprInfo(p_estate,p_expCopy2);
  }    
  
}

struct ArrayElementAccessData {
  VariableId varId;
  vector<int> subscripts;
  ArrayElementAccessData();
  ArrayElementAccessData(SgPntrArrRefExp* ref, VariableIdMapping* variableIdMapping);
  string toString(VariableIdMapping* variableIdMapping);
  //! checks validity of data. The default value is not valid (does not correspond to any array) but can be used when creating STL containers.
  bool isValid();
  bool operator==(ArrayElementAccessData& other) {
    for(size_t i=0;i<subscripts.size();++i)
      if(subscripts[i]!=other.subscripts[i])
        return false;
    return varId==other.varId;
  }
};

ArrayElementAccessData::ArrayElementAccessData() {
}

string ArrayElementAccessData::toString(VariableIdMapping* variableIdMapping) {
  if(isValid()) {
    stringstream ss;
    ss<< variableIdMapping->uniqueShortVariableName(varId);
    for(vector<int>::iterator i=subscripts.begin();i!=subscripts.end();++i) {
      ss<<"["<<*i<<"]";
    }
    return ss.str();
  } else {
    return "$non-valid-array-access$";
  }
}

bool ArrayElementAccessData::isValid() {
  return varId.isValid() && subscripts.size()>0;
}

ArrayElementAccessData::ArrayElementAccessData(SgPntrArrRefExp* ref, VariableIdMapping* variableIdMapping) {
  // determine data
  SgExpression* arrayNameExp;
  std::vector<SgExpression*> subscriptsvec;
  std::vector<SgExpression*> *subscripts=&subscriptsvec;
  SageInterface::isArrayReference(ref, &arrayNameExp, &subscripts);
  //cout<<"Name:"<<arrayNameExp->unparseToString()<<" arity"<<subscripts->size()<<"subscripts:";
  varId=variableIdMapping->variableId(SageInterface::convertRefToInitializedName(ref));
  //cout<<"NameCheck:"<<variableIdMapping->uniqueShortVariableName(access.varId)<<" ";
  for(size_t i=0;i<(*subscripts).size();++i) {
    //cout<<(*subscripts)[i]<<":"<<(*subscripts)[i]->unparseToString()<<" ";
    if(SgIntVal* subscriptint=isSgIntVal((*subscripts)[i])) {
      //cout<<"VAL:"<<subscriptint->get_value();
      this->subscripts.push_back(subscriptint->get_value());
    }
  }
  ROSE_ASSERT(this->subscripts.size()>0);
}

// searches the arrayUpdates vector backwards starting at pos, matches lhs array refs and returns a pointer to it (if not available it returns 0)
SgNode* findDefAssignOfArrayElementUse(SgPntrArrRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, VariableIdMapping* variableIdMapping) {
  ArrayElementAccessData useRefData(useRefNode,variableIdMapping);
  do {
    SgPntrArrRefExp* lhs=isSgPntrArrRefExp(SgNodeHelper::getLhs((*pos).second));
    // there can be non-array element updates on lhs
    if(lhs) {
      ArrayElementAccessData defData(isSgPntrArrRefExp(lhs),variableIdMapping);
      if(defData==useRefData) {
        (*pos).mark=true; // mark each used definition
        return (*pos).second; // return pointer to assignment expression (instead directly to def);
      }
    }
    // there is no concept for before-the-start iterator (therefore this is checked this way) -> change this rbegin/rend
    if(pos==arrayUpdates.begin()) 
      break;
    --pos; 
  } while (1);

  return 0;
}

// searches the arrayUpdates vector backwards starting at pos, matches lhs array refs and returns a pointer to it (if not available it returns 0)
SgNode* findDefAssignOfUse(SgVarRefExp* useRefNode, ArrayUpdatesSequence& arrayUpdates, ArrayUpdatesSequence::iterator pos, VariableIdMapping* variableIdMapping) {
  VariableId useRefId=variableIdMapping->variableId(useRefNode);
  do {
    SgVarRefExp* lhs=isSgVarRefExp(SgNodeHelper::getLhs((*pos).second));
    // there can be non-var-refs updates on lhs
    if(lhs) {
      VariableId defId=variableIdMapping->variableId(lhs);
      if(defId==useRefId) {
        (*pos).mark=true; // mark each used definition
        return (*pos).second; // return pointer to assignment expression (instead directly to def);
      }
    }
    // there is no concept for before-the-start iterator (therefore this is checked this way) -> change this rbegin/rend
    if(pos==arrayUpdates.begin()) 
      break;
    --pos; 
  } while (1);

  return 0;
}

class NumberAstAttribute : public AstAttribute {
public:
  int index;
  NumberAstAttribute():index(-1){}
  NumberAstAttribute(int index):index(index){}
  string toString() {
    stringstream ss;
    ss<<index;
    return ss.str();
  }
};

#include <map>
enum SAR_MODE { SAR_SUBSTITUTE, SAR_SSA };

// linear algorithm. Only works for a sequence of assignments.
void createSsaNumbering(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping) {
  std::map<string,int> defVarNumbers;
  for(size_t i=0;i<arrayUpdates.size();++i) {
    SgExpression* exp=arrayUpdates[i].second;
    SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(exp));

    // determine SSA number of uses and attach
    SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(exp));
    ROSE_ASSERT(isSgPntrArrRefExp(lhs)||SgNodeHelper::isFloatingPointAssignment(exp));
    //cout<<"EXP: "<<exp->unparseToString()<<", lhs:"<<lhs->unparseToString()<<" :: "<<endl;
    RoseAst rhsast(rhs);
    for(RoseAst::iterator j=rhsast.begin();j!=rhsast.end();++j) {
      if(SgPntrArrRefExp* useRef=isSgPntrArrRefExp(*j)) {
        j.skipChildrenOnForward();
        ArrayElementAccessData access(useRef,variableIdMapping);
        string useName=access.toString(variableIdMapping);
        AstAttribute* attr=new NumberAstAttribute(defVarNumbers[useName]); // default creates 0 int (which is exactly what we need)
        if(attr) {
          useRef->setAttribute("Number",attr);
        }
      } // if array
      // this can be rewritten once it is clear that the element type of an array is properly reported by isFloatingPointExpr(exp)
      else if(SgVarRefExp* useRef=isSgVarRefExp(*j)) {
        ROSE_ASSERT(useRef);
        j.skipChildrenOnForward();
        VariableId varId=variableIdMapping->variableId(useRef);
        string useName=variableIdMapping->uniqueShortVariableName(varId);
        AstAttribute* attr=new NumberAstAttribute(defVarNumbers[useName]); // default creates 0 int (which is exactly what we need)
        if(attr) {
          useRef->setAttribute("Number",attr);
        }
      } else {
        //cout<<"INFO: UpdateExtraction: ignored expression on rhs:"<<(*j)->unparseToString()<<endl;
      }
    }

    // compute and attach SSA number for def (lhs)
    string name;
    SgNode* toAnnotate=0;
    if(SgPntrArrRefExp* arr=isSgPntrArrRefExp(lhs)) {
      ArrayElementAccessData access(arr,variableIdMapping);
      name=access.toString(variableIdMapping);
      toAnnotate=arr;
    } else if(SgVarRefExp* var=isSgVarRefExp(lhs)) {
      VariableId varId=variableIdMapping->variableId(var);
      name=variableIdMapping->uniqueShortVariableName(varId);
      toAnnotate=var;
    } else {
      cerr<<"Error: SSA Numbering: unknown LHS."<<endl;
      exit(1);
    }
    if(toAnnotate) {
      if(defVarNumbers.count(name)==0) {
        defVarNumbers[name]=1;
      } else {
        defVarNumbers[name]=defVarNumbers[name]+1;
      }
      toAnnotate->setAttribute("Number",new NumberAstAttribute(defVarNumbers[name]));
    }
  } // end assignments for loop
}


// this function has become superfluous for SSA numbering (but for substituting uses with rhs of defs it is still necessary (1/2)
void attachSsaNumberingtoDefs(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping) {
  std::map<string,int> defVarNumbers;
  for(size_t i=0;i<arrayUpdates.size();++i) {
    SgExpression* exp=arrayUpdates[i].second;
    SgNode* lhs=SgNodeHelper::getLhs(exp);
    string name;
    SgNode* toAnnotate=0;
    if(SgPntrArrRefExp* arr=isSgPntrArrRefExp(lhs)) {
      ArrayElementAccessData access(arr,variableIdMapping);
      name=access.toString(variableIdMapping);
      toAnnotate=arr;
    } else if(SgVarRefExp* var=isSgVarRefExp(lhs)) {
      VariableId varId=variableIdMapping->variableId(var);
      name=variableIdMapping->uniqueShortVariableName(varId);
      toAnnotate=var;
    } else {
      cerr<<"Error: SSA Numbering: unknown LHS."<<endl;
      exit(1);
    }
    if(toAnnotate) {
      if(defVarNumbers.count(name)==0) {
        defVarNumbers[name]=1;
      } else {
        defVarNumbers[name]=defVarNumbers[name]+1;
      }
      toAnnotate->setAttribute("Number",new NumberAstAttribute(defVarNumbers[name]));
    }
  }
}

// this function has become superfluous for SSA numbering (but for substituting uses with rhs of defs it is still necessary (2/2)
void substituteArrayRefs(ArrayUpdatesSequence& arrayUpdates, VariableIdMapping* variableIdMapping, SAR_MODE sarMode) {
  ArrayUpdatesSequence::iterator i=arrayUpdates.begin();
  ++i; // we start at element 2 because no substitutions can be performed in the first one AND this simplifies passing the previous element (i-1) when starting the backward search
  for(;i!=arrayUpdates.end();++i) {
    SgExpression* exp=(*i).second;
    SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(exp));
    SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(exp));
    ROSE_ASSERT(isSgPntrArrRefExp(lhs)||SgNodeHelper::isFloatingPointAssignment(exp));
    //cout<<"EXP: "<<exp->unparseToString()<<", lhs:"<<lhs->unparseToString()<<" :: "<<endl;
    RoseAst rhsast(rhs);
    for(RoseAst::iterator j=rhsast.begin();j!=rhsast.end();++j) {
      if(SgPntrArrRefExp* useRef=isSgPntrArrRefExp(*j)) {
        j.skipChildrenOnForward();
        // search for def here
        ArrayUpdatesSequence::iterator i_copy=i;
        --i_copy; // necessary and guaranteed to not decrement i=begin() because the loop starts at (i=begin())++
        SgNode* defAssign=findDefAssignOfArrayElementUse(useRef, arrayUpdates, i_copy, variableIdMapping);
        if(defAssign) {
          SgExpression* defRhs=isSgExpression(SgNodeHelper::getRhs(defAssign));
          ROSE_ASSERT(defRhs);
          //cout<<"INFO: USE:"<<useRef->unparseToString()<< " DEF:"<<defAssign->unparseToString()<<"DEF-RHS"<<defRhs->unparseToString()<<endl;
          switch(sarMode) {
          case SAR_SUBSTITUTE: {
            myReplaceExpression(useRef,SageInterface::copyExpression(defRhs),true); // must be true (otherwise internal error)
            break;
          }
          case SAR_SSA: {
            AstAttribute* attr=SgNodeHelper::getLhs(defAssign)->getAttribute("Number");
            if(attr) {
              useRef->setAttribute("Number",attr);
            }
            break;
          }
          } // end switch
        }
      } // if array
      // this can be rewritten once it is clear that the element type of an array is properly reported by isFloatingPointExpr(exp)
      else if(SgVarRefExp* useRef=isSgVarRefExp(*j)) {
        ROSE_ASSERT(useRef);
        j.skipChildrenOnForward();
        // search for def here
        ArrayUpdatesSequence::iterator i_copy=i;
        --i_copy; // necessary and guaranteed to not decrement i=begin() because the loop starts at (i=begin())++
        SgNode* defAssign=findDefAssignOfUse(useRef, arrayUpdates, i_copy, variableIdMapping);
        if(defAssign) {
          SgExpression* defRhs=isSgExpression(SgNodeHelper::getRhs(defAssign));
          ROSE_ASSERT(defRhs);
          //cout<<"INFO: USE:"<<useRef->unparseToString()<< " DEF:"<<defAssign->unparseToString()<<"DEF-RHS"<<defRhs->unparseToString()<<endl;
          switch(sarMode) {
          case SAR_SUBSTITUTE: {
            myReplaceExpression(useRef,SageInterface::copyExpression(defRhs),true); // must be true (otherwise internal error)
            break;
          }
          case SAR_SSA: {
            AstAttribute* attr=SgNodeHelper::getLhs(defAssign)->getAttribute("Number");
            if(attr) {
              useRef->setAttribute("Number",attr);
            }
            break;
          }
          } // end switch
        }
      } else {
        //cout<<"INFO: UpdateExtraction: ignored expression on rhs:"<<(*j)->unparseToString()<<endl;
      }
    }
  }
}

bool compare_array_accesses(EStateExprInfo& e1, EStateExprInfo& e2) {
  //ArrayElementAccessData d1(SgNodeHelper::getLhs(e1.second));
  //SgNodeHelper::getLhs(e2.second);
  return false;
}

#include<algorithm>
void sortArrayUpdates(ArrayUpdatesSequence& arrayUpdates) {

}

void writeArrayUpdatesToFile(ArrayUpdatesSequence& arrayUpdates, string filename, SAR_MODE sarMode, bool performSorting) {
  // 1) create vector of generated assignments (preparation for sorting)
  vector<string> assignments;
  for(ArrayUpdatesSequence::iterator i=arrayUpdates.begin();i!=arrayUpdates.end();++i) {
    switch(sarMode) {
    case SAR_SSA: {
      // annotate AST for unparsing Array-SSA form
      RoseAst ast((*i).second);
      for(RoseAst::iterator j=ast.begin();j!=ast.end();++j) {
        if((*j)->attributeExists("Number")) {
          ROSE_ASSERT(isSgPntrArrRefExp(*j)||isSgVarRefExp(*j));
          if((*j)->attributeExists("AstUnparseAttribute"))
            (*j)->removeAttribute("AstUnparseAttribute");
          ROSE_ASSERT(!(*j)->attributeExists("AstUnparseAttribute"));
          AstUnparseAttribute* ssaNameAttribute=new AstUnparseAttribute((*j)->unparseToString()+string("_")+(*j)->getAttribute("Number")->toString(),AstUnparseAttribute::e_replace);
          (*j)->setAttribute("AstUnparseAttribute",ssaNameAttribute);
        }            
      }
      assignments.push_back((*i).second->unparseToString());
      break;
    }
    case SAR_SUBSTITUTE: {
      if(!(*i).mark)
        assignments.push_back((*i).second->unparseToString());
    }
    }
  }
  if(performSorting) {
    sort(assignments.begin(),assignments.end());
  }
  ofstream myfile;
  myfile.open(filename.c_str());
  for(vector<string>::iterator i=assignments.begin();i!=assignments.end();++i) {
    myfile<<(*i)<<endl;
  }
  myfile.close();
}

int main( int argc, char * argv[] ) {
  string ltl_file;
  try {
    Timer timer;
    timer.start();

  // Command line option handling.
  po::options_description desc
    ("CodeThorn V1.2\n"
     "Written by Markus Schordan and Adrian Prantl 2012\n"
     "Supported options");

  desc.add_options()
    ("help,h", "produce this help message")
    ("rose-help", "show help for compiler frontend options")
    ("version,v", "display the version")
    ("internal-checks", "run internal consistency checks (without input program)")
    ("verify", po::value< string >(), "verify all LTL formulae in the file [arg]")
    ("ltl-verifier",po::value< int >(),"specify which ltl-verifier to use [=1|2]")
    ("debug-mode",po::value< int >(),"set debug mode [arg]")
    ("csv-ltl", po::value< string >(), "output LTL verification results into a CSV file [arg]")
    ("csv-assert", po::value< string >(), "output assert reachability results into a CSV file [arg]")
    ("csv-assert-live", po::value< string >(), "output assert reachability results during analysis into a CSV file [arg]")
    ("csv-stats",po::value< string >(),"output statistics into a CSV file [arg]")
    ("tg1-estate-address", po::value< string >(), "transition graph 1: visualize address [=yes|no]")
    ("tg1-estate-id", po::value< string >(), "transition graph 1: visualize estate-id [=yes|no]")
    ("tg1-estate-properties", po::value< string >(), 
     "transition graph 1: visualize all estate-properties [=yes|no]")
    ("tg2-estate-address", po::value< string >(), "transition graph 2: visualize address [=yes|no]")
    ("tg2-estate-id", po::value< string >(), "transition graph 2: visualize estate-id [=yes|no]")
    ("tg2-estate-properties", po::value< string >(),
     "transition graph 2: visualize all estate-properties [=yes|no]")
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
    ("arith-top",po::value< string >(),"Arithmetic operations +,-,*,/,% always evaluate to top [=yes|no]")
    ("abstract-interpreter",po::value< string >(),"Run analyzer in abstract interpreter mode. Use [=yes|no]")
    ("rers-binary",po::value< string >(),"Call rers binary functions in analysis. Use [=yes|no]")
    ("print-all-options",po::value< string >(),"print all yes/no command line options.")
    ("annotate-results",po::value< string >(),"annotate results in program and output program (using ROSE unparser).")
    ("generate-assertions",po::value< string >(),"generate assertions (pre-conditions) in program and output program (using ROSE unparser).")
    ("rersformat",po::value< int >(),"Set year of rers format (2012, 2013).")
    ("max-transitions",po::value< int >(),"Passes (possibly) incomplete STG to verifier after max transitions (default: no limit).")
    ("dot-io-stg", po::value< string >(), "output STG with explicit I/O node information in dot file [arg]")
    ("stderr-like-failed-assert", po::value< string >(), "treat output on stderr similar to a failed assert [arg] (default:no)")
    ("rersmode", po::value< string >(), "sets several options such that RERS-specifics are utilized and observed.")
    ("rers-numeric", po::value< string >(), "print rers I/O values as raw numeric numbers.")
    ("exploration-mode",po::value< string >(), " set mode in which state space is explored ([breadth-first], depth-first)/")
    ("eliminate-stg-back-edges",po::value< string >(), " eliminate STG back-edges (STG becomes a tree).")
    ("spot-stg",po::value< string >(), " generate STG in SPOT-format in file [arg]")
    ("dump1",po::value< string >(), " [experimental] generates array updates in file arrayupdates.txt")
    ("dump-sorted",po::value< string >(), " [experimental] generates sorted array updates in file <file>")
    ("dump-non-sorted",po::value< string >(), " [experimental] generates non-sorted array updates in file <file>")
    ("rule-const-subst",po::value< string >(), " [experimental] use const-expr substitution rule <arg>")
    ("limit-to-fragment",po::value< string >(), "the argument is used to find fragments marked by two prgagmas of that '<name>' and 'end<name>'")
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
    cout << "CodeThorn version 1.4\n";
    cout << "Written by Markus Schordan and Adrian Prantl 2012-2014\n";
    return 0;
  }

  boolOptions.init(argc,argv);
  boolOptions.registerOption("tg1-estate-address",false);
  boolOptions.registerOption("tg1-estate-id",false);
  boolOptions.registerOption("tg1-estate-properties",true);
  boolOptions.registerOption("tg2-estate-address",false);
  boolOptions.registerOption("tg2-estate-id",true);
  boolOptions.registerOption("tg2-estate-properties",false);
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

  boolOptions.registerOption("viz",false);
  boolOptions.registerOption("update-input-var",true);
  boolOptions.registerOption("run-rose-tests",false);
  boolOptions.registerOption("reduce-cfg",true);
  boolOptions.registerOption("print-all-options",false);
  boolOptions.registerOption("annotate-results",false);
  boolOptions.registerOption("generate-assertions",false);
  boolOptions.registerOption("skip-analysis",false);

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
  boolOptions.registerOption("dump1",false);
  boolOptions.registerOption("rule-const-subst",true);

  boolOptions.processOptions();

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
    ltl_file = args["verify"].as<string>();
  }
  if(args.count("csv-assert-live")) {
    analyzer._csv_assert_live_file=args["csv-assert-live"].as<string>();
  }

  if(args.count("input-values")) {
    string setstring=args["input-values"].as<string>();
    cout << "STATUS: input-values="<<setstring<<endl;

    set<int> intSet=Parse::integerSet(setstring);
    for(set<int>::iterator i=intSet.begin();i!=intSet.end();++i) {
      analyzer.insertInputVarValue(*i);
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
    if(explorationMode=="depth-first")
      analyzer.setExplorationMode(Analyzer::EXPL_DEPTH_FIRST);
    else if(explorationMode=="breadth-first") {
      analyzer.setExplorationMode(Analyzer::EXPL_BREADTH_FIRST);
    } else {
      cerr<<"Error: unknown state space exploration mode specified with option --exploration-mode."<<endl;
      exit(1);
    }
  }
  if(args.count("max-transitions")) {
    analyzer.setMaxTransitions(args["max-transitions"].as<int>());
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

  // clean up string-options in argv
  for (int i=1; i<argc; ++i) {
    if (string(argv[i]) == "--csv-assert" 
        || string(argv[i])=="--csv-stats" 
        || string(argv[i])=="--csv-assert-live"
        || string(argv[i])=="--threads" 
        || string(argv[i])=="--display-diff"
        || string(argv[i])=="--input-values"
        || string(argv[i])=="--ltl-verifier"
        || string(argv[i])=="--dot-io-stg"
        || string(argv[i])=="--verify"
        || string(argv[i])=="--csv-ltl"
        || string(argv[i])=="--spot-stg"
        || string(argv[i])=="--dump-sorted"
        || string(argv[i])=="--dump-non-sorted"
        || string(argv[i])=="--limit-to-fragment"
        ) {
      // do not confuse ROSE frontend
      argv[i] = strdup("");
      assert(i+1<argc);
        argv[i+1] = strdup("");
    }
  }

  // reset dump1 in case sorted or non-sorted is used
  if(args.count("dump-sorted")>0 || args.count("dump-non-sorted")>0) {
    boolOptions.registerOption("dump1",true);
    if(numberOfThreadsToUse>1) {
      //cerr<<"Error: multi threaded rewrite not supported yet."<<endl;
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
  SgProject* sageProject = frontend(argc,argv);
  double frontEndRunTime=timer.getElapsedTimeInMilliSec();
  cout << "INIT: Parsing and creating AST: finished."<<endl;
  
  if(boolOptions["run-rose-tests"]) {
    cout << "INIT: Running ROSE AST tests."<<endl;
    // Run internal consistency tests on AST
    AstTests::runAllTests(sageProject);
  }

  SgNode* root=sageProject;

  if(!boolOptions["skip-analysis"])
  {
    cout << "INIT: Checking input program."<<endl;
    CodeThornLanguageRestrictor lr;
    lr.checkProgram(root);
    timer.start();

  cout << "INIT: Running variable<->symbol mapping check."<<endl;
  //VariableIdMapping varIdMap;
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

  SgNode* fragmentStartNode=0;
  SgNode* fragmentEndNode=0;

  if(option_pragma_name!="") {
    list<SgPragmaDeclaration*> pragmaDeclList=findPragmaDeclarations(root, option_pragma_name);
    if(pragmaDeclList.size()==0) {
      cerr<<"Error: pragma "<<option_pragma_name<<" marking the fragment not found."<<endl;
      exit(1);
    }
    if(pragmaDeclList.size()==1) {
      cerr<<"Error: pragma "<<option_pragma_name<<" only found once. The fragment is required to be marked with two pragmas."<<endl;
      exit(1);
    }
    if(pragmaDeclList.size()>2) {
      cerr<<"Error: pragma "<<option_pragma_name<<" : too many markers found ("<<pragmaDeclList.size()<<")"<<endl;
      exit(1);
    }
    cout<<"STATUS: Fragment marked by "<<option_pragma_name<<": correctly identified."<<endl;

    ROSE_ASSERT(pragmaDeclList.size()==2);
    list<SgPragmaDeclaration*>::iterator i=pragmaDeclList.begin();
    fragmentStartNode=*i;
    ++i;
    fragmentEndNode=*i;
  }

  cout << "INIT: creating solver."<<endl;
  analyzer.initializeSolver1("main",root);
  analyzer.initLabeledAssertNodes(sageProject);
  if(boolOptions["dump1"]) {
    // extraction of expressions: skip function calls to selected functions (also inside expressions) for defered handling.
    analyzer.setSkipSelectedFunctionCalls(true);
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
    analyzer.semanticEliminationOfTransitions();
  }

  if(boolOptions["semantic-explosion"]) {
    analyzer.semanticExplosionOfInputNodesFromOutputNodeConstraints();
  }

  double analysisRunTime=timer.getElapsedTimeInMilliSec();

  // since CT1.2 the ADT TransitionGraph ensures that no duplicates can exist
#if 0
  long removed=analyzer.getTransitionGraph()->removeDuplicates();
  cout << "Transitions reduced: "<<removed<<endl;
#endif

  cout << "=============================================================="<<endl;

    analyzer.reachabilityResults.printResults();
#if 0
  // TODO: reachability in presence of semantic folding
  if(boolOptions["semantic-fold"] || boolOptions["post-semantic-fold"]) {

  } else {
    printAsserts(analyzer,sageProject);
  }
#endif
  if (args.count("csv-assert")) {
    string filename=args["csv-assert"].as<string>().c_str();
    switch(resultsFormat) {
    case RF_RERS2012: analyzer.reachabilityResults.write2012File(filename.c_str());break;
    case RF_RERS2013: analyzer.reachabilityResults.write2013File(filename.c_str());break;
    default: assert(0);
    }
    //    OLD VERSION:  generateAssertsCsvFile(analyzer,sageProject,filename);
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

  timer.start();
  if (ltl_file.size()) {
    generateLTLOutput(analyzer,ltl_file);
    cout << "=============================================================="<<endl;
  }
  double ltlRunTime=timer.getElapsedTimeInMilliSec();
  // TODO: reachability in presence of semantic folding
  //  if(boolOptions["semantic-fold"] || boolOptions["post-semantic-fold"]) {
    analyzer.reachabilityResults.printResultsStatistics();
    //  } else {
    //printAssertStatistics(analyzer,sageProject);
    //}
  cout << "=============================================================="<<endl;

  double totalRunTime=frontEndRunTime+initRunTime+ analysisRunTime+ltlRunTime;

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

  cout <<color("white");
  cout << "Number of stdin-estates        : "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDIN_VAR))<<color("white")<<endl;
  cout << "Number of stdoutvar-estates    : "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_VAR))<<color("white")<<endl;
  cout << "Number of stdoutconst-estates  : "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_CONST))<<color("white")<<endl;
  cout << "Number of stderr-estates       : "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::STDERR_VAR))<<color("white")<<endl;
  cout << "Number of failed-assert-estates: "<<color("cyan")<<(analyzer.getEStateSet()->numberOfIoTypeEStates(InputOutput::FAILED_ASSERT))<<color("white")<<endl;
  cout << "Number of const estates        : "<<color("cyan")<<(analyzer.getEStateSet()->numberOfConstEStates(analyzer.getVariableIdMapping()))<<color("white")<<endl;

  cout << "=============================================================="<<endl;
  cout << "Number of pstates              : "<<color("magenta")<<pstateSetSize<<color("white")<<" (memory: "<<color("magenta")<<pstateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<pstateSetLoadFactor<<  "/"<<pstateSetMaxCollisions<<")"<<endl;
  cout << "Number of estates              : "<<color("cyan")<<eStateSetSize<<color("white")<<" (memory: "<<color("cyan")<<eStateSetBytes<<color("white")<<" bytes)"<<" ("<<""<<eStateSetLoadFactor<<  "/"<<eStateSetMaxCollisions<<")"<<endl;
  cout << "Number of transitions          : "<<color("blue")<<transitionGraphSize<<color("white")<<" (memory: "<<color("blue")<<transitionGraphBytes<<color("white")<<" bytes)"<<endl;
  cout << "Number of constraint sets      : "<<color("yellow")<<numOfconstraintSets<<color("white")<<" (memory: "<<color("yellow")<<constraintSetsBytes<<color("white")<<" bytes)"<<" ("<<""<<constraintSetsLoadFactor<<  "/"<<constraintSetsMaxCollisions<<")"<<endl;
  cout << "=============================================================="<<endl;
  long totalMemory=pstateSetBytes+eStateSetBytes+transitionGraphBytes+constraintSetsBytes;
  cout << "Memory total         : "<<color("green")<<totalMemory<<" bytes"<<color("white")<<endl;
  cout << "Time total           : "<<color("green")<<readableruntime(totalRunTime)<<color("white")<<endl;
  cout << "=============================================================="<<endl;
  cout <<color("normal");

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
  
  if(boolOptions["dump1"]) {
    ArrayUpdatesSequence arrayUpdates;
    cout<<"STATUS: performing array analysis on STG."<<endl;
    cout<<"STATUS: identifying array-update operations in STG and transforming them."<<endl;
    timer.start();

    Label fragmentStartLabel=Labeler::NO_LABEL;
    if(fragmentStartNode!=0) {
      fragmentStartLabel=analyzer.getLabeler()->getLabel(fragmentStartNode);
      cout<<"INFO: Fragment: start-node: "<<fragmentStartNode<<"  start-label: "<<fragmentStartLabel<<endl;
    }
    
    Label fragmentEndLabel=Labeler::NO_LABEL;
    if(fragmentEndNode!=0) {
      fragmentEndLabel=analyzer.getLabeler()->getLabel(fragmentEndNode);
      cout<<"INFO: Fragment: end-node  : "<<fragmentEndNode<<  "  end-label  : "<<fragmentEndLabel<<endl;
    }

    bool useConstSubstitutionRule=boolOptions["rule-const-subst"];
    extractArrayUpdateOperations(&analyzer,
                                 arrayUpdates,
                                 useConstSubstitutionRule,
                                 fragmentStartLabel,fragmentEndLabel);
    arrayUpdateExtractionRunTime=timer.getElapsedTimeInMilliSec();
    dump1_stats.numArrayUpdates=arrayUpdates.size();
    cout<<"STATUS: establishing array-element SSA numbering."<<endl;
    timer.start();
#if 0
    attachSsaNumberingtoDefs(arrayUpdates, analyzer.getVariableIdMapping());
    substituteArrayRefs(arrayUpdates, analyzer.getVariableIdMapping(),SAR_SSA);
#else
    createSsaNumbering(arrayUpdates, analyzer.getVariableIdMapping());
#endif
    arrayUpdateSsaNumberingRunTime=timer.getElapsedTimeInMilliSec();
    
    cout<<"STATUS: generating normalized array-assignments file \"arrayupdates.txt\"."<<endl;
    if(args.count("dump-non-sorted")) {
      string filename=args["dump-non-sorted"].as<string>();
      writeArrayUpdatesToFile(arrayUpdates, filename, SAR_SSA, false);
    }
    if(args.count("dump-sorted")) {
	  timer.start();
      string filename=args["dump-sorted"].as<string>();
      writeArrayUpdatesToFile(arrayUpdates, filename, SAR_SSA, true);
	  sortingAndIORunTime=timer.getElapsedTimeInMilliSec();
    }
    if(boolOptions["dump1"]) {
      //string filename="arrayupdates.txt";
      //writeArrayUpdatesToFile(arrayUpdates, filename, SAR_SSA, true);
    }
    totalRunTime+=arrayUpdateExtractionRunTime+arrayUpdateSsaNumberingRunTime+sortingAndIORunTime;
  }
  
  if(args.count("csv-stats")) {
    string filename=args["csv-stats"].as<string>().c_str();
    stringstream text;
    text<<"Sizes,"<<pstateSetSize<<", "
        <<eStateSetSize<<", "
        <<transitionGraphSize<<", "
        <<numOfconstraintSets<<endl;
    text<<"Memory,"<<pstateSetBytes<<", "
        <<eStateSetBytes<<", "
        <<transitionGraphBytes<<", "
        <<constraintSetsBytes<<", "
        <<totalMemory<<endl;
    text<<"Runtime(readable),"
        <<readableruntime(frontEndRunTime)<<", "
        <<readableruntime(initRunTime)<<", "
        <<readableruntime(analysisRunTime)<<", "
        <<readableruntime(ltlRunTime)<<", "
        <<readableruntime(arrayUpdateExtractionRunTime)<<", "
        <<readableruntime(arrayUpdateSsaNumberingRunTime)<<", "
        <<readableruntime(sortingAndIORunTime)<<", "
        <<readableruntime(totalRunTime)<<endl;
    text<<"Runtime(ms),"
        <<frontEndRunTime<<", "
        <<initRunTime<<", "
        <<analysisRunTime<<", "
        <<ltlRunTime<<", "
        <<arrayUpdateExtractionRunTime<<", "
        <<arrayUpdateSsaNumberingRunTime<<", "
        <<sortingAndIORunTime<<", "
        <<totalRunTime<<endl;
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
    text<<"rewrite-stats, "
        <<dump1_stats.numArrayUpdates<<", "
        <<dump1_stats.numElimMinusOperator<<", "
        <<dump1_stats.numElimAssignOperator<<", "
        <<dump1_stats.numAddOpReordering<<", "
        <<dump1_stats.numConstantFolding<<", "
        <<dump1_stats.numVariableElim<<", "
        <<dump1_stats.numConstExprElim
        <<endl;
    write_file(filename,text.str());
    cout << "generated "<<filename<<endl;
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
    string spotSTG=analyzer.generateSpotSTG();
    write_file(filename, spotSTG);
    cout << "=============================================================="<<endl;
  }




#if 0
  {
    cout << "EStateSet:\n"<<analyzer.getEStateSet()->toString()<<endl;
  }
#endif

#if 0
  {
    cout << "MAP:"<<endl;
    cout << analyzer.getLabeler()->toString();
  }
#endif

#if 0
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
  } // skip-analysis end
  
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

