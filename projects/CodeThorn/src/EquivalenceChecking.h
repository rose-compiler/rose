#ifndef EQUIVALENCE_CHECKING_H
#define EQUIVALENCE_CHECKING_H


#include "Labeler.h"
#include "VariableIdMapping.h"
#include "Specialization.h"

class SgNode;
class SgPragmaDeclaration;

class EquivalenceChecking {
 public:
  typedef map<SgForStatement*,SgPragmaDeclaration*> ForStmtToOmpPragmaMap;
  static bool isInsideOmpParallelFor(SgNode* node, EquivalenceChecking::ForStmtToOmpPragmaMap& forStmtToPragmaMap);
  static LoopInfoSet determineLoopInfoSet(SgNode* root, VariableIdMapping* variableIdMapping, Labeler* labeler);
  static ForStmtToOmpPragmaMap createOmpPragmaForStmtMap(SgNode* root);
  static list<SgPragmaDeclaration*> findPragmaDeclarations(SgNode* root, string pragmaKeyWord);
};

#endif
