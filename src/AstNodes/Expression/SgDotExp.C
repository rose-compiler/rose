#include <sage3basic.h>

SgType*
SgDotExp::get_type() const
   {
  // DQ (1/14/2006): p_expression_type has been removed, we have to compute the appropriate type (IR specific code)
  // For the SgDotExp, the type is the type of the rhs operand (e.g. "A.x" where "x" is the data member of the class "A").

     ROSE_ASSERT(get_lhs_operand() != NULL);
     ROSE_ASSERT(get_rhs_operand() != NULL);

     SgType* returnType = get_rhs_operand()->get_type();

     ROSE_ASSERT(returnType != NULL);

     return returnType;
   }
