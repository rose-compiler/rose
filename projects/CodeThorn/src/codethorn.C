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

// test
#include "Evaluator.h"

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
	cout<<"DEBUG: generating assert results for binary analysis."<<endl;
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


 int substituteConstArrayIndexExprsWithConst(VariableIdMapping* variableIdMapping, ExprAnalyzer* exprAnalyzer, const EState* estate, SgNode* root) {
   typedef pair<SgExpression*,int> SubstitutionPair;
   typedef list<SubstitutionPair > SubstitutionList;
   SubstitutionList substitutionList;
   AstMatching m;
   MatchResult res;
   int numConstExprElim=0;
#pragma omp critical(EXPRSUBSTITUTION)
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
               SgNodeHelper::replaceExpression(arrayIndexExpr,SageBuilder::buildIntVal(varIntValue),false);
               numConstExprElim++;
             }
           }
         }
       }
     }
   }
   return numConstExprElim;
 }

 int substituteVariablesWithConst(VariableIdMapping* variableIdMapping, const PState* pstate, SgNode *node) {
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
     SgNodeHelper::replaceExpression((*i).first,SageBuilder::buildIntVal((*i).second),false);
   }
   return (int)substitutionList.size();
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
void extractArrayUpdateOperations(Analyzer* ana,
                                  ArrayUpdatesSequence& arrayUpdates,
                                  RewriteSystem& rewriteSystem,
                                  bool useConstExprSubstRule=true,
                                  Label startMarkerLabel=Labeler::NO_LABEL,
                                  Label endMarkerLabel=Labeler::NO_LABEL
                                  ) {
   Labeler* labeler=ana->getLabeler();
   VariableIdMapping* variableIdMapping=ana->getVariableIdMapping();
   TransitionGraph* tg=ana->getTransitionGraph();
   const EState* estate=tg->getStartEState();
   EStatePtrSet succSet=tg->succ(estate);
   ExprAnalyzer* exprAnalyzer=ana->getExprAnalyzer();
   int numProcessedArrayUpdates=0;
   vector<pair<const EState*, SgExpression*> > stgArrayUpdateSequence;

   // initialize markers. if NO_LABEL is specified then assume
   // start-label at first node and that the end-label is never found
   // (on the path).
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
     if(succSet.size()>1) {
       cerr<<estate->toString()<<endl;
       cerr<<"Error: STG-States with more than one successor not supported in term extraction yet."<<endl;
       exit(1);
     } else {
       EStatePtrSet::iterator i=succSet.begin();
       estate=*i;
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
       int numConstExprElim=substituteConstArrayIndexExprsWithConst(variableIdMapping, exprAnalyzer,p_estate,p_expCopy);
       rewriteSystem.dump1_stats.numConstExprElim+=numConstExprElim;
       rewriteSystem.rewriteCompoundAssignments(p_expCopy,variableIdMapping);
     } else {
       rewriteSystem.dump1_stats.numVariableElim+=substituteVariablesWithConst(variableIdMapping,p_pstate,p_expCopy);
       rewriteSystem.rewriteAst(p_expCopy, variableIdMapping);
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
            SgNodeHelper::replaceExpression(useRef,SageInterface::copyExpression(defRhs),true); // must be true (otherwise internal error)
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
            SgNodeHelper::replaceExpression(useRef,SageInterface::copyExpression(defRhs),true); // must be true (otherwise internal error)
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

#if 0
bool compare_array_accesses(EStateExprInfo& e1, EStateExprInfo& e2) {
  //ArrayElementAccessData d1(SgNodeHelper::getLhs(e1.second));
  //SgNodeHelper::getLhs(e2.second);
  return false;
}

#include<algorithm>
void sortArrayUpdates(ArrayUpdatesSequence& arrayUpdates) {

}
#endif

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

#if 0
SgExpressionPtrList& getInitializerListOfArrayVariable(VariableId arrayVar, VariableIdMapping* variableIdMapping) {
  SgVariableDeclaration* decl=variableIdMapping->getVariableDeclaration(arrayVar);
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  ROSE_ASSERT(initName0);
  if(SgInitializedName* initName=isSgInitializedName(initName0)) {
    // array initializer
    SgInitializer* initializer=initName->get_initializer();
    if(SgAggregateInitializer* arrayInit=isSgAggregateInitializer(initializer)) {
      SgExprListExp* rhsOfArrayInit=arrayInit->get_initializers();
      SgExpressionPtrList& exprPtrList=rhsOfArrayInit->get_expressions();
      return exprPtrList;
    }
  }
  cerr<<"Error: getInitializerListOfArrayVariable failed."<<endl;
  exit(1);
}
#endif    

string flattenArrayInitializer(SgVariableDeclaration* decl, VariableIdMapping* variableIdMapping) {
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  ROSE_ASSERT(initName0);
  if(SgInitializedName* initName=isSgInitializedName(initName0)) {
    // array initializer
    SgInitializer* initializer=initName->get_initializer();
    ROSE_ASSERT(initializer);
    stringstream ss;
    // x[2] = {1,2};"). In this case the SgExprListExp ("{1,2}") is wrapped in an SgAggregateInitializer
    // SgExprListExp* SgAggregateInitializer->get_initializers () const 
    // pointer variable initializer
    if(SgAggregateInitializer* arrayInit=isSgAggregateInitializer(initializer)) {
      string arrayName=variableIdMapping->variableName(variableIdMapping->variableId(decl));
      SgExprListExp* rhsOfArrayInit=arrayInit->get_initializers();
      //cout<<"RHS-ARRAY-INIT:"<<rhsOfArrayInit->unparseToString()<<endl;
      SgExpressionPtrList& exprPtrList=rhsOfArrayInit->get_expressions();
      string newRhs;
      int num=0;
      SgType* type=rhsOfArrayInit->get_type();
      for(SgExpressionPtrList::iterator i=exprPtrList.begin();i!=exprPtrList.end();++i) {
        ss<<type->unparseToString()<<" "<<arrayName<<"_"<<num<<" = "<<(*i)->unparseToString()<<";\n";
        num++;
      }
      string transformedArrayInitializer=ss.str();
      return transformedArrayInitializer;
    } else {
      cerr<<"Error: attempted to transform non-array initializer."<<endl;
      exit(1);
    }
  } else {
      cerr<<"Error: attempted to transform non-initialized declaration."<<endl;
      exit(1);
  }
}


void transformArrayAccess(SgNode* node, VariableIdMapping* variableIdMapping) {
  ROSE_ASSERT(SgNodeHelper::isArrayAccess(node));
  if(SgPntrArrRefExp* arrayAccessAst=isSgPntrArrRefExp(node)) {
    ArrayElementAccessData arrayAccess(arrayAccessAst,variableIdMapping);
    ROSE_ASSERT(arrayAccess.getDimensions()==1);
    VariableId accessVar=arrayAccess.getVariable();
    int accessSubscript=arrayAccess.getSubscript(0);
    stringstream newAccess;
    newAccess<<variableIdMapping->variableName(accessVar)<<"_"<<accessSubscript;
    SgNodeHelper::replaceAstWithString(node,newAccess.str());
  } else {
    cerr<<"Error: transformation of array access failed."<<endl;
  }
}

void transformArrayProgram(SgProject* root, Analyzer* analyzer) {
  // 1) transform initializers of global variables : a[]={1,2,3} ==> int a_0=1;int a_1=2;int a_2=3;
  // 2) eliminate initializers of pointer variables: int p* = a; ==> \eps
  // 3) replace uses of p[k]: with a_k (where k is a constant)

  //ad 1 and 2)
  VariableIdMapping* variableIdMapping=analyzer->getVariableIdMapping();
  Analyzer::VariableDeclarationList usedGlobalVariableDeclarationList=analyzer->computeUsedGlobalVariableDeclarationList(root);
  cout<<"STATUS: number of used global variables: "<<usedGlobalVariableDeclarationList.size()<<endl;
  list<pair<SgNode*,string> > toReplaceArrayInitializations;
  list<SgVariableDeclaration*> toDeleteDeclarations;
  typedef map<VariableId,VariableId> ArrayPointerMapType;
  ArrayPointerMapType arrayPointer; // var,arrayName
  for(Analyzer::VariableDeclarationList::iterator i=usedGlobalVariableDeclarationList.begin();
      i!=usedGlobalVariableDeclarationList.end();
      ++i) {
    SgVariableDeclaration* decl=*i;
    //cout<<"DEBUG: variableDeclaration:"<<decl->unparseToString()<<endl;
    SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
    ROSE_ASSERT(initName0);
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      // array initializer
      SgInitializer* arrayInitializer=initName->get_initializer();
      //string arrayName=variableIdMapping->variableName(variableIdMapping->variableId(*i));
      if(isSgAggregateInitializer(arrayInitializer)) {
        string transformedArrayInitializer=flattenArrayInitializer(decl,variableIdMapping);
        toReplaceArrayInitializations.push_back(make_pair(decl,transformedArrayInitializer));
        //SgNodeHelper::replaceAstWithString(decl,transformedArrayInitializer);
      } else {
        //cout<<"initName:"<<astTermWithNullValuesToString(initName)<<endl;
        VariableId lhsVarId=variableIdMapping->variableId(*i);
        string lhsVariableName=variableIdMapping->variableName(lhsVarId);
        SgType* type=variableIdMapping->getType(variableIdMapping->variableId(*i));
        // match: SgInitializedName(SgAssignInitializer(SgVarRefExp))
        // variable on lhs
        // check if variable is a pointer variable
        if(isSgPointerType(type)) {
          AstMatching m;
          MatchResult res;
          res=m.performMatching("SgInitializedName(SgAssignInitializer($varRef=SgVarRefExp))|SgInitializedName(SgAssignInitializer(SgAddressOfOp($varRef=SgVarRefExp)))",initName);
          if(res.size()==1) {
            toDeleteDeclarations.push_back(decl);
            MatchResult::iterator j=res.begin();
            SgVarRefExp* rhsVarRef=isSgVarRefExp((*j)["$varRef"]);
            VariableId rhsVarId=variableIdMapping->variableId(rhsVarRef);
            arrayPointer[lhsVarId]=rhsVarId;
            //cout<<"Inserted pair "<<variableName<<":"<<rhsVarName<<endl;
          }
        }
      }
    }
  }

  typedef list<pair<SgPntrArrRefExp*,ArrayElementAccessData> > ArrayAccessInfoType;
  ArrayAccessInfoType arrayAccesses;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgExpression* exp=isSgExpression(*i);
    if(exp) {
      if(SgPntrArrRefExp* arrAccess=isSgPntrArrRefExp(exp)) {
        ArrayElementAccessData aead(arrAccess,analyzer->getVariableIdMapping());
        ROSE_ASSERT(aead.isValid());
        VariableId arrayVar=aead.varId;
        //cout<<"array-element: "<<variableIdMapping->variableName(arrayVar);
        if(aead.subscripts.size()==1) {
          //cout<<" ArrayIndex:"<<*aead.subscripts.begin();
          arrayAccesses.push_back(make_pair(arrAccess,aead));
        } else {
          cout<<"Error: ArrayIndex: unknown (dimension>1)";
          exit(1);
        }          
      }
    }
  }
#if 0
  cout<<"Array-Pointer Map:"<<endl;
  for(ArrayPointerMapType::iterator i=arrayPointer.begin();i!=arrayPointer.end();++i) {
    cout<<(*i).first.toString()<<":"<<(*i).second.toString()<<endl;
  }
#endif
  cout<<"STATUS: Replacing array-initializations."<<endl;
  for(list<pair<SgNode*,string> >::iterator i=toReplaceArrayInitializations.begin();i!=toReplaceArrayInitializations.end();++i) {
    //cout<<(*i).first->unparseToString()<<":\n"<<(*i).second<<endl;
    SgNodeHelper::replaceAstWithString((*i).first,"\n"+(*i).second);
  }
  cout<<"STATUS: Transforming pointer declarations."<<endl;
  for(list<SgVariableDeclaration*>::iterator i=toDeleteDeclarations.begin();i!=toDeleteDeclarations.end();++i) {
    //cout<<(*i)->unparseToString()<<endl;
    VariableId declaredPointerVar=variableIdMapping->variableId(*i);
    SgNode* initName0=(*i)->get_traversalSuccessorByIndex(1); // get-InitializedName
    ROSE_ASSERT(initName0);
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      // initializer
      SgInitializer* initializer=initName->get_initializer();
      if(SgAssignInitializer* assignInitializer=isSgAssignInitializer(initializer)) {
        //cout<<"var-initializer:"<<initializer->unparseToString()<<astTermWithNullValuesToString(initializer)<<endl;
        SgExpression* assignInitOperand=assignInitializer->get_operand_i();
        if(isSgAddressOfOp(assignInitOperand)) {
          assignInitOperand=isSgExpression(SgNodeHelper::getFirstChild(assignInitOperand));
          ROSE_ASSERT(assignInitOperand);
        }
        if(SgVarRefExp* rhsInitVar=isSgVarRefExp(assignInitOperand)) {
          VariableId arrayVar=variableIdMapping->variableId(rhsInitVar);
          SgExpressionPtrList& arrayInitializerList=variableIdMapping->getInitializerListOfArrayVariable(arrayVar);
          //cout<<"DEBUG: rhs array:"<<arrayInitializerList.size()<<" elements"<<endl;
          int num=0;
          stringstream ss;
          for(SgExpressionPtrList::iterator j=arrayInitializerList.begin();j!=arrayInitializerList.end();++j) {
            ss<<"int "<<variableIdMapping->variableName(declaredPointerVar)<<"_"<<num<<" = "
              <<(*j)->unparseToString()
              <<";\n"
              ;
            num++;
          }
          SgNodeHelper::replaceAstWithString(*i,"\n"+ss.str());
        }
      }
    }
#if 0
    ArrayElementAccessData arrayAccess=
    ROSE_ASSERT(arrayAccess.getDimensions()==1);
    VariableId accessVar=arrayAccess.getVariable();
    int accessSubscript=arrayAccess.getSubscript(0);
#endif
    //SageInterface::removeStatement(*i);
    
    //SgNodeHelper::replaceAstWithString((*i),"POINTER-INIT-ARRAY:"+(*i)->unparseToString()+"...;");
  }
  
  //list<pair<SgPntrArrRefExp*,ArrayElementAccessData> > 
  cout<<"STATUS: Replacing Expressions ... ";
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(isSgAssignOp(*i)) {
      if(SgVarRefExp* lhsVar=isSgVarRefExp(SgNodeHelper::getLhs(*i))) {
        if(SgNodeHelper::isPointerVariable(lhsVar)) {
          //cout<<"DEBUG: pointer var on lhs :"<<(*i)->unparseToString()<<endl;

          SgExpression* rhsExp=isSgExpression(SgNodeHelper::getRhs(*i));
          ROSE_ASSERT(rhsExp);
          if(isSgAddressOfOp(rhsExp)) {
            rhsExp=isSgExpression(SgNodeHelper::getFirstChild(rhsExp));
            ROSE_ASSERT(rhsExp);
          }
          if(SgVarRefExp* rhsVar=isSgVarRefExp(rhsExp)) {
            VariableId lhsVarId=variableIdMapping->variableId(lhsVar);
            VariableId rhsVarId=variableIdMapping->variableId(rhsVar);
            SgExpressionPtrList& arrayInitializerList=variableIdMapping->getInitializerListOfArrayVariable(rhsVarId);
            //cout<<"DEBUG: rhs array:"<<arrayInitializerList.size()<<" elements"<<endl;
            int num=0;
            stringstream ss;
            for(SgExpressionPtrList::iterator j=arrayInitializerList.begin();j!=arrayInitializerList.end();++j) {
              ss<<variableIdMapping->variableName(lhsVarId)<<"_"<<num<<" = "
                <<(*j)->unparseToString();

              // workaround for the fact that the ROSE unparser generates a "\n;" for a replaced assignment
              {
                SgExpressionPtrList::iterator j2=j;
                j2++;
                if(j2!=arrayInitializerList.end())
                  ss<<";\n";
              }

              num++;
            }
            SgNodeHelper::replaceAstWithString(*i,ss.str());
            //SgNodeHelper::replaceAstWithString(*i,"COPY-ARRAY("+lhsVar->unparseToString()+"<="+rhsVar->unparseToString()+")");
          }
        } else {
          RoseAst subAst(*i);
          for(RoseAst::iterator j=subAst.begin();j!=subAst.end();++j) {
            if(SgNodeHelper::isArrayAccess(*j)) {
              //cout<<"DEBUG: arrays access on rhs of assignment :"<<(*i)->unparseToString()<<endl;
              transformArrayAccess(*j,analyzer->getVariableIdMapping());
            }
          }
        }
      }
    }
    if(SgNodeHelper::isCond(*i)) {
      RoseAst subAst(*i);
      for(RoseAst::iterator j=subAst.begin();j!=subAst.end();++j) {
        if(SgNodeHelper::isArrayAccess(*j)) {
          transformArrayAccess(*j,analyzer->getVariableIdMapping());
        }
      }
    }
  }
  cout<<" done."<<endl;
