#ifndef TYPEEQUIVALENCE_TYPE_EQUIVALENCE_CHECKER_INCLUDE_GUARD
# define TYPEEQUIVALENCE_TYPE_EQUIVALENCE_CHECKER_INCLUDE_GUARD_

//# include "rose.h"
//#include "sageInterface.h"
# include "sage3basic.h"
# include "RoseAst.h"
# include "simpleExpressionEvaluator.hpp"

/*
 * FIXME For now it can only compare two types at a time.
 * To have that as a pass in ROSE it needs to be much faster and
 * more efficient!
 * How are Types actually saved in ROSE's TypeTables?
 * How can we make use of that?
 */
class TypeEquivalenceChecker {
 public:
  TypeEquivalenceChecker(bool profile);
  bool typesAreEqual(SgType *t1, SgType *t2);   // XXX BAD NAME!!

  int getNamedTypeCount();
  int getPointerTypeCount();
  int getArrayTypeCount();
  int getFunctionTypeCount();
 private:
  SgNode * getBasetypeIfApplicable(SgNode *t);
  bool profile_;
  int namedType_, pointerType_, arrayType_, functionType_;
};

namespace SageInterface{
  bool checkTypesAreEqual(SgType *typeA, SgType *typeB);
}
#endif
