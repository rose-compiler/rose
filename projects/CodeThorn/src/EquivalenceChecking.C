#include "sage3basic.h"
#include "EquivalenceChecking.h"

bool EquivalenceChecking::isInsideOmpParallelFor(SgNode* node, ForStmtToOmpPragmaMap& forStmtToPragmaMap) {
  while(!isSgForStatement(node)||isSgProject(node))
    node=node->get_parent();
  ROSE_ASSERT(!isSgProject(node));
  // assuming only omp parallel for exist
  return forStmtToPragmaMap.find(isSgForStatement(node))!=forStmtToPragmaMap.end();
}

LoopInfoSet EquivalenceChecking::determineLoopInfoSet(SgNode* root, VariableIdMapping* variableIdMapping, Labeler* labeler) {
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
          cout<<"DEBUG: NOT a for-stmt: "<<(*i)->unparseToString()<<endl;
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