#if 0
  for(ArrayAccessInfoType::iterator i=arrayAccesses.begin();i!=arrayAccesses.end();++i) {
    //cout<<(*i).first->unparseToString()<<":"/*<<(*i).second.xxxx*/<<endl;
    ArrayElementAccessData arrayAccess=(*i).second;
    ROSE_ASSERT(arrayAccess.getDimensions()==1);
    VariableId accessVar=arrayAccess.getVariable();
    int accessSubscript=arrayAccess.getSubscript(0);
    // expression is now: VariableId[subscript]
    // information available is: arrayPointer map: VariableId:pointer -> VariableId:array
    // pointerArray

    // if the variable is not a pointer var it will now be added to the stl-map but with
    // a default VariableId. Default variableIds are not valid() IDs.
    // therefor this becomes a cheap check, whether we need to replace the expression or not.
    VariableId mappedVar=arrayPointer[accessVar];
    if(mappedVar.isValid()) {
      // need to replace
      stringstream newAccess;
#if 0
      newAccess<<variableIdMapping->variableName(mappedVar)<<"["<<accessSubscript<<"]";
#else
      newAccess<<variableIdMapping->variableName(accessVar)<<"_"<<accessSubscript<<" ";
#endif
      cout<<"to replace: @"<<(*i).first<<":"<<(*i).first->unparseToString()<<" ==> "<<newAccess.str()<<endl;
      SgNodeHelper::replaceAstWithString((*i).first,newAccess.str());
    }
  }
