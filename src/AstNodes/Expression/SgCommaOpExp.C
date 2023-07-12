#include <sage3basic.h>

SgType*
SgCommaOpExp::get_type() const
   {
  // DQ (10/2/2006): Comma operator's get_type function didn't previously exist, but it should
  // because its semantics is different from the default SgBinaryOp::get_type() which returns
  // the type associated with the lhs.  This returns the type of the rhs.
     ROSE_ASSERT(get_lhs_operand() != NULL);
     ROSE_ASSERT(get_rhs_operand() != NULL);

#if 0
     printf ("In SgCommaOpExp::get_type() \n");
#endif

     SgType* returnType = get_rhs_operand()->get_type();
     ROSE_ASSERT(returnType != NULL);

     return returnType;
   }
