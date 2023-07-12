#include <sage3basic.h>

SgType*
SgOrOp::get_type() const
   {
  // This function returns an integer representing the logical type

#if 0
     printf ("In SgOrOp::get_type() \n");
#endif

  // DQ (1/14/2006): p_expression_type has been removed, we have to compute the appropriate type (IR specific code)
  // DQ (7/20/2006): Modified to remove redundant SgTypeInt qualifier.
     SgType* returnType = SgTypeInt::createType();

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