#endif
  Analyzer::VariableDeclarationList unusedGlobalVariableDeclarationList=analyzer->computeUnusedGlobalVariableDeclarationList(root);
  cout<<"STATUS: deleting unused global variables."<<endl;
  for(Analyzer::VariableDeclarationList::iterator i=unusedGlobalVariableDeclarationList.begin();
      i!=unusedGlobalVariableDeclarationList.end();
      ++i) {
    SgVariableDeclaration* decl=*i;
    SageInterface::removeStatement(decl);
  }
      
}

#ifdef EXPERIMENTAL_POINTER_ANALYSIS

#include "AbstractValue.h"
#include "AbstractValue.C"

void pointerAnalysis(SgNode* root, Analyzer* analyzer) {
  ExprAnalyzer* exprAnalyzer=analyzer->getExprAnalyzer();
  VariableIdMapping* variableIdMapping=analyzer->getVariableIdMapping();
  // VariableId->set(addr(VariableId)|Number)
  typedef AType::ConstIntLattice AbstractNumberType;
  typedef AbstractMemoryLocation<VariableId,AType::ConstIntLattice> AbstractMemoryLocationType;
  typedef AbstractValueSurrogate<AbstractNumberType,AbstractMemoryLocationType> AbstractValueSurrogateType;
  map<VariableId,set< AbstractValueSurrogateType > > aliasInfo;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
    if(SgAssignOp* assignOp=isSgAssignOp(*i)) {
      VariableId lhsVar;
      SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(assignOp));
      bool isLhsVar=exprAnalyzer->variable(lhs,lhsVar);
      if(isLhsVar) {
        SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(assignOp));
        if(SgPntrArrRefExp* arrAccess=isSgPntrArrRefExp(rhs)) {
          ArrayElementAccessData aead(arrAccess,analyzer->getVariableIdMapping());
          ROSE_ASSERT(aead.isValid());
          //cout<<"DEBUG: lhs:"<<variableIdMapping->variableName(lhsVar);
          //cout<<" rhs:"<<SgNodeHelper::getRhs(isSgAssignOp(*i))->unparseToString();
          VariableId rhsArrayVar=aead.varId;
          cout<<" rhs:"<<"array-element: "<<variableIdMapping->variableName(rhsArrayVar);
          if(aead.subscripts.size()==1) {
            cout<<" ArrayIndex:"<<*aead.subscripts.begin();
          } else {
            cout<<" ArrayIndex: unknown";
          }          
          cout<<endl;
          AbstractValueSurrogateType avst;
          aliasInfo[lhsVar].insert(avst);
        }

      } else {
        cerr<<"Error: pointer-analysis: lhs of assignment is not a variable. Not supported yet."<<endl;
        exit(1);
      }
    }
  }
}

