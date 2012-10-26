/*************************************************************
 * Copyright: (C) 2012 Markus Schordan                       *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "AstMatching.h"

using namespace CodeThorn;

AstMatching::AstMatching():_matchExpression(""),_root(0) { 
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
  printMatchOperationsSequence();
  performMatchingOnAst(_root);
}
void AstMatching::generateMatchOperationsSequence() {
  extern int matcherparse();
  extern MatchOperationList* matchOperationsSequence;
  InitializeParser(_matchExpression);
  matcherparse();
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
  if(_status.debug) 
    std::cout << "perform-single-match:"<<std::endl;    
  SingleMatchResult smr; // we intentionally avoid dynamic allocation for var-bindings of a single pattern
  if(_status.debug) 
    std::cout << "perform-single-match2:"<<std::endl;    
  RoseAst ast(node);
  if(_status.debug) 
    std::cout << "perform-single-match3:"<<std::endl;    
  RoseAst::iterator pattern_ast_iter=ast.begin().withNullValues();
  if(_status.debug) 
    std::cout << "single-match-start:"<<std::endl;    
  if(matchOperationSequence==0) {
    std::cerr << "matchOperationSequence==0. Bailing out." <<std::endl;
    exit(1);
  }
  bool tmpresult=matchOperationSequence->performOperation(_status, pattern_ast_iter, smr);
  if(_status.debug) 
    std::cout << "single-match-end"<<std::endl;    
  return tmpresult;
#if 0
  for(MatchOperationList::iterator match_op_iter=matchOperationSequence->begin();
      match_op_iter!=_matchOperationsSequence->end();
      match_op_iter++) {
    bool tmpresult=(*match_op_iter)->performOperation(_status, pattern_ast_iter, smr);
    // matchVarBindings and matchRegisteredIterators is automatically deleted if we return with false
    if(!tmpresult) {return false;}
  }
  if(!tmpresult) {return false;}
  // pattern has been successfully matched
  _status._allMatchVarBindings->push_back(smr.singleMatchVarBindings);
  // move elements (instead of copy)
  _status._allMatchMarkedLocations.splice(_status._allMatchMarkedLocations.end(),smr.singleMatchMarkedLocations);
  return true;
#endif
}

void 
AstMatching::performMatchingOnAst(SgNode* root) {
  RoseAst ast(root);
  bool result;
  for(RoseAst::iterator ast_iter=ast.begin().withNullValues();
      ast_iter!=ast.end();
      ++ast_iter) {
    if(_status.isMarkedLocation(ast_iter)) {
      //std::cout << "\nMARKED LOCATION @ " << *ast_iter << " ... skipped." << std::endl;
      ast_iter.skipChildrenOnForward();
    } else {
      result=performSingleMatch(*ast_iter,_matchOperationsSequence);
      //if(result) std::cout << "\nFOUND MATCH at node" << *ast_iter << std::endl;
    }
  }
  if(_status.debug)
    std::cout << "Matching on AST finished." << std::endl;
}
