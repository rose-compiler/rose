/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"

#include "AstMatching.h"

AstMatching::AstMatching():_matchExpression(""),_root(0),_keepMarkedLocations(false) { 
  //_allMatchVarBindings=new std::list<SingleMatchVarBindings>; 
}
AstMatching::~AstMatching() {
  //delete _allMatchVarBindings; 
}
AstMatching::AstMatching(std::string matchExpression,SgNode* root):_matchExpression(matchExpression),_root(root) {
}
MatchResult 
AstMatching::performMatching(std::string matchExpression, SgNode* root) {
  _matchExpression=matchExpression;
  _root=root;
  performMatching();
  return getResult();
}
MatchResult AstMatching::getResult() { 
  // we copy the results. Hence, after Matching the AstMatching object can be descarded.
  return *(_status._allMatchVarBindings);
}

void AstMatching::performMatching() {
  generateMatchOperationsSequence();
  if(_status.debug)
    printMatchOperationsSequence();
  performMatchingOnAst(_root);
}
void AstMatching::generateMatchOperationsSequence() {
  extern int matcherparserparse();
  extern MatchOperationList* matchOperationsSequence;
  InitializeParser(_matchExpression);
  matcherparserparse();
  // clean up possibly existing match operations sequence
#if 0
  // TODO: proper destruction not finished yet
  if(_matchOperationsSequence)
    delete _matchOperationsSequence;
#endif
  _matchOperationsSequence=matchOperationsSequence;
  FinishParser();
}

void AstMatching::printMatchOperationsSequence() {
  std::cout << "\nMatch Sequence: START" << std::endl;
  if(_matchOperationsSequence) {
    std::cout << _matchOperationsSequence->toString()<<std::endl;
#if 0
    for(MatchOperationList::iterator i=_matchOperationsSequence->begin();
    i!=_matchOperationsSequence->end();
    ++i) {
      std::cout << (*i)->toString() << std::endl;
    }
#endif
  } else {
    std::cout << "empty." << std::endl;
  }
  std::cout << "Match Sequence: END" << std::endl;
}

void AstMatching::printMarkedLocations() {
  std::cout << "\nMarked Locations: START" << std::endl;
  if(!_status._allMatchMarkedLocations.empty()) {
    for(SingleMatchMarkedLocations::iterator i=_status._allMatchMarkedLocations.begin();
    i!=_status._allMatchMarkedLocations.end();
    ++i) {
      std::cout << (**i) << "," << std::endl;
    }
  } else {
    std::cout << "no locations marked." << std::endl;
  }
  std::cout << "Marked Locations: END" << std::endl;
}

bool
AstMatching::performSingleMatch(SgNode* node, MatchOperationList* matchOperationSequence) {
  if(matchOperationSequence==0) {
    std::cerr << "matchOperationSequence==0. Bailing out." <<std::endl;
    exit(1);
  }
  if(_status.debug) 
    std::cout << "perform-single-match:"<<std::endl;    
  SingleMatchResult smr; // we intentionally avoid dynamic allocation for var-bindings of a single pattern
  RoseAst ast(node);
  RoseAst::iterator pattern_ast_iter=ast.begin().withNullValues();
  if(_status.debug) 
    std::cout << "single-match-start:"<<std::endl;    
  bool tmpresult=matchOperationSequence->performOperation(_status, pattern_ast_iter, smr);
  if(_status.debug) 
    std::cout << "single-match-end"<<std::endl;    
  if(tmpresult)
    _status.mergeSingleMatchResult(smr);
  return tmpresult;
}

void 
AstMatching::performMatchingOnAst(SgNode* root) {
  // reset match status (taking care of reuse of object)
  if(!_keepMarkedLocations)
    _status.resetAllMarkedLocations();
  _status.resetAllMatchVarBindings();
  // start matching
  RoseAst ast(root);
  bool result;
  for(RoseAst::iterator ast_iter=ast.begin().withNullValues();
      ast_iter!=ast.end();
      ++ast_iter) {
    if(_status.debug) std::cout<<"----------------------------------------------------------------------------------------------"<<std::endl;
    if(_status.isMarkedLocationAddress(ast_iter)) {
      if(_status.debug) std::cout << "DEBUG: MARKED LOCATION @ " << *ast_iter << " ... skipped." << std::endl;
      ast_iter.skipChildrenOnForward();
    } else {
      result=performSingleMatch(*ast_iter,_matchOperationsSequence);
      if(result && _status.debug) {
        std::cout << "DEBUG: FOUND MATCH at node" << *ast_iter << std::endl;
        printMarkedLocations();
      }
      if(_status.isMarkedLocationAddress(ast_iter)) {
        if(_status.debug) std::cout << "DEBUG: MARKED CURRENT LOCATION @ " << *ast_iter << " ... skipping subtree." << std::endl;
        ast_iter.skipChildrenOnForward();
      }
    }
  }
  if(_status.debug)
    std::cout << "Matching on AST finished." << std::endl;
}

void AstMatching::setKeepMarkedLocations(bool keepMarked) {
  _keepMarkedLocations=keepMarked;
}