#endif

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
    ("dump1",po::value< string >(), " [experimental] generates array updates in file arrayupdates.txt")
    ("dump-sorted",po::value< string >(), " [experimental] generates sorted array updates in file <file>")
    ("dump-non-sorted",po::value< string >(), " [experimental] generates non-sorted array updates in file <file>")
    ("rule-const-subst",po::value< string >(), " [experimental] use const-expr substitution rule <arg>")
    ("limit-to-fragment",po::value< string >(), "the argument is used to find fragments marked by two prgagmas of that '<name>' and 'end<name>'")
    ("rewrite","rewrite AST applying all rewrite system rules.")
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
        || string(argv[i])=="--check-ltl"
        || string(argv[i])=="--csv-spot-ltl"
        || string(argv[i])=="--check-ltl-sol"
        || string(argv[i])=="--ltl-in-alphabet"
        || string(argv[i])=="--ltl-out-alphabet"
        ) {
      // do not confuse ROSE frontend
      argv[i] = strdup("");
      assert(i+1<argc);
        argv[i+1] = strdup("");
    }
  }

  RewriteSystem rewriteSystem;
  // reset dump1 in case sorted or non-sorted is used
  if(args.count("dump-sorted")>0 || args.count("dump-non-sorted")>0) {
    boolOptions.registerOption("dump1",true);
    analyzer.setSkipSelectedFunctionCalls(true);
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

  if(args.count("rewrite")) {
    VariableIdMapping variableIdMapping;
    variableIdMapping.computeVariableSymbolMapping(sageProject);
    rewriteSystem.resetStatistics();
    rewriteSystem.rewriteAst(root, &variableIdMapping,true,false);
    cout<<"Rewrite statistics:"<<endl<<rewriteSystem.getStatistics().toString()<<endl;
    sageProject->unparse(0,0);
    cout<<"STATUS: generated rewritten program."<<endl;
    exit(0);
  }

  if(!boolOptions["skip-analysis"])
  {
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

  if(boolOptions["eliminate-arrays"]) {
    //analyzer.initializeVariableIdMapping(sageProject);
    transformArrayProgram(sageProject, &analyzer);
    sageProject->unparse(0,0);
    exit(0);
  }

  cout << "INIT: creating solver."<<endl;
  analyzer.initializeSolver1("main",root);
  analyzer.initLabeledAssertNodes(sageProject);
  if(boolOptions["dump1"]) {
    // extraction of expressions: skip function calls to selected functions (also inside expressions) for defered handling.
    analyzer.setSkipSelectedFunctionCalls(true);
  }
  #ifdef EXPERIMENTAL_POINTER_ANALYSIS
  if(option_debug_mode==400) {
    pointerAnalysis(root, &analyzer);
    exit(0);
  }
  #endif


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

  // since CT1.2 the ADT TransitionGraph ensures that no duplicates can exist
#if 0
  long removed=analyzer.getTransitionGraph()->removeDuplicates();
  cout << "Transitions reduced: "<<removed<<endl;
#endif

  cout << "=============================================================="<<endl;
  bool withCe = boolOptions["with-counterexamples"] || boolOptions["with-assert-counterexamples"];
  analyzer.reachabilityResults.printResults("YES (REACHABLE)", "NO (UNREACHABLE)", "error_", withCe);
#if 0
  // TODO: reachability in presence of semantic folding
  if(boolOptions["semantic-fold"] || boolOptions["post-semantic-fold"]) {

  } else {
    printAsserts(analyzer,sageProject);
  }
#endif
  if (args.count("csv-assert")) {
    string filename=args["csv-assert"].as<string>().c_str();
    analyzer.reachabilityResults.writeFile(filename.c_str(), false, 0, withCe);
    cout << "Reachability results written to file \""<<filename<<"\"." <<endl;
#if 0  //result tables of different sizes are now handled by the PropertyValueTable object itself
    switch(resultsFormat) {
    case RF_RERS2012: analyzer.reachabilityResults.write2012File(filename.c_str());break;
    case RF_RERS2013: analyzer.reachabilityResults.write2013File(filename.c_str());break;
    default: analyzer.reachabilityResults.writeFile(filename.c_str());break;
    }
    //    OLD VERSION:  generateAssertsCsvFile(analyzer,sageProject,filename);
#endif
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
  // LTL run time measurements missing
  double ltlRunTime=timer.getElapsedTimeInMilliSec();

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

  double totalRunTime=frontEndRunTime+initRunTime+ analysisRunTime+ltlRunTime;

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
                                 rewriteSystem,
                                 useConstSubstitutionRule,
                                 fragmentStartLabel,
                                 fragmentEndLabel);
    arrayUpdateExtractionRunTime=timer.getElapsedTimeInMilliSec();
    rewriteSystem.dump1_stats.numArrayUpdates=arrayUpdates.size();
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

  double overallTime =totalRunTime + totalInputTracesTime + totalLtlRunTime;

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
        <<readableruntime(ltlRunTime)<<", "
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
        <<ltlRunTime<<", "
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

    text<<"rewrite-stats, "
        <<rewriteSystem.dump1_stats.numArrayUpdates<<", "
        <<rewriteSystem.dump1_stats.numElimMinusOperator<<", "
        <<rewriteSystem.dump1_stats.numElimAssignOperator<<", "
        <<rewriteSystem.dump1_stats.numAddOpReordering<<", "
        <<rewriteSystem.dump1_stats.numConstantFolding<<", "
        <<rewriteSystem.dump1_stats.numVariableElim<<", "
        <<rewriteSystem.dump1_stats.numConstExprElim
        <<endl;
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
  }
#endif

#if 0
  {
    cout << "ConstraintSet:\n"<<analyzer.getConstraintSetMaintainer()->toString()<<endl;
  }
#endif

#if 1
  {
    if(analyzer.variableValueMonitor.isActive())
      cout << "VariableValueMonitor:\n"<<analyzer.variableValueMonitor.toString(analyzer.getVariableIdMapping())<<endl;
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

