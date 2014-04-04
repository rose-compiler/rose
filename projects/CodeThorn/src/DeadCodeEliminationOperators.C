#include <set>

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

std::set<SgFunctionDefinition*> calledFunctionDefinitions(std::list<SgFunctionCallExp*> functionCalls) {
  std::set<SgFunctionDefinition*> calledFunctions;
  for(std::list<SgFunctionCallExp*>::iterator i=functionCalls.begin();i!=functionCalls.end();++i) {
    SgFunctionDefinition* functionDef=isSgFunctionDefinition(SgNodeHelper::determineFunctionDefinition(*i));
    if(functionDef) {
      calledFunctions.insert(functionDef);
    }
  }
  return calledFunctions;
}

std::set<SgFunctionDefinition*> NonCalledTrivialFunctions(SgNode* root0) {
  SgProject* root=isSgProject(root0);
  ROSE_ASSERT(root);
  list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  std::list<SgFunctionCallExp*> tfCalls=TrivialInlining::trivialFunctionCalls(root);
  std::set<SgFunctionDefinition*> calledFunDefs=calledFunctionDefinitions(tfCalls);
  std::set<SgFunctionDefinition*> nonCalledFunDefs;
  for(list<SgFunctionDefinition*>::iterator i=funDefs.begin();i!=funDefs.end();++i) {
    if(TrivialInlining::isTrivialFunctionDefinition(*i)) {
      if(calledFunDefs.find(*i)==calledFunDefs.end()) {
        nonCalledFunDefs.insert(*i);
      }
    }
  }
  return nonCalledFunDefs;
}

void eliminateFunctions(std::set<SgFunctionDefinition*>& funDefs) {
  for(std::set<SgFunctionDefinition*>::iterator i=funDefs.begin();i!=funDefs.end();++i) {
    string funName=SgNodeHelper::getFunctionName(*i);
    SgFunctionDeclaration* funDecl=(*i)->get_declaration();
    if(funName!="main") {
      if(detailedOutput) cout<<"Deleting function: "<<funName<<endl;
      SgStatement* stmt=funDecl;
      SageInterface::removeStatement(stmt, false);
    }
  }
}

size_t eliminateNonCalledTrivialFunctions(SgNode* root) {
  std::set<SgFunctionDefinition*> nonCalledTrivialFunctions=NonCalledTrivialFunctions(root);
  eliminateFunctions(nonCalledTrivialFunctions);
  return nonCalledTrivialFunctions.size();
}
