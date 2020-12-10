#ifndef AST_MATCHING_H
#define AST_MATCHING_H

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

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
  /* This function is useful when reusing the same matcher object for
     performing multiple matches. It allows to keep all nodes that
     have been marked by a previous match using the '#' operator. The
     subtrees of those nodes are not traversed in the subsequent match
     either.
   */
  void setKeepMarkedLocations(bool keepMarked);
  /* This function is only for information purposes. It prints the
     sequence of internal match operations which are performed for a
     provided match-pattern.
  */
  void printMatchOperationsSequence();
  /* This function is only for information purposes. It prints the
     set of all marked nodes (marked with the "#' operator).
  */
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
  bool _keepMarkedLocations;
};

#endif
