#ifndef NORMALIZATION_H
#define NORMALIZATION_H

#include <list>
#include <utility>

namespace SPRAY {
class Normalization {
 public:
  static void normalizeAst(SgNode* root);
  static void normalizeExpressions(SgNode* node);
  static void normalizeExpression(SgExprStatement* stmt, SgExpression* node);
  static void generateTmpVarAssignment(SgExprStatement* stmt, SgExpression* expr);
 private:
  static void convertAllForsToWhiles (SgNode* top);
  /* If the given statement contains any break statements in its body,
	add a new label below the statement and change the breaks into
	gotos to that new label.
  */
  static void changeBreakStatementsToGotos (SgStatement *loopOrSwitch);
  static int32_t tmpVarNr;
  typedef std::list<std::pair<SgStatement*,SgVariableDeclaration*> > InsertList;
  typedef std::list<std::pair<SgExpression*,SgExpression*> > ReplaceList;
  typedef std::list<std::pair<SgStatement*,SgExpression*> > TransformationList;
  static TransformationList transformationList;
};

} // end of namespace SPRAY

#endif
