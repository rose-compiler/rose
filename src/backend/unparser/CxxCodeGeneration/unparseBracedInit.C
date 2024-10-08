
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG__unparseBracedInit 0

void Unparse_ExprStmt::unparseBracedInit(SgExpression* expr, SgUnparse_Info & info) {
  SgBracedInitializer* braced_init = isSgBracedInitializer(expr);
  ASSERT_not_null(braced_init);
#if DEBUG__unparseBracedInit
  printf("Enter unparseBracedInit()\n");
  printf("  braced_init = %p = %s\n", braced_init, braced_init->class_name().c_str());
#endif
  SgUnparse_Info newinfo(info);

#if DEBUG__unparseBracedInit
     printf ("  braced_init = %p = %s\n",braced_init,braced_init->class_name().c_str());
#endif

#if DEBUG__unparseBracedInit
  printf ("  newinfo.SkipEnumDefinition()  = %s\n", newinfo.SkipEnumDefinition() ? "true" : "false");
  printf ("  newinfo.SkipClassDefinition() = %s\n", newinfo.SkipClassDefinition() ? "true" : "false");
#endif

  SgExpressionPtrList & args = braced_init->get_initializers()->get_expressions();
#if DEBUG__unparseBracedInit
  printf ("  args.size() = %zu \n", args.size());
#endif

  curprint("{");
  for (size_t index = 0; index < args.size(); index ++) {
    unparseExpression(args[index], newinfo);
    if (index != args.size()-1) curprint ( ", ");
  }
  unparseAttachedPreprocessingInfo(braced_init, info, PreprocessingInfo::inside);
  curprint("}");

#if DEBUG__unparseBracedInit
  printf ("Leave unparseBracedInit() \n");
#endif
}
