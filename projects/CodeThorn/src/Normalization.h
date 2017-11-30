#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include <list>
#include <utility>

namespace SPRAY {
class Normalization {
 public:
  void normalizeAst(SgNode* root);
  void normalizeExpressions(SgNode* node);
  void normalizeExpression(SgExprStatement* stmt, SgExpression* node);
  void generateTmpVarAssignment(SgExprStatement* stmt, SgExpression* expr);
  size_t inlineFunctions(SgNode* root);
 private:
  // Finds needle in haystack and returns true if found.  Needle is a single node (possibly an invalid pointer and will not be
  // dereferenced) and haystack is the root of an abstract syntax (sub)tree.
  static bool isAstContaining(SgNode *haystack, SgNode *needle);
  void convertAllForsToWhiles (SgNode* top);
  /* If the given statement contains any break statements in its body,
	add a new label below the statement and change the breaks into
	gotos to that new label.
  */
  void changeBreakStatementsToGotos (SgStatement *loopOrSwitch);
  static int32_t tmpVarNr;
  typedef std::list<std::pair<SgStatement*,SgExpression*> > TransformationList;
  TransformationList transformationList;
};

} // end of namespace SPRAY

#endif
