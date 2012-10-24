#ifndef AST_MATCHING_H
#define AST_MATCHING_H

/*************************************************************
 * Copyright: (C) 2012 Markus Schordan                       *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "rose.h"
#include "matcherparser_decls.h"
#include "MatchOperation.h"
#include "RoseAst.h"
#include <list>
#include <set>

class SgNode;
class MatchOperation;

class AstMatching {
 public:
  AstMatching();
  ~AstMatching();
  AstMatching(std::string matchExpression,SgNode* root);
  MatchResult performMatching(std::string matchExpression, SgNode* root);
  MatchResult getResult();
  void printMatchOperationsSequence();
  void printMarkedLocations();
  bool performSingleMatch(SgNode* node, MatchOperationList* matchOperationSequence);
 private:
  void performMatchingOnAst(SgNode* root);
  void performMatching();
  void generateMatchOperationsSequence();

 private:
  std::string _matchExpression;
  SgNode* _root;
  MatchOperationList* _matchOperationsSequence;
  MatchStatus _status;
};

#endif
