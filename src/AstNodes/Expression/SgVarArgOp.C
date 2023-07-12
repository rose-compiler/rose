#include <sage3basic.h>

#if 0
// There is a set_expression_type function alread, soe we don't need this.
// DQ (1/17/2008): Added set_type function since this is one of a few IR nodes that require
// the type to be held explicitly, for all other IR nodes the type is computed dynamicly.
void
SgVarArgOp::set_type( SgType* type )
   {
     p_expression_type = type;
   }
#endif

// This function helps to provide a uniform interface even though the type is help in a field called p_expression_type.
SgType*
SgVarArgOp::get_type() const
   {
  // This function returns an explicitly stored type

     SgType* returnType = p_expression_type;

#if 0
     printf ("In $CLASSNAME::get_type() \n");
#endif

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }


int
SgVarArgOp::replace_expression(SgExpression *o, SgExpression *n)
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
    printf ("Warning: inside of SgVarArgOp::replace_expression original SgExpression unidentified \n");
    return 0;
  }
}
