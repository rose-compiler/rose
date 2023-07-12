#include <sage3basic.h>

SgType*
SgCAFCoExpression::get_type() const
   {
     SgType* returnType = get_referData()->get_type();

#if 0
     printf ("In SgCAFCoExpression::get_type() \n");
#endif

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
