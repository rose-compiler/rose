#include <sage3basic.h>

// DQ (2/4/2015): Added noexcept operator as part of C++11 support.
SgType*
SgNoexceptOp::get_type() const
   {
  // DQ (2/4/2015): This function returns a bool type (evaluating if exceptions are to be supported).

     ROSE_ASSERT(p_operand_expr != NULL);

#if 0
     printf ("In SgNoexceptOp::get_type() \n");
#endif

     SgType* returnType = SgTypeBool::createType();

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
