#include <sage3basic.h>

SgType*
SgAsmOp::get_type() const
   {
     SgType* returnType = NULL;

     ROSE_ASSERT(p_expression != NULL);

#if 0
     printf ("In SgAsmOp::get_type() \n");
#endif

  // This is the default case (we can look at the last statement of the block later)
     returnType = p_expression->get_type();
     ROSE_ASSERT(returnType != NULL);

     return returnType;
   }
