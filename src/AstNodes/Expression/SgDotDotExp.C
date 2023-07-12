#include <sage3basic.h>

SgType*
SgDotDotExp::get_type() const
   {
     ROSE_ASSERT(get_lhs_operand() != NULL);
     ROSE_ASSERT(get_rhs_operand() != NULL);

#if 0
     printf ("In SgDotDotExp::get_type() \n");
#endif

     SgType* returnType = get_rhs_operand()->get_type();

     ROSE_ASSERT(returnType != NULL);

     return returnType;
   }
