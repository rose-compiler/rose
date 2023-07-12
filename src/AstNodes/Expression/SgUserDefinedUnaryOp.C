#include <sage3basic.h>

SgType*
SgUserDefinedUnaryOp::get_type() const
   {
  // DQ (10/8/2008): Unclear how this should be implemented right now!
     ROSE_ASSERT(get_symbol() != NULL);

#if 1
     printf ("In SgUserDefinedUnaryOp::get_type() \n");
#endif

     SgType* returnType = get_symbol()->get_type();
     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
