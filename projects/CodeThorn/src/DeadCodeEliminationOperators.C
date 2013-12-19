bool isEmptyBlock(SgNode* node) {
  if(node==0)
    return true;
  //cout<<"isEmptyBasicBlock:"<<node->class_name()<<endl;
  if(SgBasicBlock* block=isSgBasicBlock(node)) {
    const SgStatementPtrList& stmtList=block->get_statements(); 
    return stmtList.size()==0;
  }
  return false;
}

// an if-stmt is empty if both branches are empty blocks (or do not exist).
bool isEmptyIfStmt(SgIfStmt* ifStmt) {
  SgStatement* trueBody=ifStmt->get_true_body();
  SgStatement* falseBody=ifStmt->get_false_body();
  return isEmptyBlock(trueBody) && isEmptyBlock(falseBody);
}

list<SgIfStmt*> listOfEmptyIfStmts(SgNode* node) {
  list<SgIfStmt*> ifstmts;
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
    if(SgIfStmt* ifStmt=isSgIfStmt(*i)) {
      if(isEmptyIfStmt(ifStmt)) {
        ifstmts.push_back(ifStmt);
      }
    }
  }
  return ifstmts;
}

size_t eliminateEmptyIfStmts(SgNode* node) {
  size_t numElim=0;
  list<SgIfStmt*> ifstmts=listOfEmptyIfStmts(node);
  for(list<SgIfStmt*>::iterator i=ifstmts.begin();i!=ifstmts.end();i++) {
    SageInterface::removeStatement (*i, false);
    numElim++;
  }
  return numElim;
}
