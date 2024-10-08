
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG__unparseCompInit 0
#define DEBUG__isFromAnotherFile 0

static bool isFromAnotherFile (SgLocatedNode* lnode) {
  bool result = false;
  ASSERT_not_null(lnode);

#if DEBUG__isFromAnotherFile
  printf ("Enter isFromAnotherFile()\n");
  printf ("  lnode = %p = %s\n", lnode, lnode->class_name().c_str());
#endif

  SgAssignInitializer * a_initor = isSgAssignInitializer (lnode);
  if (a_initor != NULL) {
    result = false;
    SgExpression * leaf_child = a_initor->get_operand_i();
    while (SgCastExp * cast_op = isSgCastExp(leaf_child)) {
      if (cast_op->get_originalExpressionTree() != NULL)
        leaf_child = cast_op->get_originalExpressionTree();
      else
        leaf_child = cast_op->get_operand_i();
    }
    lnode = leaf_child;
  }

  SgStatement * cur_stmt = SageInterface::getEnclosingStatement(lnode);
  ASSERT_not_null(cur_stmt);

  SgFile * cur_file = SageInterface::getEnclosingFileNode(cur_stmt);
  if (cur_file != NULL) {
    if (lnode->get_file_info()->isTransformation() == false && lnode->get_file_info()->isCompilerGenerated() == false) {
      if (lnode->get_file_info()->get_physical_filename() != "" && lnode->get_file_info()->get_physical_filename() != "NULL_FILE" && cur_file->get_file_info()->get_physical_filename() != lnode->get_file_info()->get_physical_filename()) {
        result = true;
      }
    }
  }

#if DEBUG__isFromAnotherFile
  printf ("  result = %s \n", result ? "true" : "false");
  printf ("Leave isFromAnotherFile()\n");
#endif
  return result;
}

void Unparse_ExprStmt::unparseCompInit(SgExpression* expr, SgUnparse_Info& info) {
  SgCompoundInitializer* comp_init = isSgCompoundInitializer(expr);
  ASSERT_not_null(comp_init);
#if DEBUG__unparseCompInit
  printf("Enter unparseCompInit()\n");
#endif

  SgUnparse_Info newinfo(info);
  curprint("(");

  SgExpressionPtrList& list = comp_init->get_initializers()->get_expressions();
  size_t last_index = list.size() -1;
  for (size_t index = 0; index < list.size(); index ++) {
    bool skipUnparsing = isFromAnotherFile(list[index]);
    if (!skipUnparsing) {
      unparseExpression(list[index], newinfo);
      if (index != last_index)
        curprint ( ", ");
    }
  }

  unparseAttachedPreprocessingInfo(comp_init, info, PreprocessingInfo::inside);
  curprint(")");

#if DEBUG__unparseCompInit
  printf("Leave unparseCompInit()\n");
#endif
}
