#include <sage3basic.h>

void
SgFinishExp::post_construction_initialization()
   {
   }

SgType*
SgFinishExp::get_type() const
   {
#if 0
     printf ("In SgFinishExp::get_type() \n");
#endif

     SgType* returnType = SgTypeDefault::createType();
     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }
