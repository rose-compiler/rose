#include <sage3basic.h>

SgType*
SgJavaInstanceOfOp::get_type() const
   {
  // This function returns an unsigned integer representing the size of type
  // (the unsigned int should match size_t, which may or may not be defined).

     ROSE_ASSERT(p_operand_expr != NULL || p_operand_type != NULL);

#if 0
     printf ("In SgJavaInstanceOfOp::get_type() \n");
#endif

     SgType* returnType = SgTypeUnsignedInt::createType();

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
