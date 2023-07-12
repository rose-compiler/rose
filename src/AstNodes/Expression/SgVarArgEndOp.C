#include <sage3basic.h>

int
SgVarArgEndOp::replace_expression(SgExpression *o, SgExpression *n)
{

  ROSE_ASSERT(o != NULL);
  ROSE_ASSERT(n != NULL);

  if (get_operand_expr() == o)
  {
    set_operand_expr(n);
    return 1;
  }
  else
  {
    printf ("Warning: inside of SgVarArgEndOp::replace_expression original SgExpression unidentified \n");
    return 0;
  }
}
