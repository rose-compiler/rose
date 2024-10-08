
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG__unparseAssnInit 0

void Unparse_ExprStmt::unparseAssnInit(SgExpression* expr, SgUnparse_Info& info) {
  SgAssignInitializer* assn_init = isSgAssignInitializer(expr);
  ASSERT_not_null(assn_init);
#if DEBUG__unparseAssnInit
  printf("Enter unparseAssnInit()\n");
  printf("  assn_init = %p = %s\n", assn_init, assn_init->class_name().c_str());
#endif
  if (assn_init->get_is_explicit_cast()) {
    if (assn_init->get_operand()->get_originalExpressionTree() != NULL) {
      unparseExpression(assn_init->get_operand()->get_originalExpressionTree(), info);
    } else {
      unparseExpression(assn_init->get_operand(), info);
    }
  } else {
    SgCastExp* castExp = isSgCastExp(assn_init->get_operand());
    if (castExp != NULL) {
      unparseExpression(castExp->get_operand(), info);
    } else {
      unparseExpression(assn_init->get_operand(), info);
    }
  }
#if DEBUG__unparseAssnInit
  printf("Leave unparseAssnInit()\n");
#endif
}
