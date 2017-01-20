#include "sage3basic.h"
#include "EquivalenceChecking.h"
#include "AstTerm.h"

using namespace SPRAY;

bool EquivalenceChecking::isInsideOmpParallelFor(SgNode* node, ForStmtToOmpPragmaMap& forStmtToPragmaMap) {
  ROSE_ASSERT(node);
  while(!isSgForStatement(node)) {
    if(isSgProject(node))
      return false;
    //cout<<"node: "<<node->class_name()<<endl;
    node=node->get_parent();
    ROSE_ASSERT(node);
  }
  ROSE_ASSERT(!isSgProject(node));
  // assuming only omp parallel exist in pragma map
  return forStmtToPragmaMap.find(isSgForStatement(node))!=forStmtToPragmaMap.end();
}

bool EquivalenceChecking::isOmpParallelFor(SgForStatement* node, ForStmtToOmpPragmaMap& forStmtToPragmaMap) {
  return forStmtToPragmaMap.find(isSgForStatement(node))!=forStmtToPragmaMap.end();
}

LoopInfoSet EquivalenceChecking::determineLoopInfoSet(SgNode* root, VariableIdMapping* variableIdMapping, Labeler* labeler) {
  cout<<"INFO: loop info set and determine iteration vars."<<endl;
  ForStmtToOmpPragmaMap forStmtToPragmaMap=createOmpPragmaForStmtMap(root);
  cout<<"INFO: found "<<forStmtToPragmaMap.size()<<" omp/simd loops."<<endl;
  LoopInfoSet loopInfoSet;
  RoseAst ast(root);
  AstMatching m;

  // (i) match all for-stmts and (ii) filter canonical ones
  string matchexpression="$FORSTMT=SgForStatement(_,_,..)";
  MatchResult r=m.performMatching(matchexpression,root);
  //cout << "DEBUG: Matched for loops: "<<r.size()<<endl;
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    LoopInfo loopInfo;
    SgNode* forNode=(*i)["$FORSTMT"];
    //cout << "DEBUG: Detected for loops: "<<forNode->unparseToString()<<endl;

    //cout<<"DEBUG: MATCH: "<<forNode->unparseToString()<<AstTerm::astTermWithNullValuesToString(forNode)<<endl;
    ROSE_ASSERT(isSgForStatement(forNode));
    SgInitializedName* ivar=0;
    SgExpression* lb=0;
    SgExpression* ub=0;
    SgExpression* step=0;
    SgStatement* body=0;
    bool hasIncrementalIterationSpace=false;
    bool isInclusiveUpperBound=false;
    bool isCanonicalOmpForLoop=SageInterface::isCanonicalForLoop(forNode, &ivar, &lb, &ub, &step, &body, &hasIncrementalIterationSpace, &isInclusiveUpperBound);
    if(isCanonicalOmpForLoop) {
      ROSE_ASSERT(ivar);
      SgInitializedName* node=0;
      if(isCanonicalOmpForLoop) {
        node=ivar;
      }
      ROSE_ASSERT(node);
#if 0      
      // WORKAROUND 1
      // TODO: investigate why the for pointer is not stored in the same match-result
      if(forNode==0) {
        forNode=node; // init
        while(!isSgForStatement(forNode)||isSgProject(forNode))
        forNode=forNode->get_parent();
      }
      ROSE_ASSERT(!isSgProject(forNode));
#endif
      loopInfo.iterationVarId=variableIdMapping->variableId(node);
      loopInfo.forStmt=isSgForStatement(forNode);
      loopInfo.iterationVarType=isOmpParallelFor(loopInfo.forStmt,forStmtToPragmaMap)?ITERVAR_PAR:ITERVAR_SEQ;
      if(loopInfo.forStmt) {
        const SgStatementPtrList& stmtList=loopInfo.forStmt->get_init_stmt();
        ROSE_ASSERT(stmtList.size()==1);
        loopInfo.initStmt=stmtList[0];
        loopInfo.condExpr=loopInfo.forStmt->get_test_expr();
        loopInfo.computeLoopLabelSet(labeler);
        loopInfo.computeOuterLoopsVarIds(variableIdMapping);
        loopInfo.isOmpCanonical=true;
      } else {
        cerr<<"WARNING: no for statement found."<<endl;
        if(forNode) {
          cerr<<"for-loop:"<<forNode->unparseToString()<<endl;
        } else {
          cerr<<"for-loop: 0"<<endl;
        }
      }
    } else {
      loopInfo.forStmt=isSgForStatement(forNode);
      loopInfo.isOmpCanonical=false;
    }
    loopInfoSet.push_back(loopInfo);
  }
  cout<<"INFO: found "<<forStmtToPragmaMap.size()<<" omp/simd loops."<<endl;
  cout<<"INFO: found "<<Specialization::numParLoops(loopInfoSet,variableIdMapping)<<" parallel loops."<<endl;
  return loopInfoSet;
}

    // finds the list of pragmas (in traversal order) with the prefix 'prefix' (e.g. '#pragma omp parallel' is found for prefix 'omp')
    EquivalenceChecking::ForStmtToOmpPragmaMap EquivalenceChecking::createOmpPragmaForStmtMap(SgNode* root) {
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
          cout<<"WARNING: unsupported omp pragma: "<<(*i)->unparseToString()<<endl;
        }
      }
    }
  }
  return map;
}

// finds the list of pragmas (in traversal order) with the prefix 'prefix' (e.g. '#pragma omp parallel' is found for prefix 'omp')
list<SgPragmaDeclaration*> EquivalenceChecking::findPragmaDeclarations(SgNode* root, string pragmaKeyWord) {
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
